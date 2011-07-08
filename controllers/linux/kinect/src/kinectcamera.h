#ifndef VS_KINECTCAMERA_H
#define VS_KINECTCAMERA_H

#include <string>
#include <configparser/configparser.h>
#include <visionsystem/camera.h>

using namespace std ;
using namespace visionsystem ;
using namespace configparser ;


class KinectCamera : public Camera 
{

	public:
		KinectCamera( string name ) ;
		~KinectCamera() ;

		
} ; 





#endif 
