#ifndef _H_STREAM2SOCKETPROCESS_H_
#define _H_STREAM2SOCKETPROCESS_H_

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include <configparser/configparser.h>
#include <vision/image/image.h>

#include <vision/io/H264Encoder.h>

using boost::asio::ip::udp;

namespace visionsystem
{

class Stream2SocketProcess
{
public:
    Stream2SocketProcess( boost::asio::io_service & io_service, short port, unsigned int active_cam = 0, Camera * cam = 0,
                            bool compress = true, bool raw = false, bool reverse_connection = false, const std::string & server_name = "localhost",
                            bool verbose = false);
    ~Stream2SocketProcess();

    void Initialize(vision::ImageRef size, float fps);

    void SendImage( vision::Image<uint32_t, vision::RGB> & img );

public:
    /* Socket callbacks */
    void handle_receive_from(const boost::system::error_code & error,
                                size_t bytes_recvd);
    void handle_send_to(const boost::system::error_code & error,
                                size_t bytes_send, size_t id);
public:
    /* Socket related members */
    udp::socket * socket_;
    std::string server_name_;
    short port_;
    udp::endpoint sender_endpoint_;
    std::vector<udp::endpoint> receivers_endpoint_;
    /* client request */
    enum { max_request_ = 256 };
    char* client_data_;
    /* send buffer: chunk id + 50k (max) chunk of data */
    enum { send_size_ = 32769 };
    unsigned char * send_buffer_;
    /* Protocol related */
    std::vector<uint8_t> chunkIDs_;

    /* H.264 compression if available */
    vision::H264Encoder * encoder_;

    /* Plugin related */
    bool ready_;
    unsigned int active_cam_;
    Camera * cam_;
    vision::Image<uint32_t, vision::RGB> * send_img_;
    unsigned char * send_img_raw_data_;
    unsigned int send_img_data_size_;
    bool img_lock_;
    bool next_cam_;
    bool request_cam_;
    std::string request_name_;
    bool verbose_;
    bool compress_;
    bool raw_;
    bool reverse_;
};

} //namespace visionsystem

#endif
