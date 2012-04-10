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


set(EXEC_NAME "vs_bin2png")

set(${EXEC_NAME}_SRC
    vs_bin2png.cpp)

add_executable ( ${EXEC_NAME} ${${EXEC_NAME}_SRC} )


### Linking

target_link_libraries ( ${EXEC_NAME} 
				  ${VISION_LIBRARIES}
				  ${CONFIGPARSER_LIBRARIES}
				  ${Boost_LIBRARIES}
				  ${DL_LIBRARIES}
)  

### Install

install( TARGETS "vs_bin2png"
	    RUNTIME DESTINATION bin COMPONENT main
	    LIBRARY DESTINATION lib COMPONENT main
	    ARCHIVE DESTINATION lib COMPONENT main )


set(EXEC_NAME "vs_png2bin")

set(${EXEC_NAME}_SRC
    vs_png2bin.cpp)

add_executable ( ${EXEC_NAME} ${${EXEC_NAME}_SRC} )


### Linking

target_link_libraries ( ${EXEC_NAME} 
				  ${VISION_LIBRARIES}
				  ${CONFIGPARSER_LIBRARIES}
				  ${Boost_LIBRARIES}
				  ${DL_LIBRARIES}
)  

### Install

install( TARGETS "vs_png2bin"
	    RUNTIME DESTINATION bin COMPONENT main
	    LIBRARY DESTINATION lib COMPONENT main
	    ARCHIVE DESTINATION lib COMPONENT main )

