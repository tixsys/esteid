#!/usr/bin/env bash

ARCH=universal
NAME=`date "+esteid_mac_${ARCH}_%Y%m%d.dmg"`
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
