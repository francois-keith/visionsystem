option ( PLUGINS_XMLRPC "Include XML-RPC related plugins ?" ON )

IF(PLUGINS_XMLRPC)

SET(XMLRPC_LIBRARIES "XmlRpc")

add_subdirectory( "server" )
add_subdirectory( "client" )

ENDIF(PLUGINS_XMLRPC)
