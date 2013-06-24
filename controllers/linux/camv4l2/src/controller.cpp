#include "controller.h"

#include <iostream>

ControllerV4L2::ControllerV4L2( VisionSystem *vs, std::string sandbox )
:Controller( vs, "camv4l2", sandbox), _cams(0) {

}


ControllerV4L2::~ControllerV4L2() {
}


bool ControllerV4L2::pre_fct( vector< GenericCamera* > &cams ) {

	for ( int i=0; i<10; i++ ) {
		stringstream dev_path ;
		stringstream conf_path ;

		dev_path << "/dev/video" << i ;
		conf_path << get_sandbox() << "/video" << i << ".conf" ;

		if ( boost::filesystem::exists( dev_path.str() ) ) {

			std::cout << "[camv4l2] Found " << dev_path.str() << std::endl ;

			CameraV4L2* camera = new CameraV4L2( dev_path.str() ) ;

			try {

				camera->read_config_file( conf_path.str().c_str() );

			} catch ( string msg ) {

				std::cout << "[camv4l2] Could not open config file for " << dev_path.str() << endl ;
				std::cout << "[camv4l2] Using default config " << endl ;

			}

			if(camera->is_active()) {

				if ( !camera->init_camera() ) {

					std::cerr << "[camv4l2] ERROR : could not initialize " << dev_path.str() << std::endl ;
					return false ;

				}

			}

			cams.push_back(camera);
                	_cams.push_back(camera);

		}

	}


	return true ;
}


void ControllerV4L2::preloop_fct() {

}


void ControllerV4L2::loop_fct() {


	for(size_t i = 0; i < _cams.size(); ++i) {

		if( _cams[i]->is_active() ) {
        		if ( _cams[i]->read_frame() == -1 )
				break ;
		}
        }

}


bool ControllerV4L2::post_fct() {


	for(size_t i = 0; i < _cams.size(); ++i) {

		if ( !_cams[i]->stop_camera() )
			return false ;


	}

	return true ;
}


