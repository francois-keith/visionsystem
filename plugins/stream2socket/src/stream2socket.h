#ifndef _H_STREAM2SOCKET_H_
#define _H_STREAM2SOCKET_H_

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include <configparser/configparser.h>
#include <visionsystem/plugin.h>
#include <vision/image/image.h>
#include <visionsystem/buffer.h>

#include <vision/io/H264Encoder.h>

#include "stream2socketprocess.h"

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
    /* Configparser method */
    void parse_config_line( std::vector<std::string> & line );

private:
    /* Socket related members */
    boost::asio::io_service io_service_;
    boost::thread * io_service_th_;

    /* Raw transfer support */
    bool raw_;

    /* H.264 compression if available */
    bool compress_data_;

    /* Plugin related */
    std::vector<short> ports_;
    std::vector<std::string> server_names_;
    std::vector<std::string> cam_names_;
    std::vector<Stream2SocketProcess *> processes_;
    bool reverse_connection_;
    std::vector<Camera *> cams_;
    bool verbose_;
};

} // namespace visionsystem

PLUGIN(visionsystem::Stream2Socket)

#endif
