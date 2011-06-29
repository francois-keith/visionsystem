#include "plugin.h"


GLView::GLView( VisionSystem* core, string sandbox ) 
:Viewer( core, "glview", sandbox ) 
{


}

GLView::~GLView() {

}


bool  GLView::pre_fct() {
	
	_cam = get_default_camera() ;

	register_to_cam< Image<uint32_t, RGB> >( _cam, 10 ) ;

	return true ;
}


void  GLView::preloop_fct() {
	_win = new GLWindow( "glview", 640,480, false ) ; 
}

void  GLView::loop_fct() {

	Image<uint32_t, RGB>	*img ;
	img = dequeue_image< Image<uint32_t, RGB> >( _cam ) ;

	_win->draw(img);
	_win->swap_buffers();
	_win->processEvents();
	
	enqueue_image< Image<uint32_t, RGB> >( _cam, img ) ;
}


bool  GLView::post_fct() {
	
	unregister_to_cam< Image<uint32_t, RGB> >( _cam ) ;
	
	return true ;
}


void GLView::parse_config_line( vector<string> &line ) {

}


void GLView::gl_print ( ImageRef position, string text ) {

}
