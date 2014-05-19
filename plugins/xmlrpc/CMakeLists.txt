option ( PLUGINS_XMLRPC "Include XML-RPC related plugins ?" ON )

IF(PLUGINS_XMLRPC)
SET(XMLRPC_INCLUDE_DIRS "/home/fkeith/software/groovy/sot/install/xmlrpc++0.7/src")
SET(XMLRPC_LIBRARY_DIRS "/home/fkeith/software/groovy/sot/install/xmlrpc++0.7/")
SET(XMLRPC_LIBRARIES "XmlRpc")

INCLUDE_DIRECTORIES(${XMLRPC_INCLUDE_DIRS})
link_directories(${XMLRPC_LIBRARY_DIRS})
#SET(XMLRPC_LIBRARIES "XmlRpc")

add_subdirectory( "server" )
add_subdirectory( "client" )


# create the pc files for the plugins

set(PLUGIN_NAME "visionsystem-xmlrpc-client")
set(PLUGIN_DESCRIPTION "xmlrpc client plugin for vision system")
set(PLUGIN_LIBS "${LIBDIR_KW}\${libdir}/vs_plugins ${LIBINCL_KW}xmlrpc-client${LIB_EXT} ${LIBDIR_KW}${XMLRPC_LIBRARY_DIRS}")
set(PLUGIN_CFLAGS "-I${XMLRPC_INCLUDE_DIRS}")

configure_file(${CMAKE_MODULE_PATH}/plugin.pc.cmake "${PROJECT_BINARY_DIR}/${PROJECT_NAME}_xmlrpc_client.pc")

install ( FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}_xmlrpc_client.pc" 
      DESTINATION "${INSTALL_LIB_PATH}/pkgconfig" COMPONENT dev )  


set(PLUGIN_NAME "visionsystem-server-client")
set(PLUGIN_DESCRIPTION "xmlrpc server plugin for vision system")
set(PLUGIN_LIBs "${LIBDIR_KW}\${libdir}/vs_plugins ${LIBINCL_KW}xmlrpc-client${LIB_EXT} ${LIBDIR_KW}${XMLRPC_LIBRARY_DIRS}")
set(PLUGIN_CFLAGS "-I${XMLRPC_INCLUDE_DIRS}")

configure_file(${CMAKE_MODULE_PATH}/plugin.pc.cmake "${PROJECT_BINARY_DIR}/${PROJECT_NAME}_xmlrpc_server.pc")

install ( FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}_xmlrpc_server.pc" 
      DESTINATION "${INSTALL_LIB_PATH}/pkgconfig" COMPONENT dev )  

ENDIF(PLUGINS_XMLRPC)
