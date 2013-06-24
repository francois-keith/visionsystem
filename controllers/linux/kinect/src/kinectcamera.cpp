#include "kinectcamera.h"

void depth_cb(freenect_device *dev, void *v_depth, uint32_t timestamp) ;
void rgb_cb(freenect_device *dev, void *rgb, uint32_t timestamp) ;


// **************************
// ***
// *** RgbCamera
// ***
// **************************



RgbCamera::RgbCamera( freenect_context *ctx, int dev_num ) {

    context = ctx ;
    device_num = dev_num ;
    device = NULL ;
    active = true ;

    current_video_format = FREENECT_VIDEO_RGB ;

    std::ostringstream tmp_name;
    tmp_name << "Kinect_" << dev_num << "_rgb" ;
    name = tmp_name.str() ;
}


RgbCamera::~RgbCamera() {

    device = NULL ;
    active = false ;

}


ImageRef RgbCamera::get_size() {

    if ( device == NULL ) {
        cerr << "[kinect] ERROR: Requesting size of unitialized rgb camera" << endl ;
        return ImageRef(0,0) ;
    }

    freenect_frame_mode md = freenect_get_current_video_mode( device ) ;

    uint16_t width = md.width ;
    uint16_t height = md.height ;

    return ImageRef( width,height ) ;
}

FrameCoding RgbCamera::get_coding() {

    if ( device == NULL ) {
        cerr << "[kinect] ERROR: Requesting coding of unitialized rgb camera" << endl ;
        return VS_INVALID ;
    }

    freenect_frame_mode md = freenect_get_current_video_mode( device ) ;
    freenect_video_format fmt = md.video_format ;

    switch ( fmt ) {

        case FREENECT_VIDEO_RGB:

            return VS_RGB24 ;

        case FREENECT_VIDEO_BAYER:
        case FREENECT_VIDEO_IR_8BIT:
        case FREENECT_VIDEO_IR_10BIT:
        case FREENECT_VIDEO_IR_10BIT_PACKED:
        case FREENECT_VIDEO_YUV_RGB:
        case FREENECT_VIDEO_YUV_RAW:
        default:

            cerr << "[kinect] ERROR: Unsupported video mode" << endl ;
            return VS_INVALID ;
    }

    return VS_INVALID ;
}


float RgbCamera::get_fps() {

    if ( device == NULL ) {
        cerr << "[kinect] ERROR: Requesting fps of unitialized rgb camera" << endl ;
        return -1 ;
    }

    freenect_frame_mode md = freenect_get_current_video_mode( device ) ;

    int8_t fps = md.framerate ;

    return (float) fps ;
}


string RgbCamera::get_name() {

    return name ;
}


bool RgbCamera::is_active() {

    return active ;
}

// **************************
// ***
// *** DepthCamera
// ***
// **************************


DepthCamera::DepthCamera( freenect_context *ctx, int dev_num ) {

    context = ctx ;
    device_num = dev_num ;
    device = NULL ;
    active = true ;

    current_depth_format = FREENECT_DEPTH_11BIT ;

    std::ostringstream tmp_name;
    tmp_name << "Kinect_" << dev_num << "_depth" ;
    name = tmp_name.str() ;
}


DepthCamera::~DepthCamera() {

    device = NULL ;
    active = false ;

}


ImageRef DepthCamera::get_size() {

    if ( device == NULL ) {
        cerr << "[kinect] ERROR: Requesting size of unitialized depth camera" << endl ;
        return ImageRef(0,0) ;
    }

    freenect_frame_mode md = freenect_get_current_depth_mode( device ) ;

    uint16_t width = md.width ;
    uint16_t height = md.height ;

    return ImageRef( width,height ) ;
}

FrameCoding DepthCamera::get_coding() {

    if ( device == NULL ) {
        cerr << "[kinect] ERROR: Requesting coding of unitialized depth camera" << endl ;
        return VS_INVALID ;
    }

    freenect_frame_mode md = freenect_get_current_depth_mode( device ) ;
    freenect_video_format fmt = md.video_format ;

    switch ( fmt ) {

        case FREENECT_DEPTH_11BIT:
        case FREENECT_DEPTH_10BIT:

            return VS_DEPTH16 ;

        case FREENECT_DEPTH_11BIT_PACKED:
        case FREENECT_DEPTH_10BIT_PACKED:
        default:

            cerr << "[kinect] ERROR: Unsupported video mode" << endl ;
            return VS_INVALID ;
    }

    return VS_INVALID ;
}

