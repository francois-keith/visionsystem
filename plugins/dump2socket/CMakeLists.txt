### File Lists

file( GLOB dump2socket_src "src/*.cpp" )
file( GLOB dump2socket_hdr "src/*.h*")

### Targets

add_library( "dump2socket" SHARED ${dump2socket_src} ${dump2socket_hdr} )


### INSTAll

install( TARGETS "dump2socket" LIBRARY DESTINATION
	 "${INSTALL_LIB_PATH}/vs_plugins"  COMPONENT main )


