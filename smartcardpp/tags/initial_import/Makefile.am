SUBDIRS = esteid sim

h_sources = smartcard++.h \
	    CardBase.h \
	    common.h \
	    CTAPIManager.h \
	    DynamicLibrary.h \
	    helperMacro.h \
	    ManagerInterface.h \
	    PCSCManager.h \
	    precompiled.h \
	    SCError.h \
	    SmartCardManager.h \
	    locked_allocator.h \
	    PinString.h \
	    types.h 

cpp_sources = CardBase.cpp \
	    CTAPIManager.cpp \
	    DynamicLibrary.cpp \
	    PCSCManager.cpp \
	    SCError.cpp \
	    SmartCardManager.cpp \
	    locked_allocator.cpp \
	    types.cpp

library_includedir=$(includedir)/$(SMARTCARD_LIBRARY_NAME)-$(SMARTCARD_RELEASE)/$(SMARTCARD_LIBRARY_NAME)
library_include_HEADERS = $(h_sources)

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/smartcard++ -I${oldincludedir}/PCSC
lib_LTLIBRARIES= libsmartcard++-1.0.la
libsmartcard___1_0_la_SOURCES= $(h_sources) $(cpp_sources)
libsmartcard___1_0_la_LDFLAGS= -ldl -version-info $(SMARTCARD_VERSION_INFO) -release $(SMARTCARD_RELEASE)
libsmartcard___1_0_la_LIBADD= esteid/libesteid.la sim/libsim.la

EXTRA_DIST=CMakeLists.txt premake4.lua