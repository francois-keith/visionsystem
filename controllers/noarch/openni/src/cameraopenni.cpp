#include "cameraopenni.h"

#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;

namespace visionsystem
{

CameraImageOpenNI::CameraImageOpenNI(Context & context, const std::string & name) : _name(name), _active(true), _frame(0)
{
    XnStatus rc = context.FindExistingNode(XN_NODE_TYPE_IMAGE, _image);
    if (rc != XN_STATUS_OK)
    {
        _active = false;
        return;
    }

    _image.GetMetaData(_imageMD);
    _data = new vision::Image<uint32_t, vision::RGB>(_imageMD.FullXRes(), _imageMD.FullYRes());

    _buffer.clear();
    for( unsigned int i = 0; i < ceil(get_fps()); ++i )
    {
        _buffer.enqueue( new Frame( get_coding(), get_size() ) );
    }
}

CameraImageOpenNI::~CameraImageOpenNI()
{
    delete _data;
    _image.Release();
}

void CameraImageOpenNI::update()
{
    _image.GetMetaData(_imageMD);
    if( _frame != _imageMD.FrameID() )
    {
        _frame = _imageMD.FrameID();
        const XnRGB24Pixel* pImage = _imageMD.RGB24Data();
        for(unsigned int y = 0; y < (*_data).height; ++y)
        {
            for(unsigned int x = 0; x < (*_data).width; ++x, ++pImage)
            {
                  (*_data)(x,y) = *(reinterpret_cast<const unsigned int*>(pImage));
            }
        }
        Frame * vsframe = _buffer.pull();
        std::memcpy( vsframe->_data, _data->raw_data, vsframe->_data_size );
        _buffer.push(vsframe);
    }
}

CameraDepthOpenNI::CameraDepthOpenNI(Context & context, const std::string & name) : _name(name), _active(true), _frame(0), _context(&context)
{
    XnStatus rc = context.FindExistingNode(XN_NODE_TYPE_DEPTH, _depth);
    if (rc != XN_STATUS_OK)
    {
        _active = false;
        return;
    }

    _depth.GetMetaData(_depthMD);
    _data = new vision::Image<uint16_t, vision::DEPTH>(_depthMD.FullXRes(), _depthMD.FullYRes());
    _buffer.clear();
    for( unsigned int i = 0; i < ceil(get_fps()); ++i )
    {
        _buffer.enqueue( new Frame( get_coding(), get_size() ) );
    }
}

CameraDepthOpenNI::~CameraDepthOpenNI()
{
    delete _data;
    _depth.Release();
}

void CameraDepthOpenNI::update()
{
    _depth.GetMetaData(_depthMD);
    if( _frame != _depthMD.FrameID() )
    {
        _frame = _depthMD.FrameID();
        const XnDepthPixel* pDepth = _depthMD.Data();
        for(unsigned int y = 0; y < (*_data).height; ++y)
        {
            for(unsigned int x = 0; x < (*_data).width; ++x, ++pDepth)
            {
                (*_data)(x,y) = *pDepth;
            }
        }
        Frame * vsframe = _buffer.pull();
        std::memcpy( vsframe->_data, _data->raw_data, vsframe->_data_size );
        _buffer.push(vsframe);
    }
}

CameraOpenNI::CameraOpenNI() : _name("Unnamed OpenNI"), _active(false), _xmlpath("config.xml")
{
}

CameraOpenNI::~CameraOpenNI()
{
    delete _image_cam;
    delete _depth_cam;
    _scriptNode.Release();
    _context.Release();
}

bool CameraOpenNI::init_camera()
{
    bfs::path xmlpath(_xmlpath);
    if(!bfs::exists(xmlpath))
    {
        std::cerr << "[OpenNI] " << _xmlpath << " is not a valid path" << std::endl;
        return false;
    }

    XnStatus rc;
    EnumerationErrors errors;
    rc = _context.InitFromXmlFile(_xmlpath.c_str(), _scriptNode, &errors);
    if (rc == XN_STATUS_NO_NODE_PRESENT)
    {
        XnChar strError[1024];
        errors.ToString(strError, 1024);
        std::cout << "[OpenNI] No node present: " << strError << std::endl;
        return false;
    }
    else if (rc != XN_STATUS_OK)
    {

//xnGetStatusString lead to some problem during compilation
//        std::cout << "[OpenNI] Open failed: " << xnGetStatusString(rc) << std::endl;
        std::cout << "[OpenNI] Open failed: "  << std::endl;

        return false;
    }

    {
        std::stringstream ss;
        ss << _name << "-image";
        _image_cam = new CameraImageOpenNI(_context, ss.str());
        if(! _image_cam->is_active() )
        {
            delete _image_cam;
            _image_cam = 0;
        }
    }
    {
        std::stringstream ss;
        ss << _name << "-depth";
        _depth_cam = new CameraDepthOpenNI(_context, ss.str());
        if(! _depth_cam->is_active() )
        {
            delete _depth_cam;
            _depth_cam = 0;
        }
    }
    return true;
}

void CameraOpenNI::update()
{
    _context.WaitNoneUpdateAll();
    if(_image_cam)
    {
        _image_cam->update();
    }
    if(_depth_cam)
    {
        _depth_cam->update();
    }
}

void CameraOpenNI::parse_config_line( std::vector<std::string> & line )
{
    if( fill_member(line, "Name", _name) )
        return;

    if( fill_member(line, "Active", _active) )
        return;

    if( fill_member(line, "XMLPath", _xmlpath) )
        return;
}

} // namespace visionsystem

