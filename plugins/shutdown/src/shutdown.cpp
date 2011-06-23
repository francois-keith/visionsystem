#include "shutdown.h"

#include <iostream>

Shutdown::Shutdown( VisionSystem *vs, string sandbox )
: Plugin( vs, "shutdown", sandbox ) {

}

Shutdown::~Shutdown() {

}

bool Shutdown::pre_fct() {
	return true ;
}

void Shutdown::loop_fct() {

	cout << "[shutdown] Waiting for 10 secs" << endl ;
	sleep(10) ;
	cout << "[shutdown] Killing ... " << endl ;
	
	whiteboard_write< bool >( string("core_stop") , true ) ;

	sleep(10) ;
}

bool Shutdown::post_fct() {
	return true ;
}


