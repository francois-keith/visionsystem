
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
		delete ( _trash[i] ) ;

	_frames.clear() ;
	_trash.clear()  ;
}


template<typename Data>
Data* Buffer<Data>::bl_dequeue() {
	Data* tmp ;
	_mutex.lock() ;
	for ( int counter=0; _frames.size() == 0; counter++ ) {
		
		_mutex.unlock() ;

		if ( counter == 1000 ) {
			throw( std::string ("bl_dequeue() : timeout ..." ) ) ;
		}

		usleep ( 1000 ) ;
		
		_mutex.lock() ;

	}
	tmp = _frames.front() ;
	_frames.erase(_frames.begin()) ;
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
		_frames.erase( _frames.begin() ) ;
	}
	_mutex.unlock() ;
	return tmp ;
}

template<typename Data>
void Buffer<Data>::enqueue( Data* data ) {
	_mutex.lock() ;
	_trash.push_back ( data ) ;
	_mutex.unlock() ;
}


template<typename Data>
void Buffer<Data>::push( Data* data ) {
	_mutex.lock() ;
	_frames.push_back( data ) ;
	_mutex.unlock() ;
}

template<typename Data>
Data* Buffer<Data>::pull() {
	Data* tmp ;
	_mutex.lock() ;
	if ( _trash.size() == 0 ) 
		throw ( std::string ( "Cannot pull from buffer : no available frame " ) ) ;
	else {
		tmp = _trash.front() ;	
		_trash.erase( _trash.begin() ) ;
	}
	_mutex.unlock() ;
	return tmp ;

}


template<typename Data>
void Buffer<Data>::clear() {
	_mutex.lock() ;
	for ( int i=0; i<_frames.size() ; i++ )
		delete _frames[i] ;
	for ( int i=0; i<_trash.size() ; i++ )
		delete _trash[i] ;
	_frames.clear() ;
	_trash.clear() ;
	_mutex.unlock() ;
}

template<typename Data>
bool Buffer<Data>::is_full() {
	bool tmp ;
	_mutex.lock() ;
	tmp = ( _trash.size() == 0 ) ;
	_mutex.unlock() ;
	return tmp ;
}
	
template<typename Data>
int Buffer<Data>::size() {
	int tmp ;
	_mutex.lock() ;
	tmp =  _frames.size() ;
	_mutex.unlock() ;
	return tmp ;
}

