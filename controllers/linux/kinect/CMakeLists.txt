find_package( Dc1394 ) 


if ( DC1394_FOUND )

	include_directories ( "${DC1394_INCLUDE_DIR}" )
	
	### File Lists

	file( GLOB kinect_src "src/*.cpp" )
	file( GLOB kinect_hdr "src/*.h*")

	### Targets

	add_library( "kinect" SHARED ${kinect_src} ${cam1394_hdr} )

	target_link_libraries ("kinect" ${VISION_LIBRARIES}  ${DC1394_LIBRARIES} )

	### INSTAll

	install( TARGETS "kinect" LIBRARY DESTINATION
		 "${INSTALL_LIB_PATH}/vs_controllers"  COMPONENT main )

endif ( DC1394_FOUND )

