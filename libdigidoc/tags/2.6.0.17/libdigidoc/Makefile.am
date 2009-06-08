# automake for digidoc library 

SUBDIRS = . pkcs11

MAINTAINERCLEANFILES = Makefile.in


# libdigidoc
lib_LTLIBRARIES = libdigidoc.la
libdigidoc_la_SOURCES = DigiDocConfig.c  DigiDocLib.c  DigiDocObj.c DigiDocPKCS11.c DigiDocError.c DigiDocParser.c DigiDocDebug.c DigiDocSAXParser.c DigiDocMem.c DigiDocStack.c DigiDocEnc.c DigiDocEncGen.c DigiDocEncSAXParser.c DigiDocCert.c DigiDocConvert.c DigiDocGen.c DigiDocVerify.c DigiDocOCSP.c DigiDocDfExtract.c

libdigidoc_la_LDFLAGS = @DIGIDOC_LIBS@ -version-info @DIGIDOC_LT_CURRENT@:@DIGIDOC_LT_REVISION@:@DIGIDOC_LT_AGE@ $(ts_LDFLAGS)
libdigidoc_la_CFLAGS = @DIGIDOC_CFLAGS@ -DDIGIDOC_DEBUG -DWITH_SOAPDEFS_H -DSYSCONFDIR=\"$(sysconfdir)\" 

pkginclude_HEADERS = DigiDocConfig.h DigiDocLib.h  DigiDocPKCS11.h DigiDocError.h DigiDocSAXParser.h DigiDocParser.h DigiDocMem.h DigiDocStack.h DigiDocEnc.h DigiDocObj.h DigiDocEncGen.h DigiDocEncSAXParser.h DigiDocDebug.h DigiDocDefs.h DigiDocCert.h DigiDocConvert.h DigiDocVerify.h DigiDocOCSP.h DigiDocGen.h DigiDocDfExtract.h

# there's also a small command-line program
bin_PROGRAMS = cdigidoc
cdigidoc_SOURCES = cdigidoc.c
cdigidoc_CFLAGS = @DIGIDOC_CFLAGS@ -DDIGIDOC_DEBUG -DWITH_SOAPDEFS_H
cdigidoc_LDFLAGS = @LDFLAGS@ @DIGIDOC_LIBS@ libdigidoc.la
