#ifndef _H_CAMERAFILESTREAM_H_
#define _H_CAMERAFILESTREAM_H_

#include <configparser/configparser.h>
#include <visionsystem/genericcamera.h>
#include <vision/image/image.h>
#include <ctime>

#include <vision/win32/windows.h>

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

    float get_fps() { return 1e6/_fps; }

    std::string get_name() { return _name; }

    unsigned int get_frame() { return _frame; }


private:
    /* Configparser method */
    void parse_config_line ( std::vector<std::string> &line ) ;


public:
    /* Camera related parameters */
    vision::ImageRef _img_size;
    bool _active;
    visionsystem::FrameCoding _img_coding;
    unsigned int _fps; /* read as FPS but stored as time intervall between each frame */
    unsigned int _frame;
    timeval _previous_frame_t;
    std::string _name;
    std::string _path;

    /* Filestream specific */
    std::vector<std::string> _bin_files;
    unsigned int _current_frame;
    vision::Image<unsigned char, vision::MONO> * _img_mono;
    vision::Image<uint32_t, vision::RGB> * _img_rgb;
    vision::Image<uint16_t, vision::DEPTH> * _img_depth;

    unsigned int _buffersize;

};

} // namespace visionsystem

#endif
