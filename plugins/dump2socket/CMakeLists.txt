option ( PLUGIN_DUMP2SOCKET "Include plugin dump2socket ?" ON )

if ( PLUGIN_DUMP2SOCKET )

	### File Lists

	file( GLOB dump2socket_src "src/*.cpp" )
	file( GLOB dump2socket_hdr "src/*.h*")

	### Targets

	add_library( "dump2socket" SHARED ${dump2socket_src} ${dump2socket_hdr} )

	target_link_libraries( "dump2socket" ${VISION_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY})

	### INSTAll

	install( TARGETS "dump2socket" LIBRARY DESTINATION
		 "${INSTALL_LIB_PATH}/vs_plugins"  COMPONENT main )

else ( PLUGIN_DUMP2SOCKET )

	message ( WARNING "Plugin dump2socket will not be included" ) 

endif ( PLUGIN_DUMP2SOCKET )
