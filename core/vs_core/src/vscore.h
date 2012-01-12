#ifndef VSCORE_H
#define VSCORE_H

#include <string>
#include <map>

#include <boost/filesystem.hpp>

#include <configparser/configparser.h>

#include <visionsystem/visionsystem.h>
#include <visionsystem/controller.h>
#include <visionsystem/plugin.h>
#include <visionsystem/camera.h>

#include "thread.h"

using namespace std ;
using namespace boost::filesystem ;
using namespace configparser ;
using namespace vision ;
using namespace visionsystem ;

class VsCore : public VisionSystem, public WithConfigFile 
{
	public:

		VsCore( int argc, char** argv, char** envv ) ;
		~VsCore() ;

        void sigint_handler(int signum);

		void run() ;

        bool catch_sigint;

	private:

		string _basedir ;
		string _configfile ;
		
		path _controller_path ;
		path _plugin_path ;

		vector<Controller*>  _controllers ;
		vector<Plugin*>	     _plugins ;

		vector<Thread<Controller>*>	_controller_threads ;
		vector<Thread<Plugin>*>		_plugin_threads ;

		void parse_config_line( vector<string> &line ) ;
} ;

#endif

