### CMake file for vs_core
###
###    From the off-tree build directory, invoke:
###	 cmake <PATH_TO_vision_server>
###
###
###   - Feb 2010: Initial version <druon@lirmm.fr>
###
###
###############################################################################


### File Lists

file( GLOB vs_core_srcs "src/*.cpp" )
file( GLOB vs_core_hdrs "src/*.h*")

### Targets

add_executable ( "vs_core" ${vs_core_srcs} ${vs_core_hdrs} )


### Linking

target_link_libraries ( "vs_core" visionsystem
				  ${Boost_LIBRARIES}
				  ${DL_LIBRARIES}
)

ADD_REQUIRED_DEPENDENCY(libavcodec)
PKG_CONFIG_USE_DEPENDENCY(vs_core libavcodec)

ADD_REQUIRED_DEPENDENCY(vpx)
PKG_CONFIG_USE_DEPENDENCY(vs_core vpx)

PKG_CONFIG_USE_DEPENDENCY(vs_core vision)
PKG_CONFIG_USE_DEPENDENCY(vs_core configparser)

### Install

install( TARGETS "vs_core"
	    RUNTIME DESTINATION bin COMPONENT main
	    LIBRARY DESTINATION lib COMPONENT main
	    ARCHIVE DESTINATION lib COMPONENT main )

