option ( PLUGIN_XMLRPCCLIENT "Include XML-RPC client plugin ?" ON )

if ( PLUGIN_XMLRPCCLIENT )

	### File Lists

	file( GLOB XMLRPCCLIENT_src "src/*.cpp" )
	file( GLOB XMLRPCCLIENT_hdr "src/*.h")

	### Targets

	add_library( "xmlrpc-client" SHARED ${XMLRPCCLIENT_src} ${XMLRPCCLIENT_hdr} )

	target_link_libraries( "xmlrpc-client" ${VISION_LIBRARIES} ${XMLRPC_LIBRARIES})

	### INSTAll

	install( TARGETS "xmlrpc-client" LIBRARY DESTINATION
		 "${INSTALL_LIB_PATH}/vs_plugins"  COMPONENT main )

    install( FILES ${XMLRPCCLIENT_hdr}
             DESTINATION "${INSTALL_HEADERS_PATH}/visionsystem/vs_plugins/xmlrpc/" )

else ( PLUGIN_XMLRPCCLIENT )

	message ( WARNING "Plugin xml-rpc client will not be included" ) 

endif ( PLUGIN_XMLRPCCLIENT )
