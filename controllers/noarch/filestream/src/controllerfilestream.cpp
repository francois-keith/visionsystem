#include "controllerfilestream.h"

#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;

namespace visionsystem
{

ControllerFilestream::ControllerFilestream( visionsystem::VisionSystem * vs, std::string sandbox )
: Controller(vs, "controllerfs", sandbox), _cams(0)
{
}

ControllerFilestream::~ControllerFilestream()
{
}

bool ControllerFilestream::pre_fct( std::vector< GenericCamera *> & cams)
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
                CameraFilestream * cam = new CameraFilestream();
                cam->read_config_file( (*it).string().c_str() );
                if(cam->is_active())
                {
                    cam->init_camera();
                }
                cams.push_back(cam);
                _cams.push_back(cam);
            }
        }
    }
    return true;

}

void ControllerFilestream::preloop_fct()
{
}

void ControllerFilestream::loop_fct()
{
    for(size_t i = 0; i < _cams.size(); ++i)
    {
        if(_cams[i]->is_active() && _cams[i]->has_data())
        {
            Frame * vsframe = _cams[i]->_buffer.pull();
            std::memcpy( vsframe->_data, _cams[i]->get_data(), vsframe->_data_size );
            _cams[i]->_buffer.push(vsframe);
        }
    }
}

bool ControllerFilestream::post_fct()
{
    return true;
}

} // namespace visionsystem

