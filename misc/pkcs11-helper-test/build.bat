@echo off

set target_dir=target
set include_path=C:\build\Win32\pkcs11-helper-1.07\include
set library_path=C:\build\Win32\pkcs11-helper-1.07\lib

IF EXIST build GOTO :BuildExists
md build
cd build
cmake .. -G "NMake Makefiles" ^
         -DCMAKE_BUILD_TYPE=Release ^
         -DCMAKE_INSTALL_PREFIX="" ^
         -DCMAKE_INCLUDE_PATH="%include_path%" ^
         -DCMAKE_LIBRARY_PATH="%library_path%"
nmake
nmake install DESTDIR="%target_dir%"

goto :EOF

:BuildExists
echo Directory "build" already exists, quitting.
