#ifndef VS_IMAGEFILL_H
#define VS_IMAGEFILL_H

#include <iostream>
#include <typeinfo>
#include <visionsystem/frame.h>

template < typename Timage > 
void image_fill ( Timage* img, visionsystem::Frame* frm ) {

	std::cerr << "Conversion from encoding " << frm->_coding << " to image type " << typeid( Timage ).name()  << " is not implemented. Please see visionsystem/imagefill.hpp" << std::endl ;

	throw("CONVERSION NOT IMPLEMENTED") ;
}


#endif
