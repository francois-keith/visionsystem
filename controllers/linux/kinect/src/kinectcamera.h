#ifndef VS_KINECTCAMERA_H
#define VS_KINECTCAMERA_H

#include <string>
#include <configparser/configparser.h>
#include <visionsystem/camera.h>

using namespace std ;
using namespace visionsystem ;
using namespace configparser ;


class KinectCamera : public GenericCamera 
{

	public:
		KinectCamera( freenect_context ctx, int device_num ) ;
		~KinectCamera() ;

	public:
		vision::ImageRef get_size()  ;			 
		bool is_active()   ;
		FrameCoding get_coding()  ;
		float get_fps()  ;
		std::string get_name()  ;

	private:
	
		
		freenect_context *f_ctx;
		int device_nb ;


} ; 





#endif 
