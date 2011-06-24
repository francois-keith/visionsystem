#include <visionsystem/imagefill.hpp>
#include <visionsystem/frame.h>
#include <vision/image/image.h>

using namespace std ;
using namespace vision ;
using namespace visionsystem ;


//FIXME a compléter

template<>
void image_fill < Image< unsigned char, MONO > > ( Image<unsigned char,MONO> *img, visionsystem::Frame* frm ) 
{
	
	switch ( frm->_coding ) {

		case VS_MONO8:
				memcpy( img->raw_data, frm->_data, frm->_data_size ) ;
				break ;
		default:
				cerr << "Frame content : " << frm->_coding << endl ;
				throw( string( "Image<unsigned char,MONO> : CONVERSION NOT IMPLEMENTED" ) ) ;
				break ;
	}
}



