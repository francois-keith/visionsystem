### File Lists

file( GLOB imgcam_src "src/*.cpp" )
file( GLOB imgcam_hdr "src/*.h*")

### Targets

add_library( "imgcam" SHARED ${imgcam_src} ${imgcam_hdr} )

### INSTAll

install( TARGETS "imgcam" LIBRARY DESTINATION
		 "${INSTALL_LIB_PATH}/vs_controllers"  COMPONENT main )


