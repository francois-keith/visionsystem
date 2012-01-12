#include <iostream>
#include <string>
#include <csignal>

#include "vscore.h"

using namespace std ;

VsCore * core = 0;
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

int main ( int argc, char** argv, char** envv ) {

    signal(SIGINT, &sigint_handler);
	core = new VsCore( argc, argv, envv ) ;

	try {
		core->run() ;
	
	} catch ( string msg ) {
		
		cerr << msg << endl ;
	
	}
	
	return 0 ;

}
