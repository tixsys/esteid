#!/bin/sh

MAKE=$1
PREFIX=$2
ARCH=$3

set -e

$MAKE -f Makefile clean

./Configure $ARCH-cc shared --prefix=$PREFIX \
    "-isysroot $MACOSX_SDK_DIR" \
    -mmacosx-version-min=$MACOSX_DEPLOYMENT_TARGET
# --openssldir=$MACOSX_OPENSSL_DIR

# Parallel builds fail for OpenSSL
$MAKE -f Makefile -j1

# Manually copy binary files to architecture specific directories
mkdir -p $ARCH/apps $ARCH/engines
cp *.dylib $ARCH
cp apps/openssl $ARCH/apps
cp engines/*.so $ARCH/engines
