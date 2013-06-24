#include "controller.h"

#define VERBOSE 1

KinectController::KinectController( VisionSystem *vs, string sandbox )
:Controller( vs, "kinect", sandbox ), _devices(0)
{
    f_ctx = NULL ;
    nr_devices = 0 ;
}


KinectController::~KinectController()
{

}


bool KinectController::pre_fct( vector< GenericCamera*> &cams )
{

    if ( freenect_init( &f_ctx, NULL) < 0 ) {

        cerr << "[kinect] ERROR: freenect_init() failed." << endl ;
        return 0 ;

    }

    freenect_set_log_level( f_ctx, FREENECT_LOG_NOTICE );

    nr_devices = freenect_num_devices (f_ctx);

    #if VERBOSE
        cout << "[kinect] Number of devices found: " << nr_devices << endl ;
    #endif

    if ( nr_devices < 1 ) {

        #if VERBOSE
            cout << "[kinect] No devices found. " << endl ;
        #endif

        return 1;
    }


    for ( int i=0; i<nr_devices; i++ ) {

        KinectCamera* cam = new KinectCamera( f_ctx, i ) ;

        // std::ostringstream capa_filename;
        // capa_filename << get_sandbox() << "/" << i << ".capa" ;
        // cam->save_capa( capa_filename.str() )  ;

        std::ostringstream conf_filename;
        conf_filename << get_sandbox() << "/" << i << ".conf" ;

        try {
            cam->read_config_file ( conf_filename.str().c_str() ) ;

        } catch ( string msg ) {
            std::cerr << "[kinect] WARNING : Unable to open config file : " << conf_filename.str() <<std::endl ;
            std::cerr << "[kinect] Error message : " << msg << std::endl ;
            std::cerr << "[kinect] trying with default parameters ... " <<std::endl ;
        }

        if ( !cam->apply_settings() ) {
            std::cerr << "[kinect] ERROR : Could not apply settings to device " << i <<std::endl ;
            return false ;
        }

        cams.push_back( (GenericCamera*) cam->depth_cam ) ;
        cams.push_back( (GenericCamera*) cam->rgb_cam ) ;

        _devices.push_back(cam) ;

    }

    return 1 ;

}


void KinectController::preloop_fct()
{
        for ( int i=0; i< nr_devices; i++ )
            _devices[i]->start_cam() ;
}


void KinectController::loop_fct()
{
    if ( nr_devices > 0 )
        freenect_process_events(f_ctx) ;
}


bool KinectController::post_fct()
{

    for ( int i=0; i< nr_devices; i++ )
        _devices[i]->stop_cam() ;

    for ( int i=0; i< nr_devices; i++ ) {
        delete _devices[i] ;
        _devices[i] = NULL ;
    }

    _devices.clear() ;

    freenect_shutdown(f_ctx);

    f_ctx = NULL ;
    nr_devices = 0 ;

    return 1 ;
}


