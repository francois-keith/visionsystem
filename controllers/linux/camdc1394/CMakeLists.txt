find_package( Dc1394 ) 


if ( DC1394_FOUND )

	include_directories ( "${DC1394_INCLUDE_DIR}" )
	
	### File Lists

	file( GLOB cam1394_src "src/*.cpp" )
	file( GLOB cam1394_hdr "src/*.h*")

	### Targets

	add_library( "camdc1394" SHARED ${cam1394_src} ${cam1394_hdr} )

	target_link_libraries ("camdc1394" ${VISION_LIBRARIES}  ${DC1394_LIBRARIES} )

	### INSTAll

	install( TARGETS "camdc1394" LIBRARY DESTINATION
		 "${INSTALL_LIB_PATH}/vs_controllers"  COMPONENT main )

endif ( DC1394_FOUND )

