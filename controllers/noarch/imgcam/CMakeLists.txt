option ( CONTROLLER_IMGCAM "Include imgcam ?" ON )

if ( CONTROLLER_IMGCAM )

	### File Lists

	file( GLOB imgcam_src "src/*.cpp" )
	file( GLOB imgcam_hdr "src/*.h*")

	### Targets

	add_library( "imgcam" SHARED ${imgcam_src} ${imgcam_hdr} )
	IF(WIN32)
	# This is done on win32 only because gcc does not need to resolve symbol at link time for libs
	target_link_libraries("imgcam" "visionsystem" )
	target_link_libraries("imgcam" "vision" )
	target_link_libraries("imgcam" "configparser" )
	ENDIF(WIN32)

	### INSTAll

	install( TARGETS "imgcam" 
			DESTINATION "${INSTALL_LIB_PATH}/vs_controllers"  COMPONENT main)

else ( CONTROLLER_IMGCAM )

	message ( WARNING "Controller imgcam will not be included" ) 

endif ( CONTROLLER_IMGCAM) 

