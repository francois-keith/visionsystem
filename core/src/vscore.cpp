
#include <iostream>
#include <cstdlib>

#include <visionsystem/plugin.h>

#include "vscore.h"


VsCore::VsCore( int argc, char** argv, char** envv )
{
	// Set base directory

	_basedir = string("") ;

	for (int i = 0 ; envv[i]; i++ ) {
		if ( strncmp ( envv[i], "HOME=", 5 ) == 0 ) 
			_basedir = string( &(envv[i][5]) ) + "/.vision_system/" ;
		else if ( strncmp ( envv[i], "APPDATA=", 8 ) == 0 )
			_basedir = string( &(envv[i][8]) ) + "/vision_system/" ;
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
	// FIXME
}


void VsCore::parse_config_line ( vector<string> &line ) 
{
	if ( line[0] == "Controller" ) {

		if ((line.size()!= 2) && (line.size()!=3)) 
			throw("[vs_core] ERROR : Wrong Number of arguments for command [Controller]") ;
		
		string sandbox ;
		
		if ( line.size() == 2 )
			 sandbox = _basedir + string("controllers/") + line[1] ;
		else
			 sandbox = line[2] ;

		cout << "[vs_core] Loading Controller: " << line[1] << " " << sandbox << endl ;

		if (!exists( sandbox )) {
			cout << "[vs_core] \t WARNING: Could not find sandbox. Creating ... " <<  endl ;
	 		create_directory( sandbox );	
		}

		if (!is_directory( sandbox )) {
			cerr << "[vs_core] \t ERROR: Sandbox is a file. Directory expected." << endl ;
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
			sandbox = _basedir + string("plugins/") + line[1] ;
		else
			sandbox = line[2] ;

		cout << "[vs_core] Loading Plugin: " << line[1] << " " << sandbox << endl ;

		if (!exists( sandbox )) {
			cout << "[vs_core] \t WARNING: Could not find sandbox. Creating ... " <<  endl ;
	 		create_directory( sandbox );	
		}

		if (!is_directory( sandbox )) {
			cerr << "[vs_core] \t ERROR: Sandbox is a file. Directory expected." << endl ;
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

	_controller_path = _basedir / "controllers"  ;
	_plugin_path = _basedir / "plugins" ;

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

	// Parse config file

	try {
		cout << "[vs_core] Parsing config ..." << endl ;
		read_config_file ( _configfile.c_str() ) ;
	} catch ( string msg ) {
		cerr << msg << endl ;
		return ;
	}

	// Fill controllers and plugin vectors

	for( int i=0; i< _controller_threads.size(); i++ )
		_controllers.push_back ( _controller_threads[i]->pointer ) ;

	for( int i=0; i< _plugin_threads.size(); i++ )
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

	for ( int i=0; i<_controllers.size(); i++) {
		
		cout << "[vs_core] \t initialising " << _controllers[i]->get_name() << endl ;
		
		vector<GenericCamera*> tmp ;
		if ( !_controllers[i]->pre_fct(tmp) ) {
			cerr << "[vs_core] ERROR: Controller pre_fct() returned false" << endl ;
			return ;
		}
		
		for ( int j=0; j<tmp.size(); j++ ) {
			cout << "[vs_core] \t \t Found camera " << tmp[j]->get_name() << endl ;
			add_camera( tmp[j] ) ;
		}
	}

	// Plugins pre_fect() ;

	cout << "[vs_core] Initialising all plugins " << endl ;

	for ( int i=0; i< _plugins.size(); i++ ) {
		cout << "[vs_core] \t initializing " << _plugins[i]->get_name() << " ..." << endl ;
		if ( !_plugins[i]->pre_fct() ) {
			cerr << "[vs_core] ERROR: Could not initialize plugin " << _plugins[i]->get_name() << endl ;
			return ;
		}
	}

	// Start Plugins threads

	cout << "[vs_core] Starting plugins Threads" << endl ;
/*
	for ( int i=0 ; i<_plugin_threads.size(); i++ )
		_plugin_threads[i]->start_thread() ;
*/		

	// Start Controllers threads
	
	cout << "[vs_core] Starting controllers Threads" << endl ;
/*
	for ( int i=0 ; i<_controller_threads.size(); i++ )
		_controller_threads[i]->start_thread() ;
*/	
	// Main loop
/*
	

	while (1) {
		for ( int c=0; c < _cameras.size(); c++ )
			if ( _subscriptions[_cameras[c]]->size() != 0 ) {
				boost::any img ;
				if ( _cameras[c]->image_ready() ) {
					img = _cameras[c]->dequeue_image() ;
					for ( int p=0; p < _subscriptions[ _cameras[c]]->size(); p++ )
						(*_subscriptions[_cameras[c]])[p]->push_image ( img ) ;
				}
			}
	}
*/
	// Stop Plugins threads
	
	cout << "[vs_core] Stopping plugins Threads ..." << endl ;
/*
	for ( int i=0 ; i<_plugin_threads.size(); i++ ) {
		_plugin_threads[i]->stop() ;
		_plugin_threads[i]->join_thread() ;
	}
*/
	// Stop Controllers threads
	
	cout << "[vs_core] Stopping Controller Threads ..." << endl ;
/*
	for ( int i=0 ; i<_controller_threads.size(); i++ ) {
		_controller_threads[i]->pointer->stop() ;
		_controller_threads[i]->join_thread() ;
	}
*/	

	// Plugins post_fct() 
	
	cout << "[vs_core] Closing Plugins ..." << endl ;

	for ( int i=0 ; i<_plugins.size(); i++ ) 
		_plugins[i]->post_fct() ;

	// Controllers post_fct()
	
	cout << "[vs_core] Closing Controllers ... " << endl ;

	for ( int i=0 ; i<_controllers.size(); i++ ) 
		_controllers[i]->post_fct() ;

	// Cleaning up a bit
/*
	for ( int i=0 ; i<_plugin_threads.size(); i++ ) 
		delete _plugin_threads[i] ;
	
	for ( int i=0 ; i<_controller_threads.size(); i++ )
		delete _controller_threads[i] ;

	_plugins.clear() ;
	_controllers.clear() ;
*/
}

