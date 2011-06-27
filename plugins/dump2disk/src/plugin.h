#include <visionsystem/plugin.h>
#include <vision/image/image.h>
#include <visionsystem/buffer.h>

using namespace std ;
using namespace vision ;
using namespace visionsystem ;

//TODO	penser a ajouter gestion fichier de config

class Dump2Disk : public Plugin
{
	public:

		Dump2Disk( VisionSystem *vs, std::string sandbox ) ;
		~Dump2Disk() ;

		bool pre_fct() ;
		void preloop_fct() ;
		void loop_fct() ;
		bool post_fct() ;
		
	private:

		Camera*					_cam ;
		Buffer< Image<unsigned char, MONO> >	_buffer ;
} ;

PLUGIN(Dump2Disk)



