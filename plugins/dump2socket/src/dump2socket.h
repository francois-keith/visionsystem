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
    void handle_receive_from(size_t index, const boost::system::error_code & error,
                                size_t bytes_recvd);
    void handle_send_to(size_t index, const boost::system::error_code & error, 
                                size_t bytes_send);

    /* Configparser method */
    void parse_config_line( std::vector<std::string> & line );

private:
    /* Socket related members */
    boost::asio::io_service io_service_;
    boost::thread * io_service_th_;
    std::vector<udp::socket *> sockets_;
    std::vector<short> ports_;
    std::vector<udp::endpoint> sender_endpoints_;
    /* client request */
    enum { max_request_ = 256 };
    std::vector<char *> clients_data_;
    /* send buffer: chunk id + 50k (max) chunk of data */
    enum { send_size_ = 16385 };
    std::vector<unsigned char *> send_buffers_;
    /* Protocol related */
    std::vector<uint8_t> chunkIDs_;

    /* Plugin related */
    std::vector<std::string> cam_names_;
    std::vector<Camera *> cams_;
    bool is_mono_;
    std::vector<vision::Image<unsigned char, MONO> *> send_imgs_mono_;
    std::vector<vision::Image<uint32_t, RGB> *> send_imgs_rgb_;
    std::vector<unsigned char *> send_imgs_raw_data_;
    std::vector<unsigned int> send_imgs_data_size_;
    std::vector<bool> imgs_lock_;
};

} // namespace visionsystem

PLUGIN(visionsystem::Dump2Socket)

#endif
