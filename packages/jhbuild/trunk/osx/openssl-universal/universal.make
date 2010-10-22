#
# A Makefile to build universal openssl binaries for MacOS X
# Antti Andreimann <anttix@users.sourceforge.net>
# License: LGPL2+
# 

all: openssl-lipo

openssl-lipo: darwin-ppc darwin-i386 darwin64-x86_64
	sh lipo.sh $+

# x86_64 depends on i386, i386 depends on ppc. This dependency chain is
# synthetic and is only here to ensure that parallel make will not
# try to run builds at the same time.

darwin-ppc:
	sh compile.sh $(MAKE) $(PREFIX) $@

darwin-i386: darwin-ppc
	sh compile.sh $(MAKE) $(PREFIX) $@

darwin64-x86_64: darwin-i386
	sh compile.sh $(MAKE) $(PREFIX) $@

install: openssl-lipo
	$(MAKE) -f Makefile INSTALL_PREFIX=$(DESTDIR) install_sw
