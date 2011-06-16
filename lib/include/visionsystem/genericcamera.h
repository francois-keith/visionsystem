#ifndef VS_GENERICCAM_H
#define VS_GENERICCAM_H

#include <visionsystem/camera.h>
#include <visionsystem/frame.h>
#include <visionsystem/buffer.h>

class GenericCamera : public Camera
{

	public:

		GenericCamera( std::string name ) ;

		Buffer<Frame*>	_buffer ;

} ;

#endif
