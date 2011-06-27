find_package(OpenGL)
find_package( X11)
        
include_directories( "${X11_X11_INCLUDE_PATH}"
		     "${OPENGL_INCLUDE_DIR}"
)

### File Lists

file( GLOB glview_src "src/*.cpp" )
file( GLOB glview_hdr "src/*.h*")

### Targets

add_library( "glview" SHARED ${glview_src} ${glview_hdr} )

target_link_libraries ("glview" ${CONFIGPARSER_LIBRARIES} ${VISION_LIBRARIES} ${X11_X11_LIB} ${OPENGL_gl_LIBRARY} "Xxf86vm" )

### INSTAll

install( TARGETS "glview" LIBRARY DESTINATION
	 "${INSTALL_LIB_PATH}/vs_plugins"  COMPONENT main )


