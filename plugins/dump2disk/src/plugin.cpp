#include <vision/io/imageio.h>
#include "plugin.h"

using namespace vision ;


Dump2Disk::Dump2Disk( VisionSystem *vs, std::string sandbox ) 
: Plugin ( vs, "dump2disk", sandbox ) {

	_done = false ;
}


Dump2Disk::~Dump2Disk() {

}


bool Dump2Disk::pre_fct() {

	_cam = get_default_camera() ;
	register_to_cam< Image<unsigned char, MONO> >( _cam, 10 ) ;

	for ( int i=0; i < 1000; i++ )
		_buffer.enqueue( new Image<unsigned char, MONO>( _cam->get_size() ) ) ;	

	return true ;

}

void Dump2Disk::loop_fct() {

	Image<unsigned char, MONO> *img ;
	img = dequeue_image< Image<unsigned char,MONO> > ( _cam ) ;
	
	
	if ( !_buffer.is_full() ) {

		Image<unsigned char, MONO> *cpy ;
		cpy = _buffer.pull() ;

		// Copier image ici

		_buffer.push( cpy ) ;
	
	}


	enqueue_image< Image<unsigned char, MONO> >( _cam, img ) ;

}


bool Dump2Disk::post_fct() {

	
	unregister_to_cam< Image<unsigned char, MONO> >( _cam ) ;
	
	int i=0 ;
	char buffer[255] ;
	Image< unsigned char, MONO > *img ;

	while ( _buffer.size() != 0 ) {
		sprintf( buffer, "%d.png", i )	;
		img = _buffer.nbl_dequeue() ;
		save_mono< unsigned char, MONO > ( buffer, img ) ;
		delete ( img ) ;
		i++ ;
	}
	
	return true ;

}
		

