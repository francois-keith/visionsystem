find_package( Dc1394 ) 


if ( DC1394_FOUND )

	include_directories ( "${DC1394_INCLUDE_DIR}" )
	
	### File Lists

	file( GLOB glview_src "src/*.cpp" )
	file( GLOB glview_hdr "src/*.h*")

	### Targets

	add_library( "glview" SHARED ${glview_src} ${glview_hdr} )

	target_link_libraries ("glview" ${VISION_LIBRARIES}  ${DC1394_LIBRARIES} )

	### INSTAll

	install( TARGETS "glview" LIBRARY DESTINATION
		 "${INSTALL_LIB_PATH}/vs_plugins"  COMPONENT main )

endif ( DC1394_FOUND )

