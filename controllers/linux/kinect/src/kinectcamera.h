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


class DepthCamera : public GenericCamera
{
	public:
			DepthCamera( freenect_context *ctx, int device_num ) ;
			~DepthCamera() ;
		
	public:
		vision::ImageRef get_size()  ;			 
		FrameCoding      get_coding()  ;
		float            get_fps()  ;
		std::string      get_name()  ;
		bool             is_active()   ;

	public:

		freenect_context      *context ;
		freenect_device	      *device ;
		int		      device_num ;

		freenect_depth_format current_depth_format ;
		std::string 	      name ;
		bool		      active ;

} ;



class RgbCamera : public GenericCamera
{
	public:
	
			RgbCamera( freenect_context *ctx, int device_num ) ;
			~RgbCamera() ;
		
	public:
		vision::ImageRef get_size()  ;			 
		FrameCoding      get_coding()  ;
		float            get_fps()  ;
		std::string      get_name()  ;
		bool             is_active()   ;

	public:

		freenect_context      *context ;
		freenect_device	      *device ;
		int		      device_num ;

		freenect_video_format current_video_format ;
		freenect_resolution   resolution ;
		std::string 	      name ;
		bool		      active ;

} ;



class KinectCamera : public WithConfigFile
{


	public:
		KinectCamera( freenect_context * ctx, int device_num ) ;
		~KinectCamera() ;
	public:

		bool apply_settings() ;	
		void start_cam() ;
		void stop_cam() ;

	private:

		void parse_config_line ( vector<string> &line ) ;

	private:
	
		int device_nb ;
		freenect_context   *f_ctx;
		freenect_device    *f_dev;

		freenect_led_options  freenect_led ;
		int 		      freenect_angle ;
	
	public:	
		RgbCamera	*rgb_cam ;
		DepthCamera	*depth_cam ;
} ; 





#endif 
