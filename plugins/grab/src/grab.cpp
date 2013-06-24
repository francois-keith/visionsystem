#include "grab.h"

#include <vision/io/imageio.h>
#include <boost/filesystem.hpp>
#include <iostream>

using namespace boost::filesystem ;

Grab::Grab( VisionSystem *vs, string sandbox )
: Plugin( vs, "grab", sandbox ), WithViewer( vs ),
  captured_frames(0), saved_frames(0),
  grab_next(false), grabbed_rgb(0), grabbed_depth(0)
{
}

Grab::~Grab() {
    for(size_t i = 0; i < grabbed_depth.size(); ++i)
    {
        delete grabbed_depth[i];
    }
    for(size_t i = 0; i < grabbed_rgb.size(); ++i)
    {
        delete grabbed_rgb[i];
    }
}

bool Grab::pre_fct() {

    string filename = get_sandbox() + string ( "/grab.conf") ;
    try {
        read_config_file( filename.c_str() ) ;
    } catch ( string msg ) {

        cout << "[grab] Could not read config file" << endl ;
        cout << "[grab] Will grab all active cameras" << endl ;

        vector<Camera*> cams = get_all_cameras() ;

        for (int i=0; i<cams.size(); ++i ) {
            if ( cams[i]->is_active() ) {
                Registration< vision::Image<uint32_t, vision::RGB> > * reg_rgb = 0;
                Registration< vision::Image<uint16_t, vision::DEPTH> > * reg_depth = 0;
                switch(cams[i]->get_coding())
                {
                    case VS_DEPTH8:
                    case VS_DEPTH16:
                    case VS_DEPTH32:
                        reg_depth = new Registration< vision::Image<uint16_t, vision::DEPTH> >(cams[i]);
                        grabbed_depth.push_back(reg_depth);
                        std::cout << "[grab] will grab depth on " << cams[i]->get_name() << std::endl ;
                        break;
                    default:
                        reg_rgb = new Registration< vision::Image<uint32_t, vision::RGB> > (cams[i]);
                        grabbed_rgb.push_back(reg_rgb);
                        std::cout << "[grab] will grab on " << cams[i]->get_name() << std::endl ;
                        break;
                }
            }
        }


    }

    Viewer* vw = whiteboard_read< Viewer* >( string("viewer")) ;

    if ( vw == NULL ) {
        cout << "[grab] No viewer found" << endl ;
        return true ;
    }

    register_callback() ;

    return true ;
}

void Grab::preloop_fct() {

    for ( size_t i = 0; i < grabbed_depth.size(); ++i )
    {
        register_to_cam< Image<uint16_t, vision::DEPTH> >(grabbed_depth[i]->camera, 10);
    }

    for ( size_t i = 0; i < grabbed_rgb.size(); ++i )
    {
        register_to_cam< Image<uint32_t, RGB> > ( grabbed_rgb[i]->camera, 10 ) ;
    }

}

void Grab::loop_fct() {

    for ( size_t i = 0; i < grabbed_depth.size(); ++i )
    {
        grabbed_depth[i]->current_frame = dequeue_image< Image<uint16_t, vision::DEPTH> >(grabbed_depth[i]->camera);
    }

    for ( size_t i = 0; i < grabbed_rgb.size(); ++i )
    {
        grabbed_rgb[i]->current_frame = dequeue_image< Image<uint32_t, RGB> > ( grabbed_rgb[i]->camera ) ;
    }

    if ( grab_next )
    {
        for ( size_t i=0; i<grabbed_depth.size(); ++i )
        {
            grabbed_depth[i]->grabbed_frames.push_back( new vision::Image<uint16_t, vision::DEPTH>(*grabbed_depth[i]->current_frame) ) ;
            captured_frames++;
        }
        for ( size_t i=0; i<grabbed_rgb.size(); ++i )
        {
            grabbed_rgb[i]->grabbed_frames.push_back( new vision::Image<uint32_t, vision::RGB>(*grabbed_rgb[i]->current_frame) ) ;
            captured_frames++;
        }
        grab_next = false ;

    }
    for ( size_t i=0; i < grabbed_depth.size(); ++i )
    {
        enqueue_image< Image<uint16_t, DEPTH> > ( grabbed_depth[i]->camera, grabbed_depth[i]->current_frame ) ;
    }
    for ( size_t i=0; i < grabbed_rgb.size(); ++i )
    {
        enqueue_image< Image<uint32_t, RGB> > ( grabbed_rgb[i]->camera, grabbed_rgb[i]->current_frame ) ;
    }
}

bool Grab::post_fct() {
    for ( size_t i = 0; i < grabbed_depth.size(); ++i)
    {
        dump2disk(*grabbed_depth[i]);
    }
    for ( size_t i = 0; i < grabbed_rgb.size(); ++i)
    {
        dump2disk(*grabbed_rgb[i]);
    }
    std::cout << std::endl;
    return true ;
}


void Grab::parse_config_line( vector<string> &line ) {

    // FIXME
}


void Grab::callback(Camera* cam, XEvent event) {
#ifndef WIN32
#ifndef __CYGWIN__
    if (event.type == KeyPress ) {

        switch (XLookupKeysym(&event.xkey, 0)) {

            case XK_g:
                cout << "[grab] Grabing ... " << endl ;
                grab_next = true ;
                break;

            default:
                break ;
        }

    }
#endif
#endif
}

void Grab::sdl_callback(Camera * cam, SDL_Event event)
{
#if VS_HAS_SDL == 1
    switch(event.type)
    {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
                case SDLK_g: /* Appui sur la touche Echap, on arrête le programme */
                    cout << "[grab] Grabing ... " << endl ;
                    grab_next = true ;
                    break;
            }
            break;
        default:
            break;
    }
#endif
}

void Grab::glfunc(Camera* cam) {

}

