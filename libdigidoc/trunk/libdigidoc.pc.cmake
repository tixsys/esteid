prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: libdigidoc
Description: libdigidoc
Version: @DIGIDOC_VERSION@
Libs: -L${libdir} -ldigidoc
Cflags: -I${includedir}
