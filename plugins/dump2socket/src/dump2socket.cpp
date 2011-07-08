#include "dump2socket.h"

using boost::asio::ip::udp;

namespace visionsystem
{

Dump2Socket::Dump2Socket( visionsystem::VisionSystem * vs, std::string sandbox )
: Plugin( vs, "dump2socket", sandbox ),
  io_service_(), io_service_th_(0), 
  socket_(io_service_), 
  port_(4242), chunkID_(0),
  cam_(0), is_mono_(true), send_img_mono_(0), send_img_rgb_(0), img_lock_(false)
{}

Dump2Socket::~Dump2Socket()
{
    delete send_img_mono_;
    delete send_img_rgb_;
}

bool Dump2Socket::pre_fct()
{
    std::string filename = get_sandbox() + std::string("/dump2socket.conf");

    try
    {
        read_config_file(filename.c_str());
    }
    catch(std::string msg)
    {
        throw(std::string("dump2socket will not work without a correct dump2socket.conf config file"));
    }

    cam_ = get_default_camera() ;
    if(is_mono_)
    {
        register_to_cam< vision::Image<unsigned char, MONO> >( cam_, 10 ) ;
        send_img_mono_ = new vision::Image<unsigned char, MONO>(cam_->get_size());
        send_img_data_size_ = send_img_mono_->data_size;
        send_img_raw_data_ = send_img_mono_->raw_data;
    }
    else
    {
        register_to_cam< vision::Image<uint32_t, RGB> >(cam_, 10);
        send_img_rgb_ = new vision::Image<uint32_t, RGB>(cam_->get_size());
        send_img_data_size_ = send_img_rgb_->data_size;
        send_img_raw_data_ = (unsigned char*)(send_img_rgb_->raw_data);
    }

    socket_.open(udp::v4());
    socket_.bind(udp::endpoint(udp::v4(), port_));

    socket_.async_receive_from(
        boost::asio::buffer(client_data_, max_request_), sender_endpoint_,
        boost::bind(&Dump2Socket::handle_receive_from, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));

    return true ;
}

void Dump2Socket::preloop_fct()
{
    io_service_th_ = new boost::thread(boost::bind(&boost::asio::io_service::run, &io_service_));
}

void Dump2Socket::loop_fct()
{
    if(is_mono_)
    {
        vision::Image<unsigned char, MONO> * img = dequeue_image< vision::Image<unsigned char, MONO> > (cam_);
        
        if(img_lock_)
        {
            send_img_mono_->copy(img);
            img_lock_ = false;
        }

        enqueue_image< vision::Image<unsigned char, MONO> >(cam_, img);
    }
    else
    {
        vision::Image<uint32_t, RGB> * img = dequeue_image< vision::Image<uint32_t, RGB> > (cam_);
        
        if(img_lock_)
        {
            send_img_rgb_->copy(img);
            img_lock_ = false;
        }

        enqueue_image< vision::Image<uint32_t, RGB> >(cam_, img);
    }
}

bool Dump2Socket::post_fct()
{
    if(is_mono_)
    {
        unregister_to_cam< vision::Image<unsigned char, MONO> >(cam_);
    }
    else
    {
        unregister_to_cam< vision::Image<uint32_t, RGB> >(cam_);
    }

    socket_.close();
    io_service_.stop();
    io_service_th_->join();
    delete io_service_th_;
    io_service_th_ = 0;

    return true;
}

void Dump2Socket::handle_receive_from(const boost::system::error_code & error,
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
        if(client_message == "more")
        {
            chunkID_++;
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
        socket_.async_send_to(
            boost::asio::buffer(send_buffer_, send_size), sender_endpoint_,
            boost::bind(&Dump2Socket::handle_send_to, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        socket_.async_receive_from(
            boost::asio::buffer(client_data_, max_request_), sender_endpoint_,
            boost::bind(&Dump2Socket::handle_receive_from, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }
}

void Dump2Socket::handle_send_to(const boost::system::error_code & error,
                            size_t bytes_send)
{
    if(error) { std::cerr << error.message() << std::endl; }
    socket_.async_receive_from(
        boost::asio::buffer(client_data_, max_request_), sender_endpoint_,
        boost::bind(&Dump2Socket::handle_receive_from, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void Dump2Socket::parse_config_line( std::vector<std::string> & line )
{
    if( fill_member(line, "Port", port_) )
        return;

    std::string mode;
    if( fill_member(line, "ColorMode", mode) )
    {
        is_mono_ = (mode == "MONO");
        return;
    }
}

} // namespace visionsystem

