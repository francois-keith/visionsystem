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
				throw( string( "Image<unsigned char,MONO> : CONVERSION NOT IMPLEMENTED" ) ) ;
				break ;
	}
}



template<>
void image_fill < Image< uint32_t, RGB > > ( Image<unsigned char,MONO> *img, visionsystem::Frame* frm ) 
{
	
	switch ( frm->_coding ) {

		case VS_MONO8:
		
		case VS_RGB8:
				register int i ;
				for (i=0; i<img->data_size; i++ )
					img->raw_data[i] = frm->_data[3*i] << 16 | frm->_data[3*i+1] << 8 | frm->_data[3*i+2] ;			//FIXME a tester

		default:
				throw( string( "Image<uint32_t,RGB> : CONVERSION NOT IMPLEMENTED" ) ) ;
				break ;
	}
}

