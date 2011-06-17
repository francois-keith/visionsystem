
### Linux specific controllers
###

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

	add_subdirectory(  "linux" ) 

ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

### Windows specific controllers
###

if (WIN)

	add_subdirectory( "win" ) 

endif (WIN)

### OS-independant controllers
###

add_subdirectory ("noarch") 
