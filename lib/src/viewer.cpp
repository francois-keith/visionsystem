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

	void Viewer::register_glfunc   ( Camera* cam, void (*fct) ()  ) {
		glfuncs_mutex.lock() ;
		glfuncs[cam].push_back( fct ) ;	
		glfuncs_mutex.unlock() ;
	}

	void Viewer::register_callback ( Camera* cam, void (*fct) ( XEvent event ) ) {
		callbacks_mutex.lock() ;
		callbacks[cam].push_back( fct ) ;
		callbacks_mutex.unlock() ;
	} 

	void Viewer::unregister_glfunc   ( Camera*, void (*fct) ()  ) {

		// FIXME

	}

	void Viewer::unregister_callback ( Camera*, void (*fct) ( XEvent event ) ) {

		// FIXME

	} 


	void Viewer::operator<<(const string &s){
		// FIXME
	}


	void operator<<(Viewer * plugin, const string &s){
		// FIXME
	}


}
