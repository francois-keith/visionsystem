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
	
			VisionSystem() ;
			~VisionSystem() ;
			
			std::vector<Camera*> get_all_cameras() ;
			Camera* get_camera ( std::string cam_name ) ;
			Camera* get_default_camera() ;

			void register_to_cam ( Plugin* plugin, Camera* cam ) ;
			void unregister_to_cam  ( Plugin* plugin, Camera* cam ) ;	

			template< typename Obj >
			void whiteboard_write ( std::string key, Obj value ) ;

			template< typename Obj >
			Obj whiteboard_read ( std::string key ) ;


		protected :

			void add_camera ( Camera * ) ;
			std::vector<Plugin*> get_all_subscriptions ( Camera* ) ;

		private :

			
			std::vector< Camera* >			_cameras ;
			boost::mutex			        _cameras_mutex ;

			std::map< std::string, boost::any >    whiteboard_data ;
			std::map< std::string, boost::mutex >  whiteboard_mutex ;

			std::map< Camera*, std::vector<Plugin*> >   _subscriptions ;
			boost::mutex			       	    _subscriptions_mutex ;



} ;



#include <visionsystem/visionsystem.hpp>


}


#endif


