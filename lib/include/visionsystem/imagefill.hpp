#ifndef VS_IMAGEFILL_H
#define VS_IMAGEFILL_H

#include <iostream>
#include <typeinfo>
#include <visionsystem/frame.h>

#ifndef VS_EXPORT
	#ifdef WIN32
		#define VS_EXPORT __declspec(dllexport)
	#else
		#define VS_EXPORT
	#endif
#endif

template < typename Timage > 
void image_fill ( Timage* img, visionsystem::Frame* frm ) {

	std::cerr << "Conversion from encoding " << frm->_coding << " to image type " << typeid( Timage ).name()  << " is not implemented. Please see visionsystem/imagefill.hpp" << std::endl ;

	throw("CONVERSION NOT IMPLEMENTED") ;
}

#include <vision/image/image.h>
#include <stdint.h> 

template<>
void image_fill < vision::Image< unsigned char, vision::MONO > > ( vision::Image<unsigned char, vision::MONO> *img, visionsystem::Frame* frm ) ;

template<>
void image_fill < vision::Image< uint32_t, vision::RGB > > ( vision::Image<uint32_t,vision::RGB> *img, visionsystem::Frame* frm ) ;

template<>
void image_fill < vision::Image< uint32_t, vision::HSV > > ( vision::Image<uint32_t, vision::HSV> *img, visionsystem::Frame* frm ) ;

#endif
