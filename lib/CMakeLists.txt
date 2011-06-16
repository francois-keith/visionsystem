### CMake file for vision library
###
###    From the off-tree build directory, invoke:
###	 cmake <PATH_TO_lib_vision>
###
###
###   - Feb 2010: Initial version <druon@lirmm.fr>
###
###
###############################################################################



### File Lists

file( GLOB vs_lib_src "src/*.cpp" "src/fast/*.cpp" )
file( GLOB vs_lib_hdr "src/*.h*")
file( GLOB vs_lib_hdr_global "include/visionsystem/*.h*" ) 

### Targets

add_library( "visionsystem" SHARED ${vs_lib_src} ${vs_lib_hdr} )

### INSTAll

install( TARGETS "visionsystem"
	    RUNTIME DESTINATION ${INSTALL_BIN_PATH}  COMPONENT main
	    LIBRARY DESTINATION ${INSTALL_LIB_PATH}  COMPONENT main
	    ARCHIVE DESTINATION ${INSTALL_AR_PATH}   COMPONENT main )

install( FILES ${vs_lib_hdr_global}
	    DESTINATION ${INSTALL_HEADERS_PATH}/visionsystem/${name}
	    COMPONENT main )

### Configure file

configure_file(visionsystemConfig.cmake.in "${PROJECT_BINARY_DIR}/visionsystemConfig.cmake" @ONLY)

install ( FILES "${PROJECT_BINARY_DIR}/visionsystemConfig.cmake" 
	  DESTINATION "${INSTALL_LIB_PATH}/visionsystem" COMPONENT dev )  
        

