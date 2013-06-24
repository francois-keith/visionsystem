#include "controller.h"

#include <iostream>
#include <cstring>

Controller1394::Controller1394( VisionSystem *vs, std::string sandbox )
:Controller( vs, "camdc1394", sandbox), d1394_(0), _cams(0) {

}


Controller1394::~Controller1394() {
}


bool Controller1394::pre_fct( vector< GenericCamera* > &cams ) {

    d1394_ = dc1394_new() ;

    if (!d1394_) {

        std::cerr << "[camdc1394] WARNING : Could not initialize DC1394 System !" <<std::endl ;
        d1394_ = NULL ;
        return false  ;

    } else {

        #ifdef _DEBUG
            std::cout << "[camdc1394] Initializing DC1394 system " <<std::endl ;
        #endif

        dc1394camera_list_t * list ;
        dc1394error_t    err ;

        err = dc1394_camera_enumerate( d1394_, &list ) ;

        if ( list->num == 0 ) {

            #ifdef _DEBUG
                std::cout << "[camdc1394]  No 1394 camera found." <<std::endl ;
            #endif
            return true ;

        } else {

            for ( unsigned int i=0; i<list->num; i++ ) {

                #ifdef _DEBUG
                    std::cout <<"[camdc1394] Found camera " << list->ids[i].guid <<std::endl ;
                #endif

                Camera1394* cam = new Camera1394( d1394_ , list->ids[i].guid ) ;

                std::ostringstream capa_filename;
                capa_filename << get_sandbox() << "/" << list->ids[i].guid << ".capa" ;
                cam->save_capa( capa_filename.str() )  ;

                std::ostringstream conf_filename;
                conf_filename << get_sandbox() << "/" << list->ids[i].guid << ".conf" ;

                try {
                    cam->read_config_file ( conf_filename.str().c_str() ) ;

                } catch ( string msg ) {
                    std::cerr << "[camdc1394] WARNING : Unable to open config file : " << conf_filename.str() <<std::endl ;
                    std::cerr << "[camdc1394] Error message : " << msg << std::endl ;
                    std::cerr << "[camdc1394] trying with default parameters ... " <<std::endl ;
                }

                if ( cam->is_active() )
                if ( !cam->apply_settings() ) {
                    std::cerr << "[camdc1394] ERROR : Could not apply settings to cam " << list->ids[i].guid <<std::endl ;
                    return false ;
                }

                cams.push_back( (GenericCamera*) cam ) ;
                _cams.push_back(cam) ;

            }

            dc1394_camera_free_list ( list ) ;
        }
    }

    return true ;
}


void Controller1394::preloop_fct() {

}


void Controller1394::loop_fct() {

        dc1394error_t err ;
        dc1394video_frame_t *frame = NULL ;
        Frame* vsframe = NULL ;
        for ( size_t i=0; i < _cams.size(); i++ ) {

            if ( _cams[i]->is_active() ) {

                err = dc1394_capture_dequeue ( _cams[i]->get_cam(), DC1394_CAPTURE_POLICY_POLL, &frame ) ;

                if ( err != DC1394_SUCCESS ) {
                    std::cerr << "[camdc1394] Could not dequeue frame" <<std::endl ;
                    exit(0) ;
                }

                if ( frame ) {

                    vsframe = _cams[i]->_buffer.pull() ;
                    _cams[i]->increase_frame();

                    if ( _cams[i]->get_bayer() == 0 )

                            std::memcpy ( vsframe->_data, frame->image, vsframe->_data_size ) ;

                    else {

                        dc1394_bayer_decoding_8bit( frame->image,
                                        vsframe->_data,
                                        frame->size[0],
                                        frame->size[1],
                                        _cams[i]->get_bayer_coding(),
                                        _cams[i]->get_bayer_method() );
                    }

                    _cams[i]->_buffer.push( vsframe ) ;

                    err = dc1394_capture_enqueue ( _cams[i]->get_cam(), frame ) ;

                    if ( err != DC1394_SUCCESS ) {
                        std::cerr << "[camdc1394] Could not enqueue frame" <<std::endl ;
                        exit(0) ;
                    }
                }
            }
        }
}


bool Controller1394::post_fct() {

    #ifdef _DEBUG
        std::cout << "[camdc1394] Entering Post() Function" <<std::endl ;
    #endif

    for ( size_t i=0; i< _cams.size(); i++ ) {
        _cams[i]->stop_cam() ;
        delete ( _cams[i] ) ;
    }

    #ifdef _DEBUG
        std::cout << "[camdc1394] cams are deleted" <<std::endl ;
    #endif


    if ( d1394_ != NULL ) {

        #ifdef _DEBUG
            std::cout << "[camdc1394] Closing DC1394 " <<std::endl ;
        #endif

        dc1394_free( d1394_ ) ;
    }

    #ifdef _DEBUG
        std::cout << "[camdc1394] Exiting Post() Function" <<std::endl ;
    #endif


    return true ;
}

void Controller1394::get_cameras(std::vector< GenericCamera* > & cams)
{
    for(size_t i = 0; i < _cams.size(); ++i)
    {
        cams.push_back(_cams[i]);
    }
}

