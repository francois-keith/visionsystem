#ifndef VS_GENERICCAM_H
#define VS_GENERICCAM_H

#include <visionsystem/camera.h>
#include <visionsystem/frame.h>
#include <visionsystem/buffer.h>

namespace visionsystem {

class GenericCamera : public Camera
{

    public:

        Buffer<Frame>    _buffer ;

} ;


}

#endif
