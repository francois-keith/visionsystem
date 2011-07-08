option ( PLUGIN_DUMMY "Include plugin dummy ?" ON )

if ( PLUGIN_DUMMY )

	### File Lists

	file( GLOB dummy_src "src/*.cpp" )
	file( GLOB dummy_hdr "src/*.h*")

	### Targets

	add_library( "dummy" SHARED ${dummy_src} ${dummy_hdr} )


	### INSTAll

	install( TARGETS "dummy" LIBRARY DESTINATION
		 "${INSTALL_LIB_PATH}/vs_plugins"  COMPONENT main )

else ( PLUGIN_DUMMY )

	message ( WARNING " Plugin dummy will not be included" ) 

endif ( PLUGIN_DUMMY )
