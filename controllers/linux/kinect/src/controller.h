#ifndef VS_CONTROLLER_KINECT_H
#define VS_CONTROLLER_KINECT_H

#include <string>

#include <configparser/configparser.h>
#include <visionsystem/controller.h>
#include <libfreenect/libfreenect.h>

#include "kinectcamera.h"

using namespace std ;
using namespace vision ;
using namespace visionsystem ;

class KinectController : public Controller
{

    public:

        KinectController( VisionSystem *vs, string sandbox ) ;
        ~KinectController() ;

        bool pre_fct( vector< GenericCamera*> &cams ) ;
        void preloop_fct() ;
        void loop_fct() ;
        bool post_fct() ;

        void get_cameras(std::vector<GenericCamera *> & cams);
    private:

        vector< KinectCamera* >    _devices ;

    private:

        freenect_context* f_ctx;

        int nr_devices ;
} ;


CONTROLLER( KinectController )

#endif
