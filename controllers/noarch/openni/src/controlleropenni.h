#ifndef _H_OPENNICONTROLLER_H_
#define _H_OPENNICONTROLLER_H_

#include <visionsystem/controller.h>

#include "cameraopenni.h"

namespace visionsystem
{

class ControllerOpenNI : public visionsystem::Controller
{
public:
    ControllerOpenNI( visionsystem::VisionSystem * vs, std::string sandbox );
    ~ControllerOpenNI();

    /* To be implemented from visionsystem::Controller */
    bool pre_fct( std::vector< GenericCamera *> & cams);
    void preloop_fct();
    void loop_fct();
    bool post_fct();

    void get_cameras(std::vector<GenericCamera*> & cams);
private:
    std::vector<visionsystem::CameraOpenNI *> _cams;
};

}  // namespace vision

CONTROLLER( visionsystem::ControllerOpenNI )

#endif
