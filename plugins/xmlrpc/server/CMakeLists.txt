option ( PLUGIN_XMLRPCSERVER "Include XML-RPC server plugin ?" ON )

if ( PLUGIN_XMLRPCSERVER )

	### File Lists

	file( GLOB XMLRPCSERVER_src "src/*.cpp" )
	file( GLOB XMLRPCSERVER_hdr "src/*.h")

	### Targets

	add_library( "xmlrpc-server" SHARED ${XMLRPCSERVER_src} ${XMLRPCSERVER_hdr} )

	target_link_libraries( "xmlrpc-server" ${XMLRPC_LIBRARIES})
	PKG_CONFIG_USE_DEPENDENCY(xmlrpc-server vision)
    if(WIN32)
    target_link_libraries( "xmlrpc-server" "visionsystem" )
    target_link_libraries( "xmlrpc-server" "configparser" )
    target_link_libraries( "xmlrpc-server" "ws2_32.lib" )
    endif(WIN32)

	### INSTAll

	install( TARGETS "xmlrpc-server" 
			DESTINATION "${INSTALL_LIB_PATH}/vs_plugins"  COMPONENT main )

    install( FILES ${XMLRPCSERVER_hdr}
             DESTINATION "${INSTALL_HEADERS_PATH}/visionsystem/vs_plugins/xmlrpc/" 
             COMPONENT main) 

else ( PLUGIN_XMLRPCSERVER )

	message ( WARNING "Plugin xml-rpc server will not be included" ) 

endif ( PLUGIN_XMLRPCSERVER )
