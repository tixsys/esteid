#!/bin/sh

# Generate dummy Makefile to install xsd binary

cat > $1/Makefile <<E_O_F
INSTALL ?= install -p

all:
	echo "DO NOTHING"
install:
	\$(INSTALL) -d $2/bin
	\$(INSTALL) -m 775 bin/xsd $2/bin/xsdcxx
	install-headers "*.*" libxsd/xsd $2/include/xsd
E_O_F
