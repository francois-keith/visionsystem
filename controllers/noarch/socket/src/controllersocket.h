#ifndef _H_CONTROLLERSOCKET_H_
#define _H_CONTROLLERSOCKET_H_

#include <visionsystem/controller.h>

#include "camerasocket.h"

namespace visionsystem
{

class ControllerSocket : public Controller
{
public:
    ControllerSocket( VisionSystem * vs, std::string sandbox );
    ~ControllerSocket();

    /* Virtual implementations */
    bool pre_fct( std::vector< GenericCamera *> & cams );
    void preloop_fct();
    void loop_fct();
    bool post_fct();

private:
    boost::asio::io_service io_service_;
    std::vector<visionsystem::CameraSocket *> cams_;
};

} // namespace visionsystem

CONTROLLER( visionsystem::ControllerSocket )

#endif
