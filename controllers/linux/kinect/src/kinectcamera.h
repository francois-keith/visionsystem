#ifndef VS_KINECTCAMERA_H
#define VS_KINECTCAMERA_H

#include <string>
#include <configparser/configparser.h>
#include <visionsystem/genericcamera.h>
#include <libfreenect/libfreenect.h>

using namespace std ;
using namespace vision ;
using namespace visionsystem ;
using namespace configparser ;


class KinectCamera : public GenericCamera, public WithConfigFile
{

	public:
		KinectCamera( freenect_context * ctx, int device_num ) ;
		~KinectCamera() ;

	public:
		vision::ImageRef get_size()  ;			 
		bool is_active()   ;
		FrameCoding get_coding()  ;
		float get_fps()  ;
		std::string get_name()  ;

	public:

		void save_capa( string filename ) ;
		bool apply_settings() ;	
		void stop_cam() ;

	private:

		void parse_config_line ( vector<string> &line ) ;

	private:
	
		
		int device_nb ;
		freenect_context *f_ctx;


} ; 





#endif 
