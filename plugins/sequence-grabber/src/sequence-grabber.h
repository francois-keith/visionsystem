#ifndef SEQUENCE_GRABBER_H
#define SEQUENCE_GRABBER_H

#include <configparser/configparser.h>

#include <vision/image/image.h>
#include <visionsystem/plugin.h>

#include <boost/thread.hpp>

#include <XmlRpc.h>

using namespace std;
using namespace vision;
using namespace visionsystem;
using namespace configparser;
using namespace XmlRpc;

class SequenceGrabber : public Plugin, public WithConfigFile, public XmlRpcServerMethod 
{

	public:
		
		SequenceGrabber( VisionSystem *vs, string sandbox ) ;
		~SequenceGrabber() ;

		bool pre_fct()  ;
		void preloop_fct() ;
		void loop_fct() ;
		bool post_fct() ;

        /* Method for XML-RPC call */
        /* Params should be start/stop string */
        void execute(XmlRpcValue & params, XmlRpcValue & result); 

	private:
		void parse_config_line( vector<string> &line ) ;

        void save_images_loop_mono();
        void save_images_loop_rgb();

        boost::thread * m_save_th; 
        bool m_close;

        bool m_started;
        unsigned int m_frame;

        bool is_mono;

        std::vector< Camera * > m_cameras;
        std::vector< std::pair< std::string, vision::Image<unsigned char, vision::MONO> *> > m_images_mono; 
        std::vector< std::pair< std::string, vision::Image<uint32_t, vision::RGB> *> > m_images_rgb; 
} ;



PLUGIN( SequenceGrabber ) 

#endif
