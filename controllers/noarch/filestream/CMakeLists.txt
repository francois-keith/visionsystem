include_directories ( "${Boost_INCLUDE_DIRS}" )

### File Lists

file( GLOB filestream_src "src/*.cpp" )
file( GLOB filestream_hdr "src/*.h*")

### Targets

add_library( "filestream" SHARED ${filestream_src} ${filestream_hdr} )

target_link_libraries ("filestream" ${VISION_LIBRARIES}  ${Boost_FILESYSTEM_LIBRARY} )

### INSTAll

install( TARGETS "filestream" LIBRARY DESTINATION
	 "${INSTALL_LIB_PATH}/vs_controllers"  COMPONENT main )

