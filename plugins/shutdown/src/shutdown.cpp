#include "shutdown.h"

#include <iostream>

Shutdown::Shutdown( VisionSystem *vs, string sandbox )
:Plugin( vs, "shutdown", sandbox ), WithViewer( vs )  
{
	time = 3600 ;
	esc = true ;
}

Shutdown::~Shutdown() {

}

bool Shutdown::pre_fct() {

	string filename = get_sandbox() + string ( "/shutdown.conf") ;
	try {
		read_config_file( filename.c_str() ) ; 
	} catch ( string msg ) {
		cout << "[shutdown] Could not read config file" << endl ;
		cout << "[shutdown] Using default settings" << endl ;
	}

	cout << "[shutdown] Time out is set at " << time << " seconds" << endl ;
	
	Viewer* vw = whiteboard_read< Viewer* >( string("viewer")) ;
	
	if ( vw == NULL ) {
		cout << "[shutdown] No viewer found" << endl ;
	} else {
		if (esc) {
			cout << "[shutdown] Esc. key is enabled." << endl ;
			register_callback() ;
		} else {
			cout << "[shutdown] Esc. key is disabled." << endl ;
		}
	}
	
	return true ;
}

void Shutdown::preloop_fct() {

	for (int i=0; i<time; i++ ) {
		boost::this_thread::sleep(boost::posix_time::seconds(1));
	}

	cout << "[shutdown] Timeout. Killing ... " << endl ;
	
	whiteboard_write< bool >( string("core_stop") , true ) ;

}

void Shutdown::loop_fct() {

}

bool Shutdown::post_fct() {
	return true ;
}


void Shutdown::parse_config_line( vector<string> &line ) {

	fill_member<int>( line, "Time", time ) ; 
	fill_member<bool>( line, "esc", esc ) ;
}


void Shutdown::callback(Camera* cam, XEvent event) {
#ifndef WIN32
	if (event.type == KeyPress ) {

		switch (XLookupKeysym(&event.xkey, 0)) {

			case XK_Escape:
				cout << "[shutdown] Esc key detected : Killing ... " << endl ;
				whiteboard_write< bool >( string("core_stop"), true ) ;
				break;
		}

	}
#endif
}


void Shutdown::glfunc(Camera* cam) {

}

