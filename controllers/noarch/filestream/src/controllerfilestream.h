#ifndef _H_CONTROLLERFS_H_
#define _H_CONTROLLERFS_H_

#include <visionsystem/controller.h>

#include "camerafilestream.h"

namespace visionsystem
{

class ControllerFilestream : public visionsystem::Controller
{
public:
    ControllerFilestream( visionsystem::VisionSystem * vs, std::string sandbox );
    ~ControllerFilestream();

    /* To be implemented from visionsystem::Controller */
    bool pre_fct( std::vector< GenericCamera *> & cams);
    void preloop_fct();
    void loop_fct();
    bool post_fct();

    void get_cameras(std::vector<GenericCamera *> & cams);
private:
    std::vector<visionsystem::CameraFilestream *> _cams;
};

}  // namespace vision

CONTROLLER( visionsystem::ControllerFilestream )

#endif
