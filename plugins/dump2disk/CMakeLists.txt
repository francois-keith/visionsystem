option ( PLUGIN_DUMP2DISK "Include plugin dump2disk ?" ON )

if ( PLUGIN_DUMP2DISK )

	### File Lists

	file( GLOB dump2disk_src "src/*.cpp" )
	file( GLOB dump2disk_hdr "src/*.h*")

	### Targets

	add_library( "dump2disk" SHARED ${dump2disk_src} ${dump2disk_hdr} )
    target_link_libraries("dump2disk" ${Boost_LIBRARIES})
	IF(WIN32)
	# This is done on win32 only because gcc does not need to resolve symbol at link time for libs
	target_link_libraries("dump2disk" "visionsystem" )
	ENDIF(WIN32)
	PKG_CONFIG_USE_DEPENDENCY(dump2disk vision)

	### INSTAll

	install( TARGETS "dump2disk" 
			DESTINATION "${INSTALL_LIB_PATH}/vs_plugins"  COMPONENT main)

else ( PLUGIN_DUMP2DISK )

	message ( WARNING "Plugin Dump2Disk will not be included" ) 

endif ( PLUGIN_DUMP2DISK )
