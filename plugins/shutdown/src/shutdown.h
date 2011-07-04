#ifndef SHUTDOWN_H
#define SHUTDOWN_H

#include <visionsystem/plugin.h>
#include <visionsystem/viewer.h>
#include <configparser/configparser.h>

using namespace std ;
using namespace visionsystem ;
using namespace configparser ;

class Shutdown : public Plugin, public WithViewer, public WithConfigFile {

	public:
		
		Shutdown( VisionSystem *vs, string sandbox ) ;
		~Shutdown() ;

		bool pre_fct()  ;
		void preloop_fct() ;
		void loop_fct() ;
		bool post_fct() ;

	private:

		void callback( Camera* cam, XEvent event ) ;
		void glfunc( Camera* cam ) ;

		void parse_config_line( vector<string> &line ) ;

		int time ;		// Number of seconds before the kill signal is sent
		bool esc ;		// If this is true, a press on the esc key will kill the core.
} ;



PLUGIN( Shutdown ) 

#endif
