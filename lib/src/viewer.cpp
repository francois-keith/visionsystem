#include <visionsystem/viewer.h>

using namespace std ;

namespace visionsystem {


	Viewer::Viewer( VisionSystem* core, string name, string sandbox )
	:Plugin( core, name, sandbox )
	{
		core->whiteboard_write< Viewer* >( string("viewer"), this ) ;
	}

	Viewer::~Viewer() {

	}

	void Viewer::register_glfunc   ( WithViewer* vw  ) {
		glfuncs_mutex.lock() ;
		glfuncs.push_back( vw ) ;
		glfuncs_mutex.unlock() ;
	}

	void Viewer::register_callback ( WithViewer* vw ) {
		callbacks_mutex.lock() ;
		callbacks.push_back( vw )  ;
		callbacks_mutex.unlock() ;
	}

	void Viewer::unregister_glfunc   ( WithViewer* vw ) {
		glfuncs_mutex.lock() ;
		for ( int i=0; i<glfuncs.size(); i++ )
			if ( glfuncs[i] == vw )
				glfuncs.erase (glfuncs.begin()+i);
		glfuncs_mutex.unlock() ;
	}

	void Viewer::unregister_callback ( WithViewer* vw ) {
		callbacks_mutex.lock() ;
		for ( int i=0; i<callbacks.size(); i++ )
			if ( callbacks[i] == vw )
				callbacks.erase (callbacks.begin()+i);
		callbacks_mutex.unlock() ;
	}


	void Viewer::operator<<(const string &s){
		// FIXME
	}


	void operator<<(Viewer * plugin, const string &s){
		// FIXME
	}


}
