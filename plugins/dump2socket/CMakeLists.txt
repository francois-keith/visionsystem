option ( PLUGIN_DUMP2SOCKET "Include plugin dump2socket ?" ON )

if ( PLUGIN_DUMP2SOCKET )

	### File Lists

	SET( dump2socket_src "src/dump2socket.cpp" )
	SET( dump2socket_hdr "src/dump2socket.h")

	### Targets

	add_library( "dump2socket" SHARED ${dump2socket_src} ${dump2socket_hdr} )

	target_link_libraries( "dump2socket" ${VISION_LIBRARIES} ${Boost_SYSTEM_LIBRARY})
	IF(WIN32)
	# This is done on win32 only because gcc does not need to resolve symbol at link time for libs
	target_link_libraries("dump2socket" "visionsystem" )
	target_link_libraries("dump2socket" "vision" )
	target_link_libraries("dump2socket" "configparser" )
	ELSE(WIN32)
	target_link_libraries("dump2socket" ${Boost_THREAD_LIBRARY})
	ENDIF(WIN32)

    # Check for ZLIB
    SET(ZLIB_FOUND_NUMERAL 0)
    find_package(ZLIB)
    if(ZLIB_FOUND)
        SET(ZLIB_FOUND_NUMERAL 1)
        include_directories(${ZLIB_INCLUDE_DIRS})
        target_link_libraries("dump2socket" ${ZLIB_LIBRARIES})
    endif(ZLIB_FOUND)
    configure_file("config.h.in" "${CMAKE_SOURCE_DIR}/plugins/dump2socket/src/config.h")

	### INSTAll

	install( TARGETS "dump2socket" 
			DESTINATION "${INSTALL_LIB_PATH}/vs_plugins"  COMPONENT main 
			)

else ( PLUGIN_DUMP2SOCKET )

	message ( WARNING "Plugin dump2socket will not be included" ) 

endif ( PLUGIN_DUMP2SOCKET )
