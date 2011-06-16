
template< typename Timage >
Camera* Plugin::register_to_cam ( std::string name, int N ) {
	
	Camera* cam = _vscore->get_camera ( name ) ;
	
	_vscore->register_to_cam ( this, cam ) ;

	if ( cam == NULL )
		return NULL ;
	
	Buffer<Frame*>  *frmbuff = new Buffer<Frame>() ;
	Buffer<Timage*> *imgbuff = new Buffer<Timage*>() ;
	boost::any tmp = imgbuff ;
	
	frame_buffers[cam] = frmbuff ;
	img_buffers[cam] = tmp ;

	for ( int i=0; i<N; i++ ) {
		frmbuff->enqueue ( new Frame ( cam->get_coding(), cam->get_size() ) ) ;
		imgbuff->enqueue ( new Timage( cam->get_size() ) ) ;
	}

	return cam ;
}


template< typename Timage >
Timage* Plugin::dequeue_image ( Camera* cam ) 
{
	Buffer<Frame*>  *frmbuff = frame_buffers[cam] ;
	Buffer<Timage*> *imgbuff = boost::any_cast< Buffer<Timage*>* >( img_buffers[cam] ) ;

	Frame*  frm = frmbuff->bl_dequeue() ;
	Timage* img = imgbuff->bl_dequeue() ;

	image_fill<Timage> ( img, frm ) ;

	frmbuff->enqueue ( frm ) ;

	return img ;
}



template< typename Timage >
void Plugin::enqueue_image ( Camera* cam, Timage* img ) 
{
	Buffer<Timage*> *imgbuff = boost::any_cast< Buffer<Timage*>* >( img_buffers[cam] ) ;
	imgbuff->enqueue ( img ) ;
}



template< typename Timage >
void Plugin::unregister_to_cam ( Camera* cam ) {

	_vscore->unregister_to_cam( this, cam ) ;	
	
	Buffer<Frame*>  *frmbuff = frame_buffers[cam] ;
	Buffer<Timage*> *imgbuff = boost::any_cast< Buffer<Timage*>* >( img_buffers[cam] ) ;	
	
//	while ( imgbuff->size() > 0 )			FIXME - Leak memoire
//		delete ( (*imgbuff)[0] ) ;

//	while ( frmbuff->size() > 0 )
//		delete ( (*frmbuff)[0] ) ;

	delete imgbuff ;
	delete frmbuff ;
}


template < typename Object >
void Plugin::whiteboard_write ( std::string key, Object value ) {
	_vscore->whiteboard_write< Object >(key, value ) ;
}

template < typename Object >
Object Plugin::whiteboard_read ( std::string key ) {
	return _vscore->whiteboard_read< Object >( key ) ; 
}


