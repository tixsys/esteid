@echo off

set target_dir=target

set libdigidoc_include=C:\build\Win32\libdigidoc++\build\target\include
set libdigidoc_library=C:\build\Win32\libdigidoc++\build\target\lib
set openssl_include=C:\build\Win32\openssl-shared\include
set openssl_library=C:\build\Win32\openssl-shared\out32dll
set zlib_include=C:\build\Win32\zlib123
set zlib_library=C:\build\Win32\zlib123
set libp11_include=C:\build\Win32\libp11-0.2.5-r178\src
set libp11_library=C:\build\Win32\libp11-0.2.5-r178\src
set xercesc_include=C:\build\Win32\xerces-c-3.0.1\src
set xercesc_library=C:\build\Win32\xerces-c-3.0.1\Build\Win32\VC9\Release
set xmlsecurityc_include=C:\build\Win32\xml-security-c-1.5.0_pre20090310\include
set xmlsecurityc_library=C:\build\Win32\xml-security-c-1.5.0_pre20090310\Build\Win32\VC9\Release No Xalan
set xsd_include=C:\build\Win32\xsd-3.2.0-i686-windows\libxsd

set include_path=%libdigidoc_include%;%openssl_include%;%zlib_include%;%libp11_include%;%xercesc_include%;%xmlsecurityc_include%;%xsd_include%
set library_path=%libdigidoc_library%;%openssl_library%;%zlib_library%;%libp11_library%;%xercesc_library%;%xmlsecurityc_library%;%xsd_library%

       

rm -r build
md build
cd build

cmake .. -G "NMake Makefiles" ^
		 -DCMAKE_BUILD_TYPE=Release ^
         -DCMAKE_INSTALL_PREFIX="" ^
         -DCMAKE_INCLUDE_PATH="%include_path%" ^
         -DCMAKE_LIBRARY_PATH="%library_path%" ^
         -DLIBDIGIDOC++_CONF="C:/Program Files/Estonian ID Card/libdigidoc++.conf"
nmake VERBOSE=1
REM nmake install DESTDIR="%target_dir%"
cd..
