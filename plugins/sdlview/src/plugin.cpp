#include "plugin.h"

SDLView::SDLView( VisionSystem* core, string sandbox ) 
:Viewer( core, "sdlview", sandbox ), refresh(true) 
{
	active_cam = 0 ;
	next_cam = 0 ;
}

SDLView::~SDLView() {
}


bool  SDLView::pre_fct() {
	cameras = get_all_cameras() ;

	active_cam = -1 ;
	for ( int i=0; i<cameras.size(); i++ )
		if ( cameras[i]->is_active() )
			active_cam = i ;

	next_cam = active_cam ;
	
	if ( active_cam == -1 )	{
		cerr << "[glview] ERROR : No cam is active. " << endl ;
		return false ;
	}

	register_to_cam< Image<uint32_t, RGB> >( cameras[active_cam], 10 ) ;

	return true ;
}


void  SDLView::preloop_fct() {
    SDL_Init(SDL_INIT_VIDEO);
    screen = SDL_SetVideoMode(cameras[active_cam]->get_size().x, cameras[active_cam]->get_size().y, 32, SDL_HWSURFACE);
    SDL_WM_SetCaption("SDLView", NULL);
}

void  SDLView::loop_fct() {

	Image<uint32_t, RGB>	*img ;
	img = dequeue_image< Image<uint32_t, RGB> >( cameras[active_cam] ) ;

    if(refresh)
    {
        refresh = false;
    	if ( active_cam != next_cam ) {
    	    enqueue_image< Image<uint32_t, RGB> >( cameras[active_cam], img ) ;
    		unregister_to_cam< Image<uint32_t, RGB> >( cameras[active_cam] ) ;
    		register_to_cam< Image<uint32_t, RGB> >( cameras[next_cam], 10 ) ;
    		active_cam = next_cam ;
	        img = dequeue_image< Image<uint32_t, RGB> >( cameras[active_cam] ) ;
    	}

        boost::thread(boost::bind(&SDLView::refresh_screen, this, img));
    }
    else
    {
    	enqueue_image< Image<uint32_t, RGB> >( cameras[active_cam], img ) ;
    }
}

bool  SDLView::post_fct() {
	unregister_to_cam< Image<uint32_t, RGB> >( cameras[active_cam] ) ;
    SDL_Quit();
	return true ;
}

void SDLView::gl_print ( ImageRef position, string text ) {
}

void SDLView::refresh_screen(vision::Image<uint32_t, vision::RGB> * img)
{
	// Draw video 
    if ( SDL_MUSTLOCK(screen) ) {
        if ( SDL_LockSurface(screen) < 0 ) {
            return;
        }
    }
    for(unsigned int i = 0; i < img->width; ++i)
    {
        for(unsigned int j = 0; j < img->height; ++j)
        {
            DrawPixel(screen, i, j, (*img)(i,j) & 0xFF, ( (*img)(i,j) >> 8 ) & 0xFF, ( (*img)(i,j) >> 16 ) & 0xFF );
        }
    }
    SDL_UpdateRect(screen, 0, 0, img->width, img->height);
    if ( SDL_MUSTLOCK(screen) ) {
        SDL_UnlockSurface(screen);
    }

    // Handle events
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT:
                std::cout << "[SDLView] Exit requested ... " << endl ;
                whiteboard_write< bool >( string("core_stop"), true ) ;
                break; 
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_PAGEUP:
                        for(int i = active_cam + 1; i < cameras.size(); ++i)
                        {
                            if ( cameras[i]->is_active() )
                            {
                                next_cam = i;
                                i = cameras.size();
                            }
                        }
                        break;

                    case SDLK_PAGEDOWN:
                        for(int i = active_cam - 1; i >= 0; --i)
                        {
                            if ( cameras[i]->is_active() )
                            {
                                next_cam = i;
                                i = -1;
                            }
                        }
                        break;
                    case SDLK_ESCAPE: /* Appui sur la touche Echap, on arrête le programme */
                        std::cout << "[SDLView] Exit requested ... " << endl ;
                        whiteboard_write< bool >( string("core_stop"), true ) ;
                        break;
                }
                break;
            default:
                break;
        }
        callbacks_mutex.lock() ;

        for(int i = 0; i < callbacks.size(); ++i)
        {
            callbacks[i]->sdl_callback( cameras[active_cam], event ) ;
        }

        callbacks_mutex.unlock() ;
    }

	enqueue_image< Image<uint32_t, RGB> >( cameras[active_cam], img ) ;
    refresh = true;
}

inline void SDLView::DrawPixel(SDL_Surface *screen, unsigned int x, unsigned int y, Uint8 R, Uint8 G, Uint8 B)
{
    Uint32 color = SDL_MapRGB(screen->format, R, G, B);

    switch (screen->format->BytesPerPixel) {
        case 1: { /* Assuming 8-bpp */
            Uint8 *bufp;

            bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
            *bufp = color;
        }
        break;

        case 2: { /* Probably 15-bpp or 16-bpp */
            Uint16 *bufp;

            bufp = (Uint16 *)screen->pixels + y*screen->pitch/2 + x;
            *bufp = color;
        }
        break;

        case 3: { /* Slow 24-bpp mode, usually not used */
            Uint8 *bufp;

            bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
            *(bufp+screen->format->Rshift/8) = R;
            *(bufp+screen->format->Gshift/8) = G;
            *(bufp+screen->format->Bshift/8) = B;
        }
        break;

        case 4: { /* Probably 32-bpp */
            Uint32 *bufp;

            bufp = (Uint32 *)screen->pixels + y*screen->pitch/4 + x;
            *bufp = color;
        }
        break;
    }
}
