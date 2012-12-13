#include "controlleropenni.h"

#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;

namespace visionsystem
{

ControllerOpenNI::ControllerOpenNI( visionsystem::VisionSystem * vs, std::string sandbox )
: Controller(vs, "controllerOpenNI", sandbox), _cams(0)
{
}

ControllerOpenNI::~ControllerOpenNI()
{
    for(size_t i = 0; i < _cams.size(); ++i)
    {
        delete _cams[i];
    }
}

bool ControllerOpenNI::pre_fct( std::vector< GenericCamera *> & cams)
{

    bfs::path sandbox( get_sandbox() );
    if( bfs::is_directory(sandbox) )
    {
        std::vector<bfs::path> ls_path;
        copy(bfs::directory_iterator(sandbox), bfs::directory_iterator(), back_inserter(ls_path));
        for(std::vector< bfs::path >::const_iterator it = ls_path.begin(); it != ls_path.end(); ++it)
        {
            if( bfs::is_regular_file(*it) && bfs::extension(*it) == ".conf" )
            {
                CameraOpenNI * cam = new CameraOpenNI();
                cam->read_config_file( (*it).string().c_str() );
                if(cam->is_active())
                {
                    cam->init_camera();
                }
                _cams.push_back(cam);
                if(cam->get_image_camera())
                {
                    cams.push_back(cam->get_image_camera());
                }
                if(cam->get_depth_camera())
                {
                    cams.push_back(cam->get_depth_camera());
                }
            }
        }
    }
    return true;

}

void ControllerOpenNI::preloop_fct()
{
}

void ControllerOpenNI::loop_fct()
{
    for(size_t i = 0; i < _cams.size(); ++i)
    {
        if(_cams[i]->is_active())
        {
            _cams[i]->update();
        }
    }
}

bool ControllerOpenNI::post_fct()
{
    return true;
}

} // namespace visionsystem

