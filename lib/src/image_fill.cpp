#include <visionsystem/imagefill.hpp>
#include <visionsystem/frame.h>
#include <vision/image/image.h>
#include <vision/io/pixelcoding.h>

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
        case VS_RGB32:
                uint32_t grey;
                for( i = 0; i < img->pixels; ++i)
                {
                    grey = frm->_data[4*i] + frm->_data[4*i+1] + frm->_data[4*i+2];
                    grey /= 3;
                    img->raw_data[i] = (unsigned char)grey;
                }
                break;
        case VS_RAW:
                memcpy(img->raw_data, frm->_data, frm->_data_size);
                img->data_size = frm->_data_size;
                break;
		default:
				throw( string( "Image<unsigned char,MONO> : CONVERSION NOT IMPLEMENTED" ) ) ;
				break ;
	}
}

inline void __YUV422_YUYV_TO_RGB__(unsigned char * data, unsigned char * rgb_out)
{
    int y1, y2, u, v ;
    int ug_plus_vg, ub, vr ;
    int r,g,b;

    // This was inspired from libcvd source code

    y1  = data[0] << 8 ;
    u   = data[1] - 128 ;
    y2  = data[2] << 8 ;
    v   = data[3] - 128 ;

    ug_plus_vg = u * 88 + v * 183;
            ub = u * 454;
            vr = v * 359;

    r = (y1 + vr) >> 8;
    g = (y1 - ug_plus_vg) >> 8;
    b = (y1 + ub) >> 8;

    rgb_out[0] = r < 0 ? 0 : ( r > 255 ? 255 : (unsigned char) r );
    rgb_out[1] = g < 0 ? 0 : ( g > 255 ? 255 : (unsigned char) g );
    rgb_out[2] = b < 0 ? 0 : ( b > 255 ? 255 : (unsigned char) b );

    r = (y2 + vr) >> 8;
    g = (y2 - ug_plus_vg) >> 8;
    b = (y2 + ub) >> 8;

    rgb_out[3] = r < 0 ? 0 : ( r > 255 ? 255 : (unsigned char) r );
    rgb_out[4] = g < 0 ? 0 : ( g > 255 ? 255 : (unsigned char) g );
    rgb_out[5] = b < 0 ? 0 : ( b > 255 ? 255 : (unsigned char) b );
}

