#!/usr/bin/env bash

svn update ../..

REVISION=`svn info -r HEAD | grep Revision | cut -d ' ' -f 2`
ARCH=universal
NAME=`date "+installer_${ARCH}_%Y%m%d_r${REVISION}.dmg"`
FILE=build/Packages/${NAME}

if test -n "$1"; then
	ARCH=$1 # universal, ppc or i386
fi

./make.rb -V --arch ${ARCH} installer

hdiutil create -fs HFS+ -srcfolder build/Packages/esteid-dev.mpkg -volname esteid-dev ${FILE}
setfile -a E ${FILE}

if test -n "$2"; then
	scp ${FILE} $2
fi
