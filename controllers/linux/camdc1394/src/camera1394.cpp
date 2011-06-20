#include "camera1394.h"

Camera1394::Camera1394( dc1394_t* d , uint64_t gid )
{
	char tmp[255] ;
	
	handler = d ;
	_gid = gid ;
 	
	sprintf(tmp, "%ld", gid ) ;
	_name = string ( tmp ) ;

	_active = 1 ;

	mode  = 	DC1394_VIDEO_MODE_640x480_MONO8 ;
	speed = 	DC1394_ISO_SPEED_400 ; 
	framerate = 	DC1394_FRAMERATE_30 ;
	flags =		DC1394_CAPTURE_FLAGS_DEFAULT ;
	
	cam = dc1394_camera_new ( handler, gid ) ;	

	if (!cam) {
		cerr << "[vision_server] Could not initialise camera with gid " << gid << endl ;
		cerr << "[vision_server] Aborting." << endl ;
		exit(1) ;
	}
	 
}

Camera1394::~Camera1394() {

}


ImageRef Camera1394::get_size() {
	// FIXME
}


bool Camera1394::is_active() {
	return _active ;
}


FrameCoding Camera1394::get_coding() {
	// FIXME
}


float  Camera1394::get_fps() {
	// FIXME
}


string Camera1394::get_name() {
	return _name ;
}


void Camera1394::parse_config_line( vector<string> &line ) {
          
	if ( fill_member( line, "Name", _name ) ) 
		return ;

	if ( fill_member( line, "Active", _active ) ) 
		return ;


}
