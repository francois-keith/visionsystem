#ifndef GRAB_H
#define GRAB_H

#include <vision/image/image.h>
#include <visionsystem/plugin.h>
#include <visionsystem/viewer.h>
#include <configparser/configparser.h>

using namespace std ;
using namespace vision ;
using namespace visionsystem ;
using namespace configparser ;

class Grab : public Plugin, public WithViewer, public WithConfigFile {

	public:
		
		Grab( VisionSystem *vs, string sandbox ) ;
		~Grab() ;

		bool pre_fct()  ;
		void preloop_fct() ;
		void loop_fct() ;
		bool post_fct() ;

	private:

		void callback( Camera* cam, XEvent event ) ;
		void glfunc( Camera* cam ) ;

		void parse_config_line( vector<string> &line ) ;


		class Registration {

			public:

			Camera* camera ;
			Image<uint32_t, RGB> *current_frame ;
			vector< Image< uint32_t, RGB >* > grabbed_frames ;
		} ;


		bool grab_next ;			// should next frames be grabbed ?
		vector< Registration* >	grabbed ;

} ;



PLUGIN( Grab ) 

#endif
