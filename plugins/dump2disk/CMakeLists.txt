option ( PLUGIN_DUMP2DISK "Include plugin dump2disk ?" ON )

if ( PLUGIN_DUMP2DISK )

	### File Lists

	file( GLOB dump2disk_src "src/*.cpp" )
	file( GLOB dump2disk_hdr "src/*.h*")

	### Targets

	add_library( "dump2disk" SHARED ${dump2disk_src} ${dump2disk_hdr} )


	### INSTAll

	install( TARGETS "dump2disk" LIBRARY DESTINATION
		 "${INSTALL_LIB_PATH}/vs_plugins"  COMPONENT main )

else ( PLUGIN_DUMP2DISK )

	message ( WARNING "Plugin Dump2Disk will not be included" ) 

endif ( PLUGIN_DUMP2DISK )
