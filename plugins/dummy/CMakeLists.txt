### File Lists

file( GLOB dummy_src "src/*.cpp" )
file( GLOB dummy_hdr "src/*.h*")

### Targets

add_library( "dummy" SHARED ${dummy_src} ${dummy_hdr} )


### INSTAll

install( TARGETS "dummy" LIBRARY DESTINATION
	 "${INSTALL_LIB_PATH}/vs_plugins"  COMPONENT main )


