### File Lists

file( GLOB shutdown_src "src/*.cpp" )
file( GLOB shutdown_hdr "src/*.h*")

### Targets

add_library( "shutdown" SHARED ${shutdown_src} ${shutdown_hdr} )


### INSTAll

install( TARGETS "shutdown" LIBRARY DESTINATION
	 "${INSTALL_LIB_PATH}/vs_plugins"  COMPONENT main )


