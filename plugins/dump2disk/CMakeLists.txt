### File Lists

file( GLOB dump2disk_src "src/*.cpp" )
file( GLOB dump2disk_hdr "src/*.h*")

### Targets

add_library( "dump2disk" SHARED ${dump2disk_src} ${dump2disk_hdr} )


### INSTAll

install( TARGETS "dump2disk" LIBRARY DESTINATION
	 "${INSTALL_LIB_PATH}/vs_plugins"  COMPONENT main )


