#include <visionsystem/controller.h>

namespace visionsystem {


Controller::Controller( VisionSystem *vscore, std::string name, std::string sandbox ) {
    _vscore = vscore ;
    _name = name ;
    _sandbox = sandbox ;
}


Controller::~Controller() {


}


std::string Controller::get_name() {
    return _name ;
}


std::string Controller::get_sandbox() {
    return _sandbox ;
}


}
