#include <iostream>
#include <cstdlib>

#include "utils.h"


#define MACROCASE(A)    case A : ret=#A ; break ;

using namespace std ;

std::string mode_2_string ( dc1394video_mode_t mode ) {

        std::string ret ;

        switch ( mode ) {


                  MACROCASE(DC1394_VIDEO_MODE_160x120_YUV444)
            MACROCASE(DC1394_VIDEO_MODE_320x240_YUV422)
            MACROCASE(DC1394_VIDEO_MODE_640x480_YUV411)
            MACROCASE(DC1394_VIDEO_MODE_640x480_YUV422)
            MACROCASE(DC1394_VIDEO_MODE_640x480_RGB8)
            MACROCASE(DC1394_VIDEO_MODE_640x480_MONO8)
            MACROCASE(DC1394_VIDEO_MODE_640x480_MONO16)
            MACROCASE(DC1394_VIDEO_MODE_800x600_YUV422)
            MACROCASE(DC1394_VIDEO_MODE_800x600_RGB8)
            MACROCASE(DC1394_VIDEO_MODE_800x600_MONO8)
            MACROCASE(DC1394_VIDEO_MODE_800x600_MONO16)
            MACROCASE(DC1394_VIDEO_MODE_1024x768_YUV422)
            MACROCASE(DC1394_VIDEO_MODE_1024x768_RGB8)
            MACROCASE(DC1394_VIDEO_MODE_1024x768_MONO8)
            MACROCASE(DC1394_VIDEO_MODE_1024x768_MONO16)
            MACROCASE(DC1394_VIDEO_MODE_1280x960_YUV422)
            MACROCASE(DC1394_VIDEO_MODE_1280x960_RGB8)
            MACROCASE(DC1394_VIDEO_MODE_1280x960_MONO8)
            MACROCASE(DC1394_VIDEO_MODE_1280x960_MONO16)
            MACROCASE(DC1394_VIDEO_MODE_1600x1200_YUV422)
            MACROCASE(DC1394_VIDEO_MODE_1600x1200_RGB8)
            MACROCASE(DC1394_VIDEO_MODE_1600x1200_MONO8)
            MACROCASE(DC1394_VIDEO_MODE_1600x1200_MONO16)
            MACROCASE(DC1394_VIDEO_MODE_EXIF)
            MACROCASE(DC1394_VIDEO_MODE_FORMAT7_0)
            MACROCASE(DC1394_VIDEO_MODE_FORMAT7_1)
            MACROCASE(DC1394_VIDEO_MODE_FORMAT7_2)
            MACROCASE(DC1394_VIDEO_MODE_FORMAT7_3)
            MACROCASE(DC1394_VIDEO_MODE_FORMAT7_4)
            MACROCASE(DC1394_VIDEO_MODE_FORMAT7_5)
            MACROCASE(DC1394_VIDEO_MODE_FORMAT7_6)
            MACROCASE(DC1394_VIDEO_MODE_FORMAT7_7)
            default:
                cerr << "[vision_server] ERROR : Unknown format in mode_2_string" << endl ;
                exit(0) ;
                break ;
        }

        return ret ;
    }



std::string feature_2_string ( dc1394feature_t ft ) {

        std::string ret ;

        switch (ft) {

                MACROCASE(DC1394_FEATURE_BRIGHTNESS)
                MACROCASE(DC1394_FEATURE_EXPOSURE)
                MACROCASE(DC1394_FEATURE_SHARPNESS)
                MACROCASE(DC1394_FEATURE_WHITE_BALANCE)
                MACROCASE(DC1394_FEATURE_HUE)
                MACROCASE(DC1394_FEATURE_SATURATION)
                MACROCASE(DC1394_FEATURE_GAMMA)
                MACROCASE(DC1394_FEATURE_SHUTTER)
                MACROCASE(DC1394_FEATURE_GAIN)
                MACROCASE(DC1394_FEATURE_IRIS)
                MACROCASE(DC1394_FEATURE_FOCUS)
                MACROCASE(DC1394_FEATURE_TEMPERATURE)
                MACROCASE(DC1394_FEATURE_TRIGGER)
                MACROCASE(DC1394_FEATURE_TRIGGER_DELAY)
                MACROCASE(DC1394_FEATURE_WHITE_SHADING)
                MACROCASE(DC1394_FEATURE_FRAME_RATE)
                MACROCASE(DC1394_FEATURE_ZOOM)
                MACROCASE(DC1394_FEATURE_PAN)
                MACROCASE(DC1394_FEATURE_TILT)
                MACROCASE(DC1394_FEATURE_OPTICAL_FILTER)
                MACROCASE(DC1394_FEATURE_CAPTURE_SIZE)
                MACROCASE(DC1394_FEATURE_CAPTURE_QUALITY)
                default:
                    cerr << "[vision_server] ERROR : Unknown feature in feature_2_string" << endl ;
                exit(0) ;
                break ;
        }

        return ret ;
    }



std::string featuremode_2_string ( dc1394feature_mode_t ft ) {

    std::string ret ;

    switch (ft) {
        MACROCASE(DC1394_FEATURE_MODE_MANUAL)
        MACROCASE(DC1394_FEATURE_MODE_AUTO)
        MACROCASE(DC1394_FEATURE_MODE_ONE_PUSH_AUTO)
        default:
            cerr << "[vision_server] ERROR : Unknown mode in mode_2_string" << endl ;
            exit(0) ;
            break ;
    }

    return ret ;
}



