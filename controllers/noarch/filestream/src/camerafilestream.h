#ifndef _H_CAMERAFILESTREAM_H_
#define _H_CAMERAFILESTREAM_H_

#include <configparser/configparser.h>
#include <visionsystem/genericcamera.h>
#include <vision/image/image.h>
#include <ctime>

namespace visionsystem
{

class CameraFilestream : public visionsystem::GenericCamera, public configparser::WithConfigFile
{
public:
    /* Generic constructor */
    CameraFilestream();

    ~CameraFilestream();

    /* Filestream specifics methods */

    /* Create the buffer once the camera has been configured */
    bool init_camera();

    /* Should send new frame */
    bool has_data();

    /* Get current image as an unsigned char * buffer */
    unsigned char * get_data();

    /* Camera methods to implement */
    vision::ImageRef get_size() { return _img_size; }

    bool is_active() { return _active; }

    visionsystem::FrameCoding get_coding() { return _img_coding; }

    float get_fps() { return _fps; }

    std::string get_name() { return _name; }


private:
    /* Configparser method */
    void parse_config_line ( std::vector<std::string> &line ) ;


public:
    /* Camera related parameters */
    vision::ImageRef _img_size;
    bool _active;
    visionsystem::FrameCoding _img_coding;
    unsigned int _fps; /* read as FPS but stored as time intervall between each frame */
    timeval _previous_frame_t;
    std::string _name;

    /* Filestream specific */
    std::vector<std::string> _bin_files;
    unsigned int _current_frame;
    vision::Image<unsigned char, MONO> * _img_mono;
    vision::Image<uint32_t, RGB> * _img_rgb;

    unsigned int _buffersize;

};

} // namespace visionsystem

#endif
