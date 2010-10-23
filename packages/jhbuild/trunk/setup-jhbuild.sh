#!/bin/sh
#
# Script that sets up jhbuild for Open EstEID.
# Run this to checkout jhbuild and the required configuration.
# Copyright 2010 Antti Andreimann
#
# Quick HOWTO:
#
# sh setup-jhbuild.sh
#
# jhbuild -f config bootstrap
# jhbuild -f config build
#

mydir=`dirname $0`
mydir=`pwd`/$mydir

BDIR=$mydir/build

set -e

die()
{
    echo $1
    exit 1
}

# Sanity Check
[ x`which svn` == x ] && die "Svn (subversion) must be installed."
[ x`which hg` == x ]  && die "Hg (mercurial) must be installed."
[ x`which git` == x ] && die "Git must be installed."

[ -f $mydir/jhbuild-revision ] || die "Can't find $mydir/jhbuild-revision"

JHB_REV=`cat $mydir/jhbuild-revision`

echo "Checking out jhbuild from git ..."
if ! test -d $BDIR/jhbuild; then
    mkdir -p $BDIR/work
    cd $BDIR
    git clone git://git.gnome.org/jhbuild 
    cd jhbuild
    git checkout $JHB_REV
else
    cd $BDIR/jhbuild && \
        git checkout -f $JHB_REV 2> /dev/null || \
            (git fetch && git checkout -f $JHB_REV)
fi

for p in $mydir/patches/jhbuild-*.patch; do
    if [ -f "$p" ]; then
        echo "Applying `basename $p` ..."
        git apply $p
    fi
done

echo "Installing jhbuild ..."
make -f Makefile.plain DISABLE_GETTEXT=1 install >/dev/null

if test "x`echo $PATH | grep $HOME/.local/bin`" == x; then
    echo "PATH does not contain $HOME/.local/bin, it is recommended that you add that."
    echo 
fi

echo "Done."
