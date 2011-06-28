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
		case VS_RGB8:
				register int i ;
				int tmp ;
				for (i=0; i< img->data_size; i++ ) {
				 	tmp = frm->_data[3*i] + frm->_data[3*i+1] + frm->_data[3*i+2] ;
				 	img->raw_data[i] = (unsigned char) ( tmp / 3 ) ;
				}



				break ;
		default:
				cerr << "Frame content : " << frm->_coding << endl ;
				throw( string( "Image<unsigned char,MONO> : CONVERSION NOT IMPLEMENTED" ) ) ;
				break ;
	}
}



