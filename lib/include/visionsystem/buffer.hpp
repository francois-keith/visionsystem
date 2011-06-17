
#define VS_BUFFERSIZE	100

template<typename Data>
Buffer<Data>::Buffer() {
 	_frames.reserve( VS_BUFFERSIZE ) ;                                                           
        _trash.reserve( VS_BUFFERSIZE ) ;
}


template<typename Data>
Buffer<Data>::~Buffer() {
	
	for ( int i=0; i<_frames.size() ; i++ )
		delete (_frames[i]) ;

	for ( int i=0; i<_trash.size(); i++ )
		delete ( _trash[0] ) ;

	_frames.clear() ;
	_trash.clear()  ;
}


template<typename Data>
Data* Buffer<Data>::bl_dequeue() {
	Data* tmp ;
	_mutex.lock() ;
	while ( _frames.size() == 0 ) {
		_mutex.unlock() ;
		usleep ( 1000 ) ;
		_mutex.lock() ;
	}
	tmp = _frames.front() ;
	_frames.erase(frames.begin()) ;
	_mutex.unlock() ;
	return tmp ;

}

template<typename Data>
Data* Buffer<Data>::nbl_dequeue() {
	Data* tmp ;
	_mutex.lock() ;
	if ( _frames.size() == 0 )
		tmp = (Data*) NULL ;
	else {
		tmp = _frames.front() ;	
		_frames.erase( frames.begin() ) ;
	}
	_mutex.unlock() ;
	return tmp ;
}

template<typename Data>
void Buffer<Data>::enqueue( Data* data ) {
	_mutex.lock() ;
	_trash.pushback ( data ) ;
	_mutex.unlock() ;
}


template<typename Data>
void Buffer<Data>::push( Data* data ) {
	_mutex.lock() ;
	_frames.pushback( data ) ;
	_mutex.unlock() ;
}

template<typename Data>
Data* Buffer<Data>::pull() {
	Data* tmp ;
	_mutex.lock() ;
	if ( _trash.size() == 0 ) 
		throw ( string ( "Cannot pull from buffer : no available frame " ) ) ;
	else {
		tmp = _trash.front() ;	
		_trash.erase( trash.begin() ) ;
	}
	_mutex.unlock() ;
	return tmp ;

}
