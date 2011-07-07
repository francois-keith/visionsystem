#ifndef _H_CAMERASOCKET_H_
#define _H_CAMERASOCKET_H_

#include <configparser/configparser.h>
#include <visionsystem/genericcamera.h>
#include <vision/image/image.h>

#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

namespace visionsystem
{

class CameraSocket : public visionsystem::GenericCamera, public configparser::WithConfigFile
{
public:
    CameraSocket(boost::asio::io_service & io_service);

    ~CameraSocket();

    /* Camera socket specific */
    bool is_ready() { return cam_ready_; }

    void start_cam();

    bool has_data();

    unsigned char * get_data();

    void stop_cam();

    /* Camera methods to implement */
    vision::ImageRef get_size() { return img_size_; }

    bool is_active() { return active_; }

    visionsystem::FrameCoding get_coding() { return img_coding_; }

    float get_fps() { return fps_; }

    std::string get_name() { return name_; }
private:
    /* Configparser method */
    void parse_config_line ( std::vector<std::string> & line );

    /* Async sockets callbacks */
    void handle_receive_from(const boost::system::error_code & error,
                                size_t bytes_recvd);
    void handle_send_to(const boost::system::error_code & error,
                                size_t bytes_send);

public:
    /* Camera related parameters */
    vision::ImageRef img_size_;
    bool active_;
    visionsystem::FrameCoding img_coding_;
    unsigned int fps_; /* read as FPS but stored as time intervall between each frame */
    timeval previous_frame_t_;
    std::string name_;
private:
    /* CameraSocket specific */
    bool cam_ready_;
    std::string server_name_;
    short server_port_;

    /* Socket members */
    boost::asio::io_service & io_service_;
    udp::socket socket_;
    udp::endpoint receiver_endpoint_;
    udp::endpoint sender_endpoint_;
    std::string request_;
    enum { chunk_size_ = 50001 };
    unsigned char chunk_buffer_[chunk_size_];
    uint8_t chunkID_;    

    /* Camera related */
    vision::Image<unsigned char, MONO> * shw_img_;
    vision::Image<unsigned char, MONO> * rcv_img_;
    unsigned int buffersize_;
};

}

#endif
