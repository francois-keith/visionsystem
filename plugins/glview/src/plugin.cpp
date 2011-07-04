#include "plugin.h"


GLView::GLView( VisionSystem* core, string sandbox ) 
:Viewer( core, "glview", sandbox ) 
{

}

GLView::~GLView() {

}


bool  GLView::pre_fct() {
	
	cameras= get_all_cameras() ;
	active_cam = 0 ;
	next_cam = 0 ;

	register_to_cam< Image<uint32_t, RGB> >( cameras[active_cam], 10 ) ;

	return true ;
}


void  GLView::preloop_fct() {
	win = new GLWindow( "glview", 640,480, false ) ; 
}

void  GLView::loop_fct() {

	Image<uint32_t, RGB>	*img ;
	img = dequeue_image< Image<uint32_t, RGB> >( cameras[active_cam] ) ;

	// Draw video 
	
	win->draw(img);
	
	// Call all registered glfuncs

	glfuncs_mutex.lock() ;	
	for ( int i=0; i<glfuncs.size(); i++ )
		glfuncs[i]->glfunc( cameras[active_cam] ) ;
	glfuncs_mutex.unlock() ;
	
	// Swap buffers

	win->swap_buffers();
	
	// Process events

	XEvent event = win->processEvents();
	
	// Call all registered callbacks

	callbacks_mutex.lock() ;	
	for ( int i=0; i<callbacks.size(); i++ )
		callbacks[i]->callback( cameras[active_cam], event ) ;

	callbacks_mutex.unlock() ;	

	enqueue_image< Image<uint32_t, RGB> >( cameras[active_cam], img ) ;

	if ( active_cam != next_cam ) {
		unregister_to_cam< Image<uint32_t, RGB> >( cameras[active_cam] ) ;
		register_to_cam< Image<uint32_t, RGB> >( cameras[next_cam], 10 ) ;
		active_cam = next_cam ;
	}

}


bool  GLView::post_fct() {
	
	unregister_to_cam< Image<uint32_t, RGB> >( cameras[active_cam] ) ;
	
	return true ;
}


void GLView::parse_config_line( vector<string> &line ) {

}


void GLView::gl_print ( ImageRef position, string text ) {

}
