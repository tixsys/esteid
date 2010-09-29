@echo off

set target_dir=target

set OPENSSL_SHARED_INC=C:\build\Win32\openssl-shared\include
set OPENSSL_SHARED_LIBDIR=C:\build\Win32\openssl-shared\out32dll
set ZLIB_INC=C:\build\Win32\zlib123
set ZLIB_LIBDIR=C:\build\Win32\zlib123
set LIBP11_INC=C:\build\Win32\libp11-0.2.6\src
set LIBP11_LIBDIR=C:\build\Win32\libp11-0.2.6\src
set XERCES_INC=C:\build\Win32\xerces-c-3.0.1\src
set XERCES_LIBDIR=C:\build\Win32\xerces-c-3.0.1\Build\Win32\VC9\Release
set XML_SECURITY_C_INC=C:\build\Win32\xml-security-c-1.5.1\include
set XML_SECURITY_C_LIBDIR=C:\build\Win32\xml-security-c-1.5.1\Build\Win32\VC9\Release No Xalan
set XSD_BUILD=C:\build\Win32\xsd-3.2.0-i686-windows
set XSD_INC=C:\build\Win32\xsd-3.2.0-i686-windows\libxsd
set LIBICONV_INC=C:\build\Win32\libiconv-1.11.1\include
set LIBICONV_LIBDIR=C:\build\Win32\libiconv-1.11.1\lib
set LIBDIGIDOC_INC=C:\build\Win32\libdigidoc\build\target\include
set LIBDIGIDOC_LIBDIR=C:\build\Win32\libdigidoc\build\target\lib
set PKCS11_HELPER_INC=C:\build\Win32\pkcs11-helper-1.07\include
set PKCS11_HELPER_LIBDIR=C:\build\Win32\pkcs11-helper-1.07\lib

set INCLUDE=%PKCS11_HELPER_INC%;%PKCS11_HELPER_INC%/pkcs11-helper-1.0;%INCLUDE%
set LIB=%PKCS11_HELPER_LIBDIR%;%LIB%
set PATH=%XSD_BUILD%/bin;%PATH%

set include_path=%OPENSSL_SHARED_INC%;%ZLIB_INC%;%LIBP11_INC%;%XERCES_INC%;%XML_SECURITY_C_INC%;%XSD_INC%;%LIBICONV_INC%;%LIBDIGIDOC_INC%;%PKCS11_HELPER_INC%
set library_path=%OPENSSL_SHARED_LIBDIR%;%ZLIB_LIBDIR%;%LIBP11_LIBDIR%;%XERCES_LIBDIR%;%XML_SECURITY_C_LIBDIR%;%LIBICONV_LIBDIR%;%LIBDIGIDOC_LIBDIR%;%PKCS11_HELPER_LIBDIR%


IF EXIST build GOTO :BuildExists

:beginbuild
md build
cd build
cmake .. -G "NMake Makefiles" ^
         -DCMAKE_BUILD_TYPE=RelwithDebInfo ^
         -DCMAKE_INSTALL_PREFIX="" ^
         -DCMAKE_INCLUDE_PATH="%include_path%" ^
         -DCMAKE_LIBRARY_PATH="%library_path%"
nmake
nmake install DESTDIR="%target_dir%"
cd..

goto :EOF

:BuildExists
echo Directory "build" already exists, removing...
rm -r build
goto :beginbuild
