#include "dummy.h"

#include <iostream>

Dummy::Dummy( VisionSystem *vs, string sandbox )
: Plugin( vs, "dummy", sandbox ) {

}

Dummy::~Dummy() {

}

bool Dummy::pre_fct() {
	cout << "[Dummy] pre_fct()" << endl ;
	return true ;
}

void Dummy::loop_fct() {

	cout << "[Dummy] loop_fct() - 10 Seconds wait" << endl ;
	sleep(10) ;
}

bool Dummy::post_fct() {
	cout << "[Dummy] post_fct()" << endl ;
	return true ;
}


