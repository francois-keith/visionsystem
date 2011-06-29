#include <visionsystem/imagefill.hpp>
#include <visionsystem/frame.h>
#include <vision/image/image.h>
#include <vision/io/pixelcoding.h>
#include <stdint.h>

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
				register unsigned int i ;
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
void image_fill < Image< uint32_t, RGB > > ( Image<uint32_t,RGB> *img, visionsystem::Frame* frm ) 
{
	
	register unsigned int i ;
	
	switch ( frm->_coding ) {

		case VS_MONO8:
				for (i=0; i<img->width*img->height; i++ )
					img->raw_data[i] =  ( (uint32_t) frm->_data[3*i] ) << 16  |
							    ( (uint32_t) frm->_data[3*i] ) << 8 | 
							    ( (uint32_t) frm->_data[3*i] ) ;  
				break ;
		
		case VS_RGB8:
				for (i=0; i<img->width*img->height; i++ )
					img->raw_data[i] =  ( (uint32_t) frm->_data[3*i+2] ) << 16  |
							    ( (uint32_t) frm->_data[3*i+1] ) << 8 | 
							    ( (uint32_t) frm->_data[3*i] ) ;  
				break ;
		default:
				throw( string( "Image<uint32_t,RGB> : CONVERSION NOT IMPLEMENTED" ) ) ;
				break ;
	}
}

