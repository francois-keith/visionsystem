option( CONTROLLER_CAMV4L2 "include camv4l2 controller ?" ON )

if (CONTROLLER_CAMV4L2)
	
	### File Lists

	file( GLOB cam1394_src "src/*.cpp" )
	file( GLOB cam1394_hdr "src/*.h*")

	### Targets

	add_library( "camv4l2" SHARED ${cam1394_src} ${cam1394_hdr} )

	target_link_libraries ( "camv4l2" ${VISION_LIBRARIES} )

	### INSTAll

	install( TARGETS "camv4l2" LIBRARY DESTINATION
			 "${INSTALL_LIB_PATH}/vs_controllers"  COMPONENT main )

else(CONTROLLER_CAMV4L2)
        
	message(WARNING "camv4l2 controller will not be included")
	
endif(CONTROLLER_CAMV4L2)
