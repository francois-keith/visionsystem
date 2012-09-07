option ( PLUGIN_SHUTDOWN "Include plugin shutdown ?" ON )

if ( PLUGIN_SHUTDOWN )

	### File Lists

	file( GLOB shutdown_src "src/*.cpp" )
	file( GLOB shutdown_hdr "src/*.h*")

	### Targets

	add_library( "shutdown" SHARED ${shutdown_src} ${shutdown_hdr} )
	IF(WIN32)
	# This is done on win32 only because gcc does not need to resolve symbol at link time for libs
	target_link_libraries("shutdown" "visionsystem" )
	target_link_libraries("shutdown" "vision" )
	target_link_libraries("shutdown" "configparser" )
	ENDIF(WIN32)


	### INSTAll

	install( TARGETS "shutdown" 
			DESTINATION "${INSTALL_LIB_PATH}/vs_plugins"  COMPONENT main)

else ( PLUGIN_SHUTDOWN )

	message ( WARNING "Plugin shutdown will not be included" )

endif ( PLUGIN_SHUTDOWN ) 
