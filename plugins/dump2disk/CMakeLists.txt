option ( PLUGIN_DUMP2DISK "Include plugin dump2disk ?" ON )

if ( PLUGIN_DUMP2DISK )

	### File Lists

	file( GLOB dump2disk_src "src/*.cpp" )
	file( GLOB dump2disk_hdr "src/*.h*")

	### Targets

	add_library( "dump2disk" SHARED ${dump2disk_src} ${dump2disk_hdr} )
	IF(WIN32)
	# This is done on win32 only because gcc does not need to resolve symbol at link time for libs
	target_link_libraries("dump2disk" "visionsystem" )
	target_link_libraries("dump2disk" "vision" )
	target_link_libraries("dump2disk" "configparser" )
	ENDIF(WIN32)


	### INSTAll

	install( TARGETS "dump2disk" 
			DESTINATION "${INSTALL_LIB_PATH}/vs_plugins"  COMPONENT main)

else ( PLUGIN_DUMP2DISK )

	message ( WARNING "Plugin Dump2Disk will not be included" ) 

endif ( PLUGIN_DUMP2DISK )
