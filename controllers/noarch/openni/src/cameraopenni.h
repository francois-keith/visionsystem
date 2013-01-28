#ifndef _H_CAMERAOPENNI_H_
#define _H_CAMERAOPENNI_H_

#include <configparser/configparser.h>
#include <visionsystem/genericcamera.h>
#include <vision/image/image.h>

#include <XnOS.h>
#include <XnCppWrapper.h>

using namespace xn;

namespace visionsystem
{

class CameraImageOpenNI : public visionsystem::GenericCamera
{
public:
    CameraImageOpenNI(Context & context, const std::string & name);

    ~CameraImageOpenNI();

    void update();

    /* Camera methods to implement */
    vision::ImageRef get_size() { return vision::ImageRef(_imageMD.FullXRes(), _imageMD.FullYRes()); }
    
    bool is_active() { return _active; }
    
    visionsystem::FrameCoding get_coding() { return VS_RGB32; }
    
    float get_fps() { return _imageMD.FPS(); }
    
    std::string get_name() { return _name; }
    
    unsigned int get_frame() { return _imageMD.FrameID(); }
private:
    std::string _name;
    bool _active;
    unsigned int _frame;
    ImageGenerator _image;
    ImageMetaData  _imageMD;
    vision::Image<uint32_t, vision::RGB> * _data;
};

class CameraDepthOpenNI : public visionsystem::GenericCamera
{
public:
    CameraDepthOpenNI(Context & context, const std::string & name);

    ~CameraDepthOpenNI();

    void update();

    /* Camera methods to implement */
    vision::ImageRef get_size() { return vision::ImageRef(_depthMD.FullXRes(), _depthMD.FullYRes()); }
    
    bool is_active() { return _active; }
    
    visionsystem::FrameCoding get_coding() { return VS_DEPTH16; }
    
    float get_fps() { return _depthMD.FPS(); }
    
    std::string get_name() { return _name; }
    
    unsigned int get_frame() { return _depthMD.FrameID(); }

    DepthGenerator* get_DepthGenerator() { return &_depth; }

    DepthMetaData* get_DepthMetaData() { return &_depthMD; }

    Context* get_Context() { return _context; }


private:
    std::string _name;
    bool _active;
    unsigned int _frame;
    DepthGenerator _depth;
    DepthMetaData  _depthMD;
    vision::Image<uint16_t, vision::DEPTH> * _data;
    Context* _context;

};

class CameraOpenNI : public configparser::WithConfigFile
{
public:
    /* Generic constructor */
    CameraOpenNI();

    ~CameraOpenNI();

    /* Create the buffer once the camera has been configured */
    bool init_camera();

    void update();

    visionsystem::GenericCamera * get_image_camera() { return _image_cam; }
    
    visionsystem::GenericCamera * get_depth_camera() { return _depth_cam; }

    bool is_active() { return _active; }

private:
    /* Configparser method */
    void parse_config_line ( std::vector<std::string> &line ) ;

public:
    /* Camera related parameters */
    bool _active;
    std::string _name;
    std::string _xmlpath;

    /* OpenNI specific */
    Context _context;
    ScriptNode _scriptNode;

    CameraImageOpenNI * _image_cam;
    CameraDepthOpenNI * _depth_cam;
};

} // namespace visionsystem

#endif
