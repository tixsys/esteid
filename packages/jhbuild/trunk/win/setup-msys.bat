@ECHO OFF

REM Installation Paths
SET TOOLSDIR=C:\tools
SET DOWNLOAD=%TOOLSDIR%\download
SET MINGWDIR=%TOOLSDIR%
SET MSYSDIR=%TOOLSDIR%\msys
SET MINGW_GET=%MINGWDIR%\bin\mingw-get.exe
SET BINDIR=%MSYSDIR%\bin
SET TMPDIR=%TOOLSDIR%\tmp

REM Download URLs
SET SEVENZIP=http://downloads.sourceforge.net/sevenzip/7za465.zip
SET MSYSGIT=http://msysgit.googlecode.com/files/PortableGit-1.7.3.1-preview20101002.7z
SET SVN=http://downloads.sourceforge.net/project/win32svn/1.6.13/svn-win32-1.6.13.zip
SET CMAKE=http://www.cmake.org/files/v2.8/cmake-2.8.3-win32-x86.zip
SET PERL=http://downloads.activestate.com/ActivePerl/releases/5.12.2.1202/ActivePerl-5.12.2.1202-MSWin32-x86-293621.zip
SET NASM=http://www.nasm.us/pub/nasm/releasebuilds/2.10rc2/win32/nasm-2.10rc2-win32.zip
SET PKGCONFIG=http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/pkg-config_0.23-3_win32.zip
SET GLIB=http://ftp.gnome.org/pub/gnome/binaries/win32/glib/2.26/glib_2.26.0-2_win32.zip
SET GETTEXT=http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/gettext-runtime_0.18.1.1-2_win32.zip

REM This is repacked from official .exe installer
SET MERCURIAL=http://id.anttix.org/util/mercurial-1.7.zip

IF EXIST %MINGW_GET% (
    SET PATH=%PATH%;%BINDIR%

    MD %DOWNLOAD%
    CD %DOWNLOAD%
    MD %TMPDIR%

    REM %MINGW_GET% update

    ECHO ======= Installing unzip and wget ========
    %MINGW_GET% install msys-wget msys-unzip

    IF NOT EXIST %BINDIR%\wget.exe (
        ECHO ======= Installing wget ========
        %MINGW_GET% install msys-wget
    )

    IF NOT EXIST %BINDIR%\7za.exe (
        ECHO ======= Installing 7-zip command line version ========
        wget -nv -c -O 7za.zip %SEVENZIP%
        unzip -q -d %TMPDIR% 7za.zip
        copy %TMPDIR%\7za.exe %BINDIR%
    )
   
    IF NOT EXIST %BINDIR%\git.exe (
        ECHO ======= Installing MSYS "portable" Git ========
        wget -nv -c -O msysgit.7z %MSYSGIT%
        CD %MSYSDIR%
        7za -y x %DOWNLOAD%\msysgit.7z
        CD %DOWNLOAD%
    )

    ECHO ======= Installing needed MSYS packages ========
    %MINGW_GET% install msys-core msys-zip msys-make msys-m4 msys-autoconf msys-automake msys-libtool

    IF NOT EXIST %BINDIR%\svn.exe (
        ECHO ======= Installing command-line SVN client ========
        wget -nv -c -O svn.zip %SVN%
        unzip -q -j -d %TMPDIR% svn.zip *.exe *.dll
        DEL %TMPDIR%\openssl.exe
        sh -c "mv $TMPDIR/* $BINDIR"
    )

    IF NOT EXIST %BINDIR%\cmake.exe (
        ECHO ======= Installing CMake ========
        wget -nv -c -O cmake.zip %CMAKE%
        unzip -q -d %TMPDIR% cmake.zip
        CD %TMPDIR%
        sh -c "mv cmake-*/bin/* $MSYSDIR/bin ; mv cmake-*/share/* $MSYSDIR/share"
        sh -c "rm -rf *"
        CD %DOWNLOAD%
    )

    IF NOT EXIST %MSYSDIR%\perl\bin\perl.exe (
        ECHO ======= Installing Perl ========
        wget -nv -c -O perl.zip %PERL%
        unzip -q -d %TMPDIR% perl.zip
        sh -c "mv $TMPDIR/ActivePerl-*/perl $MSYSDIR"
    )

    IF NOT EXIST %BINDIR%\hg.exe (
        ECHO ======= Installing Mercurial ========
        wget -nv -c -O mercurial.zip %MERCURIAL%
        unzip -o -q -d %BINDIR% mercurial.zip
    )

    IF NOT EXIST %BINDIR%\nasm.exe (
        ECHO ======= Installing NASM ========
        wget -nv -c -O nasm.zip %NASM%
        unzip -q -d %TMPDIR% nasm.zip
        sh -c "mv $TMPDIR/nasm-*/*.exe $BINDIR"
    )

    IF NOT EXIST %BINDIR%\pkg-config.exe (
        ECHO ======= Installing pkg-config ========
        wget -nv -c -O gettext.zip %GETTEXT%
        wget -nv -c -O glib.zip %GLIB%
        wget -nv -c -O pkgconfig.zip %PKGCONFIG%
        unzip -q -d %TMPDIR% gettext.zip
        unzip -q -d %TMPDIR% glib.zip
        unzip -q -d %TMPDIR% pkgconfig.zip
        sh -c "mv $TMPDIR/bin/*.{exe,dll} $BINDIR"
    )

    ECHO Clean up
    rm -rf %TMPDIR%
    pause

) ELSE (
    start http://code.google.com/p/esteid/wiki/WindowsBuilderSetup
)
