#!/usr/bin/env bash

svn update ../..

ARCH=universal
REVISION=`svn info -r HEAD | grep Revision | cut -d ' ' -f 2`

if test -n "$1"; then
	ARCH=$1 # universal, ppc or i386
fi

CSUM_FILE=build/Build_${ARCH}.txt
CSUM_NEW=${ARCH}${REVISION}
CSUM_OLD=${ARCH}0

if test -f ${CSUM_FILE}; then
	CSUM_OLD=`cat ${CSUM_FILE}`
fi

if test "${CSUM_OLD}" = "${CSUM_NEW}"; then
	echo "Build is up to date (revision: ${REVISION}, architecture: ${ARCH})"
	exit 0
fi

NAME=`date "+installer_${ARCH}_%Y%m%d_r${REVISION}.dmg"`
FILE=build/Packages/${NAME}

./make.rb -V --arch ${ARCH} installer

hdiutil create -fs HFS+ -srcfolder build/Packages/esteid-dev.mpkg -volname esteid-dev ${FILE}
setfile -a E ${FILE}

if test -n "$2"; then
	scp ${FILE} $2
fi

echo "${CSUM_NEW}" > ${CSUM_FILE}