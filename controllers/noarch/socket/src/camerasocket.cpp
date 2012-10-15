#include "camerasocket.h"

#include <boost/date_time/posix_time/posix_time.hpp>

inline void rgb24_to_rgba(unsigned char * rgb24_buffer, unsigned int nb_pixels, unsigned char * rgba_buffer)
{
    for(unsigned int i = 0; i < nb_pixels; ++i)
    {
        memcpy(&(rgba_buffer[4*i]), &(rgb24_buffer[3*i]), 3);
        rgba_buffer[4*i+3] = 0xFF;
    }
}

using boost::asio::ip::udp;

namespace visionsystem
{

CameraSocket::CameraSocket(boost::asio::io_service & io_service)
: img_size_(0,0), active_(false), img_coding_(VS_MONO8), name_("network-unconfigured"),
  from_stream_(false), next_cam_(false),
  cam_ready_(false), server_name_(""), server_port_(0), 
  reverse_connection_(false), port_(0),
  frame_(0),
  data_compress_(false), m_decoder(0),
  io_service_(io_service), socket_(io_service), request_(""), chunkID_(0), 
  timeout_timer_(io_service, boost::posix_time::seconds(1)),
  shw_img_mono_(0), rcv_img_mono_(0), shw_img_rgb_(0), rcv_img_rgb_(0), shw_img_raw_data_(0), rcv_img_raw_data_(0), 
  buffersize_(100), verbose_(false)
{
    previous_frame_t_.tv_sec = 0;
    previous_frame_t_.tv_usec = 0;
}

CameraSocket::~CameraSocket()
{
    delete shw_img_mono_;
    delete rcv_img_mono_;
    delete shw_img_rgb_;
    delete rcv_img_rgb_;
    delete m_decoder;
}

void CameraSocket::start_cam()
{
    _buffer.clear();
    for( unsigned int i = 0; i < buffersize_; ++i )
    {
        _buffer.enqueue( new Frame( get_coding(), get_size() ) );
    }
    if(img_coding_ == VS_MONO8)
    {
        shw_img_mono_ = new vision::Image<unsigned char, vision::MONO>(get_size());
        shw_img_raw_data_ = shw_img_mono_->raw_data;
        rcv_img_mono_ = new vision::Image<unsigned char, vision::MONO>(get_size());
        rcv_img_raw_data_ = rcv_img_mono_->raw_data;
    }
    else
    {
        shw_img_rgb_ = new vision::Image<uint32_t, vision::RGB>(get_size());
        shw_img_raw_data_ = (unsigned char*)(shw_img_rgb_->raw_data);
        rcv_img_rgb_ = new vision::Image<uint32_t, vision::RGB>(get_size());
        rcv_img_raw_data_ = (unsigned char*)(rcv_img_rgb_->raw_data);
    }

    if(data_compress_ && img_coding_ == VS_MONO8)
    {
        throw(std::string("Compress option and MONO cameras are not compatible"));
    }

    if(data_compress_)
    {
        m_decoder = new vision::H264Decoder(get_size().x, get_size().y);
        #if Vision_HAS_LIBAVCODEC != 1
            std::cerr << "[camerasocket] " << get_name() << " is configured with Compress option but libvision does not have H.264 support" << std::endl;
        #endif
    }

    if(!reverse_connection_)
    {
        /* TODO DNS resolution */
        if(verbose_)
        {
            std::cout << "[camerasocket] " << get_name() << " will connect to " << server_name_ << ":" << server_port_ << std::endl;
        }
        receiver_endpoint_ = udp::endpoint(boost::asio::ip::address::from_string(server_name_), server_port_);
        socket_.open(udp::v4());
        request_ = "get";
        if(verbose_)
        {
            std::cout << "[camerasocket] " << get_name() << " sending request for image" << std::endl;
        }
        socket_.async_send_to(
            boost::asio::buffer(request_.c_str(), request_.size()+1), receiver_endpoint_,
            boost::bind(&CameraSocket::handle_send_to, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        socket_.open(udp::v4());
        socket_.bind(udp::endpoint(udp::v4(), port_));
        if(verbose_)
        {
            std::cout << "[camerasocket] " << get_name() << " bind on port " << port_ << ", Waiting for request now" << std::endl;
        }
        socket_.async_receive_from(
           boost::asio::buffer(chunk_buffer_, chunk_size_), sender_endpoint_,
           boost::bind(&CameraSocket::handle_receive_from, this,
               boost::asio::placeholders::error,
               boost::asio::placeholders::bytes_transferred));
    }
}

bool CameraSocket::has_data()
{
    timeval now;
    gettimeofday(&now, 0);
    unsigned int elapsed_time = 0;
    if(now.tv_usec < previous_frame_t_.tv_usec)
    {
        elapsed_time = 1000000*(now.tv_sec - previous_frame_t_.tv_sec) - (previous_frame_t_.tv_usec - now.tv_usec);
    }
    else
    {
        elapsed_time = 1000000*(now.tv_sec - previous_frame_t_.tv_sec) + (now.tv_usec - previous_frame_t_.tv_usec);
    }
    if(elapsed_time > fps_)
    {
        previous_frame_t_ = now;
        frame_++;
        return true;
    }
    return false;
}

unsigned char * CameraSocket::get_data()
{
    return shw_img_raw_data_;
}

void CameraSocket::stop_cam()
{
    socket_.close();
}

void CameraSocket::parse_config_line( std::vector<std::string> & line )
{
    if( fill_member(line, "Active", active_) )
        return;

    if( fill_member(line, "Name", name_) )
        return;

    if( fill_member(line, "Server", server_name_) )
        return;

    if( fill_member(line, "Port", server_port_) )
        return;

    if( fill_member( line, "FPS", fps_ ) )
    {
        fps_ = 1000000/fps_;
        return;
    }

    std::string coding;
    if( fill_member( line, "ColorMode", coding ) )
    {
        if( coding == "MONO" )
        {
            img_coding_ = VS_MONO8;
        }
        else if( coding == "RGB" )
        {
            img_coding_ = VS_RGB32;
        }
        else
        {
            throw("[CameraFilestream] ColorMode not valid, set MONO or RGB (case sensitive)");
        }
        return;
    }

    std::vector<int> resolution(0);
    if( fill_member( line, "Resolution", resolution) )
    {
        if(resolution.size() > 1)
        {
            img_size_ = vision::ImageRef(resolution[0], resolution[1]);
        }
        else
        {
            throw("[CameraSocket] Error when reading resolution");
        }
        return;
    }

    if( fill_member( line, "Stream", from_stream_) )
        return;

    if( fill_member( line, "Compress", data_compress_) )
    {
#if VS_HAS_ZLIB != 1
        std::cerr << "[WARNING] Socket camera configured for compressed data without support for zlib" << std::endl;
#endif
        return;
    }

    if( fill_member( line, "ReverseConnection", reverse_connection_) )
        return;

    if( fill_member( line, "ReversePort", port_) )
        return;

    if( fill_member( line, "Verbose", verbose_) )
        return;
}

void CameraSocket::handle_receive_from(const boost::system::error_code & error,
                                        size_t bytes_recvd)
{
    timeout_timer_.cancel();
    if(!error && bytes_recvd)
    {
        if(bytes_recvd == 5)
        {
            std::string request((const char*)chunk_buffer_);
            if(request == "init")
            {
                receiver_endpoint_ = sender_endpoint_;
                request_ = "get";
                socket_.async_send_to(
                    boost::asio::buffer(request_.c_str(), request_.size()+1), receiver_endpoint_,
                    boost::bind(&CameraSocket::handle_send_to, this,
                      boost::asio::placeholders::error,
                      boost::asio::placeholders::bytes_transferred));
                return;
            }
        }
        if(chunkID_ != chunk_buffer_[0])
        {
            /* Missed packet */
//            std::cerr << "[CameraSocket] " << name_ << " : missed an upstream frame" << std::endl;
            chunkID_ = 0;
            if(next_cam_ && from_stream_)
            {
                request_ = "next";
                next_cam_ = false;
            }
            else
            {
                request_ = "get";
            }
        }
        else
        {
            std::memcpy(&(rcv_img_raw_data_[chunkID_*(chunk_size_-1)]), &(chunk_buffer_[1]), bytes_recvd);
            if(bytes_recvd < chunk_size_)
            {
                if(verbose_)
                {
                    std::cout << "[camerasocket] " << get_name() << " received enough data, will now Decode" << std::endl;
                }
                if(img_coding_ == VS_MONO8) 
                {
                    shw_img_mono_->copy(rcv_img_mono_);
                }
                else
                {
                    if(data_compress_)
                    {
                        m_decoder->Decode(chunkID_*chunk_size_ + bytes_recvd, rcv_img_raw_data_, *shw_img_rgb_);
                    }
                    else
                    {
                        rgb24_to_rgba(rcv_img_raw_data_, shw_img_rgb_->pixels, (unsigned char *)(shw_img_rgb_->raw_data));
                    }
                }
                chunkID_ = 0;
                if(next_cam_ && from_stream_)
                {
                    request_ = "next";
                    next_cam_ = false;
                }
                else
                {
                    request_ = "get";
                }
            }
            else
            {
                chunkID_++;
                if(verbose_)
                {
                    std::cout << "[camerasocket] " << get_name() << " requesting another chunk" << std::endl;
                }
                socket_.async_receive_from(
                      boost::asio::buffer(chunk_buffer_, chunk_size_), sender_endpoint_,
                      boost::bind(&CameraSocket::handle_receive_from, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
                timeout_timer_.expires_from_now(boost::posix_time::seconds(1));
                timeout_timer_.async_wait(boost::bind(&CameraSocket::handle_timeout, this,
                        boost::asio::placeholders::error));
                return;
            }
        }
        if(verbose_)
        {
            std::cout << "[camerasocket] " << get_name() << " sending new request " << request_ << std::endl;
        }
        socket_.async_send_to(
            boost::asio::buffer(request_.c_str(), request_.size()+1), receiver_endpoint_,
            boost::bind(&CameraSocket::handle_send_to, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        if(next_cam_ && from_stream_)
        {
            request_ = "next";
            next_cam_ = false;
        }
        else
        {
            request_ = "get";
        }
        if(verbose_)
        {
            std::cout << "[camerasocket] " << get_name() << " error in reception, requesting another frame" << std::endl;
        }
        socket_.async_send_to(
            boost::asio::buffer(request_.c_str(), request_.size()+1), receiver_endpoint_,
            boost::bind(&CameraSocket::handle_send_to, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
    }
}

void CameraSocket::handle_send_to(const boost::system::error_code & error,
                                size_t bytes_send)
{
    if(verbose_)
    {
        std::cout << "[camerasocket] " << get_name() << " sent request " << request_ << ", waiting for data" << std::endl;
    }
    socket_.async_receive_from(
          boost::asio::buffer(chunk_buffer_, chunk_size_), sender_endpoint_,
          boost::bind(&CameraSocket::handle_receive_from, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    timeout_timer_.expires_from_now(boost::posix_time::seconds(1));
    timeout_timer_.async_wait(boost::bind(&CameraSocket::handle_timeout, this,
            boost::asio::placeholders::error));
}

void CameraSocket::handle_timeout(const boost::system::error_code & error)
{
    if(error != boost::asio::error::operation_aborted)
    {
        if(verbose_)
        {
            std::cout << "[camerasocket] " << get_name() << ": timeout, requesting another frame" << std::endl;
        }
        request_ = "get";
        chunkID_ = 0;
        socket_.async_send_to(
            boost::asio::buffer(request_.c_str(), request_.size()+1), receiver_endpoint_,
            boost::bind(&CameraSocket::handle_send_to, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
    }
}

} // namespace visionsystem
