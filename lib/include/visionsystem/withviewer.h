#ifndef VS_WITHVIEWER_H
#define VS_WITHVIEWER_H

#include <visionsystem/config.h>

#include <visionsystem/visionsystem.h>
#include <visionsystem/plugin.h>
#include <visionsystem/camera.h>

#ifndef _WIN32

	#include <X11/Xlib.h>
	#include <X11/keysym.h>

#else

	// FIXME	Windows compatibility ?
	typedef union _XEvent
	{} XEvent;
#endif

#if VS_HAS_SDL == 1

    #include <SDL/SDL_events.h>

#else

    typedef union SDL_Event
    {} SDL_Event;

#endif

namespace visionsystem {


class WithViewer
{

	public:
			WithViewer( VisionSystem *core ) ;
			~WithViewer() ;

			virtual void glfunc ( Camera* ) = 0  ;
			virtual void callback ( Camera* , XEvent ) = 0 ;
            virtual void sdl_callback( Camera *, SDL_Event ) {}

	protected:

			void register_glfunc  () ;
			void unregister_glfunc  () ;
			void register_callback() ;
			void unregister_callback() ;

	private:
			VisionSystem* wv_core ;
} ;


}

#endif
