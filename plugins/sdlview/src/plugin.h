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
        inline void DrawPixel(SDL_Surface *screen, unsigned int x, unsigned int y, Uint8 R, Uint8 G, Uint8 B);

        SDL_Surface * screen;
		vector<Camera*> cameras ;
		int		active_cam ;
		int 		next_cam ;
} ;

PLUGIN( SDLView ) 

#endif

