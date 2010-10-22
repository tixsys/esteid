#!/bin/sh

# Generate dummy Makefile to install xsd binary

cat > $1/Makefile <<E_O_F
all:
	echo "DO NOTHING"
install:
	\$(INSTALL) -d $2/bin
	\$(INSTALL) -m 775 bin/xsd $2/bin/xsd
E_O_F
