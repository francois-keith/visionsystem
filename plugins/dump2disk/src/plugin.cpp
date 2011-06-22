#include <vision/image/image.h>
#include <vision/io/imageio.h>
#include "plugin.h"

using namespace vision ;


Dump2Disk::Dump2Disk( VisionSystem *vs, std::string sandbox ) 
: Plugin ( vs, "dump2disk", sandbox ) {

}


Dump2Disk::~Dump2Disk() {

}


bool Dump2Disk::pre_fct() {

	cam = get_default_camera() ;
	register_to_cam< Image<unsigned char, MONO> >( cam, 100 ) ;

	return true ;

}

void Dump2Disk::loop_fct() {

	Image<unsigned char, MONO> *tmp ;

	tmp = dequeue_image< Image<unsigned char,MONO> > ( cam ) ;

		save_mono<unsigned char, MONO> ( "test.png", tmp ) ;

	enqueue_image< Image<unsigned char, MONO> >( cam, tmp ) ;

}


bool Dump2Disk::post_fct() {

	unregister_to_cam< Image<unsigned char, MONO> > ( cam ) ;

	return true ;

}
		

