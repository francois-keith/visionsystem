#include <visionsystem/plugin.h>

namespace visionsystem {


Plugin::Plugin( VisionSystem* vs, std::string plugin_name, std::string sandbox )  {

	_vscore = vs ;
	_name = plugin_name ;
	_sandbox = sandbox ;

}


std::string Plugin::get_name() {
	return _name ;
}


std::string Plugin::get_sandbox() {
	return _sandbox ;
}


void Plugin::push_frame( Camera* cam, Frame* newfrm ) {
	Frame* frm ;
	frm = frame_buffers[cam]->pull() ;
	frm->clone ( newfrm ) ;
	frame_buffers[cam]->push(frm) ;
}


std::vector<Camera*> Plugin::get_all_cameras() {
	return _vscore->get_all_cameras() ;
}


Camera* Plugin::get_camera ( std::string cam_name ) {
	return _vscore->get_camera( cam_name ) ;
}


}
