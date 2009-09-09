noinst_LTLIBRARIES = libutility.la
libutility_la_SOURCES = asnObject.cpp \
  asnCertificate.cpp \
  asnObject.cpp \
  converters.cpp \
  logger.cpp \
  monitorThread.cpp \
  opensslObj.cpp \
  pkcs11module.cpp \
  threadObj.cpp \
  winInetObj.cpp \
  asnCertificate.h \
  asnObject.h \
  converters.h \
  logger.h \
  monitorThread.h \
  netObj.h \
  opensslObj.h \
  pinDialog_res.h \
  pkcs11module.h \
  precompiled.h \
  threadObj.h \
  winInetObj.h

#  pinDialog.cpp \ pinDialog.h 

INCLUDES = -I$(top_srcdir) -I@oldincludedir@/gtkmm-2.4/include -I@oldincludedir@/gtkmm-2.4/ 
libutility_la_LDFLAGS = -lpthread

EXTRA_DIST=CMakeLists.txt premake4.lua