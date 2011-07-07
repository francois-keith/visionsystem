#ifndef VS_FRAME_H
#define VS_FRAME_H

#include <vision/image/imageref.h>

namespace visionsystem {


// A frame is what every camera outputs ... 
// Inside an image, the data is encoded pixel by pixel. In a frame,
// it is not that simple ... see YUV411 or YUV422 for example.

typedef enum {

	VS_MONO8 =0 ,
	VS_MONO16,
	VS_RGB24,
	VS_RGB32,
	VS_YUV411,
	VS_YUV422_YYUV,
	VS_YUV422_YUYV,
	VS_YUV422_YVYU,
	VS_YUV422_UYVY,
	VS_YUV422_VYUY,
	VS_YUV444

} FrameCoding ;




class Frame {

	public:

		FrameCoding	 _coding ;
		size_t		 _data_size ;
		vision::ImageRef _frame_size ;
		unsigned char*	 _data ;

		Frame( FrameCoding, vision::ImageRef size ) ;
		~Frame() ;

		void clone ( Frame* frm ) ;

} ;



}

#endif
