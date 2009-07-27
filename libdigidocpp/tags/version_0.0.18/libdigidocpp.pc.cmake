prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@LIB_INSTALL_DIR@
includedir=@CMAKE_INSTALL_PREFIX@/include

Name: libdigidocpp
Description: Libdigidocpp C++ library for handling digitally signed documents
Version: @DIGIDOCPP_VERSION@
Libs: -L${libdir} -ldigidocpp
Cflags: -I${includedir}
