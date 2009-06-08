# Microsoft Developer Studio Project File - Name="digiDocLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=digiDocLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "digiDocLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "digiDocLib.mak" CFG="digiDocLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "digiDocLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "digiDocLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "digiDocLib - Win32 debug_2_Rel" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PKI/DDOC_COM/digidocs_vs6/digiDocLib", EYHAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "digiDocLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "digiDocLib___Win32_Release"
# PROP BASE Intermediate_Dir "digiDocLib___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\Digidoclib" /I "..\add" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "BUILDINGDLL" /YX /FD /c
# ADD BASE RSC /l 0x425 /d "NDEBUG"
# ADD RSC /l 0x425 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "digiDocLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\add" /I "..\Digidoclib" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "BUILDINGDLL" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x425 /d "_DEBUG"
# ADD RSC /l 0x425 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "digiDocLib - Win32 debug_2_Rel"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "digiDocLib___Win32_debug_2_Rel"
# PROP BASE Intermediate_Dir "digiDocLib___Win32_debug_2_Rel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "digiDocLib___Win32_debug_2_Rel"
# PROP Intermediate_Dir "digiDocLib___Win32_debug_2_Rel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "..\Digidoclib" /I "..\add" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Zi /O2 /I "..\Digidoclib" /I "..\add" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "BUILDINGDLL" /FR /YX /FD /c
# ADD BASE RSC /l 0x425 /d "NDEBUG"
# ADD RSC /l 0x425 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "digiDocLib - Win32 Release"
# Name "digiDocLib - Win32 Debug"
# Name "digiDocLib - Win32 debug_2_Rel"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DigiCrypt.c
# End Source File
# Begin Source File

SOURCE=.\DigiDocCert.c
# End Source File
# Begin Source File

SOURCE=.\DigiDocConfig.c
# End Source File
# Begin Source File

SOURCE=.\DigiDocConvert.c
# End Source File
# Begin Source File

SOURCE=.\DigiDocCsp.c
# End Source File
# Begin Source File

SOURCE=.\DigiDocDebug.c
# End Source File
# Begin Source File

SOURCE=.\DigiDocEnc.c
# End Source File
# Begin Source File

SOURCE=.\DigiDocEncGen.c
# End Source File
# Begin Source File

SOURCE=.\DigiDocEncSAXParser.c
# End Source File
# Begin Source File

SOURCE=.\DigiDocError.c
# End Source File
# Begin Source File

SOURCE=.\DigiDocLib.c
# End Source File
# Begin Source File

SOURCE=.\DigiDocMem.c
# End Source File
# Begin Source File

SOURCE=.\DigiDocPKCS11.c
# End Source File
# Begin Source File

SOURCE=.\DigiDocSAXParser.c
# End Source File
# Begin Source File

SOURCE=.\DigiDocStack.c
# End Source File
# Begin Source File

SOURCE=.\DlgUnit.c
# End Source File
# Begin Source File

SOURCE=.\DlgUnitS.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DigiCrypt.h
# End Source File
# Begin Source File

SOURCE=.\DigiDocCert.h
# End Source File
# Begin Source File

SOURCE=.\DigiDocConfig.h
# End Source File
# Begin Source File

SOURCE=.\DigiDocConvert.h
# End Source File
# Begin Source File

SOURCE=.\DigiDocCsp.h
# End Source File
# Begin Source File

SOURCE=.\DigiDocDebug.h
# End Source File
# Begin Source File

SOURCE=.\DigiDocEnc.h
# End Source File
# Begin Source File

SOURCE=.\DigiDocEncGen.h
# End Source File
# Begin Source File

SOURCE=.\DigiDocEncSAXParser.h
# End Source File
# Begin Source File

SOURCE=.\DigiDocError.h
# End Source File
# Begin Source File

SOURCE=.\DigiDocLib.h
# End Source File
# Begin Source File

SOURCE=.\DigiDocMem.h
# End Source File
# Begin Source File

SOURCE=.\DigiDocParser.h
# End Source File
# Begin Source File

SOURCE=.\DigiDocPKCS11.h
# End Source File
# Begin Source File

SOURCE=.\DigiDocSAXParser.h
# End Source File
# Begin Source File

SOURCE=.\DigiDocStack.h
# End Source File
# Begin Source File

SOURCE=.\DlgUnit.h
# End Source File
# End Group
# End Target
# End Project
