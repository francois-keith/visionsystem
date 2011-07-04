### CMake file for utilities 
###
###    From the off-tree build directory, invoke:
###	 cmake <PATH_TO_vision_server>
###
###
###   - Feb 2010: Initial version <druon@lirmm.fr>
###
###
###############################################################################


set(EXEC_NAME "bin2png")

set(${EXEC_NAME}_SRC
    bin2png.cpp)

add_executable ( ${EXEC_NAME} ${${EXEC_NAME}_SRC} )


### Linking

target_link_libraries ( ${EXEC_NAME} 
				  ${VISION_LIBRARIES}
				  ${CONFIGPARSER_LIBRARIES}
				  ${Boost_FILESYSTEM_LIBRARIES}
				  ${DL_LIBRARIES}
)  

### Install

install( TARGETS "bin2png"
	    RUNTIME DESTINATION bin COMPONENT main
	    LIBRARY DESTINATION lib COMPONENT main
	    ARCHIVE DESTINATION lib COMPONENT main )

