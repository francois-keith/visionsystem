option ( PLUGIN_STREAM2SOCKET "Include plugin stream2socket ?" ON )

if ( PLUGIN_STREAM2SOCKET )

	### File Lists

	SET( stream2socket_src "src/stream2socket.cpp" )
	SET( stream2socket_hdr "src/stream2socket.h")

	### Targets

	add_library( "stream2socket" SHARED ${stream2socket_src} ${stream2socket_hdr} )

	target_link_libraries( "stream2socket" ${VISION_LIBRARIES} ${Boost_LIBRARIES})
	IF(WIN32)
	# This is done on win32 only because gcc does not need to resolve symbol at link time for libs
	target_link_libraries("stream2socket" "visionsystem" )
	target_link_libraries("stream2socket" "vision" )
	target_link_libraries("stream2socket" "configparser" )
	ENDIF(WIN32)

    # Check for ZLIB
    SET(ZLIB_FOUND_NUMERAL 0)
    find_package(ZLIB)
    if(ZLIB_FOUND)
        SET(ZLIB_FOUND_NUMERAL 1)
        include_directories(${ZLIB_INCLUDE_DIRS})
        target_link_libraries("stream2socket" ${ZLIB_LIBRARIES})
    endif(ZLIB_FOUND)
    configure_file("config.h.in" "${CMAKE_SOURCE_DIR}/plugins/stream2socket/src/config.h")

	### INSTAll

	install( TARGETS "stream2socket" 
			DESTINATION "${INSTALL_LIB_PATH}/vs_plugins"  COMPONENT main 
			)

else ( PLUGIN_STREAM2SOCKET )

	message ( WARNING "Plugin stream2socket will not be included" ) 

endif ( PLUGIN_STREAM2SOCKET )
