#!/bin/sh
#
# Script that executes jhbuild for building Open EstEID.
# Copyright 2010 Antti Andreimann
#
# Quick HOWTO:
#
# sh jhbuild.sh build

mydir=`dirname $0`
mydir=`pwd`/$mydir

BDIR=$mydir/build

set -

# Set HOME environment variable on windows
[ "$HOME" ] || export HOME="$HOMEDRIVE/$HOMEPATH"

"$BDIR"/bin/jhbuild -f "$mydir/config" "$@"
