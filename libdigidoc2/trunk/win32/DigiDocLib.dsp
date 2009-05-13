# Microsoft Developer Studio Project File - Name="DigiDocLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=DigiDocLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DigiDocLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DigiDocLib.mak" CFG="DigiDocLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DigiDocLib - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DigiDocLib - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DigiDocLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "P:\iconv-1.8.win32\include" /I "P:\libxml2-2.5.1.win32\include" /I "P:\openssl\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "BUILDINGDLL" /D "WITH_SOAPDEFS_H" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x41d /d "NDEBUG"
# ADD RSC /l 0x41d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libeay32.lib ssleay32.lib libxml2.lib zdll.lib wsock32.lib /nologo /subsystem:windows /dll /machine:I386 /libpath:"P:\iconv-1.8.win32\lib" /libpath:"P:\libxml2-2.5.1.win32\lib" /libpath:"P:\openssl\lib\release"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "DigiDocLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "BUILDINGDLL" /D "WITH_SOAPDEFS_H" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x41d /d "_DEBUG"
# ADD RSC /l 0x41d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libxml2.lib libeay32.lib ssleay32.lib zdll.lib wsock32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "DigiDocLib - Win32 Release"
# Name "DigiDocLib - Win32 Debug"
# Begin Source File

SOURCE=..\src\config.h
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocCert.c
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocCert.h
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocConfig.c
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocConfig.h
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocConvert.c
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocConvert.h
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocDebug.c
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocDebug.h
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocDefs.h
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocEnc.c
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocEnc.h
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocEncGen.c
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocEncGen.h
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocEncSAXParser.c
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocEncSAXParser.h
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocError.c
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocError.h
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocLib.c
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocLib.h
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocMem.c
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocMem.h
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocParser.c
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocParser.h
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocPKCS11.c
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocPKCS11.h
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocSAXParser.c
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocSAXParser.h
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocStack.c
# End Source File
# Begin Source File

SOURCE=..\src\DigiDocStack.h
# End Source File
# Begin Source File

SOURCE=..\src\resource.h
# End Source File
# End Target
# End Project
