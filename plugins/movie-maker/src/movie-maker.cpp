#include "movie-maker.h"

#include <iomanip>
#include <vision/io/imageio.h>
#include <boost/filesystem.hpp>
#include <iostream>

using namespace boost::filesystem ;

namespace visionsystem
{

MovieMaker::MovieMaker( VisionSystem *vs, std::string sandbox )
:Plugin( vs, "movie-maker", sandbox ),
 m_encode_th(0),
 cam_count(0),
 m_close(false), m_started(false),
 m_frame(0),
 m_cameras(0), m_encoders(), m_images(0)
{
}

MovieMaker::~MovieMaker() {
}

bool MovieMaker::pre_fct() {

    /* Grab all cameras */
    std::vector<Camera*> cams = get_all_cameras() ;
    for(size_t i = 0; i < cams.size(); ++i)
    {
        if(cams[i]->is_active())
        {
            switch(cams[i]->get_coding())
            {
                case VS_MONO8:
                case VS_MONO16:
                case VS_DEPTH8:
                case VS_DEPTH16:
                case VS_DEPTH24:
                case VS_DEPTH32:
                    break;
                /* Default cases (include RGB/YUV variations and unhandled like IR */
                default:
                    m_cameras.push_back(cams[i]);
                    m_encoders[cams[i]->get_name()+".mkv"] = new vision::MKVEncoder(cams[i]->get_name()+".mkv", cams[i]->get_size().x, cams[i]->get_size().y, cams[i]->get_fps());
                    cam_count++;
                    break;
            }
        }
    }

    return true ;
}

void MovieMaker::preloop_fct() {

    for ( unsigned int i=0; i< m_cameras.size(); ++i )
    {
        register_to_cam< vision::Image<uint32_t, vision::RGB> > ( m_cameras[i], 50 ) ;
    }
    m_encode_th = new boost::thread(boost::bind(&MovieMaker::EncoderThread, this));
}

void MovieMaker::loop_fct() {

    if(m_started)
    {
        for(unsigned int i = 0; i < m_cameras.size(); ++i)
        {
            vision::Image<uint32_t, vision::RGB> * img = 0;
            img = dequeue_image<vision::Image<uint32_t, vision::RGB> > (m_cameras[i]);
            m_images.push_back(make_pair(m_cameras[i]->get_name()+".mkv", new vision::Image<uint32_t, vision::RGB>(*img)));
            enqueue_image(m_cameras[i], img);
        }
    }
    else
    {
        for(unsigned int i = 0; i < m_cameras.size(); ++i)
        {
            vision::Image<uint32_t, vision::RGB> * img = 0;
            img = dequeue_image<vision::Image<uint32_t, vision::RGB> > (m_cameras[i]);
            enqueue_image(m_cameras[i], img);
        }
        m_frame++;
    }
}

bool MovieMaker::post_fct() {
    for ( unsigned int i=0; i< m_cameras.size(); ++i )
    {
        unregister_to_cam<vision::Image<uint32_t, vision::RGB> > ( m_cameras[i] ) ;
    }
    m_close = true;
    if(m_encode_th)
    {
        m_encode_th->join();
        delete m_encode_th;
        m_encode_th = 0;
    }
    for(std::map<std::string, vision::MKVEncoder *>::iterator it = m_encoders.begin(); it != m_encoders.end(); ++it)
    {
        delete it->second;
    }
    return true ;
}

void MovieMaker::EncoderThread()
{
    unsigned int treated_frames = m_images.size()/cam_count;
    m_started = true;
    while(!m_close)
    {
        while(!m_close && m_images.size() < cam_count * (treated_frames + 1))
        {
            usleep(1000);
        }
        unsigned int n = m_images.size();
        for(size_t i = cam_count * treated_frames; i < n; ++i)
        {
            m_encoders[m_images[i].first]->EncodeFrame(*m_images[i].second);
            delete m_images[i].second;
            if(i % cam_count == cam_count - 1)
            {
                m_frame++;
                treated_frames++;
            }
        }
    }
}

} // namespace visionsystem

