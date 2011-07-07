#include "dump2socket.h"

using boost::asio::ip::udp;

namespace visionsystem
{

Dump2Socket::Dump2Socket( visionsystem::VisionSystem * vs, std::string sandbox )
: Plugin( vs, "dump2socket", sandbox ),
  io_service_(), 
  socket_(io_service_), 
  port_(4242), chunkID_(0),
  cam_(0), is_sending_data_(false), current_img_(0), send_img_(0), img_lock_(false)
{}

Dump2Socket::~Dump2Socket()
{
    delete current_img_;
    delete send_img_;
}

bool Dump2Socket::pre_fct()
{
    cam_ = get_default_camera() ;
    register_to_cam< vision::Image<unsigned char, MONO> >( cam_, 10 ) ;

    current_img_ = new vision::Image<unsigned char, MONO>(cam_->get_size());
    send_img_ = new vision::Image<unsigned char, MONO>(cam_->get_size());

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
    io_service_.run();
}

void Dump2Socket::loop_fct()
{
    vision::Image<unsigned char, MONO> * img = dequeue_image< vision::Image<unsigned char, MONO> > (cam_);
    
    while(img_lock_);
    img_lock_ = true;
    current_img_->copy(img);
    img_lock_ = false;

    enqueue_image< vision::Image<unsigned char, MONO> >(cam_, img);
}

bool Dump2Socket::post_fct()
{
    unregister_to_cam< vision::Image<unsigned char, MONO> >(cam_);

    socket_.close();
    io_service_.stop();

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
            while(img_lock_);
            img_lock_ = true;
            send_img_->copy(current_img_);
            img_lock_ = false;
        }
        if(client_message == "more")
        {
            chunkID_++;
        }
        send_buffer_[0] = chunkID_;
        size_t send_size = 0;
        if( (chunkID_ + 1)*(send_size_ - 1) > send_img_->data_size )
        {
            send_size = send_img_->data_size - chunkID_*(send_size_ - 1) + 1;
            std::memcpy(&(send_buffer_[1]), &(send_img_->raw_data[chunkID_*(send_size_ - 1)]), send_size - 1);
        }
        else
        {
            send_size = send_size_;
            std::memcpy(&(send_buffer_[1]), &(send_img_->raw_data[chunkID_*(send_size_ - 1)]), (send_size_ - 1));
        }
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
}

} // namespace visionsystem

