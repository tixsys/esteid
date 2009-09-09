
h_sources = PKCS11Context.h  precompiled.h

cpp_sources = esteidpkcs11.cpp  PKCS11Context.cpp  precompiled.cpp

library_includedir=$(includedir)/$(SMARTCARD_LIBRARY_NAME)-$(PKCS11_RELEASE)/$(SMARTCARD_LIBRARY_NAME)
library_include_HEADERS = $(h_sources)

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/src -I${oldincludedir}/PCSC
lib_LTLIBRARIES= libesteidpkcs11-1.0.la
libesteidpkcs11_1_0_la_SOURCES= $(h_sources) $(cpp_sources)
libesteidpkcs11_1_0_la_LDFLAGS= -version-info $(PKCS11_VERSION_INFO) -release $(PKCS11_RELEASE)
libesteidpkcs11_1_0_la_LIBADD=@top_builddir@/src/utility @top_builddir@/smartcard++/.libs/libsmartcard++-1.0.la

EXTRA_DIST=CMakeLists.txt premake4.lua
