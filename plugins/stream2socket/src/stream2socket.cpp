#include "stream2socket.h"

#include "config.h"
#if VS_HAS_ZLIB == 1

#include <zlib.h>

unsigned int compress(unsigned char * data_in, unsigned int data_in_size, unsigned char * data_out)
{
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree  = Z_NULL;
    strm.opaque = Z_NULL;
    int ret = deflateInit(&strm, Z_BEST_SPEED);
    
    strm.avail_in = data_in_size;
    strm.next_in  = data_in;

    strm.avail_out = data_in_size;
    strm.next_out  = data_out;

    int flush = Z_FINISH;
    ret = deflate(&strm, flush);
    unsigned int zsize = data_in_size - strm.avail_out;
    deflateEnd(&strm);
    return zsize;
}

#else

unsigned int compress(unsigned char * data_in, unsigned int data_in_size, unsigned char * data_out)
{
    memcpy(data_out, data_in, data_in_size);
    return data_in_size;
} 

#endif // ZLIB specific function

inline void remove_alpha(unsigned char * data_in, unsigned int nb_pixels, unsigned char * data_out)
{
    for(unsigned int i = 0; i < nb_pixels; ++i)
    {
        memcpy(&(data_out[3*i]), &(data_in[4*i]), 3);
    }
}

using boost::asio::ip::udp;

