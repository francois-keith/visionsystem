#ifndef GRAB_H
#define GRAB_H

#include <configparser/configparser.h>

#include <visionsystem/config.h>
#if VS_HAS_SDL == 1
    #include <SDL/SDL_events.h>
#else
    typedef union SDL_Event
    {} SDL_Event;
#endif

#include <vision/image/image.h>
#include <visionsystem/plugin.h>
#include <visionsystem/viewer.h>

#include <sstream>
#include <iomanip>
#include <boost/filesystem.hpp>

using namespace std ;
using namespace vision ;
using namespace visionsystem ;
using namespace configparser ;
using namespace boost::filesystem ;

class Grab : public Plugin, public WithViewer, public WithConfigFile {

    public:

        Grab( VisionSystem *vs, string sandbox ) ;
        ~Grab() ;

        bool pre_fct()  ;
        void preloop_fct() ;
        void loop_fct() ;
        bool post_fct() ;

    private:

        void callback( Camera* cam, XEvent event ) ;
        void sdl_callback(Camera * cam, SDL_Event event);
        void glfunc( Camera* cam ) ;

        void parse_config_line( vector<string> &line ) ;


        template<typename TImage>
        class Registration {
        public:
            Registration() : camera(0), current_frame(0), grabbed_frames(0) {}
            Registration(Camera * cam) : camera(cam), current_frame(0), grabbed_frames(0) {}

            ~Registration()
            {
                for(size_t i = 0; i < grabbed_frames.size(); ++i)
                {
                    delete grabbed_frames[i];
                }
            }
        public:
            Camera* camera ;
            TImage *current_frame ;
            vector< TImage* > grabbed_frames ;
        } ;

        template<typename TImage>
        void dump2disk(Registration<TImage> & reg)
        {
            if(!reg.camera) return;
            path camera_path = path( get_sandbox() ) / path( reg.camera->get_name() )  ;

            if ( exists( camera_path ) )
            {
                remove_all( camera_path ) ;
            }

            create_directory ( camera_path ) ;

            for ( size_t i = 0; i < reg.grabbed_frames.size(); ++i )
            {
                std::ostringstream filename;
                filename << camera_path.string() << "/" << std::setfill('0') << std::setw(6) << i << ".png" ;
                save_color ( filename.str() , reg.grabbed_frames[i] ) ;
                std::ostringstream binname;
                binname << camera_path.string() << "/" << std::setfill('0') << std::setw(6) << i << ".bin" ;
                serialize(binname.str(), *reg.grabbed_frames[i]);
                saved_frames++;
                std::cout << "\r[grab] Saved " << saved_frames << "/" << captured_frames << " (last saved " << filename.str() << ")" << std::flush;
            }
        }

        unsigned int captured_frames;
        unsigned int saved_frames;

        bool grab_next ;            // should next frames be grabbed ?
        vector< Registration< Image<uint32_t, RGB> >* >    grabbed_rgb ;
        vector< Registration< Image<uint16_t, DEPTH> >* > grabbed_depth;
} ;



PLUGIN( Grab )

#endif
