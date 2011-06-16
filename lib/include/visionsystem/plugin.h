#ifndef VS_PLUGIN_H
#define VS_PLUGIN_H

#include <string>

#include <visionsystem/visionsystem.h>
#include <visionsystem/frame.h>
#include <visionsystem/buffer.h>

#include <visionsystem/imagefill.hpp>

// FIXME faire la version window pour export dll

#define PLUGIN(NAME) extern "C" visionsystem::Plugin* create( visionsystem::VisionSystem* vs, std::string sandbox ) { return new NAME(vs, sandbox) ; } ; \
		     extern "C" void destroy( visionsystem::Plugin* p ) { delete p ; }

namespace visionsystem {

class Plugin
{
	public: 

		Plugin( VisionSystem* vs, std::string plugin_name, std::string sandbox )  ;		
		~Plugin() ;

		std::string   get_name();
		std::string   get_sandbox() ;
	
		virtual bool pre_fct()  = 0 ;		// Called by VisionSystem core.
		virtual void loop_fct() = 0 ;		// Called by VisionSystem core.
		virtual bool post_fct() = 0 ;		// Called by VisionSystem core.

		void push_frame( Camera*, Frame* ) ;	// Called by VisionSystem core.
	
	protected:
		

		std::vector<Camera*> get_all_cameras() ;
		Camera* get_camera ( std::string cam_name )  ;
		
		template < typename Timage >
		Camera* register_to_cam ( std::string name, int N ) ;
		
		template < typename Timage >
		Timage* dequeue_image ( Camera* ) ;

		template < typename Timage >
		void enqueue_image ( Camera*, Timage* img ) ;

		template < typename Timage >
		void unregister_to_cam ( Camera* ) ;

		template < typename Obj >
		void whiteboard_write ( std::string key, Obj value ) ;

		template < typename Obj >
		Obj whiteboard_read ( std::string ) ;

	private:

		std::string	_sandbox ;					// Path to the sandbox
		VisionSystem*	_vscore ;					// Pointer to the VisionSystem core running this plugin
		std::string 	_name ;						// Name of the plugin.

		std::map< Camera*, Buffer<Frame*>*  >	frame_buffers ;		// This buffer for the frames received from the cameras	
		std::map< Camera*, boost::any >		img_buffers ;		// This buffer tor the images which will be filled from the frames.
};

#include <visionsystem/plugin.hpp>

}

#endif

