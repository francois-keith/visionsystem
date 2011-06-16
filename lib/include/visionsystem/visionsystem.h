#ifndef VS_VISIONSYSTEM_H
#define VS_VISIONSYSTEM_H

#include <vector>
#include <map>
#include <boost/any.hpp>
#include <boost/thread/mutex.hpp>
#include <visionsystem/camera.h>

namespace visionsystem {


	class Plugin ;

	
	class VisionSystem {

		public :
	
			virtual std::vector<Camera*> get_all_cameras() = 0 ;
			virtual Camera* get_camera ( std::string cam_name ) = 0 ;
			
			virtual Camera* register_to_cam ( Plugin* plugin, Camera* cam ) = 0 ;
			virtual void unregister_to_cam  ( Plugin* plugin, Camera* cam ) = 0 ;	

			template< typename Obj >
			void whiteboard_write ( std::string key, Obj value ) ;

			template< typename Obj >
			Obj whiteboard_read ( std::string key ) ;

		private :

			std::map< std::string, boost::any >    whiteboard_data ;
			std::map< std::string, boost::mutex >  whiteboard_mutex ;
	} ;



#include <visionsystem/visionsystem.hpp>


}


#endif


