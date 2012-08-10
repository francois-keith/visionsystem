#include <iostream>
#include <cstdlib>
#include <cstring>

#ifndef WIN32
#include <csignal>
#endif

#include <visionsystem/plugin.h>
#include <visionsystem/viewer.h>

#include "vscore.h"


VsCore::VsCore( int argc, char** argv, char** envv ) : catch_sigint(true), skip_frames(false)
{
	// Set base directory

	_basedir = string("") ;

	for (int i = 0 ; envv[i]; i++ ) {
		if ( strncmp ( envv[i], "HOME=", 5 ) == 0 ) 
			_basedir = string( &(envv[i][5]) ) + "/.vs_core/" ;
		else if ( strncmp ( envv[i], "APPDATA=", 8 ) == 0 )
			_basedir = string( &(envv[i][8]) ) + "/vs_core/" ;
	}

	if ( _basedir == "" ) {
		cerr << "[vs_core] ERROR: Could not determine base directory" << endl ;
		exit(0) ;
	}

	// Set config file
	
	if ( argc >= 2 )
		_configfile = string( argv[1] ) ;
	else
		_configfile = _basedir + "vs_core.conf" ;

}

VsCore::~VsCore()
{

}

void VsCore::sigint_handler(int signum)
{
    #ifndef WIN32
    if(signum == SIGINT)
    {
        whiteboard_mutex["core_stop"]->unlock();
        whiteboard_write< bool >( string("core_stop") , true ) ;
    }
    else
    {
        exit(signum);
    }
    #endif
}

void VsCore::parse_config_line ( vector<string> &line ) 
{
    if( fill_member(line, "CatchSIGINT", catch_sigint) ) return;

    if( fill_member(line, "SkipFrames", skip_frames) ) return;

	if ( line[0] == "Controller" ) {

		if ((line.size()!= 2) && (line.size()!=3)) 
			throw("[vs_core] ERROR : Wrong Number of arguments for command [Controller]") ;
		
		string sandbox ;
		
		if ( line.size() == 2 )
			 sandbox = _basedir + string("controllers/") + line[1] + std::string("/");
		else
			 sandbox = line[2] ;

		cout << "[vs_core] Loading Controller: " << line[1] << " " << sandbox << endl ;

		if (!exists( sandbox )) {
			cout << "[vs_core]  WARNING: Could not find sandbox. Creating ... " <<  endl ;
	 		create_directory( sandbox );	
		}

		if (!is_directory( sandbox )) {
			cerr << "[vs_core]  ERROR: Sandbox is a file. Directory expected." << endl ;
			return ;
		}

		_controller_threads.push_back( new Thread<Controller>( this, line[1], sandbox ) ) ;
		
		return ;

	}

	if ( line[0] == "Plugin" ) {

		if ((line.size()!= 2) && (line.size()!=3)) 
			throw("[vs_core] ERROR : Wrong Number of arguments for command [Plugin]") ;
		
		string sandbox ;
		
		if ( line.size() == 2 )
			sandbox = _basedir + string("plugins/") + line[1] + std::string("/");
		else
			sandbox = line[2] ;

		cout << "[vs_core] Loading Plugin: " << line[1] << " " << sandbox << endl ;

		if (!exists( sandbox )) {
			cout << "[vs_core]  WARNING: Could not find sandbox. Creating ... " <<  endl ;
	 		create_directory( sandbox );	
		}

		if (!is_directory( sandbox )) {
			cerr << "[vs_core]  ERROR: Sandbox is a file. Directory expected." << endl ;
			return ;
		}

		_plugin_threads.push_back( new Thread<Plugin>( this, line[1], sandbox ) ) ;

		return ;
	}

	cerr << "[vs_core] WARNING: Can't parse line in config file beginning with " << line[0] <<endl ;
	
}

