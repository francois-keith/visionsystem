#ifndef VS_VISIONSYSTEM_H
#define VS_VISIONSYSTEM_H

#include <vector>
#include <map>
#include <boost/any.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <visionsystem/genericcamera.h>

namespace visionsystem {


    class Plugin ;


    class VisionSystem {

        public :

            VisionSystem() ;
            virtual ~VisionSystem() ;

            std::vector<Camera*> get_all_cameras() ;
            Camera* get_camera ( std::string cam_name ) ;
            Camera* get_default_camera() ;

            void register_to_cam ( Plugin* plugin, Camera* cam ) ;
            void unregister_to_cam  ( Plugin* plugin, Camera* cam ) ;

            template< typename Obj >
            void whiteboard_write ( std::string key, Obj value ) ;

            template< typename Obj >
            Obj whiteboard_read ( std::string key ) ;

            /* Those can be overrided for dynamic camera loading/unloading */
            virtual const std::vector<std::string> & get_loaded_controllers() = 0;

            virtual const std::vector<std::string> & get_available_controllers() = 0;

            virtual void unload_controller(const std::string & controller_name) = 0;

            virtual void load_controller(const std::string & controller_name) = 0;


        protected :

            void add_camera ( GenericCamera * ) ;

            std::vector<GenericCamera*> get_all_genericcameras() ;
            std::vector<Plugin*>        get_all_subscriptions ( GenericCamera* ) ;

            void whiteboard_wipe();

            /* WARNING: don't use those when you inherit from VisionSystem unless you KNOW what you're doing */
            std::map< std::string, boost::recursive_mutex* >  whiteboard_mutex ;

            std::map< Camera*, std::vector<Plugin*> >   _subscriptions ;
            boost::mutex                           _subscriptions_mutex ;

            std::vector< GenericCamera* >            _cameras ;
            boost::mutex                    _cameras_mutex ;
        private :
            std::map< std::string, boost::any >    whiteboard_data ;

} ;



#include <visionsystem/visionsystem.hpp>


}


#endif


