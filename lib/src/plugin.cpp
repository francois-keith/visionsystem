#include <visionsystem/plugin.h>

namespace visionsystem {


Plugin::Plugin( VisionSystem* vs, std::string plugin_name, std::string sandbox )  {

    _vscore = vs ;
    _name = plugin_name ;
    _sandbox = sandbox ;

}


Plugin::~Plugin() {

}


std::string Plugin::get_name() {
    return _name ;
}


std::string Plugin::get_sandbox() {
    return _sandbox ;
}


void Plugin::push_frame( Camera* cam, Frame* newfrm, bool skip_frame ) {
    Frame* frm ;
    if(!skip_frame)
    {
        frm = frame_buffers[cam]->pull() ;
        frm->clone ( newfrm ) ;
        frame_buffers[cam]->push(frm) ;
    }
    else
    {
        frm = frame_buffers[cam]->lock_front();
        frm->clone( newfrm );
        frame_buffers[cam]->update_front( frm );
    }
}


std::vector<Camera*> Plugin::get_all_cameras() {
    return _vscore->get_all_cameras() ;
}


Camera* Plugin::get_camera ( std::string cam_name ) {
    return _vscore->get_camera( cam_name ) ;
}

Camera* Plugin::get_default_camera () {
    return _vscore->get_default_camera() ;
}

void Plugin::notify_end_of_camera(Camera * cam)
{
    std::cerr << "[WARNING] Plugin " << get_name() << " received a notification about the end of camera " << cam->get_name() << std::endl;
    std::cerr << "This plugin is not prepared for this, expect the worst..." << std::endl;
}

}
