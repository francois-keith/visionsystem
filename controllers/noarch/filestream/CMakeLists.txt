option ( CONTROLLER_FILESTREAM "Include controller filestream ?" ON )

if ( CONTROLLER_FILESTREAM )

	include_directories ( "${Boost_INCLUDE_DIRS}" )

	### File Lists

	file( GLOB filestream_src "src/*.cpp" )
	file( GLOB filestream_hdr "src/*.h*")

	### Targets

	add_library( "filestream" SHARED ${filestream_src} ${filestream_hdr} )

	target_link_libraries ("filestream" ${VISION_LIBRARIES}  ${Boost_LIBRARIES} )
	IF(WIN32)
	# This is done on win32 only because gcc does not need to resolve symbol at link time for libs
	target_link_libraries("filestream" "visionsystem" )
	ENDIF(WIN32)

	PKG_CONFIG_USE_DEPENDENCY(filestream configparser)
	PKG_CONFIG_USE_DEPENDENCY(filestream vision)

	### INSTAll

	install( TARGETS "filestream" 
			DESTINATION "${INSTALL_LIB_PATH}/vs_controllers"  COMPONENT main )

else ( CONTROLLER_FILESTREAM )

	message ( WARNING "Controller filestream will not be included" ) 

endif ( CONTROLLER_FILESTREAM ) 
