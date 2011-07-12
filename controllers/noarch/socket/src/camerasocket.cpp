#include "camerasocket.h"

using boost::asio::ip::udp;

namespace visionsystem
{

CameraSocket::CameraSocket(boost::asio::io_service & io_service)
: img_size_(0,0), active_(false), img_coding_(VS_MONO8), name_("network-unconfigured"),
  cam_ready_(false), server_name_(""), server_port_(0),
  io_service_(io_service), socket_(io_service), request_(""), chunkID_(0), 
  shw_img_mono_(0), rcv_img_mono_(0), shw_img_rgb_(0), rcv_img_rgb_(0), shw_img_raw_data_(0), rcv_img_raw_data_(0), 
  buffersize_(100)
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
        shw_img_mono_ = new vision::Image<unsigned char, MONO>(get_size());
        shw_img_raw_data_ = shw_img_mono_->raw_data;
        rcv_img_mono_ = new vision::Image<unsigned char, MONO>(get_size());
        rcv_img_raw_data_ = rcv_img_mono_->raw_data;
    }
    else
    {
        shw_img_rgb_ = new vision::Image<uint32_t, RGB>(get_size());
        shw_img_raw_data_ = (unsigned char*)(shw_img_rgb_->raw_data);
        rcv_img_rgb_ = new vision::Image<uint32_t, RGB>(get_size());
        rcv_img_raw_data_ = (unsigned char*)(rcv_img_rgb_->raw_data);
    }

    /* TODO DNS resolution */
    receiver_endpoint_ = udp::endpoint(boost::asio::ip::address::from_string(server_name_), server_port_);
    socket_.open(udp::v4());
    request_ = "get";
    socket_.async_send_to(
        boost::asio::buffer(request_.c_str(), request_.size()+1), receiver_endpoint_,
        boost::bind(&CameraSocket::handle_send_to, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
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
}

void CameraSocket::handle_receive_from(const boost::system::error_code & error,
                                        size_t bytes_recvd)
{
    if(!error && bytes_recvd)
    {
        if(chunkID_ != chunk_buffer_[0])
        {
            /* Missed packet */
            std::cerr << "[CameraSocket] " << name_ << " : missed an upstream frame" << std::endl;
            chunkID_ = 0;
            request_ = "get";
        }
        else
        {
            std::memcpy(&(rcv_img_raw_data_[chunkID_*(chunk_size_-1)]), &(chunk_buffer_[1]), bytes_recvd);
            if(bytes_recvd < chunk_size_)
            {
                if(img_coding_ == VS_MONO8) 
                {
                    shw_img_mono_->copy(rcv_img_mono_);
                }
                else
                {
                    shw_img_rgb_->copy(rcv_img_rgb_);
                }
                chunkID_ = 0;
                request_ = "get";
            }
            else
            {
                chunkID_++;
                request_ = "more";
            }
        }
        socket_.async_send_to(
            boost::asio::buffer(request_.c_str(), request_.size()+1), receiver_endpoint_,
            boost::bind(&CameraSocket::handle_send_to, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        request_ = "get";
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
    socket_.async_receive_from(
          boost::asio::buffer(chunk_buffer_, chunk_size_), sender_endpoint_,
          boost::bind(&CameraSocket::handle_receive_from, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

} // namespace visionsystem