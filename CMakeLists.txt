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

INCLUDE(cmake/base.cmake)
INCLUDE(cmake/cpack.cmake)

set(CMAKE_LEGACY_CYGWIN_WIN32 1)
SET(CMAKE_MODULE_PATH ${${PROJECT_NAME}_SOURCE_DIR}/CMakeModules)

# name of the project
SET(PROJECT_NAME visionsystem)
SET(PROJECT_CONTACT )
SET(PROJECT_DESCRIPTION "")
SET(PROJECT_URL "")

set( CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /lib /usr/lib /usr/local/lib )
set( CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} /usr/include /usr/local/include )

set( CMAKE_MODULE_PATH  ${PROJECT_SOURCE_DIR}/CMakeModules) 

set (visionsystem_VERSION_MAJOR 0)
set (visionsystem_VERSION_MINOR 3)

set ( INSTALL_BIN_PATH bin )
set ( INSTALL_LIB_PATH lib )
set ( INSTALL_AR_PATH  lib )
set ( INSTALL_HEADERS_PATH include )

SET(CXX_DISABLE_WERROR True)
SET(CXX_DISABLE_WALL True)
SET(CMAKE_VERBOSE_MAKEFILE ON)

SETUP_PROJECT()

### Search for Lib/Headers

ADD_REQUIRED_DEPENDENCY("vision")

find_package( Boost 1.40.0 COMPONENTS system thread filesystem serialization)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

	find_package( Dl ) 

ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

### Assume libavcodec is available under WIN32 and reachable by Visual Studio
IF(WIN32)
	SET(LIBAVCODEC_FOUND 1)
	SET(LIBAVCODEC_FOUND_NUMERAL 1)
	SET(LIBAVCODEC_LIBRARIES "avutil.lib;avcodec.lib;libx264.lib")

	SET(LIBSWSCALE_FOUND 1)
	SET(LIBSWSCALE_LIBRARIES "swscale.lib")
ENDIF(WIN32)

### OpenMP support
if(OPENMP_FOUND)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif(OPENMP_FOUND)


### Locate xmlrpc
set(XMLRPC_LIBRARIES "XmlRpc")
set(XMLRPC_DIR "")
find_path(XMLRPC_INCLUDE_DIRS "XmlRpc.h" 
	PATHS ${CMAKE_INCLUDE_PATH} ${XMLRPC_DIR}/src/)
find_library(XMLRPC_LIBRARY_PATH ${XMLRPC_LIBRARIES}
	PATHS ${CMAKE_LIBRARY_PATH} ${XMLRPC_DIR})

get_filename_component(XMLRPC_LIBRARY_DIRS ${XMLRPC_LIBRARY_PATH} PATH)

IF(XMLRPC_INCLUDE_DIRS AND XMLRPC_LIBRARY_DIRS)
  SET(XMLRPC_FOUND TRUE)
ELSE(XMLRPC_INCLUDE_DIRS AND XMLRPC_LIBRARY_DIRS)
  SET(XMLRPC_FOUND FALSE)
ENDIF(XMLRPC_INCLUDE_DIRS AND XMLRPC_LIBRARY_DIRS)


### Set include directories 
        
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/lib/include"
	             "${CMAKE_CURRENT_BINARY_DIR}"
		     ${Boost_INCLUDE_DIRS}
		     ${DL_INCLUDE_DIR} )

link_directories( ${Boost_LIBRARY_DIRS} )

### Go to subdirs

add_subdirectory(  "lib" )
add_subdirectory(  "controllers" )
add_subdirectory(  "core" )
add_subdirectory(  "plugins" )
add_subdirectory(  "utils" )

# Add dependency towards the library in the pkg-config file.
PKG_CONFIG_APPEND_LIBS("visionsystem")

SETUP_PROJECT_FINALIZE()
SETUP_PROJECT_CPACK()
