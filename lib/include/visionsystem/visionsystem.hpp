
template< typename Obj >
void VisionSystem::whiteboard_write ( std::string key, Obj value ) {
	if ( whiteboard_data.count( key ) ) {
	
		whiteboard_mutex[key]->lock() ;
		boost::any tmp = value ;
		whiteboard_data[key] = tmp ;
		whiteboard_mutex[key]->unlock() ;
	
	} else {

		boost::mutex *tmp = new boost::mutex();
		whiteboard_mutex[key] = tmp ;
		boost::any tmp2 = value ;
		whiteboard_data[key] = tmp2 ;
	}
}


template< typename Obj >
Obj VisionSystem::whiteboard_read ( std::string key )  {
	Obj ret ;
	if ( whiteboard_data.count( key ) ) {
		whiteboard_mutex[key]->lock() ;
		ret = boost::any_cast< Obj > ( whiteboard_data[key] ) ;
		whiteboard_mutex[key]->unlock() ;
		return ret ;
	} 
	
	throw ( std::string ( "Whiteboard error: reading unknown key" ) ) ;
}



