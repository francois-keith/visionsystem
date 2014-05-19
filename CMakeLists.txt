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

set(CMAKE_LEGACY_CYGWIN_WIN32 1)

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


IF(WIN32)
  SET(LIBDIR_KW "/LIBPATH:")
  SET(LIBINCL_KW "")
  SET(LIB_EXT ".lib")
ENDIF(WIN32)

IF(UNIX)
  SET(LIBDIR_KW "-L")
  SET(LIBINCL_KW "-l")
  SET(LIB_EXT "")
ENDIF(UNIX)

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
find_package( Boost 1.40.0 COMPONENTS system thread filesystem serialization)
find_package( ImageMagick COMPONENTS Magick++ ) 

IF(ImageMagick_Magick++_FOUND)
SET(ImageMagick_Magick++_FOUND_BOOL 1)
ELSE(ImageMagick_Magick++_FOUND)
SET(ImageMagick_Magick++_FOUND_BOOL 0)
ENDIF(ImageMagick_Magick++_FOUND)

if ( ImageMagick_Magick++_FOUND )		 
	include_directories ( "${ImageMagick_Magick++_INCLUDE_DIR}" )
endif ( ImageMagick_Magick++_FOUND )	


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
ELSE()
    include(FindPkgConfig)
    pkg_check_modules(LIBXXF86VM xxf86vm)
ENDIF(WIN32)

### OpenMP support
if(OPENMP_FOUND)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif(OPENMP_FOUND)

### Set include directories 
        
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/lib/include"
	             "${CMAKE_CURRENT_BINARY_DIR}"
		     ${VISION_INCLUDE_DIRS} 
		     ${CONFIGPARSER_INCLUDE_DIRS}
		     ${Boost_INCLUDE_DIRS}
		     ${DL_INCLUDE_DIR} )

link_directories( "${VISION_LIBRARY_DIR}" 
		   ${CONFIGPARSER_LIBRARY_DIRS}
		   ${Boost_LIBRARY_DIRS} )

### Go to subdirs

add_subdirectory(  "lib" )
add_subdirectory(  "controllers" )
add_subdirectory(  "core" )
add_subdirectory(  "plugins" )
add_subdirectory(  "utils" )
