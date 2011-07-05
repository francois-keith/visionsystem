#ifndef GLVIEW_H
#define GLVIEW_H

#include <configparser/configparser.h>
#include <vision/image/image.h>
#include <visionsystem/viewer.h>

#include "glwindow.h"

using namespace std ;
using namespace configparser ;
using namespace vision ;
using namespace visionsystem ;


class GLView: public Viewer, public WithConfigFile
{
	public:

		GLView( VisionSystem* core, string sandbox ) ;
		~GLView() ;

		 bool  pre_fct() ;
		 void  preloop_fct() ;
		 void  loop_fct() ;
		 bool  post_fct() ;

		 void gl_print ( ImageRef position, string text ) ;

	private:

		void parse_config_line( vector<string> &line ) ;
		void callback( XEvent event ) ;

	private:

		GLWindow*	   win ;
		vector<Camera*> cameras ;
		int		active_cam ;
		int 		next_cam ;

		bool		callback_active ;
} ;

PLUGIN( GLView ) 

#endif

