#ifndef DUMMY_H
#define DUMMY_H

#include <visionsystem/plugin.h>

using namespace std ;
using namespace visionsystem ;

class Dummy : public Plugin {

	public:
		
		Dummy( VisionSystem *vs, string sandbox ) ;
		~Dummy() ;

		bool pre_fct()  ;
		void loop_fct() ;
		bool post_fct() ;


} ;



PLUGIN( Dummy ) 

#endif
