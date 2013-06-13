option ( PLUGIN_DUMMY "Include plugin dummy ?" ON )

if ( PLUGIN_DUMMY )

	### File Lists

	file( GLOB dummy_src "src/*.cpp" )
	file( GLOB dummy_hdr "src/*.h*")

	### Targets

	add_library( "dummy" SHARED ${dummy_src} ${dummy_hdr} )

	IF(WIN32)
	# This is done on win32 only because gcc does not need to resolve symbol at link time for libs
	target_link_libraries("dummy" "visionsystem")
	ENDIF(WIN32)
	PKG_CONFIG_USE_DEPENDENCY(dummy vision)

	### INSTAll

	install( TARGETS "dummy" 
			DESTINATION "${INSTALL_LIB_PATH}/vs_plugins"  COMPONENT main)

else ( PLUGIN_DUMMY )

	message ( WARNING " Plugin dummy will not be included" ) 

endif ( PLUGIN_DUMMY )
