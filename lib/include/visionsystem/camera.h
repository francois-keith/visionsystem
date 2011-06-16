#ifndef VS_CAMERA_H
#define VS_CAMERA_H

#include <string>
#include <boost/any.hpp>
#include <vision/image/imageref.h>
#include <visionsystem/frame.h>

namespace visionsystem {


	class Controller ;

	class Camera 
	{
		public:
			
			Camera( std::string name ) ;			
			~Camera() ;

			virtual vision::ImageRef get_size() = 0 ;			// These methods can be called by plugins. 
			virtual bool is_active()  = 0 ;
			virtual FrameCoding get_coding() = 0 ;
			virtual float get_fps() = 0 ;
	
			std::string      get_name() ;
	
		private:

			std::string 	    _name ;
	} ;



}
#endif

