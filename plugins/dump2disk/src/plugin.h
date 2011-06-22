#include <visionsystem/plugin.h>

using namespace std ;
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

		Camera*		cam ;
} ;

PLUGIN(Dump2Disk)



