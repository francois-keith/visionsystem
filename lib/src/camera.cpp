#include <visionsystem/camera.h>

namespace visionsystem {

Camera::Camera( std::string name ) {
	_name = name ;
}

std::string Camera::get_name() {
	return _name ;
}


}