template<>
void image_fill < Image< uint32_t, RGB > > ( Image<uint32_t,RGB> *img, visionsystem::Frame* frm ) 
{
	
	register unsigned int i ;
	
	switch ( frm->_coding ) {


		case VS_YUV422_YUYV: 

				unsigned char rgb_out[6];

				for (i=0; i< img->pixels; i+=2 ) {
			
                    __YUV422_YUYV_TO_RGB__(&(frm->_data[ 2*i ]), rgb_out);

					img->raw_data[i] =  ( (uint32_t) rgb_out[2] ) << 16  |
							    ( (uint32_t) rgb_out[1] ) << 8 |
							    ( (uint32_t) rgb_out[0] ) ;
				
					img->raw_data[i+1] =  ( (uint32_t) rgb_out[5] ) << 16  |
							      ( (uint32_t) rgb_out[4] ) << 8 |
							      ( (uint32_t) rgb_out[3] ) ;
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

		
		case VS_DEPTH16:

				for ( int i=0; i<img->pixels; i++ ) {

					uint16_t *depth = (uint16_t*) frm->_data ;
					uint8_t R,G,B ;
					R = G = B = (uint8_t) ( ((float) depth[i]) / 2048.0 * 255.0) ;

					img->raw_data[i] =  ( (uint32_t) B ) << 16  |
							    ( (uint32_t) G ) << 8 | 
							    ( (uint32_t) R ) ;  
				}	


				break ;

        case VS_RAW:
                memcpy(img->raw_data, frm->_data, frm->_data_size);
                img->data_size = frm->_data_size;
                break;
		default:
				throw( string( "Image<uint32_t,RGB> : CONVERSION NOT IMPLEMENTED" ) ) ;
				break ;
	}

}

inline void __RGB32_TO_HSV__(unsigned char * data, uint16_t & h, uint8_t & s, uint8_t & v)
{
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

    uint8_t rgb_min = 0;
    uint8_t rgb_max = 255;

    r = data[0];
    g = data[1];
    b = data[2];
    h = 0;
    s = 0;
    rgb_min = min(min(r,g),b);
    rgb_max = max(max(r,g),b);
    v = rgb_max;
    if( v != 0 )
    {
        s = 255*( rgb_max - rgb_min )/v;
        if( s == 0 )
        {
            h = 0;
        }
        else
        {
            int h_in = 0;
            if( rgb_max == r )
            {
                h_in = 0 + 60*(g - b)/(rgb_max - rgb_min);
            }
            else if( rgb_max == g )
            {
                h_in = 120 + 60*(b - r)/(rgb_max - rgb_min);
            }
            else
            {
                h_in = 240 + 60*(r - g)/(rgb_max - rgb_min);
            }
            if(h_in < 0)
            {
                h_in += 360;
            }
            h = h_in;
        }
    }
    else
    {
        h = 0;
        s = 0;
    }
}

template<>
void image_fill < Image< uint32_t, HSV > > ( Image<uint32_t,HSV> *img, visionsystem::Frame* frm ) 
{
	
	register unsigned int i ;
    uint16_t h = 0;
    uint8_t s = 0;
    uint8_t v = 0;

	switch ( frm->_coding ) {

		case VS_YUV422_YUYV:

				unsigned char rgb_out[6];

                for (i=0; i< img->pixels; i+=2 ) {

                    __YUV422_YUYV_TO_RGB__(&(frm->_data[ 2*i ]), rgb_out);

                    __RGB32_TO_HSV__(&(rgb_out[0]), h, s, v);

					img->raw_data[i] =  ( (uint32_t) h ) << 16  |
							    ( (uint32_t) s ) << 8 |
							    ( (uint32_t) v ) ;

                    __RGB32_TO_HSV__(&(rgb_out[3]), h, s, v);

					img->raw_data[i+1] =  ( (uint32_t) h ) << 16  |
							    ( (uint32_t) s ) << 8 |
							    ( (uint32_t) v ) ;
				}

				break ;

        case VS_RGB24:
			for (i=0; i<img->pixels; i++ )
            {
                __RGB32_TO_HSV__(&(frm->_data[3*i]), h, s, v);

				img->raw_data[i] =  ( (uint32_t) h ) << 16  |
						    ( (uint32_t) s ) << 8 | 
						    ( (uint32_t) v ) ;  
            }
            break;

		case VS_RGB32:
				for (i=0; i<img->pixels; i++ )
                {
                    __RGB32_TO_HSV__(&(frm->_data[4*i]), h, s, v);

					img->raw_data[i] =  ( (uint32_t) h ) << 16  |
							    ( (uint32_t) s ) << 8 | 
							    ( (uint32_t) v ) ;  
                }
				break ;
        case VS_RAW:
                memcpy(img->raw_data, frm->_data, frm->_data_size);
                img->data_size = frm->_data_size;
                break;
		default:
				throw( string( "Image<uint32_t,HSV> : CONVERSION NOT IMPLEMENTED" ) ) ;
				break ;
	}

}

template<>
void image_fill < Image< uint16_t, DEPTH > > ( Image<uint16_t,DEPTH> *img, visionsystem::Frame* frm ) 
{
	switch ( frm->_coding ) {
        case VS_DEPTH16:
            std::memcpy(img->raw_data, frm->_data, img->data_size);
            break;
		default:
				throw( string( "Image<uint16_t, DEPTH> : CONVERSION NOT IMPLEMENTED" ) ) ;
				break ;
	}
}
