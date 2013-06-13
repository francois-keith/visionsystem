option ( PLUGIN_MOVIE_MAKER "Include plugin movie-maker ?" ON )

if ( PLUGIN_MOVIE_MAKER )

	### File Lists

	file( GLOB movie-maker_src "src/*.cpp" )
	file( GLOB movie-maker_hdr "src/*.h*")

	### Targets

	add_library( "movie-maker" SHARED ${movie-maker_src} ${movie-maker_hdr} )

    target_link_libraries( "movie-maker" ${Boost_LIBRARIES})

	IF(WIN32)
	# This is done on win32 only because gcc does not need to resolve symbol at link time for libs
	target_link_libraries("movie-maker" "visionsystem" )
    target_link_libraries("movie-maker" "x264" )
	ENDIF(WIN32)
	PKG_CONFIG_USE_DEPENDENCY(movie-maker  vision)


	### INSTAll

	install( TARGETS "movie-maker" 
			DESTINATION "${INSTALL_LIB_PATH}/vs_plugins"  COMPONENT main )

else ( PLUGIN_MOVIE_MAKER )

	message ( WARNING "Plugin movie-maker will not be included" )

endif ( PLUGIN_MOVIE_MAKER ) 
