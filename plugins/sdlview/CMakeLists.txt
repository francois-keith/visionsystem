option ( PLUGIN_SDLVIEW "Include plugin sdlview ?" ON )

if ( PLUGIN_SDLVIEW )

	find_package( SDL )

endif ( PLUGIN_SDLVIEW )

if ( PLUGIN_SDLVIEW AND SDL_FOUND)

    if (CONTROLLER_SOCKET)
        add_definitions(-DVS_HAS_CONTROLLER_SOCKET)
    endif(CONTROLLER_SOCKET)

		
	include_directories( "${SDL_INCLUDE_DIR}" )

	### File Lists

	file( GLOB sdlview_src "src/plugin.cpp" )
	file( GLOB sdlview_hdr "src/plugin.h")

	### Targets

	add_library( "sdlview" SHARED ${sdlview_src} ${sdlview_hdr} )

	target_link_libraries ("sdlview" ${CONFIGPARSER_LIBRARIES} ${VISION_LIBRARIES} ${SDL_LIBRARY} )
    if(CONTROLLER_SOCKET)
        target_link_libraries("sdlview" "socket")
    endif(CONTROLLER_SOCKET)

	### INSTAll

	install( TARGETS "sdlview" 
			DESTINATION "${INSTALL_LIB_PATH}/vs_plugins"  COMPONENT main)

elseif ( PLUGIN_SDLVIEW AND NOT SDL_FOUND )

    message( ERROR "plugin sdlview included but SDL library not found" )

else ( PLUGIN_SDLVIEW AND SDL_FOUND )

	message ( WARNING "plugin sdlview will not be included" )

endif ( PLUGIN_SDLVIEW AND SDL_FOUND )
