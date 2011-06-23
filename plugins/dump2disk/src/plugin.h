#include <visionsystem/plugin.h>
#include <vision/image/image.h>
#include <visionsystem/buffer.h>

using namespace std ;
using namespace vision ;
using namespace visionsystem ;

class Dump2Disk : public Plugin
{
	public:

		Dump2Disk( VisionSystem *vs, std::string sandbox ) ;
		~Dump2Disk() ;

		bool pre_fct() ;
		void loop_fct() ;
		bool post_fct() ;
		
	private:

		Camera*					_cam ;
		Buffer< Image<unsigned char, MONO> >	_buffer ;
		bool _done ;
} ;

PLUGIN(Dump2Disk)



