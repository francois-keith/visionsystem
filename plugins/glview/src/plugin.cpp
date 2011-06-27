#include "plugin.h"


GLView::GLView( VisionSystem* core, string sandbox ) 
:Viewer( core, "glview", sandbox ) 
{


}

GLView::~GLView() {

}


bool  GLView::pre_fct() {
	
	_win = new GLWindow( "glview", 800,600, false ) ; 
	_cam = get_default_camera() ;

	register_to_cam< Image<unsigned char, MONO> >( _cam, 10 ) ;

	return true ;
}


void  GLView::loop_fct() {

	Image<unsigned char, MONO>	*img ;
	img = dequeue_image< Image<unsigned char, MONO> >( _cam ) ;

	_win->draw(img);
	_win->swap_buffers();
	_win->processEvents();
	
	enqueue_image< Image<unsigned char, MONO> >( _cam, img ) ;
}


bool  GLView::post_fct() {
	
	unregister_to_cam< Image<unsigned char, MONO> >( _cam ) ;
	
	return true ;
}


void GLView::parse_config_line( vector<string> &line ) {

}


void GLView::gl_print ( ImageRef position, string text ) {

}
