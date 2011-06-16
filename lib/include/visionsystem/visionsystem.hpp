
template< typename Obj >
void VisionSystem::whiteboard_write ( std::string key, Obj value ) {
	whiteboard_mutex[key].lock() ;
	boost::any tmp = value ;
	whiteboard_data[key] = tmp ;
	whiteboard_mutex[key].unlock() ;
}


template< typename Obj >
Obj VisionSystem::whiteboard_read ( std::string key )  {
	Obj ret ;
	whiteboard_mutex[key].lock() ;
	ret = boost::any_cast< Obj > ( whiteboard_data[key] ) ;
	whiteboard_mutex[key].unlock() ;
	return ret ;
}



