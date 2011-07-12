#include "grab.h"

#include <iostream>

Grab::Grab( VisionSystem *vs, string sandbox )
:Plugin( vs, "grab", sandbox ), WithViewer( vs )  
{

}

Grab::~Grab() {

}

bool Grab::pre_fct() {

	string filename = get_sandbox() + string ( "/grab.conf") ;
	try {
		read_config_file( filename.c_str() ) ; 
	} catch ( string msg ) {
		cout << "[grab] Could not read config file" << endl ;
		cout << "[grab] Using default settings" << endl ;
	}

	Viewer* vw = whiteboard_read< Viewer* >( string("viewer")) ;
	
	if ( vw == NULL ) {
		cout << "[grab] No viewer found" << endl ;
		return false ;
	}
	
	register_callback() ;
	
	return true ;
}

void Grab::preloop_fct() {


}

void Grab::loop_fct() {

}

bool Grab::post_fct() {
	return true ;
}


void Grab::parse_config_line( vector<string> &line ) {

	// FIXME
}


void Grab::callback(Camera* cam, XEvent event) {

	if (event.type == KeyPress ) {

		switch (XLookupKeysym(&event.xkey, 0)) {

			case XK_g:
				cout << "[grab] Grabing ... " << endl ;
				break;
			
			default:
				break ;
		}

	}

}


void Grab::glfunc(Camera* cam) {

}

