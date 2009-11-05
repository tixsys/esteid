#!/bin/sh
cd build
rm -rf *
cmake -G "Xcode" -DCMAKE_OSX_SYSROOT=/Developer/SDKs/MacOSX10.4u.sdk/ -DCMAKE_OSX_ARCHITECTURES="i386" ..
xcodebuild -project OOoDigiDocPlugin.xcodeproj -configuration Release -sdk macosx10.4
cd ..