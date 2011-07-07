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
	register unsigned int i ;

	switch ( frm->_coding ) {

		case VS_YUV422_YUYV:

				for (i=0; i< img->pixels; i+=2 ) {
				
					img->raw_data[i]   = frm->_data[ 2*i ] ;
					img->raw_data[i+1] = frm->_data[ 2*i + 2 ] ;
				
				}
				break ;
		
		case VS_MONO8:
				memcpy( img->raw_data, frm->_data, frm->_data_size ) ;
				break ;
		
		case VS_RGB24:
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


		case VS_YUV422_YUYV: 

				unsigned char R1,G1,B1 ;
				unsigned char R2,G2,B2 ;
				
				int y1, y2, u, v ;
	 			int ug_plus_vg, ub, vr ;
				int r,g,b;			


				for (i=0; i< img->pixels; i+=2 ) {
			
					// This was inspired from libcvd source code

					y1  = frm->_data[ 2*i + 0 ] << 8 ;
					u   = frm->_data[ 2*i + 1 ] - 128 ;
					y2  = frm->_data[ 2*i + 2 ] << 8 ;
					v   = frm->_data[ 2*i + 3 ] - 128 ;
					
					ug_plus_vg = u * 88 + v * 183;
			                ub = u * 454;
			                vr = v * 359;
					
					r = (y1 + vr) >> 8;
					g = (y1 - ug_plus_vg) >> 8;
					b = (y1 + ub) >> 8;
					
					R1 = r < 0 ? 0 : ( r > 255 ? 255 : (unsigned char) r );
					G1 = g < 0 ? 0 : ( g > 255 ? 255 : (unsigned char) g );
					B1 = b < 0 ? 0 : ( b > 255 ? 255 : (unsigned char) b );

					r = (y2 + vr) >> 8;
					g = (y2 - ug_plus_vg) >> 8;
					b = (y2 + ub) >> 8;
		
					R2 = r < 0 ? 0 : ( r > 255 ? 255 : (unsigned char) r );
					G2 = g < 0 ? 0 : ( g > 255 ? 255 : (unsigned char) g );
					B2 = b < 0 ? 0 : ( b > 255 ? 255 : (unsigned char) b );

					img->raw_data[i] =  ( (uint32_t) B1 ) << 16  |
							    ( (uint32_t) G1 ) << 8 | 
							    ( (uint32_t) R1 ) ;  
				
					img->raw_data[i+1] =  ( (uint32_t) B2 ) << 16  |
							      ( (uint32_t) G2 ) << 8 | 
							      ( (uint32_t) R2 ) ;  
				}

				break ;
		

		case VS_MONO8:
				for (i=0; i<img->pixels; i++ )
					img->raw_data[i] =  ( (uint32_t) frm->_data[i] ) << 16  |
							    ( (uint32_t) frm->_data[i] ) << 8 | 
							    ( (uint32_t) frm->_data[i] ) ;  
				break ;
		
		case VS_RGB24:
				for (i=0; i<img->pixels; i++ )
					img->raw_data[i] =  ( (uint32_t) frm->_data[3*i+2] ) << 16  |
							    ( (uint32_t) frm->_data[3*i+1] ) << 8 | 
							    ( (uint32_t) frm->_data[3*i] ) ;  
				break ;

		case VS_RGB32:
				for (i=0; i<img->pixels; i++ )
					img->raw_data[i] =  ( (uint32_t) frm->_data[4*i+2] ) << 16  |
							    ( (uint32_t) frm->_data[4*i+1] ) << 8 | 
							    ( (uint32_t) frm->_data[4*i] ) ;  
				break ;
		
		default:
				throw( string( "Image<uint32_t,RGB> : CONVERSION NOT IMPLEMENTED" ) ) ;
				break ;
	}

}



