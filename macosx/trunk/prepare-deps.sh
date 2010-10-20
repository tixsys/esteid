#!/bin/sh

LIBP11VER="0.2.7"
XMLSECVER="1.5.1"
XERCESVER="3.0.1"
XSDVER="3.3.0"
OSSLVER="1.0.0"
OSSLNAME="openssl-1.0.0a"

# ======================================== #
# Install various utilities from macports  #
# ======================================== #

sudo /opt/local/bin/port install mercurial git-core cmake pkgconfig wget

# ======================================== #
#         Install Ruby modules             #
# ======================================== #

sudo gem install plist

# ========================== #
#         Install XSD        #
# ========================== #

if [ ! -x /usr/local/bin/xsd ]; then
    tar -jtvf xsd-$XSDVER-i686-macosx.tar.bz2
    sudo mv xsd-$XSDVER-i686-macosx/bin/xsd /usr/local/bin
    rm -rf xsd-$XSDVER-i686-macosx
fi


# ======================== #
#       Build OpenSSL      #
# ======================== #

# Note: The order of architectures IS important.
# The resulting openssl binary is used during build.

for ARCH in darwin-ppc darwin-i386 darwin64-x86_64
do
    mkdir $OSSLNAME-$ARCH
    tar xzf $OSSLNAME.tar.gz -C $OSSLNAME-$ARCH
    pushd $OSSLNAME-$ARCH/$OSSLNAME

    ./Configure $ARCH-cc shared --prefix=/usr/local \
                --openssldir=/usr/local/etc/openssl \
                "-isysroot /Developer/SDKs/MacOSX10.5.sdk" \
                -mmacosx-version-min=10.5
    make
    sudo make install

done

sudo lipo -create $OSSLNAME-darwin-ppc/$OSSLNAME/libcrypto.$OSSLVER.dylib \
                  $OSSLNAME-darwin-i386/$OSSLNAME/libcrypto.$OSSLVER.dylib \
                  $OSSLNAME-darwin64-x86_64/$OSSLNAME/libcrypto.$OSSLVER.dylib \
          -output /usr/local/lib/libcrypto.$OSSLVER.dylib

sudo lipo -create $OSSLNAME-darwin-ppc/$OSSLNAME/libssl.$OSSLVER.dylib \
                  $OSSLNAME-darwin-i386/$OSSLNAME/libssl.$OSSLVER.dylib \
                  $OSSLNAME-darwin64-x86_64/$OSSLNAME/libssl.$OSSLVER.dylib \
          -output /usr/local/lib/libssl.$OSSLVER.dylib

# ==================================== #
# Set up Autotools config              #
# used by all other configure scripts  #
# ==================================== #

export PKG_CONFIG=/opt/local/bin/pkg-config
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig
export CFLAGS="-O2 -arch i386 -arch x86_64 -isysroot /Developer/SDKs/MacOSX10.5.sdk -mmacosx-version-min=10.5"
export LTLIB_LIBS=/Developer/SDKs/MacOSX10.5.sdk/usr/lib/libltdl.a

unpack() {
    rm -rf $PKG
    tar xzf $PKG.tar.gz
    pushd $PKG
}

makeinstall() {
    make
    sudo make install
    popd
}

# ======================== #
#       Build libp11       #
# ======================== #

PKG=libp11-$LIBP11VER
unpack
./configure --disable-dependency-tracking --disable-static
makeinstall
sudo rm /usr/local/lib/libp11.la

# ======================== #
#      Build xerces-c      #
# ======================== #

PKG=xerces-c-3.0.1
unpack
./configure --disable-dependency-tracking --disable-shared \
            --enable-static --disable-pretty-make --disable-sse2 \
            --disable-network --enable-transcoder-iconv
makeinstall

# ======================== #
#   Build xml-security-c   #
# ======================== #

PKG=xml-security-c-1.5.1
unpack
./configure --disable-dependency-tracking --disable-shared \
            --enable-static XERCESCROOT=/usr/local
makeinstall
