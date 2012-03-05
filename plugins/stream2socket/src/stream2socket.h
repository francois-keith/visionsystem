#ifndef _H_STREAM2SOCKET_H_
#define _H_STREAM2SOCKET_H_

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include <configparser/configparser.h>
#include <visionsystem/plugin.h>
#include <vision/image/image.h>
#include <visionsystem/buffer.h>

using boost::asio::ip::udp;

namespace visionsystem
{

class Stream2Socket : public visionsystem::Plugin, public configparser::WithConfigFile
{
public:
    Stream2Socket( visionsystem::VisionSystem * vs, std::string sandbox );
    ~Stream2Socket();

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
    udp::socket * socket_;
    short port_;
    udp::endpoint sender_endpoint_;
    /* client request */
    enum { max_request_ = 256 };
    char* client_data_;
    /* send buffer: chunk id + 50k (max) chunk of data */
    enum { send_size_ = 32769 };
    unsigned char * send_buffer_;
    /* Protocol related */
    uint8_t chunkID_;

    /* Plugin related */
    unsigned int active_cam_;
    std::vector<Camera *> cams_;
    bool is_mono_;
    bool compress_data_;
    vision::Image<unsigned char, vision::MONO> * send_img_mono_;
    vision::Image<uint32_t, vision::RGB> * send_img_rgb_;
    unsigned char * send_img_raw_data_;
    unsigned int send_img_data_size_;
    bool img_lock_;
    bool next_cam_;
};

} // namespace visionsystem

PLUGIN(visionsystem::Stream2Socket)

#endif
