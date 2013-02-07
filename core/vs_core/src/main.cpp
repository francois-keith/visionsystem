#include <iostream>
#include <string>

#ifndef WIN32
#include <csignal>
#endif

#include "vscore.h"

using namespace std ;

VsCore * core = 0;
#ifndef WIN32
uint32_t SIGINT_CNT = 0;
void sigint_handler(int signum)
{
    if(core && core->catch_sigint && SIGINT_CNT == 0)
    {
        core->sigint_handler(signum);
        SIGINT_CNT++;
    }
    else
    {
        exit(signum);
    }
}
#endif

int main ( int argc, char** argv, char** envv ) {

#ifndef WIN32
    signal(SIGINT, &sigint_handler);
    signal(SIGQUIT, &sigint_handler);
#endif
	core = new VsCore( argc, argv, envv ) ;

	try {
		core->run() ;
	
	} catch ( string msg ) {
		
		cerr << msg << endl ;
	
	}
	
	return 0 ;

}
