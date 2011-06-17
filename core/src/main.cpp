#include <iostream>
#include <string>

#include "vscore.h"

using namespace std ;

int main ( int argc, char** argv, char** envv ) {

	VsCore core( argc, argv, envv ) ;

	core.run() ;

	return 0 ;

}
