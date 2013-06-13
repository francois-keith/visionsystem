option ( CONTROLLER_SOCKET "Include controller socket ?" ON )

if ( CONTROLLER_SOCKET )

	include_directories ( "${Boost_INCLUDE_DIRS}" )

	### File Lists

	SET( socket_src "src/camerasocket.cpp" "src/controllersocket.cpp")
	SET( socket_hdr "src/camerasocket.h" "src/controllersocket.h")

	### Targets

	add_library( "socket" SHARED ${socket_src} ${socket_hdr} )

	target_link_libraries (socket visionsystem)

	PKG_CONFIG_USE_DEPENDENCY(socket configparser)
	PKG_CONFIG_USE_DEPENDENCY(socket vision)

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

	set(PLUGIN_NAME "visionsystem_socket")
	set(PLUGIN_DESCRIPTION "socket plugin for vision system")
	set(PKG_CONFIG_REQUIRES "configparser, vision")
	set(PLUGIN_LIBS "${LIBDIR_KW}\${libdir}/vs_controllers ${LIBINCL_KW}socket${LIB_EXT}")
	set(PLUGIN_CFLAGS "")

	configure_file(${CMAKE_MODULE_PATH}/plugin.pc.cmake "${PROJECT_BINARY_DIR}/${PROJECT_NAME}_socket.pc")

	install ( FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}_socket.pc" 
	      DESTINATION "${INSTALL_LIB_PATH}/pkgconfig" COMPONENT dev )  

else ( CONTROLLER_SOCKET )

	message ( WARNING "Controller socket will not be included" ) 

endif ( CONTROLLER_SOCKET )
