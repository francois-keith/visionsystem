option ( CONTROLLER_SOCKET "Include controller socket ?" ON )

if ( CONTROLLER_SOCKET )

	include_directories ( "${Boost_INCLUDE_DIRS}" )

	### File Lists

	SET( socket_src "src/camerasocket.cpp" "src/controllersocket.cpp")
	SET( socket_hdr "src/camerasocket.h" "src/controllersocket.h")

	### Targets

	add_library( "socket" SHARED ${socket_src} ${socket_hdr} )

	target_link_libraries ("socket" ${VISION_LIBRARIES}  ${Boost_SYSTEM_LIBRARY} )
	IF(WIN32)
	# This is done on win32 only because gcc does not need to resolve symbol at link time for libs
	target_link_libraries("socket" "visionsystem" )
	target_link_libraries("socket" "vision" )
	target_link_libraries("socket" "configparser" )
	ENDIF(WIN32)
    # Check for ZLIB
    SET(ZLIB_FOUND_NUMERAL 0)
    find_package(ZLIB)
    if(ZLIB_FOUND)
        SET(ZLIB_FOUND_NUMERAL 1)
        include_directories(${ZLIB_INCLUDE_DIRS})
        target_link_libraries("socket" ${ZLIB_LIBRARIES})
    endif(ZLIB_FOUND)
    configure_file("config.h.in" "${CMAKE_SOURCE_DIR}/controllers/noarch/socket/src/config.h")

	### INSTAll

	install( TARGETS "socket" 
			DESTINATION "${INSTALL_LIB_PATH}/vs_controllers"  COMPONENT main)

else ( CONTROLLER_SOCKET )

	message ( WARNING "Controller socket will not be included" ) 

endif ( CONTROLLER_SOCKET )