namespace visionsystem
{

Stream2Socket::Stream2Socket( visionsystem::VisionSystem * vs, std::string sandbox )
: Plugin( vs, "stream2socket", sandbox ),
  io_service_(), io_service_th_(0), 
  socket_(0), 
  port_(0), chunkID_(0),
  active_cam_(0), cams_(0), is_mono_(true), compress_data_(false),
  send_img_mono_(0), send_img_rgb_(0), img_lock_(false)
{}

Stream2Socket::~Stream2Socket()
{
    delete send_img_mono_;
    delete send_img_rgb_;
    delete[] client_data_;
    delete[] send_buffer_;
    delete socket_;
}

bool Stream2Socket::pre_fct()
{
    std::string filename = get_sandbox() + std::string("/stream2socket.conf");

    try
    {
        read_config_file(filename.c_str());
    }
    catch(std::string msg)
    {
        throw(std::string("stream2socket will not work without a correct stream2socket.conf config file"));
    }

    /* Store all active cameras */
    std::vector<Camera *> cameras = get_all_cameras();
    for(size_t i = 0; i < cameras.size(); ++i)
    {
        if(cameras[i]->is_active())
        {
            cams_.push_back(cameras[i]);
        }
    }
    if(cams_.size() == 0)
    {
        throw(std::string("No active cameras in the server, stream2socket cannot operate"));
    }

    if(is_mono_)
    {
        vision::ImageRef image_size = cams_[0]->get_size();
        register_to_cam< vision::Image<unsigned char, vision::MONO> >( cams_[0], 10 ) ;
        send_img_mono_ = new vision::Image<unsigned char, vision::MONO>(image_size);
        send_img_data_size_ = send_img_mono_->data_size;
        send_img_raw_data_ = send_img_mono_->raw_data;
    }
    else
    {
        vision::ImageRef image_size = cams_[0]->get_size();
        register_to_cam< vision::Image<uint32_t, vision::RGB> >( cams_[0], 10 ) ;
        send_img_rgb_ = new vision::Image<uint32_t, vision::RGB>(image_size);
        send_img_data_size_ = 3 * send_img_rgb_->pixels;
        send_img_raw_data_ = (unsigned char *)(send_img_rgb_->raw_data);
    }

    socket_ = new udp::socket(io_service_);
    socket_->open(udp::v4());
    socket_->bind(udp::endpoint(udp::v4(), port_));
    client_data_ = new char[max_request_];
    send_buffer_ = new unsigned char[send_size_];
    
    socket_->async_receive_from(
       boost::asio::buffer(client_data_, max_request_), sender_endpoint_,
       boost::bind(&Stream2Socket::handle_receive_from, this, 
           boost::asio::placeholders::error,
           boost::asio::placeholders::bytes_transferred));

    return true ;
}

void Stream2Socket::preloop_fct()
{
    io_service_th_ = new boost::thread(boost::bind(&boost::asio::io_service::run, &io_service_));
}

void Stream2Socket::loop_fct()
{
    if(is_mono_)
    {
        vision::Image<unsigned char, vision::MONO> * img = dequeue_image< vision::Image<unsigned char, vision::MONO> > (cams_[active_cam_]);
        
        if(img_lock_)
        {
            if(compress_data_)
            {
                send_img_data_size_ = compress(img->raw_data, img->data_size, send_img_raw_data_);
            }
            else
            {
                send_img_mono_->copy(img);
            }
            img_lock_ = false;
        }

        enqueue_image< vision::Image<unsigned char, vision::MONO> >(cams_[active_cam_], img);
    }
    else
    {
        vision::Image<uint32_t, vision::RGB> * img = dequeue_image< vision::Image<uint32_t, vision::RGB> > (cams_[active_cam_]);
        
        if(img_lock_)
        {
            if(compress_data_)
            {
                send_img_data_size_ = compress((unsigned char *)(img->raw_data), img->data_size, send_img_raw_data_);
            }
            else
            {
                remove_alpha((unsigned char*)(img->raw_data), img->pixels, send_img_raw_data_);
            }
            img_lock_ = false;
        }

        enqueue_image< vision::Image<uint32_t, vision::RGB> >(cams_[active_cam_], img);
    }
    if(next_cam_)
    {
        if(is_mono_)
        {
            unregister_to_cam< vision::Image<unsigned char, vision::MONO> > (cams_[active_cam_]);
            active_cam_ = (active_cam_ + 1) % cams_.size();
            register_to_cam< vision::Image<unsigned char, vision::MONO> > (cams_[active_cam_], 10);
        }
        else
        {
            unregister_to_cam< vision::Image<uint32_t, vision::RGB> > (cams_[active_cam_]);
            active_cam_ = (active_cam_ + 1) % cams_.size();
            register_to_cam< vision::Image<uint32_t, vision::RGB> > (cams_[active_cam_], 10);
        }
        next_cam_ = false;
    }
}

bool Stream2Socket::post_fct()
{
    if(is_mono_)
    {
        unregister_to_cam< vision::Image<unsigned char, vision::MONO> >(cams_[active_cam_]);
    }
    else
    {
        unregister_to_cam< vision::Image<uint32_t, vision::RGB> >(cams_[active_cam_]);
    }

    socket_->close();
    io_service_.stop();
    io_service_th_->join();
    delete io_service_th_;
    io_service_th_ = 0;

    return true;
}

void Stream2Socket::handle_receive_from(const boost::system::error_code & error,
                                 size_t bytes_recvd)
{
    if(!error && bytes_recvd > 0)
    {
        std::string client_message(client_data_);
        if(client_message == "get")
        {
            /* Client request: reset sending vision::Image */
            chunkID_ = 0;
            img_lock_ = true;
            while(img_lock_) { usleep(100); }
        }
        if(client_message == "next")
        {
            /* Client requested to change camera stream */
            next_cam_ = true;
            while(next_cam_) { usleep(100); }
            chunkID_ = 0;
            img_lock_ = true;
            while(img_lock_) { usleep(100); }
        }
        send_buffer_[0] = chunkID_;
        size_t send_size = 0;
        if( (chunkID_ + 1)*(send_size_ - 1) > send_img_data_size_ )
        {
            send_size = send_img_data_size_ - chunkID_*(send_size_ - 1) + 1;
        }
        else
        {
            send_size = send_size_;
        }
        std::memcpy(&(send_buffer_[1]), &(send_img_raw_data_[chunkID_*(send_size_ - 1)]), send_size - 1);
        socket_->async_send_to(
            boost::asio::buffer(send_buffer_, send_size), sender_endpoint_,
            boost::bind(&Stream2Socket::handle_send_to, this, 
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        socket_->async_receive_from(
            boost::asio::buffer(client_data_, max_request_), sender_endpoint_,
            boost::bind(&Stream2Socket::handle_receive_from, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }
}

void Stream2Socket::handle_send_to(const boost::system::error_code & error,
                            size_t bytes_send)
{
    if(error) { std::cerr << error.message() << std::endl; }
    if(bytes_send < send_size_) // after last packet sent
    {
        socket_->async_receive_from(
            boost::asio::buffer(client_data_, max_request_), sender_endpoint_,
            boost::bind(&Stream2Socket::handle_receive_from, this, 
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
        return;
    }
    chunkID_++;
    send_buffer_[0] = chunkID_;
    size_t send_size = 0;
    if( (chunkID_ + 1)*(send_size_ - 1) > send_img_data_size_ )
    {
        send_size = send_img_data_size_ - chunkID_*(send_size_ - 1) + 1;
    }
    else
    {
        send_size = send_size_;
    }
    std::memcpy(&(send_buffer_[1]), &(send_img_raw_data_[chunkID_*(send_size_ - 1)]), send_size - 1);
    socket_->async_send_to(
        boost::asio::buffer(send_buffer_, send_size), sender_endpoint_,
        boost::bind(&Stream2Socket::handle_send_to, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void Stream2Socket::parse_config_line( std::vector<std::string> & line )
{
    if( fill_member(line, "Port", port_) )
        return;

    std::string mode;
    if( fill_member(line, "ColorMode", mode) )
    {
        is_mono_ = (mode == "MONO");
        return;
    }

    if( fill_member(line, "Compress", compress_data_) )
    {
#if VS_HAS_ZLIB != 1
        if(compress_data_)
        {
            std::cerr << "[WARNING] You configured dump2socket to compress data without ZLIB support" << std::endl;
        }
#endif
        return;
    }
}

} // namespace visionsystem

