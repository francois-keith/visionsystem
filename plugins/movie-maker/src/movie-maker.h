#ifndef _H_MOVIEMAKER_H_
#define _H_MOVIEMAKER_H_

#include <vision/io/MKVEncoder.h>
#include <visionsystem/plugin.h>

#include <boost/thread.hpp>

#include <iomanip>
#include <map>

namespace visionsystem
{

class MovieMaker : public Plugin
{

	public:

		MovieMaker( VisionSystem *vs, std::string sandbox ) ;
		~MovieMaker() ;

		bool pre_fct()  ;
		void preloop_fct() ;
		void loop_fct() ;
		bool post_fct() ;

	private:
        void EncoderThread();

        boost::thread * m_encode_th;
        unsigned int cam_count;
        bool m_close;

        bool m_started;
        unsigned int m_frame;

        std::vector<Camera *> m_cameras;
        std::map< std::string, vision::MKVEncoder *> m_encoders;
        std::vector< std::pair< std::string, vision::Image<uint32_t, vision::RGB> *> > m_images;
} ;

} // namespace visionsystem

PLUGIN( visionsystem::MovieMaker )

#endif
