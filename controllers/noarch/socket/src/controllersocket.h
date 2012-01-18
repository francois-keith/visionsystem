#ifndef _H_CONTROLLERSOCKET_H_
#define _H_CONTROLLERSOCKET_H_

#include "camerasocket.h"
#include <boost/thread.hpp>
#include <visionsystem/controller.h>

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
    boost::thread * io_service_th_;
    std::vector<visionsystem::CameraSocket *> cams_;
};

} // namespace visionsystem

CONTROLLER( visionsystem::ControllerSocket )

#endif
