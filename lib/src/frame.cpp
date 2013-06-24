#include <string>
#include <string.h>
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
        case VS_DEPTH8:
        case VS_IR8:

            _data_size = size.area() ;
            break ;

        case VS_MONO16 :
        case VS_YUV422_YYUV :
        case VS_YUV422_YUYV :
        case VS_YUV422_YVYU :
        case VS_YUV422_UYVY :
        case VS_YUV422_VYUY :
        case VS_IR16 :
        case VS_DEPTH16 :

            _data_size = 2 * size.area() ;
            break ;

        case VS_RGB24  :
        case VS_YUV444 :
        case VS_IR24:
        case VS_DEPTH24:

            _data_size = 3 * size.area() ;
            break ;

        case VS_RGB32 :
        case VS_IR32 :
        case VS_DEPTH32 :

            _data_size = 4 * size.area() ;
            break ;

        case VS_RAW:

            _data_size = 4 * size.area() ;
            break;

        default:
            throw ( string( "Frame.cpp : Unknown Frame Format" )  );
            break ;
    }

    _data = new unsigned char[ _data_size]  ;

}


Frame::~Frame()
{
    delete[] _data ;

}

void Frame::clone( Frame* frm ) {

    if ( _data_size != frm->_data_size ) {
        delete ( _data ) ;
        _data = new unsigned char[ frm->_data_size] ;
        _data_size = frm->_data_size ;
    }

    _coding = frm->_coding ;
    _frame_size = frm->_frame_size ;

    memcpy( _data, frm->_data, _data_size ) ;

}


}
