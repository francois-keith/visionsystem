### CMake file for vision_server
###
###    From the off-tree build directory, invoke:
###	 cmake <PATH_TO_vision_server>
###
###
###   - Feb 2010: Initial version <druon@lirmm.fr>
###
###
###############################################################################


cmake_minimum_required (VERSION 2.6)

project (visionsystem)
set (visionsystem_VERSION_MAJOR 0)
set (visionsystem_VERSION_MINOR 3)

set( CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /lib /usr/lib /usr/local/lib )
set( CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} /usr/include /usr/local/include )

set( CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  
			"${PROJECT_SOURCE_DIR}/cmake" ) 

set ( INSTALL_BIN_PATH bin )
set ( INSTALL_LIB_PATH lib )
set ( INSTALL_AR_PATH  lib )
set ( INSTALL_HEADERS_PATH include )

# Make relative paths absolute (needed later on)

foreach(p LIB BIN HEADERS AR)
        set(var INSTALL_${p}_PATH)
        if(NOT IS_ABSOLUTE "${${var}}")
                set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
        endif()
endforeach()

### Search for Lib/Headers

find_package( vision )
find_package( configparser ) 
find_package( Boost 1.40.0 COMPONENTS thread filesystem)

### Set include directories 
        
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/lib/include"
	             "${CMAKE_CURRENT_BINARY_DIR}"
		     ${VISION_INCLUDE_DIRS} 
		     ${CONFIGPARSER_INCLUDE_DIRS}
		     ${Boost_INCLUDE_DIRS} )

link_directories( "${VISION_LIBRARY_DIR}" 
		   ${CONFIGPARSER_LIBRARY_DIRS}
		   ${Boost_LIBRARY_DIRS} )

### Go to subdirs

add_subdirectory(  "lib" )
add_subdirectory(  "controllers" )
add_subdirectory(  "core" )
#add_subdirectory(  "plugins" )
