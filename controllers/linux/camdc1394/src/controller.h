#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <vector>

#include <visionsystem/controller.h>
#include <visionsystem/camera.h>

#include <dc1394/dc1394.h>

#include "camera1394.h"

using namespace visionsystem ;
using namespace std ;


class Controller1394 : public visionsystem::Controller
{
	public:

		Controller1394( VisionSystem *vs, std::string sandbox ) ;
		~Controller1394() ;

		bool pre_fct( vector< GenericCamera* > &cams ) ;
		void loop_fct() ;
		bool post_fct() ;

	private:

		dc1394_t*             d1394_ ;  // handler on the 1394 controller
		vector< Camera1394* >  _cams ;	// Vectors of cameras detected

};






CONTROLLER( Controller1394 ) 


#endif
