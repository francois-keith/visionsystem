#ifndef VS_V4LCONTROLLER_H
#define VS_V4LCONTROLLER_H

#include <vector>

#include <visionsystem/controller.h>
#include <configparser/configparser.h>
#include <visionsystem/camera.h>

#include <boost/filesystem.hpp>

#include "camerav4l2.h"

using namespace visionsystem ;
using namespace std ;


class ControllerV4L2 : public Controller
{
    public:

        ControllerV4L2( VisionSystem *vs, std::string sandbox ) ;
        ~ControllerV4L2() ;

        bool pre_fct( vector< GenericCamera* > &cams ) ;
        void preloop_fct() ;
        void loop_fct() ;
        bool post_fct() ;

    private:

        vector< CameraV4L2* >  _cams ;    // Vectors of cameras detected

};






CONTROLLER( ControllerV4L2 )


#endif
