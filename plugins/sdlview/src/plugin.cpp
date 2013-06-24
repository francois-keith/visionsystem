#ifdef VS_HAS_CONTROLLER_SOCKET
    #include "camerasocket.h"
#endif

#include "plugin.h"

SDLView::SDLView( VisionSystem* core, string sandbox )
:Viewer( core, "sdlview", sandbox ), sdl_running(true)
{
    active_cam = 0 ;
    next_cam = 0 ;
}

SDLView::~SDLView() {
    sdl_th->join();
    delete sdl_th;
}


bool  SDLView::pre_fct() {
    cameras = get_all_cameras() ;

    active_cam = -1 ;
    for ( int i=0; i<cameras.size(); i++ )
        if ( cameras[i]->is_active() )
            active_cam = i ;

    next_cam = active_cam ;

    if ( active_cam == -1 )    {
        cerr << "[glview] ERROR : No cam is active. " << endl ;
        return false ;
    }

    register_to_cam< Image<uint32_t, RGB> >( cameras[active_cam], 10 ) ;

    return true ;
}

void  SDLView::preloop_fct() {
    sdl_th = new boost::thread(boost::bind(&SDLView::SDL_Loop, this));
}

void  SDLView::loop_fct() {

    img = dequeue_image< Image<uint32_t, RGB> >( cameras[active_cam] ) ;

    if(sdl_running && !refresh)
    {
        if ( active_cam != next_cam ) {
            enqueue_image< Image<uint32_t, RGB> >( cameras[active_cam], img ) ;
            unregister_to_cam< Image<uint32_t, RGB> >( cameras[active_cam] ) ;
            register_to_cam< Image<uint32_t, RGB> >( cameras[next_cam], 10 ) ;
            active_cam = next_cam ;
            img = dequeue_image< Image<uint32_t, RGB> >( cameras[active_cam] ) ;
        }
        refresh = true;
        unsigned int cnt = 0;
        while(refresh) { cnt++; if(cnt > 10) { break; } usleep(1000); }
    }
    enqueue_image< Image<uint32_t, RGB> >( cameras[active_cam], img ) ;
}

bool  SDLView::post_fct() {
    unregister_to_cam< Image<uint32_t, RGB> >( cameras[active_cam] ) ;
    SDL_Quit();
    return true ;
}

void SDLView::gl_print ( ImageRef position, string text ) {
}

void SDLView::refresh_screen()
{
    // Draw video
    if ( SDL_MUSTLOCK(screen) ) {
        if ( SDL_LockSurface(screen) < 0 ) {
            return;
        }
    }
    memcpy(sdl_img->pixels, img->raw_data, img->data_size);
    SDL_BlitSurface(sdl_img, NULL, screen, NULL);
    SDL_Flip(screen);
    if ( SDL_MUSTLOCK(screen) ) {
        SDL_UnlockSurface(screen);
    }
}

#if    SDL_BYTEORDER == SDL_BIG_ENDIAN
    uint32_t rmask = 0xff000000;
    uint32_t gmask = 0x00ff0000;
    uint32_t bmask = 0x0000ff00;
    uint32_t amask = 0x000000ff;
#else
    uint32_t rmask = 0x000000ff;
    uint32_t gmask = 0x0000ff00;
    uint32_t bmask = 0x00ff0000;
    uint32_t amask = 0xff000000;
#endif

void SDLView::SDL_Loop()
{
    SDL_Init(SDL_INIT_VIDEO);
    screen = SDL_SetVideoMode(cameras[active_cam]->get_size().x, cameras[active_cam]->get_size().y, 32, SDL_SWSURFACE);
    sdl_img = SDL_CreateRGBSurface(SDL_SWSURFACE, cameras[active_cam]->get_size().x, cameras[active_cam]->get_size().y, 32, rmask, gmask, bmask, amask);
    SDL_SetAlpha(sdl_img, 0, 0xFF);
    SDL_WM_SetCaption("SDLView", NULL);
    sdl_running = true;

    while(sdl_running)
    {
        if(refresh)
        {
            refresh_screen();
            refresh = false;
        }

        // Handle events
        #ifdef VS_HAS_CONTROLLER_SOCKET
        CameraSocket * current_cam = 0;
        #endif
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
                        case SDLK_ESCAPE: // Appui sur la touche Echap, on arrête le programme
                            std::cout << "[SDLView] Exit requested ... " << endl ;
                            whiteboard_write< bool >( string("core_stop"), true ) ;
                            sdl_running = false;
                            break;
                        #ifdef VS_HAS_CONTROLLER_SOCKET
                        case SDLK_SPACE:
                            current_cam = dynamic_cast<CameraSocket*>(cameras[active_cam]);
                            if(current_cam && current_cam->from_stream())
                            {
                                current_cam->next_cam();
                            }
                            break;
                        #endif
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
    }
}
