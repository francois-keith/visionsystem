#ifndef SHUTDOWN_H
#define SHUTDOWN_H

#include <visionsystem/plugin.h>

using namespace std ;
using namespace visionsystem ;

class Shutdown : public Plugin {

	public:
		
		Shutdown( VisionSystem *vs, string sandbox ) ;
		~Shutdown() ;

		bool pre_fct()  ;
		void loop_fct() ;
		bool post_fct() ;


} ;



PLUGIN( Shutdown ) 

#endif
