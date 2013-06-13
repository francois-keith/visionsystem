option ( CONTROLLER_OPENNI "Include controller openni ?" ON )

if ( CONTROLLER_OPENNI )

	include_directories ( "${Boost_INCLUDE_DIRS}" )

    ### Find OpenNI
    #add a hint so that it can find it without the pkg-config
    find_path(OPENNI_INCLUDE_DIR XnStatus.h
              HINTS ${NESTK_ROOT_DIRS_HINTS} ${PC_OPENNI_INCLUDEDIR} ${PC_OPENNI_INCLUDE_DIRS} /usr/include/openni /usr/include/ni
              PATHS "$ENV{PROGRAMFILES}/OpenNI/Include" "$ENV{PROGRAMW6432}/OpenNI/Include"
              PATH_SUFFIXES openni)
    #add a hint so that it can find it without the pkg-config
    find_library(OPENNI_LIBRARY
                 NAMES OpenNI64 OpenNI
                 HINTS ${NESTK_ROOT_DIRS_HINTS} ${PC_OPENNI_LIBDIR} ${PC_OPENNI_LIBRARY_DIRS} /usr/lib
                 PATHS "$ENV{PROGRAMFILES}/OpenNI/Lib${OPENNI_SUFFIX}" "$ENV{PROGRAMW6432}/OpenNI/Lib${OPENNI_SUFFIX}"
                 PATH_SUFFIXES lib
    )

    include_directories(${OPENNI_INCLUDE_DIR})
    IF(OPENNI_INCLUDE_DIR AND OPENNI_LIBRARY)
        message(STATUS "OpenNI found (include: ${OPENNI_INCLUDE_DIR}, lib: ${OPENNI_LIBRARY})")
    ELSE(OPENNI_INCLUDE_DIR AND OPENNI_LIBRARY)
        message(FATAL_ERROR "OpenNI not found (include: ${OPENNI_INCLUDE_DIR}, lib: ${OPENNI_LIBRARY})\n"
		"Note: you can turn off this dependency using the option CONTROLLER_OPENNI")
    ENDIF(OPENNI_INCLUDE_DIR AND OPENNI_LIBRARY)


	### File Lists

	file( GLOB openni_src "src/*.cpp" )
	file( GLOB openni_hdr "src/*.h*")

	### Targets

	add_library( "openni" SHARED ${openni_src} ${openni_hdr} )

	target_link_libraries ("openni" ${VISION_LIBRARIES}  ${Boost_LIBRARIES} ${OPENNI_LIBRARY})
	IF(WIN32)
	# This is done on win32 only because gcc does not need to resolve symbol at link time for libs
	target_link_libraries("openni" "visionsystem" )
	target_link_libraries("openni" "vision" )
	ENDIF(WIN32)
	PKG_CONFIG_USE_DEPENDENCY(openni configparser)

	### INSTAll

	install( TARGETS "openni" 
			DESTINATION "${INSTALL_LIB_PATH}/vs_controllers"  COMPONENT main )

    install( FILES "src/cameraopenni.h"
             DESTINATION "${INSTALL_HEADERS_PATH}/visionsystem/vs_controllers/openni/"
             COMPONENT main)


else ( CONTROLLER_OPENNI )

	message ( WARNING "Controller openni will not be included" ) 

endif ( CONTROLLER_OPENNI ) 
