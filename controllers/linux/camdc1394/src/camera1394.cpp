#include "camera1394.h"

#include <iostream>

Camera1394::Camera1394( dc1394_t* d , uint64_t gid )
{
    _frame = 0;
	handler = d ;
	_gid = gid ;
 	
    {
    std::stringstream ss;
    ss << gid;
	_name = ss.str() ;
    }

	_active = 1 ;

        bayer = 0 ;

	mode  = 	DC1394_VIDEO_MODE_640x480_MONO8 ;
	speed = 	DC1394_ISO_SPEED_400 ; 
	framerate = 	DC1394_FRAMERATE_30 ;
	flags =		DC1394_CAPTURE_FLAGS_DEFAULT ;
	
	_buffersize = 100 ;	
	
	cam = dc1394_camera_new ( handler, gid ) ;	

	if (!cam) {
		std::cerr << "[vision_server] Could not initialise camera with gid " << gid << std::endl ;
		std::cerr << "[vision_server] Aborting." << std::endl ;
		exit(1) ;
	}
	 
}

Camera1394::~Camera1394() {
    dc1394_camera_free(cam);
}


ImageRef Camera1394::get_size() {
	switch (mode)
	{
		case DC1394_VIDEO_MODE_160x120_YUV444:
			return ImageRef(160,120) ;
			break ;

		case DC1394_VIDEO_MODE_320x240_YUV422:
			return ImageRef(320,240) ;
			break ;

		case DC1394_VIDEO_MODE_640x480_YUV411:
		case DC1394_VIDEO_MODE_640x480_YUV422:
		case DC1394_VIDEO_MODE_640x480_MONO8:
		case DC1394_VIDEO_MODE_640x480_MONO16:
		case DC1394_VIDEO_MODE_640x480_RGB8:
			return ImageRef(640,480) ;
			break ;
		
		case DC1394_VIDEO_MODE_800x600_YUV422:
		case DC1394_VIDEO_MODE_800x600_MONO8:
		case DC1394_VIDEO_MODE_800x600_MONO16:
		case DC1394_VIDEO_MODE_800x600_RGB8:
			return ImageRef(800,600) ;
			break ;

		case DC1394_VIDEO_MODE_1024x768_YUV422:
		case DC1394_VIDEO_MODE_1024x768_MONO8:
		case DC1394_VIDEO_MODE_1024x768_MONO16:
		case DC1394_VIDEO_MODE_1024x768_RGB8:
			return ImageRef(1027,768) ;
			break ;

		case DC1394_VIDEO_MODE_1280x960_YUV422:
		case DC1394_VIDEO_MODE_1280x960_MONO8:
		case DC1394_VIDEO_MODE_1280x960_MONO16:
		case DC1394_VIDEO_MODE_1280x960_RGB8:
			return ImageRef(1280,960) ;
			break ;
		
		case DC1394_VIDEO_MODE_1600x1200_YUV422:
		case DC1394_VIDEO_MODE_1600x1200_MONO8:
		case DC1394_VIDEO_MODE_1600x1200_MONO16:
		case DC1394_VIDEO_MODE_1600x1200_RGB8:
			return ImageRef(1600,1200) ;
			break ;
        default:
            std::cerr << "[cam_dc1394] Unhandled mode: cannot set image resolution" << std::endl;
            break;
	}

	return ImageRef(1,1) ;

}


bool Camera1394::is_active() {
	return _active ;
}


FrameCoding Camera1394::get_coding() {

if ( bayer == 0 ) {

// If there is no Bayerisation/DeBayerisation ... it's easy

	switch (mode)
	{
		case DC1394_VIDEO_MODE_160x120_YUV444:
			return VS_YUV444 ;
			break ;

		case DC1394_VIDEO_MODE_320x240_YUV422:
		case DC1394_VIDEO_MODE_640x480_YUV422:
		case DC1394_VIDEO_MODE_800x600_YUV422:
		case DC1394_VIDEO_MODE_1024x768_YUV422:
		case DC1394_VIDEO_MODE_1280x960_YUV422:
		case DC1394_VIDEO_MODE_1600x1200_YUV422:
			return VS_YUV422_UYVY ;				// FIXME verifier l'ordre des bytes
			break ;

		case DC1394_VIDEO_MODE_640x480_YUV411:
			return VS_YUV411 ;
			break ;
		
		case DC1394_VIDEO_MODE_640x480_MONO8:
		case DC1394_VIDEO_MODE_800x600_MONO8:
		case DC1394_VIDEO_MODE_1024x768_MONO8:
		case DC1394_VIDEO_MODE_1280x960_MONO8:
		case DC1394_VIDEO_MODE_1600x1200_MONO8:
			return VS_MONO8 ;
			break ;


		case DC1394_VIDEO_MODE_640x480_MONO16:
		case DC1394_VIDEO_MODE_800x600_MONO16:
		case DC1394_VIDEO_MODE_1024x768_MONO16:
		case DC1394_VIDEO_MODE_1280x960_MONO16:
		case DC1394_VIDEO_MODE_1600x1200_MONO16:
			return VS_MONO16 ;
			break ;


		case DC1394_VIDEO_MODE_640x480_RGB8:
		case DC1394_VIDEO_MODE_800x600_RGB8:
		case DC1394_VIDEO_MODE_1024x768_RGB8:
		case DC1394_VIDEO_MODE_1280x960_RGB8:
		case DC1394_VIDEO_MODE_1600x1200_RGB8:
			return VS_RGB24 ;
			break ;
        default:
            break;
	}
	
} else {

// else ... it's less easy :)

	switch ( mode ) 
	{
		case DC1394_VIDEO_MODE_640x480_MONO8:
		case DC1394_VIDEO_MODE_800x600_MONO8:
		case DC1394_VIDEO_MODE_1024x768_MONO8:
		case DC1394_VIDEO_MODE_1280x960_MONO8:
		case DC1394_VIDEO_MODE_1600x1200_MONO8:
			return VS_RGB24 ;
			break ;
        default:
            break;
	}
}


	throw("Unsupported camera mode" ) ;

}


float Camera1394::get_fps() {
	switch ( framerate) {
		case DC1394_FRAMERATE_1_875 :
			return 1.875 ;
			break ;
		case DC1394_FRAMERATE_3_75 :
			return 3.75 ;
			break ;
		case DC1394_FRAMERATE_7_5 :
			return 7.5 ;
			break ;
		case DC1394_FRAMERATE_15 :
			return 15 ;
			break ;
		case DC1394_FRAMERATE_30 :
			return 30 ;
			break ;
		case DC1394_FRAMERATE_60 :
			return 60 ;
			break ;
		case DC1394_FRAMERATE_120 :
			return 120 ;
			break ;
		case DC1394_FRAMERATE_240 :
			return 240 ;
			break ;
        default:
            std::cerr << "[cam_dc1394] Unhandled framerate" << std::endl;
            break;
	}

	return 0 ;
}


string Camera1394::get_name() {
	return _name ;
}

unsigned int Camera1394::get_frame()
{
    return _frame;
}

void Camera1394::increase_frame()
{
    _frame++;
}

dc1394camera_t* Camera1394::get_cam() {
	return cam ;
}

bool Camera1394::stop_cam() {

	dc1394error_t err ;

	err = dc1394_video_set_transmission( cam, DC1394_OFF ) ;
	err = dc1394_capture_stop ( cam ) ;

	return true ;
}



