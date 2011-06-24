#ifndef VS_VIEWER_H
#define VS_VIEWER_H

#include <map>
#include <visionsystem/plugin.h>
#include <visionsystem/camera.h>
#include <vision/image/imageref.h>
#include <boost/thread.hpp>

#ifndef _WIN32
	
	#include <X11/Xlib.h> 	
	#include <X11/keysym.h>

#else

	// FIXME	Windows compatibility ?

#endif

namespace visionsystem {

class Viewer: public Plugin
{

	public:
		
		Viewer( VisionSystem* core, std::string name, std::string sandbox ) ;
		~Viewer() ;

		void register_glfunc   ( Camera*, void (*fct) ()  ) ;
		void unregister_glfunc   ( Camera*, void (*fct) ()  ) ;
		void register_callback ( Camera*, void (*fct) ( XEvent event ) ) ; 
		void unregister_callback ( Camera*, void (*fct) ( XEvent event ) ) ; 

		virtual void gl_print ( vision::ImageRef position, std::string s ) =0 ;

		void operator<<(const std::string& s);
		friend void operator<<(Viewer * plugin, const std::string & s);



	protected:

		std::map< Camera*, std::vector< void (*)() > >		glfuncs ;
		boost::mutex						glfuncs_mutex ;
		
		std::map< Camera*, std::vector< void (*)( XEvent ) > >		callbacks ;
		boost::mutex							callbacks_mutex ;
		
		std::vector< std::string >				captions ;
		boost::mutex						captions_mutex ;

} ;


}

#endif
