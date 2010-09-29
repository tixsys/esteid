prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@LIB_INSTALL_DIR@
includedir=@CMAKE_INSTALL_PREFIX@/include

Name: libdigidoc
Description: Libdigidoc library for handling digitally signed documents
Version: @DIGIDOC_VERSION@
Libs: -L${libdir} -ldigidoc
Cflags: -I${includedir}
