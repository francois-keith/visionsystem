#ifdef VS_HAS_CONTROLLER_SOCKET
    #include "camerasocket.h"
#endif

#include "stream2socketprocess.h"

#include <visionsystem/camera.h>

using boost::asio::ip::udp;

inline void remove_alpha(unsigned char * data_in, unsigned int nb_pixels, unsigned char * data_out)
{
    for(unsigned int i = 0; i < nb_pixels; ++i)
    {
        memcpy(&(data_out[3*i]), &(data_in[4*i]), 3);
    }
}

namespace visionsystem
{

Stream2SocketProcess::Stream2SocketProcess( boost::asio::io_service & io_service, short port, unsigned int active_cam, Camera * cam, 
                            bool compress, bool raw, bool reverse_connection, const std::string & server_name,
                            bool verbose)
: socket_(0), server_name_(server_name), port_(port), chunkIDs_(0),
  encoder_(0), ready_(false), active_cam_(active_cam), cam_(cam), send_img_(0), send_img_raw_data_(0), send_img_data_size_(0),
  img_lock_(false), next_cam_(false), request_cam_(false), request_name_(""),
  compress_(compress), raw_(raw), reverse_(reverse_connection), verbose_(verbose)
{
    if(cam)
    {
        Initialize(cam->get_size(), cam->get_fps());
    }

    socket_ = new udp::socket(io_service);
    socket_->open(udp::v4());
    send_buffer_ = new unsigned char[send_size_];
    client_data_ = new char[max_request_];

    if(reverse_connection)
    {
        /* DNS Resolution */
        {
            udp::resolver resolver(io_service);
            std::stringstream ss;
            ss << port;
            udp::resolver::query query(udp::v4(), server_name_, ss.str());
            receivers_endpoint_.push_back(*resolver.resolve(query));
            chunkIDs_.push_back(0);
        }
        if(verbose_)
        {
            std::cout << "[stream2socket] connected to " << server_name_ << ":" << port_ << " to stream images" << std::endl;
        }
        ready_ = true;
        /* Get ready for eventual request */
        socket_->async_receive_from(
           boost::asio::buffer(client_data_, max_request_), sender_endpoint_,
           boost::bind(&Stream2SocketProcess::handle_receive_from, this,
               boost::asio::placeholders::error,
               boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        socket_->bind(udp::endpoint(udp::v4(), port_));
        if(verbose_)
        {
            std::cout << "[stream2socket] Waiting for request now" << std::endl;
        }
        socket_->async_receive_from(
           boost::asio::buffer(client_data_, max_request_), sender_endpoint_,
           boost::bind(&Stream2SocketProcess::handle_receive_from, this,
               boost::asio::placeholders::error,
               boost::asio::placeholders::bytes_transferred));
    }
}

Stream2SocketProcess::~Stream2SocketProcess()
{
    delete send_img_;
    delete[] client_data_;
    delete[] send_buffer_;
    delete socket_;
    delete encoder_;
}

void Stream2SocketProcess::Initialize(vision::ImageRef size, float fps)
{
    if(compress_)
    {
        encoder_ = new vision::H264Encoder(size.x, size.y, fps);
        #if Vision_HAS_LIBAVCODEC != 1
        std::cerr << "[stream2socket] H.264 support not built in vision library, Compress option is not usable" << std::endl;
        #endif
    }

    send_img_ = new vision::Image<uint32_t, vision::RGB>(size);
    send_img_data_size_ = 3 * send_img_->pixels;
    send_img_raw_data_ = (unsigned char *)(send_img_->raw_data);
}

void Stream2SocketProcess::SendImage( vision::Image<uint32_t, vision::RGB> & img )
{
    if(!img_lock_ && ready_)
    {
        if(compress_)
        {
            vision::H264EncoderResult res = encoder_->Encode(img);
            send_img_data_size_ = res.frame_size;
            send_img_raw_data_  = res.frame_data;
        }
        else if(raw_)
        {
            memcpy(send_img_raw_data_, img.raw_data, img.data_size);
            send_img_data_size_ = img.data_size;
        }
        else
        {
            remove_alpha((unsigned char*)(img.raw_data), img.pixels, send_img_raw_data_);
        }
        img_lock_ = true;
        send_buffer_[0] = 0;
        size_t send_size = 0;
        if( (0 + 1)*(send_size_ - 1) > send_img_data_size_ )
        {
            send_size = send_img_data_size_ - 0*(send_size_ - 1) + 1;
        }
        else
        {
            send_size = send_size_;
        }
        std::memcpy(&(send_buffer_[1]), &(send_img_raw_data_[0*(send_size_ - 1)]), send_size - 1);
        if(verbose_)
        {
            std::cout << "[stream2socket] Sending data to client, data size: " << send_size << std::endl;
        }
        for(size_t id = 0; id < receivers_endpoint_.size(); ++id)
        {
            chunkIDs_[id] = 0;
            socket_->async_send_to(
                boost::asio::buffer(send_buffer_, send_size), receivers_endpoint_[id],
                boost::bind(&Stream2SocketProcess::handle_send_to, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred, id));
        }
    }
}

void Stream2SocketProcess::handle_send_to(const boost::system::error_code & error,
                            size_t bytes_send, size_t id)
{
    if(error) { std::cerr << error.message() << std::endl; }
    if(bytes_send < send_size_) // after last packet sent
    {
        img_lock_ = false;
        if(verbose_)
        {
            std::cout << "[stream2socket] Image sent, waiting for next image" << std::endl;
        }
        return;
    }
    chunkIDs_[id]++;
    send_buffer_[0] = chunkIDs_[id];
    size_t send_size = 0;
    if( (chunkIDs_[id] + 1)*(send_size_ - 1) > send_img_data_size_ )
    {
        send_size = send_img_data_size_ - chunkIDs_[id]*(send_size_ - 1) + 1;
    }
    else
    {
        send_size = send_size_;
    }
    std::memcpy(&(send_buffer_[1]), &(send_img_raw_data_[chunkIDs_[id]*(send_size_ - 1)]), send_size - 1);
    if(verbose_)
    {
        std::cout << "[stream2socket] More data to send, next packet size: " << send_size << std::endl;
    }
    socket_->async_send_to(
        boost::asio::buffer(send_buffer_, send_size), receivers_endpoint_[id],
        boost::bind(&Stream2SocketProcess::handle_send_to, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred, id));
}

void Stream2SocketProcess::handle_receive_from(const boost::system::error_code & error,
                                 size_t bytes_recvd)
{
    if(!error && bytes_recvd > 0)
    {
        std::string client_message(client_data_);
        if(verbose_)
        {
            std::cout << "[stream2socket] Got request " << client_message << std::endl;
        }
        if(client_message == "get")
        {
            /* Client request: reset sending vision::Image */
            bool new_client = true;
            for(size_t id = 0; id < receivers_endpoint_.size(); ++id)
            {
                if(sender_endpoint_ == receivers_endpoint_[id])
                {
                    new_client = false;
                    break;
                }
            }
            if(new_client)
            {
                receivers_endpoint_.push_back(sender_endpoint_);
                chunkIDs_.push_back(0);
            }
            ready_ = true;
        }
        else if(client_message == "next")
        {
            /* Client requested to change camera stream */
            next_cam_ = true;
        }
        else if(client_message.substr(0, 8) == "request ")
        {
            request_cam_ = true;
            request_name_ = client_message.substr(8);
        }
    }
    else
    {
        if(verbose_)
        {
            std::cout << "[stream2socket] Reception error, waiting for next message" << std::endl;
        }
        if(error) { std::cerr << error.message() << std::endl; }
    }
    socket_->async_receive_from(
        boost::asio::buffer(client_data_, max_request_), sender_endpoint_,
        boost::bind(&Stream2SocketProcess::handle_receive_from, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

} // namespace visionsystem

