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
		
		cout << "[camdc1394] Initializing DC1394 system " << endl ; 
		
		dc1394camera_list_t * list ;
		dc1394error_t	err ;

		err = dc1394_camera_enumerate( d1394_, &list ) ;

		if ( list->num == 0 ) {

			cout << "[camdc1394]  No 1394 camera found." << endl ;
			return true ;

		} else {

			for ( int i=0; i<list->num; i++ ) {
				
				cout <<"[camdc1394] Found camera " << list->ids[i].guid << endl ;					
	
				Camera1394* cam = new Camera1394( d1394_ , list->ids[i].guid ) ;

				std::ostringstream capa_filename;
				capa_filename << get_sandbox() << "/" << list->ids[i].guid << ".capa" ;  
				cam->save_capa( capa_filename.str() )  ;
				
				std::ostringstream conf_filename;
				conf_filename << get_sandbox() << "/" << list->ids[i].guid << ".conf" ;  
				cam->read_config_file ( conf_filename.str().c_str() ) ;

				cam->apply_settings() ;

				//FIXME

//				cameraVector.push_back(cam) ;
//				cam1394Vector_.push_back(cam) ;

			}

			dc1394_camera_free_list ( list ) ;
		}
	}

	return true ;
}


void Controller1394::loop_fct() {

}


bool Controller1394::post_fct() {

	for ( int i=0; i< _cams.size(); i++ )
		delete ( _cams[i] ) ;

	if ( d1394_ != NULL ) {	
		cout << "[camdc1394] Closing DC1394 " << endl ;
		dc1394_free( d1394_ ) ;
	}

	return true ;
}