void VsCore::run()
{

	// Initialization

	_controller_path = path(_basedir) / path("controllers")  ;
	_plugin_path = path(_basedir) / path("plugins") ;

	// Create base filesystem if needed

	cout << "[vs_core] Base directory is set to: " << _basedir << endl ;
	cout << "[vs_core] Config file is est to: " << _configfile << endl ;

	if (!exists( _basedir)) {
		cout << "[vs_core] WARNING: Could not find Base directory. Creating ... " << _basedir << endl ;
	 	create_directory( _basedir );	
	}

	if (!is_directory(_basedir)) {
		cerr << "[vs_core] ERROR: Expecting Base directory, found a file. << " << endl ;
		cerr << "[vs_core] ERROR: " << _basedir << endl ; 
		return ;
	}


	if (!exists( _controller_path )) {
		cout << "[vs_core] WARNING: Could not find 'controllers' subdirectory. Creating ... " << endl ;
	 	create_directory( _controller_path );	
	}

	if (!is_directory(_controller_path)) {
		cerr << "[vs_core] ERROR: Expecting 'controllers' subdirectory, found a file. << " << endl ;
		return ;
	}

	if (!exists( _plugin_path )) {
		cout << "[vs_core] WARNING: Could not find 'plugins' subdirectory. Creating ... " <<  endl ;
	 	create_directory( _plugin_path );	
	}

	if (!is_directory(_plugin_path)) {
		cerr << "[vs_core] ERROR: Expecting 'plugins' directory, found a file." << endl ;
		return ;
	}

	if (exists( _configfile) ) {
		if (!is_directory( _configfile) ) 
			cout << "[vs_core] Found config file" << endl ;
		else {
			cerr << "[vs_core] ERROR: Expected config file, found a directory." << endl ;
			return ;
		}
	} else {
		cerr << "[vs_core] ERROR: Could not find config file." << endl ;
		return ;
	}

	// WhiteBoard init

	whiteboard_write< bool >( string("core_stop") , true ) ;
	whiteboard_write< Viewer* >( string("viewer"), (Viewer*)0 );
    whiteboard_write< unsigned int > ( std::string("threads_ready") , 0 );


	// Parse config file

	try {
		cout << "[vs_core] Parsing config ..." << endl ;
		read_config_file ( _configfile.c_str() ) ;
	} catch ( string msg ) {
		cerr << msg << endl ;
		return ;
	}

	// Fill controllers and plugin vectors

	for( size_t i=0; i< _controller_threads.size(); i++ )
		_controllers.push_back ( _controller_threads[i]->pointer ) ;

	for( size_t i=0; i< _plugin_threads.size(); i++ )
		_plugins.push_back ( _plugin_threads[i]->pointer ) ;

	if ( _controllers.size() == 0 )
		cout << "[vs_core] WARNING: no controller loaded" << endl ;
	else
		cout << "[vs_core] " << _controllers.size() << " controllers loaded" << endl ;
	
	if (_plugins.size() == 0 )
		cout << "[vs_core] WARNING: no plugin loaded" << endl ;
	else
		cout << "[vs_core] " << _plugins.size() << " plugins loaded" << endl ;

	// Controllers pre_fct() 

	cout << "[vs_core] Initialising all controllers" << endl ;

	for ( size_t i=0; i<_controllers.size(); i++) {
		
		cout << "[vs_core] Initializing " << _controllers[i]->get_name() << endl ;
		
		vector<GenericCamera*> tmp ;
		if ( !_controllers[i]->pre_fct(tmp) ) {
			cerr << "[vs_core] ERROR: Controller pre_fct() returned false" << endl ;
			return ;
		}
		
		for ( size_t j=0; j<tmp.size(); j++ ) {
			cout << "[vs_core] Found camera " << tmp[j]->get_name() << endl ;
			add_camera( tmp[j] ) ;
		}
	}

	// Test if at least one camera is found
	
	Camera* _cm = get_default_camera() ;
	if ( _cm == NULL ) {
		cerr << "[vs_core] ERROR : No camera Found. Aborting." << endl ;
		return ;
	}

	// Plugins pre_fct() ;

	cout << "[vs_core] Initialising all plugins " << endl ;

	for ( size_t i=0; i< _plugins.size(); i++ ) {
		cout << "[vs_core] Initializing " << _plugins[i]->get_name() << " ..." << endl ;
		if ( !_plugins[i]->pre_fct() ) {
			cerr << "[vs_core] ERROR: Could not initialize plugin " << _plugins[i]->get_name() << endl ;
			return ;
		}
	}

	// Start Plugins threads

	cout << "[vs_core] Starting plugins Threads" << endl ;

	for ( size_t i=0 ; i<_plugin_threads.size(); i++ )
		_plugin_threads[i]->start_thread() ;
		

	// Start Controllers threads
	
	cout << "[vs_core] Starting controllers Threads" << endl ;

	for ( size_t i=0 ; i<_controller_threads.size(); i++ )
		_controller_threads[i]->start_thread() ;
	
	// Main loop
	
	Frame* frm ;

    while( whiteboard_read<unsigned int>("threads_ready") < _plugin_threads.size() + _controller_threads.size() );
	
    std::cout << "[vs_core] Acquisition starting" << std::endl;
    whiteboard_write("core_stop", false);
    
	while ( ! whiteboard_read< bool >("core_stop") ) {
		
		vector<GenericCamera*> all_cameras = get_all_genericcameras() ;	
		
		for ( size_t c=0; c < all_cameras.size(); c++ ) {
			
			frm = all_cameras[c]->_buffer.nbl_dequeue() ;	

			if ( frm != ( Frame*) NULL ) {

				vector<Plugin*> subscribed = get_all_subscriptions ( all_cameras[c] ) ;

				for ( size_t p=0; p < subscribed.size(); p++ )
					subscribed[p]->push_frame( (Camera*) all_cameras[c] , frm, skip_frames ) ;
				
			all_cameras[c]->_buffer.enqueue( frm ) ;
			
			}


		}
	}

	// Stop Controllers threads
	
	cout << "[vs_core] Sending stop signal to controller Threads ..." << endl ;

	for ( size_t i=0 ; i<_controller_threads.size(); i++ ) {
		_controller_threads[i]->request_stop() ;
	}

	// Join Controllers threads
	
	cout << "[vs_core] Waiting for controller threads to stop ..." << endl ;

	for ( size_t i=0 ; i<_controller_threads.size(); i++ ) {
		cout << "[vs_core] Waiting for " << _controller_threads[i]->pointer->get_name() << endl ;
		_controller_threads[i]->join() ;
	}


	// Stop Plugins threads
	
	cout << "[vs_core] Sending stop signal to plugins Threads ..." << endl ;

	for ( size_t i = _plugin_threads.size() ; i > 0 ; --i ) {
		_plugin_threads[i-1]->request_stop() ;
	}
	
	// Join Plugins threads
	
	cout << "[vs_core] Waiting for plugin threads to stop ..." << endl ;

	for ( size_t i = _plugin_threads.size() ; i > 0 ; --i ) {
		cout << "[vs_core] Waiting for " << _plugin_threads[i-1]->pointer->get_name() << endl ;
		_plugin_threads[i-1]->join() ;
	}
	
	// Plugins post_fct() 
	
	cout << "[vs_core] Calling post_fct() for Plugins ..." << endl ;

	for ( size_t i = _plugins.size() ; i > 0 ; --i ) {
		_plugins[i-1]->post_fct() ;
    }

	// Controllers post_fct()
	
	cout << "[vs_core] Calling post_fct() for Controllers ... " << endl ;

	for ( size_t i=0 ; i<_controllers.size(); i++ ) 
		_controllers[i]->post_fct() ;

	// Cleaning up a bit

    whiteboard_wipe();

	for ( size_t i = _plugin_threads.size() ; i > 0 ; --i ) 
    {
		_plugin_threads[i-1]->cleanup();
    }

	for ( size_t i = _plugin_threads.size() ; i > 0 ; --i ) 
    {
		delete _plugin_threads[i-1] ;
    }
	
	for ( size_t i=0 ; i<_controller_threads.size(); i++ )
		delete _controller_threads[i] ;

	_plugins.clear() ;
	_controllers.clear() ;

}

