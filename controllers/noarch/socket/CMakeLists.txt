option ( CONTROLLER_SOCKET "Include controller socket ?" ON )

if ( CONTROLLER_SOCKET )

	include_directories ( "${Boost_INCLUDE_DIRS}" )

	### File Lists

	SET( socket_src "src/camerasocket.cpp" "src/controllersocket.cpp")
	SET( socket_hdr "src/camerasocket.h" "src/controllersocket.h")

	### Targets

	add_library( "socket" SHARED ${socket_src} ${socket_hdr} )

	target_link_libraries ("socket" ${VISION_LIBRARIES})
	target_link_libraries("socket" "visionsystem" )
	target_link_libraries("socket" "vision" )
	target_link_libraries("socket" "configparser" )
    if(${LIBAVCODEC_FOUND})
	target_link_libraries("socket" ${LIBSWSCALE_LIBRARIES})
	target_link_libraries("socket" ${LIBAVCODEC_LIBRARIES})
	target_link_libraries("socket" ${LIBAVUTIL_LIBRARIES})
    endif(${LIBAVCODEC_FOUND})
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
    install( FILES "src/camerasocket.h"
             DESTINATION "${INSTALL_HEADERS_PATH}/visionsystem/vs_controllers/socket/" 
             COMPONENT main)

else ( CONTROLLER_SOCKET )

	message ( WARNING "Controller socket will not be included" ) 

endif ( CONTROLLER_SOCKET )
