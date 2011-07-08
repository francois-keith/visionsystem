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

	freenect_set_log_level( f_ctx, FREENECT_LOG_DEBUG );

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
		

		

	}

/*
	iif (freenect_open_device(f_ctx, &f_dev, user_device_number) < 0) {
		printf("Could not open device\n");
		return 1;
	}

	res = pthread_create(&freenect_thread, NULL, freenect_threadfunc, NULL);
*/
	return 1 ;

}


void KinectController::preloop_fct()
{

}


void KinectController::loop_fct()
{

}


bool KinectController::post_fct()
{
	for ( int i=0; i< nr_devices; i++ ) {
		delete _devices[i] ;
		_devices[i] = NULL ;
	}

	_devices.clear() ;

	freenect_shutdown(f_ctx);
	
	f_ctx = NULL ;
	nr_devices = 0 ;

}


