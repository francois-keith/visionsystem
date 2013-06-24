#ifndef VS_VIEWER_H
#define VS_VIEWER_H

#include <visionsystem/withviewer.h>
#include <vision/image/imageref.h>
#include <boost/thread.hpp>

namespace visionsystem {

class Viewer: public Plugin
{

	public:

		Viewer( VisionSystem* core, std::string name, std::string sandbox ) ;
		~Viewer() ;

		void register_glfunc     ( WithViewer* ) ;
		void unregister_glfunc   ( WithViewer* ) ;
		void register_callback   ( WithViewer* ) ;
		void unregister_callback ( WithViewer* ) ;

		virtual void gl_print ( vision::ImageRef position, std::string s ) =0 ;

		void operator<<(const std::string& s);
		friend void operator<<(Viewer * plugin, const std::string & s);

	protected:

		std::vector< WithViewer* >	glfuncs ;
		boost::mutex			glfuncs_mutex ;

		std::vector< WithViewer* >	callbacks ;
		boost::mutex			callbacks_mutex ;

		std::vector< std::string >	captions ;
		boost::mutex			captions_mutex ;

} ;


}

#endif
