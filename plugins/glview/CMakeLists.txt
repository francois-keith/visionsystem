option ( PLUGIN_GLVIEW "Include plugin glview ?" ON )

if ( PLUGIN_GLVIEW )

    if (CONTROLLER_SOCKET)
        add_definitions(-DVS_HAS_CONTROLLER_SOCKET)
        include_directories( "${CMAKE_SOURCE_DIR}/controllers/noarch/socket/src/" ) 
    endif(CONTROLLER_SOCKET)

	find_package(OpenGL)
	find_package( X11)
		
        include_directories( "${X11_X11_INCLUDE_PATH}"
			     "${OPENGL_INCLUDE_DIR}"
	)

	### File Lists

	file( GLOB glview_src "src/*.cpp" )
	file( GLOB glview_hdr "src/*.h*")

	### Targets

	add_library( "glview" SHARED ${glview_src} ${glview_hdr} )

	target_link_libraries ("glview" ${CONFIGPARSER_LIBRARIES} ${VISION_LIBRARIES} ${X11_X11_LIB} ${OPENGL_gl_LIBRARY})
  if(WIN32)
    target_link_libraries ("glview"  "Xxf86vm")
  else(WIN32)
    target_link_libraries ("glview"  "${LIBXXF86VM_LIBRARIES}")
  endif(WIN32)

    if(CONTROLLER_SOCKET)
        target_link_libraries("glview" "socket")
    endif(CONTROLLER_SOCKET)

	### INSTAll

	install( TARGETS "glview" 
			LIBRARY DESTINATION "${INSTALL_LIB_PATH}/vs_plugins"  COMPONENT main 
			RUNTIME DESTINATION "${INSTALL_BIN_PATH}/vs_plugins"  COMPONENT main)

else ( PLUGIN_GLVIEW )

	message ( WARNING "plugin glview will not be included" )

endif ( PLUGIN_GLVIEW )
