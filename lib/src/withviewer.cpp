#include <iostream>
#include <visionsystem/withviewer.h>
#include <visionsystem/viewer.h>

using namespace std ;

namespace visionsystem {


WithViewer::WithViewer( VisionSystem *vs_core ) {
	wv_core = vs_core ;
}


WithViewer::~WithViewer() {

}


void WithViewer::register_callback() {
	Viewer* vw = wv_core->whiteboard_read<Viewer*>("viewer") ;
	if ( vw != NULL )
		vw->register_callback( this ) ;
}

void WithViewer::unregister_callback() {
	Viewer* vw = wv_core->whiteboard_read<Viewer*>("viewer") ;
	if ( vw != NULL )
		vw->unregister_callback( this ) ;
}

void WithViewer::register_glfunc() {
	Viewer* vw = wv_core->whiteboard_read<Viewer*>("viewer") ;
	if ( vw != NULL )
		vw->register_glfunc( this ) ;
}

void WithViewer::unregister_glfunc() {
	Viewer* vw = wv_core->whiteboard_read<Viewer*>("viewer") ;
	if ( vw != NULL )
		vw->unregister_glfunc( this ) ;


}


}
