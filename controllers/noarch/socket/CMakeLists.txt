include_directories ( "${Boost_INCLUDE_DIRS}" )

### File Lists

file( GLOB socket_src "src/*.cpp" )
file( GLOB socket_hdr "src/*.h")
file( GLOB socket_hdr "src/*.hpp")

### Targets

add_library( "socket" SHARED ${socket_src} ${socket_hdr} )

target_link_libraries ("socket" ${VISION_LIBRARIES}  ${Boost_SYSTEM_LIBRARY} )

### INSTAll

install( TARGETS "socket" LIBRARY DESTINATION
	 "${INSTALL_LIB_PATH}/vs_controllers"  COMPONENT main )

