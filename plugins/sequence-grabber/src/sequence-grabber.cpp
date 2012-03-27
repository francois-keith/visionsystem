#include "sequence-grabber.h"

#include <vision/io/imageio.h>
#include <boost/filesystem.hpp>
#include <iostream>

using namespace boost::filesystem ;

SequenceGrabber::SequenceGrabber( VisionSystem *vs, string sandbox )
:Plugin( vs, "sequence-grabber", sandbox ), m_close(false), m_cameras(0), m_images(0)
{
}

SequenceGrabber::~SequenceGrabber() {
}

bool SequenceGrabber::pre_fct() {

	string filename = get_sandbox() + string ( "/sequence-grabber.conf") ;
	try {
		read_config_file( filename.c_str() ) ; 
	} catch ( string msg ) {
		
		cout << "[sequence-grabber] Could not read config file" << endl ;
		cout << "[sequence-gragrab] Will grab all active cameras" << endl ;

		vector<Camera*> cams = get_all_cameras() ;

		for (int i=0; i<cams.size(); i++ ) {
			if ( cams[i]->is_active() ) {
                m_cameras.push_back(cams[i]);
			}
		}


	}

	return true ;
}

void SequenceGrabber::preloop_fct() {

	for ( unsigned int i=0; i< m_cameras.size(); ++i )
    {
		register_to_cam< Image<unsigned char, vision::MONO> > ( m_cameras[i], 50 ) ;
    }
    m_save_th = new boost::thread(boost::bind(&SequenceGrabber::save_images_loop, this));
}

void SequenceGrabber::loop_fct() {

    for(unsigned int i = 0; i < m_cameras.size(); ++i)
    {
        vision::Image<unsigned char, vision::MONO> * img = dequeue_image< Image<unsigned char, vision::MONO> > (m_cameras[i]);

        m_images.push_back(make_pair(m_cameras[i]->get_name(), new vision::Image<unsigned char, vision::MONO>(*img)));

        enqueue_image(m_cameras[i], img);
    }
}

bool SequenceGrabber::post_fct() {
    for(unsigned int i = 0; i < m_cameras.size(); ++i)
    {
        unregister_to_cam< vision::Image<unsigned char, vision::MONO> >(m_cameras[i]);	
	}
    m_close = true;
    m_save_th->join();
    delete m_save_th;
	return true ;
}


void SequenceGrabber::parse_config_line( vector<string> &line ) {

	// FIXME
}

void SequenceGrabber::save_images_loop()
{
    unsigned int frame = 0;
    for(size_t i = 0; i < m_cameras.size(); ++i)
    {
        path camera_path = path(get_sandbox()) / path(m_cameras[i]->get_name())  ;
        if ( exists(camera_path) )
        {
            remove_all(camera_path);
        }
        create_directory(camera_path);
    }

    while(!m_close)
    {
        while(!m_close && m_images.size() < m_cameras.size() * (frame + 1))
        {
            usleep(1000);
        }
        unsigned int n = m_images.size();
        for(size_t i = m_cameras.size() * frame; i < n; ++i)
        {
            std::stringstream filename;
            filename << get_sandbox()  << "/" << m_images[i].first << "/" << frame << ".bin";
            serialize(filename.str(), *(m_images[i].second));
            delete m_images[i].second;
            if(i % m_cameras.size() == m_cameras.size() - 1)
            {
                frame++;
            }
        }
    }
}

