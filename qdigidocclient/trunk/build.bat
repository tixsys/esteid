@echo off

set target_dir=target

set libdigidoc_include=C:\src\libdigidoc-svn\build\target\include
set libdigidoc_library=C:\src\libdigidoc-svn\build\target\lib
set openssl_include=C:\OpenSSL\include
set openssl_library=C:\OpenSSL\lib
set zlib_include=C:\src\zlib123
set zlib_library=C:\src\zlib123
set libp11_include=C:\src\libp11-0.2.3\src
set libp11_library=C:\src\libp11-0.2.3\src
set xercesc_include=C:\src\xerces-c-3.0.1-x86-windows-vc-9.0\include
set xercesc_library=C:\src\xerces-c-3.0.1-x86-windows-vc-9.0\lib
set xmlsecurityc_include=C:\src\xml-security-c-1.5.0_pre20090310\include
set xmlsecurityc_library=C:\src\xml-security-c-1.5.0_pre20090310\Build\Win32\VC9\Release No Xalan
set xsd_include=C:\XSD-3.2\include
set xsd_library=C:\XSD-3.2\lib\vc-9.0

set include_path=%libdigidoc_include%;%openssl_include%;%zlib_include%;%libp11_include%;%xercesc_include%;%xmlsecurityc_include%;%xsd_include%
set library_path=%libdigidoc_library%;%openssl_library%;%zlib_library%;%libp11_library%;%xercesc_library%;%xmlsecurityc_library%;%xsd_library%

IF EXIST build GOTO :BuildExists
md build
cd build
cmake .. -G "NMake Makefiles" ^
         -DCMAKE_BUILD_TYPE=Release ^
         -DCMAKE_INSTALL_PREFIX="" ^
         -DCMAKE_INCLUDE_PATH="%include_path%" ^
         -DCMAKE_LIBRARY_PATH="%library_path%" ^
         -DLIBDIGIDOC_CONF="C:/Program Files/Estonian ID Card/libdigidoc/bdoclib.conf"
nmake
nmake install DESTDIR="%target_dir%"

goto :EOF

:BuildExists
echo Directory "build" already exists, quitting.
