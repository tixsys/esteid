# automake for digidoc library 

# MSSP support
if WITH_MSSP
  msspdir=mssp
  mssp_SOURCES=mssp/soapC.c mssp/soapClient.c mssp/DigiDocMsspGw.c mssp/stdsoap2.c
  mssp_CFLAGS=-Imssp
endif

# WITH_TS support
if WITH_TS
  tsdir=ts
  ts_SOURCES=ts/DigiDocTS.c
  ts_CFLAGS=-I/usr/local/ssl/include
  ts_LDFLAGS=-L/usr/local/ssl/lib
endif

SUBDIRS = . pkcs11 $(msspdir) $(tsdir)

MAINTAINERCLEANFILES = Makefile.in


# libdigidoc
lib_LTLIBRARIES = libdigidoc.la
libdigidoc_la_SOURCES = DigiDocConfig.c  DigiDocLib.c  DigiDocObj.c DigiDocPKCS11.c DigiDocError.c DigiDocParser.c DigiDocDebug.c DigiDocSAXParser.c DigiDocMem.c DigiDocStack.c DigiDocEnc.c DigiDocEncGen.c DigiDocEncSAXParser.c DigiDocCert.c DigiDocConvert.c DigiDocGen.c DigiDocVerify.c DigiDocOCSP.c $(ts_SOURCES) $(mssp_SOURCES)

libdigidoc_la_LDFLAGS = @DIGIDOC_LIBS@ -version-info @DIGIDOC_LT_CURRENT@:@DIGIDOC_LT_REVISION@:@DIGIDOC_LT_AGE@ $(ts_LDFLAGS)
libdigidoc_la_CFLAGS = @DIGIDOC_CFLAGS@ -DDIGIDOC_DEBUG -DWITH_SOAPDEFS_H -DSYSCONFDIR=\"$(sysconfdir)\" $(mssp_CFLAGS) $(ts_CFLAGS) 

#if WITH_MSSP and WITH_TS
pkginclude_HEADERS = DigiDocConfig.h DigiDocLib.h  DigiDocPKCS11.h DigiDocError.h DigiDocSAXParser.h DigiDocParser.h DigiDocMem.h DigiDocStack.h DigiDocEnc.h DigiDocObj.h DigiDocEncGen.h DigiDocEncSAXParser.h DigiDocDebug.h DigiDocDefs.h DigiDocCert.h DigiDocConvert.h DigiDocVerify.h DigiDocOCSP.h mssp/stdsoap2.h mssp/soapdefs.h mssp/DigiDocMsspGw.h ts/DigiDocTS.h DigiDocGen.h
#else if WITH_MSSP
pkginclude_HEADERS = DigiDocConfig.h DigiDocLib.h  DigiDocPKCS11.h DigiDocError.h DigiDocSAXParser.h DigiDocParser.h DigiDocMem.h DigiDocStack.h DigiDocEnc.h DigiDocObj.h DigiDocEncGen.h DigiDocEncSAXParser.h DigiDocDebug.h DigiDocDefs.h DigiDocCert.h DigiDocConvert.h DigiDocVerify.h DigiDocOCSP.h  mssp/stdsoap2.h mssp/soapdefs.h mssp/DigiDocMsspGw.h DigiDocGen.h
#else if WITH_TS
pkginclude_HEADERS = DigiDocConfig.h DigiDocLib.h  DigiDocPKCS11.h DigiDocError.h DigiDocSAXParser.h DigiDocParser.h DigiDocMem.h DigiDocStack.h DigiDocEnc.h DigiDocObj.h DigiDocEncGen.h DigiDocEncSAXParser.h DigiDocDebug.h DigiDocDefs.h DigiDocCert.h DigiDocConvert.h DigiDocVerify.h DigiDocOCSP.h ts/DigiDocTS.h DigiDocGen.h
#else
pkginclude_HEADERS = DigiDocConfig.h DigiDocLib.h  DigiDocPKCS11.h DigiDocError.h DigiDocSAXParser.h DigiDocParser.h DigiDocMem.h DigiDocStack.h DigiDocEnc.h DigiDocObj.h DigiDocEncGen.h DigiDocEncSAXParser.h DigiDocDebug.h DigiDocDefs.h DigiDocCert.h DigiDocConvert.h DigiDocVerify.h DigiDocOCSP.h DigiDocGen.h
#endif

# there's also a small command-line program
bin_PROGRAMS = cdigidoc
cdigidoc_SOURCES = cdigidoc.c
cdigidoc_CFLAGS = @DIGIDOC_CFLAGS@ -DDIGIDOC_DEBUG -DWITH_SOAPDEFS_H $(mssp_CFLAGS) $(ts_CFLAGS)
cdigidoc_LDFLAGS = @LDFLAGS@ @DIGIDOC_LIBS@ libdigidoc.la
