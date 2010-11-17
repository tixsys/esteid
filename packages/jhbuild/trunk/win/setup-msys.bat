@REM Installation Paths
@SET TOOLSDIR=C:\tools
@SET DOWNLOAD=%TOOLSDIR%\download
@SET MINGWDIR=%TOOLSDIR%
@SET MSYSDIR=%TOOLSDIR%\msys
@SET MINGW_GET=%MINGWDIR%\bin\mingw-get.exe
@SET BINDIR=%MSYSDIR%\bin
@SET TMPDIR=%TOOLSDIR%\tmp

@REM Download URLs
@SET SEVENZIP=http://downloads.sourceforge.net/sevenzip/7za465.zip
@SET MSYSGIT=http://msysgit.googlecode.com/files/PortableGit-1.7.3.1-preview20101002.7z
@SET SVN=http://downloads.sourceforge.net/project/win32svn/1.6.13/svn-win32-1.6.13.zip
@SET CMAKE=http://www.cmake.org/files/v2.8/cmake-2.8.3-win32-x86.zip
@SET PERL=http://downloads.activestate.com/ActivePerl/releases/5.12.2.1202/ActivePerl-5.12.2.1202-MSWin32-x86-293621.zip

@SET These two are repacked from official .exe and .msi installers.
@SET MERCURIAL=http://id.anttix.org/util/mercurial-1.7.zip
@SET PYTHON=http://id.anttix.org/util/python-2.7-x64.zip

IF EXIST %MINGW_GET% (
    SET PATH=%PATH%;%BINDIR%

    MD %DOWNLOAD%
    CD %DOWNLOAD%
    MD %TMPDIR%

    @REM Install wget
    %MINGW_GET% update
    %MINGW_GET% install msys-wget
    %MINGW_GET% install msys-zip

    @REM Install 7-zip command line version
    wget -c -O 7za.zip %SEVENZIP%
    unzip -q -d %TMPDIR% 7za.zip
    copy %TMPDIR%\7za.exe %BINDIR%
    DEL /y %TMPDIR%\*.*

    @REM Install MSYS "portable" Git
    wget -c -O msysgit.7z %MSYSGIT%
    CD %MSYSDIR%
    7za x %DOWNLOAD%\msysgit.7z
    CD %DOWNLOAD%

    @REM Install needed MSYS packages
    %MINGW_GET% install msys-core
    %MINGW_GET% install msys-unzip
    %MINGW_GET% install msys-make
    %MINGW_GET% install msys-m4
    %MINGW_GET% install msys-autoconf
    %MINGW_GET% install msys-automake
    %MINGW_GET% install msys-libtool

    @REM Install command-line SVN client
    wget -c -O svn.zip %SVN%
    unzip -q -j -d %TMPDIR% svn.zip *.exe *.dll
    DEL %TMPDIR%\openssl.exe
    mv %TMPDIR%\*.* %BINDIR%

    @REM Install CMake
    wget -c -O cmake.zip %CMAKE%
    unzip -q -d %TMPDIR% cmake.zip
    CD %TMPDIR%
    sh -c "mv cmake-*/bin/* $MSYSDIR/bin ; mv cmake-*/share/* $MSYSDIR/share"
    sh -c "rm -rf *"
    CD %DOWNLOAD%

    @REM Install Perl
    wget -c -O perl.zip %PERL%
    unzip -q -d %TMPDIR% perl.zip
    sh -c "mv ActivePerl-*/perl $TOOLSDIR"

    @REM Install Python
    wget -c -O python.zip %PYTHON%
    MD %TOOLSDIR%\python
    unzip -q -d %TOOLSDIR%\python python.zip

    @REM Install Mercurial
    wget -c -O mercurial.zip %MERCURIAL%
    unzip -q -d %BINDIR% mercurial.zip

    @REM Clean up
    rm -rf %TMPDIR%
    
) ELSE (
    start http://code.google.com/p/esteid/wiki/WindowsBuilderSetup
)
