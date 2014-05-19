prefix=${CMAKE_INSTALL_PREFIX}
exec_prefix=${CMAKE_INSTALL_PREFIX}
libdir=${CMAKE_INSTALL_PREFIX}/lib
bindir=${CMAKE_INSTALL_PREFIX}/bin
pkglibdir=${CMAKE_INSTALL_PREFIX}/lib/${_PKG_CONFIG_PROJECT_NAME}
includedir=${CMAKE_INSTALL_PREFIX}/include
datarootdir=${CMAKE_INSTALL_PREFIX}/share
pkgdatarootdir=${CMAKE_INSTALL_PREFIX}/share

Name: ${PLUGIN_NAME}
Description: ${PLUGIN_DESCRIPTION}
URL: ${PKG_CONFIG_URL}
Version: ${PKG_CONFIG_VERSION}
Requires: ${PKG_CONFIG_REQUIRES}
Conflicts: 
Libs: ${PLUGIN_LIBS}
Cflags: ${PLUGIN_CFLAGS}

