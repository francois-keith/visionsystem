#include <visionsystem/camera.h>

namespace visionsystem {

Camera::Camera( std::string name ) {
	_name = name ;
}

Camera::~Camera( ) {

}


std::string Camera::get_name() {
	return _name ;
}


}
