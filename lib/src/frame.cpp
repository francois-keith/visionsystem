#include <string>
#include <visionsystem/frame.h>

using namespace std ;
using namespace vision ;


namespace visionsystem {

Frame::Frame( FrameCoding coding, ImageRef size ) 
{
	_coding = coding ;
	_frame_size = size ;
	switch (coding) {

		case VS_MONO8 :	
			_data_size = size.area() ;
			break ;
		case VS_MONO16 :
			_data_size = 2 * size.area() ;
			break ;
		case VS_RGB8 :
			_data_size = 3 * size.area() ;
			break ;
		case VS_RGBA :
			_data_size = 4 * size.area() ;
			break ;
		case VS_YUV411:
			_data_size = 6 * size.area() / 4 ;
			break ;
		case VS_YUV422 :
			_data_size = 2 * size.area() ;
			break ;
		case VS_YUV444 :
			_data_size = 3 * size.area() ;
			break ;
		default:
			throw ( string( "Unknown Frame Format" )  );

	}

	_data = new unsigned char[ _data_size]  ;

}


Frame::~Frame() 
{
	delete( _data ) ;

}



}
