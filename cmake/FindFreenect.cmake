find_path ( FREENECT_INCLUDE_DIR NAMES libfreenect.hpp )
find_library( FREENECT_LIBRARY NAMES freenect )

IF ( FREENECT_INCLUDE_DIR AND FREENECT_LIBRARY )

	SET (FREENECT_FOUND TRUE)

ELSE (FREENECT_INCLUDE_DIR AND FREENECT_LIBRARY)

	SET (FREENECT_FOUND FALSE)

ENDIF (FREENECT_INCLUDE_DIR AND FREENECT_LIBRARY)

