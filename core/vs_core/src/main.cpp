#include <iostream>
#include <string>

#include "vscore.h"

using namespace std ;

int main ( int argc, char** argv, char** envv ) {

	VsCore core( argc, argv, envv ) ;

	try {
		core.run() ;
	
	} catch ( string msg ) {
		
		cerr << msg << endl ;
	
	}
	
	return 0 ;

}
