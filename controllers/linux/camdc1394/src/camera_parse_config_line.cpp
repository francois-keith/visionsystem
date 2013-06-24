#include "camera1394.h"

void Camera1394::parse_config_line ( vector<string> &line ) {

	if ( fill_member( line, "Name", _name ) )
		return ;

	if ( fill_member( line, "Active", _active ) )
		return ;

	if ( fill_member( line, "Bayer", bayer ) )
		return ;

	if (line[0] == "Mode") {
		if (line.size() == 2 )  {

			if ( line[1]=="DC1394_VIDEO_MODE_640x480_YUV411")  { mode = DC1394_VIDEO_MODE_640x480_YUV411 ; return ; }

			if ( line[1]=="DC1394_VIDEO_MODE_320x240_YUV422")  { mode = DC1394_VIDEO_MODE_320x240_YUV422 ; return ; }
			if ( line[1]=="DC1394_VIDEO_MODE_640x480_YUV422")  { mode = DC1394_VIDEO_MODE_640x480_YUV422 ; return ; }
			if ( line[1]=="DC1394_VIDEO_MODE_800x600_YUV422")  { mode = DC1394_VIDEO_MODE_800x600_YUV422 ; return ; }
			if ( line[1]=="DC1394_VIDEO_MODE_1024x768_YUV422") { mode = DC1394_VIDEO_MODE_1024x768_YUV422 ; return ; }
			if ( line[1]=="DC1394_VIDEO_MODE_1280x960_YUV422") { mode = DC1394_VIDEO_MODE_1280x960_YUV422 ; return ; }
			if ( line[1]=="DC1394_VIDEO_MODE_1600x1200_YUV422"){ mode = DC1394_VIDEO_MODE_1600x1200_YUV422 ; return ; }

			if ( line[1]=="DC1394_VIDEO_MODE_160x120_YUV444")  { mode = DC1394_VIDEO_MODE_160x120_YUV444 ; return ; }

			if ( line[1]=="DC1394_VIDEO_MODE_640x480_MONO8")   { mode = DC1394_VIDEO_MODE_640x480_MONO8 ; return ; }
			if ( line[1]=="DC1394_VIDEO_MODE_800x600_MONO8")   { mode = DC1394_VIDEO_MODE_800x600_MONO8 ; return ; }
			if ( line[1]=="DC1394_VIDEO_MODE_1024x768_MONO8")  { mode = DC1394_VIDEO_MODE_1024x768_MONO8 ; return ; }
			if ( line[1]=="DC1394_VIDEO_MODE_1280x960_MONO8")  { mode = DC1394_VIDEO_MODE_1280x960_MONO8 ; return ; }
			if ( line[1]=="DC1394_VIDEO_MODE_1600x1200_MONO8") { mode = DC1394_VIDEO_MODE_1600x1200_MONO8 ; return ; }

			if ( line[1]=="DC1394_VIDEO_MODE_640x480_MONO16")  { mode = DC1394_VIDEO_MODE_640x480_MONO16 ; return ; }
			if ( line[1]=="DC1394_VIDEO_MODE_800x600_MONO16")  { mode = DC1394_VIDEO_MODE_800x600_MONO16 ; return ; }
			if ( line[1]=="DC1394_VIDEO_MODE_1024x768_MONO16") { mode = DC1394_VIDEO_MODE_1024x768_MONO16 ; return ; }
			if ( line[1]=="DC1394_VIDEO_MODE_1280x960_MONO16") { mode = DC1394_VIDEO_MODE_1280x960_MONO16 ; return ; }
			if ( line[1]=="DC1394_VIDEO_MODE_1600x1200_MONO16"){ mode = DC1394_VIDEO_MODE_1600x1200_MONO16 ; return ; }

			if ( line[1]=="DC1394_VIDEO_MODE_640x480_RGB8" )   { mode = DC1394_VIDEO_MODE_640x480_RGB8 ; return ; }
			if ( line[1]=="DC1394_VIDEO_MODE_800x600_RGB8" )   { mode = DC1394_VIDEO_MODE_800x600_RGB8 ; return ; }
			if ( line[1]=="DC1394_VIDEO_MODE_1024x768_RGB8" )  { mode = DC1394_VIDEO_MODE_1024x768_RGB8 ; return ; }
			if ( line[1]=="DC1394_VIDEO_MODE_1280x960_RGB8" )  { mode = DC1394_VIDEO_MODE_1280x960_RGB8 ; return ; }
			if ( line[1]=="DC1394_VIDEO_MODE_1600x1200_RGB8" ) { mode = DC1394_VIDEO_MODE_1600x1200_RGB8 ; return ; }
		}
		throw ( string( "Unknown [Mode] ... aborting" ) ) ;
	}

	if (line[0] == "Framerate") {
		if ( line.size() == 2 ) {
			if (line[1]=="DC1394_FRAMERATE_1_875") { framerate = DC1394_FRAMERATE_1_875 ; return ; }
			if (line[1]=="DC1394_FRAMERATE_3_75")  { framerate = DC1394_FRAMERATE_3_75 ; return ; }
			if (line[1]=="DC1394_FRAMERATE_7_5")   { framerate = DC1394_FRAMERATE_7_5 ; return ; }
			if (line[1]=="DC1394_FRAMERATE_15")    { framerate = DC1394_FRAMERATE_15 ; return ; }
			if (line[1]=="DC1394_FRAMERATE_30")    { framerate = DC1394_FRAMERATE_30 ; return ; }
			if (line[1]=="DC1394_FRAMERATE_60")    { framerate = DC1394_FRAMERATE_60 ; return ; }
			if (line[1]=="DC1394_FRAMERATE_120")   { framerate = DC1394_FRAMERATE_120 ; return ; }
			if (line[1]=="DC1394_FRAMERATE_240")   { framerate = DC1394_FRAMERATE_240 ; return ; }
	  	}
		throw ( string("Unknown [Framerate] ... aborting") ) ;
	}

	if (line[0] == "Speed") {
		if ( line.size() == 2 ) {
			if (line[1]=="DC1394_ISO_SPEED_100") { speed = DC1394_ISO_SPEED_100 ; return ; }
			if (line[1]=="DC1394_ISO_SPEED_200") { speed = DC1394_ISO_SPEED_200 ; return ; }
			if (line[1]=="DC1394_ISO_SPEED_400") { speed = DC1394_ISO_SPEED_400 ; return ; }
			if (line[1]=="DC1394_ISO_SPEED_800") { speed = DC1394_ISO_SPEED_800 ; return ; }
			if (line[1]=="DC1394_ISO_SPEED_1600") { speed = DC1394_ISO_SPEED_1600 ; return ; }
			if (line[1]=="DC1394_ISO_SPEED_3200") { speed = DC1394_ISO_SPEED_3200 ; return ; }
		}
		throw ( string("Unknown [Speed] ... aborting") ) ;
	}

	if (line[0] == "BayerCoding") {
		if ( line.size() == 2 ) {
			if (line[1]=="DC1394_COLOR_FILTER_RGGB") { bayer_coding = DC1394_COLOR_FILTER_RGGB ; return ; }
			if (line[1]=="DC1394_COLOR_FILTER_GBRG") { bayer_coding = DC1394_COLOR_FILTER_GBRG ; return ; }
			if (line[1]=="DC1394_COLOR_FILTER_GRBG") { bayer_coding = DC1394_COLOR_FILTER_GRBG ; return ; }
			if (line[1]=="DC1394_COLOR_FILTER_BGGR") { bayer_coding = DC1394_COLOR_FILTER_BGGR ; return ; }
		}
		throw ( string("Unknown [BayerCoding] ... aborting") ) ;
	}

	if (line[0] == "BayerMethod") {
		if ( line.size() == 2 ) {
			if (line[1]=="DC1394_BAYER_METHOD_NEAREST")   { bayer_method = DC1394_BAYER_METHOD_NEAREST ; return ; }
			if (line[1]=="DC1394_BAYER_METHOD_SIMPLE")    { bayer_method = DC1394_BAYER_METHOD_SIMPLE ; return ; }
			if (line[1]=="DC1394_BAYER_METHOD_BILINEAR")  { bayer_method = DC1394_BAYER_METHOD_BILINEAR ; return ; }
			if (line[1]=="DC1394_BAYER_METHOD_HQLINEAR")  { bayer_method = DC1394_BAYER_METHOD_HQLINEAR ; return ; }
			if (line[1]=="DC1394_BAYER_METHOD_DOWNSAMPLE"){ bayer_method = DC1394_BAYER_METHOD_DOWNSAMPLE ; return ; }
			if (line[1]=="DC1394_BAYER_METHOD_EDGESENSE") { bayer_method = DC1394_BAYER_METHOD_EDGESENSE ; return ; }
			if (line[1]=="DC1394_BAYER_METHOD_VNG")       { bayer_method = DC1394_BAYER_METHOD_VNG ; return ; }
			if (line[1]=="DC1394_BAYER_METHOD_AHD")       { bayer_method = DC1394_BAYER_METHOD_AHD ; return ; }
		}
		throw ( string ( "Unknown [BayerMethod] ... aborting" ) ) ;
	}


	if ( line[0]=="Feature" ) {
		line.erase(line.begin()) ;
		features.push_back(line) ;
		return ;
	}

}
