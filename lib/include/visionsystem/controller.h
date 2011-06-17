#ifndef VS_CONTROLLER_H
#define VS_CONTROLLER_H

#include <vector>
#include <string>

#include <visionsystem/visionsystem.h>
#include <visionsystem/camera.h>

#define CONTROLLER(NAME) extern "C" visionsystem::Controller* create( visionsystem::VisionSystem* vs, std::string sandbox ) { return new NAME(vs, sandbox) ; } ; \
			 extern "C" void destroy( visionsystem::Controller* p ) { delete p ; }

namespace visionsystem {


	class Controller {

		public:

			Controller( VisionSystem *vscore, std::string name, std::string sandbox ) ; 
			~Controller() ;

			virtual bool pre_fct( std::vector<Camera*> &cams) = 0 ;	// This function does all the init stuff and should fill vector of pointers to cams.
			virtual void loop_fct() = 0 ;				// This function will be run in a thread. Put your acquisition loop there.	
			virtual bool post_fct() = 0 ;				// This function is called after the loop is stopped. Close everything smoothly here. 
	
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

