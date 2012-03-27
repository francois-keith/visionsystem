#ifndef SEQUENCE_GRABBER_H
#define SEQUENCE_GRABBER_H

#include <configparser/configparser.h>

#include <vision/image/image.h>
#include <visionsystem/plugin.h>

#include <boost/thread.hpp>

using namespace std ;
using namespace vision ;
using namespace visionsystem ;
using namespace configparser ;

class SequenceGrabber : public Plugin, public WithConfigFile {

	public:
		
		SequenceGrabber( VisionSystem *vs, string sandbox ) ;
		~SequenceGrabber() ;

		bool pre_fct()  ;
		void preloop_fct() ;
		void loop_fct() ;
		bool post_fct() ;

	private:
		void parse_config_line( vector<string> &line ) ;

        void save_images_loop();

        boost::thread * m_save_th; 
        bool m_close;

        std::vector< Camera * > m_cameras;
        std::vector< std::pair< std::string, vision::Image<unsigned char, vision::MONO> *> > m_images; 
} ;



PLUGIN( SequenceGrabber ) 

#endif
