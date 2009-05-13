#!/bin/bash
echo "deleting automatically generated files. Use autoreconf --install to generate them again"
rm -f aclocal.m4 compile config.guess config.h config.log config.status 
rm -f config.sub configure depcomp install-sh libdigidoc.pc libtool ltmain.sh 
rm -f Makefile Makefile.in missing stamp-h1 stampg.in
rm -rf autom4te.cache src/.deps src/.lib
rm -f src/Makefile src/Makefile.in 
rm -f src/mssp/Makefile src/mssp/Makefile.in 
rm -f src/pkcs11/Makefile src/pkcs11/Makefile.in 
rm -f src/dds/Makefile src/dds/Makefile.in 
rm -f etc/Makefile etc/Makefile.in 
rm -f doc/Makefile doc/Makefile.in 
rm -f win32/Makefile win32/Makefile.in 
