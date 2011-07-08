option ( CONTROLLER_KINECT "Include kinect controller ?" OFF )

if ( CONTROLLER_KINECT )


	FIND_PACKAGE( Freenect REQUIRED )

	if ( FREENECT_FOUND )

		include_directories ( "${FREENECT_INCLUDE_DIR}" )
		
		### File Lists

		file( GLOB kinect_src "src/*.cpp" )
		file( GLOB kinect_hdr "src/*.h*")

		### Targets

		add_library( "kinect" SHARED ${kinect_src} ${cam1394_hdr} )

		target_link_libraries ( "kinect" 
					${VISION_LIBRARIES}
					${FREENECT_LIBRARY} )

		### INSTAll

		install( TARGETS "kinect" LIBRARY DESTINATION
			 "${INSTALL_LIB_PATH}/vs_controllers"  COMPONENT main )

	endif ( FREENECT_FOUND )

else ( CONTROLLER_KINECT )

	message ( WARNING "Controller kinect will not be included" ) 

endif ( CONTROLLER_KINECT )