float DepthCamera::get_fps() {

    if ( device == NULL ) {
        cerr << "[kinect] ERROR: Requesting fps of unitialized depth camera" << endl ;
        return -1 ;
    }

    freenect_frame_mode md = freenect_get_current_depth_mode( device ) ;

    int8_t fps = md.framerate ;

    return (float) fps ;
}

string DepthCamera::get_name() {

    return name ;
}


bool DepthCamera::is_active() {

    return active ;
}





// **************************
// ***
// *** KinectCamera
// ***
// **************************


KinectCamera::KinectCamera( freenect_context * ctx, int device_num ) {

    f_ctx = ctx ;
    device_nb = device_num ;

    freenect_angle = 0;
    freenect_led   = LED_RED ;

    rgb_cam   = new RgbCamera ( f_ctx, device_nb ) ;
    depth_cam = new DepthCamera( f_ctx, device_nb ) ;

}


KinectCamera::~KinectCamera() {

    delete rgb_cam ;
    delete depth_cam ;
}


bool KinectCamera::apply_settings() {

    if ( freenect_open_device( f_ctx, &f_dev, device_nb ) < 0 ) {
                cerr << "[kinect] ERROR: Could not open device" << endl ;
        return 0;
    }

    rgb_cam->device = f_dev ;
    depth_cam->device = f_dev ;

    freenect_set_user( f_dev, (void*) this );

    freenect_set_tilt_degs( f_dev,freenect_angle );
    freenect_set_led( f_dev, freenect_led );

    freenect_set_video_mode( f_dev, freenect_find_video_mode( FREENECT_RESOLUTION_MEDIUM, rgb_cam->current_video_format ) ) ;
    freenect_set_depth_mode( f_dev, freenect_find_depth_mode( FREENECT_RESOLUTION_MEDIUM, depth_cam->current_depth_format ) ) ;

    freenect_set_depth_callback( f_dev, depth_cb );
    freenect_set_video_callback( f_dev, rgb_cb );

    // Fill camera buffers

    rgb_cam->_buffer.clear() ;
    depth_cam->_buffer.clear() ;

    for ( int i=0; i<50 ; i++ ){
        rgb_cam->_buffer.enqueue   ( new Frame( rgb_cam->get_coding(), rgb_cam->get_size() ) ) ;
        depth_cam->_buffer.enqueue ( new Frame( depth_cam->get_coding(), depth_cam->get_size() ) ) ;
    }

    return 1 ;
}

void KinectCamera::start_cam() {

    freenect_start_video(f_dev);
    freenect_start_depth(f_dev);
}

void KinectCamera::stop_cam() {

    freenect_set_led( f_dev, LED_BLINK_GREEN );

    cerr << "[kinect] Shutting down streams..." << endl  ;

    freenect_stop_depth ( f_dev ) ;
    freenect_stop_video ( f_dev ) ;

    freenect_close_device ( f_dev ) ;

}


void KinectCamera::parse_config_line ( vector<string> &line ) {

    //FIXME

}


// *********************************
// ***
// *** CALLBACKS
// ***
// *********************************


void depth_cb(freenect_device *dev, void *v_depth, uint32_t timestamp)
{
    KinectCamera* camera = (KinectCamera*) freenect_get_user( dev );
    if (camera->depth_cam->is_active() ) {
        Frame* frm = camera->depth_cam->_buffer.pull() ;
        memcpy( frm->_data, v_depth, frm->_data_size ) ;
        camera->depth_cam->_buffer.push( frm ) ;
    }
}


void rgb_cb(freenect_device *dev, void *rgb, uint32_t timestamp)
{
    KinectCamera* camera = (KinectCamera*) freenect_get_user( dev );
    if ( camera->rgb_cam->is_active() ) {
        Frame* frm = camera->rgb_cam->_buffer.pull() ;
        memcpy( frm->_data, rgb, frm->_data_size ) ;
        camera->rgb_cam->_buffer.push( frm ) ;
    }
}

