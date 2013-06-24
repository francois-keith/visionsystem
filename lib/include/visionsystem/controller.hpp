template < typename Obj >
void Controller::whiteboard_write ( std::string key, Obj value ) {
	_vscore->whiteboard_write< Obj >( key, value ) ;
}

template < typename Obj >
Obj Controller::whiteboard_read ( std::string key ) {
	return _vscore->whiteboard_read< Obj >( key ) ;

}

