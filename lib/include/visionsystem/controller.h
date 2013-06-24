#ifndef VS_CONTROLLER_H
#define VS_CONTROLLER_H

#include <vector>
#include <string>

#include <visionsystem/visionsystem.h>
#include <visionsystem/genericcamera.h>

#include <vision/win32/windows.h>

#ifdef WIN32
#define VS_CONTROLLER_EXPORT __declspec(dllexport)
#else
#define VS_CONTROLLER_EXPORT
#endif

#define CONTROLLER(NAME) extern "C" VS_CONTROLLER_EXPORT visionsystem::Controller* create( visionsystem::VisionSystem* vs, std::string sandbox ) { return new NAME(vs, sandbox) ; } ; \
             extern "C" VS_CONTROLLER_EXPORT void destroy( visionsystem::Controller* p ) { delete p ; }

namespace visionsystem {


    class Controller {

        public:

            Controller( VisionSystem *vscore, std::string name, std::string sandbox ) ;
            virtual ~Controller() ;

            virtual bool pre_fct( std::vector<GenericCamera*> &cams) = 0 ;    // This function does all the init stuff and should fill vector of pointers to cams.
            virtual void preloop_fct() = 0 ;                // This function will be run in a thread before the main loop
            virtual void loop_fct() = 0 ;                // This function will be run in a thread. Put your acquisition loop there.
            virtual bool post_fct() = 0 ;                // This function is called after the loop is stopped. Close everything smoothly here.

            virtual void get_cameras( std::vector<GenericCamera *> & cams) = 0; // Should provide all available cameras

            std::string get_name() ;
            std::string get_sandbox() ;

        protected:

            template < typename Obj >
            void whiteboard_write ( std::string key, Obj value ) ;

            template < typename Obj >
            Obj whiteboard_read ( std::string ) ;

        private:

            VisionSystem* _vscore ;
            std::string   _name ;
            std::string   _sandbox ;

    } ;

#include <visionsystem/controller.hpp>

}


#endif

