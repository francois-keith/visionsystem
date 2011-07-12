#include "grab.h"

#include <iostream>

Grab::Grab( VisionSystem *vs, string sandbox )
:Plugin( vs, "grab", sandbox ), WithViewer( vs )  
{
	grab_next = false ;
	grabbed.clear() ;
}

Grab::~Grab() {

}

bool Grab::pre_fct() {

	string filename = get_sandbox() + string ( "/grab.conf") ;
	try {
		read_config_file( filename.c_str() ) ; 
	} catch ( string msg ) {
		
		cout << "[grab] Could not read config file" << endl ;
		cout << "[grab] Will grab all active cameras" << endl ;

		vector<Camera*> cams = get_all_cameras() ;

		for (int i=0; i<cams.size(); i++ ) {
			if ( cams[i]->is_active() ) {
				Registration* newreg = new Registration() ;
				newreg->camera = cams[i] ;
				newreg->grabbed_frames.clear() ;
				grabbed.push_back( newreg ) ;
				cout << "[grab] will grab on " << cams[i]->get_name() << endl ;
			}
		}


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

	for ( int i=0; i<grabbed.size(); i++ )
		register_to_cam< Image<uint32_t, RGB> > ( grabbed[i]->camera, 50 ) ;

}

void Grab::loop_fct() {

	for ( int i=0; i<grabbed.size(); i++ )
		grabbed[i]->current_frame = dequeue_image< Image<uint32_t, RGB> > ( grabbed[i]->camera ) ;


	if ( grab_next ) {

		for ( int i=0; i<grabbed.size(); i++ )
			grabbed[i]->grabbed_frames.push_back( grabbed[i]->current_frame ) ;

		grab_next = false ;

	} else {

		for ( int i=0; i<grabbed.size(); i++ )
			enqueue_image< Image<uint32_t, RGB> > ( grabbed[i]->camera, grabbed[i]->current_frame ) ;
	}

}

bool Grab::post_fct() {

	for ( int i=0; i<grabbed.size(); i++ )
	for ( int j=0; j<grabbed[i]->grabbed_frames.size(); j++ ) {

		std::ostringstream filename;
		filename << get_sandbox() << "/" << grabbed[i]->camera->get_name() << "-" << j << ".png" ;  
		save_color < uint32_t, RGB > ( filename.str() , grabbed[i]->current_frame ) ;
	}

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
				grab_next = true ;
				break;
			
			default:
				break ;
		}

	}

}


void Grab::glfunc(Camera* cam) {

}

