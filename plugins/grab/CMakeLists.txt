option ( PLUGIN_GRAB "Include plugin grab ?" ON )

if ( PLUGIN_GRAB )

	### File Lists

	file( GLOB grab_src "src/*.cpp" )
	file( GLOB grab_hdr "src/*.h*")

	### Targets

	add_library( "grab" SHARED ${grab_src} ${grab_hdr} )
    target_link_libraries("grab" ${Boost_LIBRARIES})

	IF(WIN32)
	# This is done on win32 only because gcc does not need to resolve symbol at link time for libs
	target_link_libraries("grab" "visionsystem" )
	ENDIF(WIN32)
	PKG_CONFIG_USE_DEPENDENCY(grab  vision)

	### INSTAll

	install( TARGETS "grab" 
			DESTINATION "${INSTALL_LIB_PATH}/vs_plugins"  COMPONENT main )

else ( PLUGIN_GRAB )

	message ( WARNING "Plugin grab will not be included" )

endif ( PLUGIN_GRAB ) 
