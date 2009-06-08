prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: libdigidoc2
Description: libdigidoc2
Version: @DIGIDOC_VERSION@
Libs: -L${libdir} -ldigidoc2
Cflags: -I${includedir}/libdigidoc2
