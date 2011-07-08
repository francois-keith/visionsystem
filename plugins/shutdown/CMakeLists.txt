option ( PLUGIN_SHUTDOWN "Include plugin shutdown ?" ON )

if ( PLUGIN_SHUTDOWN )

	### File Lists

	file( GLOB shutdown_src "src/*.cpp" )
	file( GLOB shutdown_hdr "src/*.h*")

	### Targets

	add_library( "shutdown" SHARED ${shutdown_src} ${shutdown_hdr} )


	### INSTAll

	install( TARGETS "shutdown" LIBRARY DESTINATION
		 "${INSTALL_LIB_PATH}/vs_plugins"  COMPONENT main )

else ( PLUGIN_SHUTDOWN )

	message ( WARNING "Plugin shutdown will not be included" )

endif ( PLUGIN_SHUTDOWN ) 
