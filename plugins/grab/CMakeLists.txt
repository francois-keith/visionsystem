option ( PLUGIN_GRAB "Include plugin grab ?" ON )

if ( PLUGIN_GRAB )

	### File Lists

	file( GLOB grab_src "src/*.cpp" )
	file( GLOB grab_hdr "src/*.h*")

	### Targets

	add_library( "grab" SHARED ${grab_src} ${grab_hdr} )


	### INSTAll

	install( TARGETS "grab" LIBRARY DESTINATION
		 "${INSTALL_LIB_PATH}/vs_plugins"  COMPONENT main )

else ( PLUGIN_GRAB )

	message ( WARNING "Plugin grab will not be included" )

endif ( PLUGIN_GRAB ) 
