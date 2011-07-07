#ifndef _H_DUMP2SOCKET_H_
#define _H_DUMP2SOCKET_H_

#include <configparser/configparser.h>
#include <visionsystem/plugin.h>
#include <vision/image/image.h>
#include <visionsystem/buffer.h>

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

namespace visionsystem
{

class Dump2Socket : public visionsystem::Plugin, public configparser::WithConfigFile
{
public:
    Dump2Socket( visionsystem::VisionSystem * vs, std::string sandbox );
    ~Dump2Socket();

    /* Implements method from Plugin */
    bool pre_fct();
    void preloop_fct();
    void loop_fct();
    bool post_fct();

private:
    /* Socket callbacks */
    void handle_receive_from(const boost::system::error_code & error,
                                size_t bytes_recvd);
    void handle_send_to(const boost::system::error_code & error, 
                                size_t bytes_send);

    /* Configparser method */
    void parse_config_line( std::vector<std::string> & line );

private:
    /* Socket related members */
    boost::asio::io_service io_service_;
    boost::thread * io_service_th_;
    udp::socket socket_;
    short port_;
    udp::endpoint sender_endpoint_;
    /* client request */
    enum { max_request_ = 256 };
    char client_data_[max_request_];
    /* send buffer: chunk id + 50k (max) chunk of data */
    enum { send_size_ = 8193 };
    unsigned char send_buffer_[send_size_];
    /* Protocol related */
    uint8_t chunkID_;

    /* Plugin related */
    Camera * cam_;
    vision::Image<unsigned char, MONO> * send_img_;
    bool img_lock_;
};

} // namespace visionsystem

PLUGIN(visionsystem::Dump2Socket)

#endif
