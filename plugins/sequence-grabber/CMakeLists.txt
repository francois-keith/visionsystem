option ( PLUGIN_SEQUENCE_GRABBER "Include plugin sequence-grabber ?" ON )

if ( PLUGINS_XMLRPC AND PLUGIN_SEQUENCE_GRABBER )

	### File Lists

	file( GLOB sequence-grabber_src "src/*.cpp" )
	file( GLOB sequence-grabber_hdr "src/*.h*")

	### Targets

	add_library( "sequence-grabber" SHARED ${sequence-grabber_src} ${sequence-grabber_hdr} )

  target_link_libraries( "sequence-grabber" ${Boost_LIBRARIES} "-L${CMAKE_BINARY_DIR}/xmlrpc/server" xmlrpc-server)
  add_dependencies(sequence-grabber xmlrpc-server)

	IF(WIN32)
	# This is done on win32 only because gcc does not need to resolve symbol at link time for libs
	target_link_libraries("sequence-grabber" "visionsystem" )
	target_link_libraries("sequence-grabber" "vision" )
	target_link_libraries("sequence-grabber" "configparser" )
	if ( ImageMagick_Magick++_FOUND )		 
	target_link_libraries ( "sequence-grabber" "${ImageMagick_Magick++_LIBRARY}" ) 
    endif ( ImageMagick_Magick++_FOUND )
	ENDIF(WIN32)

	### INSTAll

	install( TARGETS "sequence-grabber" 
			DESTINATION "${INSTALL_LIB_PATH}/vs_plugins"  COMPONENT main )

else ( PLUGINS_XMLRPC AND PLUGIN_SEQUENCE_GRABBER )

	message ( WARNING "Plugin sequence-grabber will not be included" )

endif ( PLUGINS_XMLRPC AND PLUGIN_SEQUENCE_GRABBER )
