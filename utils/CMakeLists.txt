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

PKG_CONFIG_USE_DEPENDENCY(${EXEC_NAME} vision)
target_link_libraries ( ${EXEC_NAME} 
				  ${VISION_LIBRARIES}
				  ${CONFIGPARSER_LIBRARIES}
				  ${Boost_LIBRARIES}
				  ${DL_LIBRARIES}
)  


ADD_REQUIRED_DEPENDENCY(libavcodec)
PKG_CONFIG_USE_DEPENDENCY(vs_bin2png libavcodec)

ADD_REQUIRED_DEPENDENCY(vpx)
PKG_CONFIG_USE_DEPENDENCY(vs_bin2png vpx)
### Install

install( TARGETS "vs_bin2png"
	    RUNTIME DESTINATION bin COMPONENT main
	    LIBRARY DESTINATION lib COMPONENT main
	    ARCHIVE DESTINATION lib COMPONENT main )


set(EXEC_NAME "vs_bin2pgm")

set(${EXEC_NAME}_SRC
    vs_bin2pgm.cpp)

add_executable ( ${EXEC_NAME} ${${EXEC_NAME}_SRC} )


### Linking

target_link_libraries ( ${EXEC_NAME} 
				  ${Boost_LIBRARIES}
				  ${DL_LIBRARIES}
)  
PKG_CONFIG_USE_DEPENDENCY(${EXEC_NAME} vision)

### Install

install( TARGETS "vs_bin2pgm"
	    RUNTIME DESTINATION bin COMPONENT main
	    LIBRARY DESTINATION lib COMPONENT main
	    ARCHIVE DESTINATION lib COMPONENT main )



set(EXEC_NAME "vs_png2bin")

set(${EXEC_NAME}_SRC
    vs_png2bin.cpp)

add_executable ( ${EXEC_NAME} ${${EXEC_NAME}_SRC} )


### Linking

target_link_libraries ( ${EXEC_NAME} 
				  ${Boost_LIBRARIES}
				  ${DL_LIBRARIES}
)  
PKG_CONFIG_USE_DEPENDENCY(${EXEC_NAME} vision)

### Install

install( TARGETS "vs_png2bin"
	    RUNTIME DESTINATION bin COMPONENT main
	    LIBRARY DESTINATION lib COMPONENT main
	    ARCHIVE DESTINATION lib COMPONENT main )


set(EXEC_NAME "vs_pgm2bin")

set(${EXEC_NAME}_SRC
    vs_pgm2bin.cpp)

add_executable ( ${EXEC_NAME} ${${EXEC_NAME}_SRC} )


### Linking

target_link_libraries ( ${EXEC_NAME} 
				  ${Boost_LIBRARIES}
				  ${DL_LIBRARIES}
)  
PKG_CONFIG_USE_DEPENDENCY(${EXEC_NAME} vision)

### Install

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

install( TARGETS "vs_pgm2bin"
	    RUNTIME DESTINATION bin COMPONENT main
	    LIBRARY DESTINATION lib COMPONENT main
	    ARCHIVE DESTINATION lib COMPONENT main )


if(NOT WIN32)
    set(EXEC_NAME "vs_bin2mkv")
    
    set(${EXEC_NAME}_SRC
        vs_bin2mkv.cpp
    )
    
    add_executable ( ${EXEC_NAME} ${${EXEC_NAME}_SRC} )
    
    
    ### Linking
    
    target_link_libraries ( ${EXEC_NAME} 
    				  ${Boost_LIBRARIES}
    				  ${DL_LIBRARIES}
    )  
    PKG_CONFIG_USE_DEPENDENCY(${EXEC_NAME} vision)

    ### Install
    
    install( TARGETS "vs_bin2mkv"
    	    RUNTIME DESTINATION bin COMPONENT main
    	    LIBRARY DESTINATION lib COMPONENT main
    	    ARCHIVE DESTINATION lib COMPONENT main )

    ### Install kill-vs_core utility

    install( FILES "kill-vs_core"
                DESTINATION ${INSTALL_BIN_PATH}
                PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE )
    
endif(NOT WIN32)
