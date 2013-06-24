#ifndef GLVIEW_H
#define GLVIEW_H

#include <configparser/configparser.h>
#include <vision/image/image.h>
#include <visionsystem/viewer.h>
#include <SDL/SDL.h>

using namespace std ;
using namespace configparser ;
using namespace vision ;
using namespace visionsystem ;


class SDLView: public Viewer
{
	public:

		SDLView( VisionSystem* core, string sandbox ) ;
		~SDLView() ;

		 bool  pre_fct() ;
		 void  preloop_fct() ;
		 void  loop_fct() ;
		 bool  post_fct() ;

		 void gl_print ( ImageRef position, string text ) ;

	private:
        inline void refresh_screen();

        void SDL_Loop();

        SDL_Surface * screen;
        SDL_Surface * sdl_img;

		boost::thread * sdl_th;
		Image<uint32_t, RGB>	*img ;

		vector<Camera*> cameras ;
		int		active_cam ;
		int 		next_cam ;
		bool sdl_running;
		bool refresh;
} ;

PLUGIN( SDLView )

#endif

