#include "plugin.h"


GLView::GLView( VisionSystem* core, string sandbox ) 
:Viewer( core, "glview", sandbox ) 
{


}

GLView::~GLView() {

}


bool  GLView::pre_fct() {
	return true ;
}


void  GLView::loop_fct() {

}


bool  GLView::post_fct() {
	return true ;
}


void GLView::parse_config_line( vector<string> &line ) {

}


void GLView::gl_print ( ImageRef position, string text ) {

}
