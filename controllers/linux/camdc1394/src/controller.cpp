#include "controller.h"

Controller1394::Controller1394( VisionSystem *vs, std::string sandbox )
:Controller( vs, "camdc1394", sandbox) {

}


Controller1394::~Controller1394() {

}


bool Controller1394::pre_fct( vector< GenericCamera* > &cams ) {
	
	d1394_ = dc1394_new() ;

	if (!d1394_) {

		cerr << "[camdc1394] WARNING : Could not initialize DC1394 System !" << endl ;
		d1394_ = NULL ;
		return false  ;

	} else {
		
		#ifdef _DEBUG
			cout << "[camdc1394] Initializing DC1394 system " << endl ; 
		#endif

		dc1394camera_list_t * list ;
		dc1394error_t	err ;

		err = dc1394_camera_enumerate( d1394_, &list ) ;

		if ( list->num == 0 ) {

			#ifdef _DEBUG
				cout << "[camdc1394]  No 1394 camera found." << endl ;
			#endif
			return true ;

		} else {

			for ( int i=0; i<list->num; i++ ) {
				
				#ifdef _DEBUG
					cout <<"[camdc1394] Found camera " << list->ids[i].guid << endl ;					
				#endif

				Camera1394* cam = new Camera1394( d1394_ , list->ids[i].guid ) ;

				std::ostringstream capa_filename;
				capa_filename << get_sandbox() << "/" << list->ids[i].guid << ".capa" ;  
				cam->save_capa( capa_filename.str() )  ;
				
				std::ostringstream conf_filename;
				conf_filename << get_sandbox() << "/" << list->ids[i].guid << ".conf" ;  
				try {
					cam->read_config_file ( conf_filename.str().c_str() ) ;
				} catch ( string msg ) {
					cerr << "[camdc1394] WARNING : Unable to open config file for cam " << list->ids[i].guid << endl ;
					cerr << "[camdc1394] trying with default parameters ... " << endl ;
				}

				if ( !cam->apply_settings() ) {
					cerr << "[camdc1394] ERROR : Could not apply settings to cam " << list->ids[i].guid << endl ;
					return false ;
				}

				cams.push_back( (GenericCamera*) cam ) ;
				_cams.push_back(cam) ;

			}

			dc1394_camera_free_list ( list ) ;
		}
	}

	return true ;
}


void Controller1394::loop_fct() {

		dc1394error_t err ;
		dc1394video_frame_t *frame = NULL ;
		Frame* vsframe = NULL ;	
		for ( int i=0; i < _cams.size(); i++ ) {

			err = dc1394_capture_dequeue ( _cams[i]->get_cam(), DC1394_CAPTURE_POLICY_POLL, &frame ) ;
		
			if ( err != DC1394_SUCCESS ) {
				cerr << "[camdc1394] Could not dequeue frame" << endl ;
				exit(0) ;
			}

			if ( frame ) {

				vsframe = _cams[i]->_buffer.pull() ;						

				memcpy ( vsframe->_data, frame->image, vsframe->_data_size ) ;

				_cams[i]->_buffer.push( vsframe ) ;

				err = dc1394_capture_enqueue ( _cams[i]->get_cam(), frame ) ;
			
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Could not enqueue frame" << endl ;
					exit(0) ;
				}
			}

		}
}


bool Controller1394::post_fct() {

	#ifdef _DEBUG
		cout << "[camdc1394] Entering Post() Function" << endl ;
	#endif

	for ( int i=0; i< _cams.size(); i++ ) {
		_cams[i]->stop_cam() ;
		delete ( _cams[i] ) ;
	}
	
	#ifdef _DEBUG
		cout << "[camdc1394] cams are deleted" << endl ;
	#endif


	if ( d1394_ != NULL ) {	
		
		#ifdef _DEBUG
			cout << "[camdc1394] Closing DC1394 " << endl ;
		#endif

		dc1394_free( d1394_ ) ;
	}

	#ifdef _DEBUG
		cout << "[camdc1394] Exiting Post() Function" << endl ;
	#endif


	return true ;
}


