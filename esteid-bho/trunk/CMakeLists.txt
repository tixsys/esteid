include_directories (../.. ../../src ../ ../../.. ${CMAKE_CURRENT_SOURCE_DIR}/Win32/ ./)

set(ESTEIDSIGNINGPLUGINBHO_SOURCES
  dlldatax.c		dllmain.cpp
  EstEIDSigningBHO.cpp	EstEIDSigningPluginBHO.cpp
  precompiled.cpp	SmartCardCertificate.cpp
  SmartCardSigner.cpp	EstEIDSigningPluginBHO.def
  EstEIDSigningBHO.rgs	EstEIDSigningPluginBHO.rc
  EstEIDSigningPluginBHO.rgs	SmartCardCertificate.rgs
  SmartCardSigner.rgs)

set(ESTEIDSIGNINGPLUGINBHO_HEADERS
  _ISmartCardSignerEvents_CP.h	CardMonitorThread.h
  dlldatax.h		dllmain.h
  EstEIDSigningBHO.h	precompiled.h
  Resource.h		Setup.h
  SmartCardCertificate.h	SmartCardSigner.h
  targetver.h)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Win32)
set_source_files_properties(
  ${CMAKE_CURRENT_SOURCE_DIR}/Win32/EstEIDSigningPluginBHO_i.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Win32/EstEIDSigningPluginBHO_i.h
  PROPERTIES GENERATED 1)

add_library (EstEIDSigningPluginBHO SHARED 
  ${ESTEIDSIGNINGPLUGINBHO_SOURCES} 
  ${ESTEIDSIGNINGPLUGINBHO_HEADERS}
  ${CMAKE_CURRENT_SOURCE_DIR}/Win32/EstEIDSigningPluginBHO_i.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/Win32/EstEIDSigningPluginBHO_i.h)
 
add_definitions(-DUNICODE -D_UNICODE -D_WINDOWS -D_USRDLL)

link_directories (${ESTEIDUTIL_BINARY_DIR}/smartcard++/ ${ESTEIDUTIL_BINARY_DIR}/src/utility) 
target_link_libraries (EstEIDSigningPluginBHO smartcard++ utility)

add_custom_command(TARGET EstEIDSigningPluginBHO PRE_BUILD
  COMMAND Midl.Exe ARGS
    /nologo
    /I ${CMAKE_SOURCE_DIR}
    /char signed
    /env win32
    /Oicf
    /tlb ${CMAKE_CURRENT_SOURCE_DIR}/Win32/EstEIDSigningPluginBHO.tlb
    /h ${CMAKE_CURRENT_SOURCE_DIR}/Win32/EstEIDSigningPluginBHO_i.h
    /iid ${CMAKE_CURRENT_SOURCE_DIR}/Win32/EstEIDSigningPluginBHO_i.c
    /proxy ${CMAKE_CURRENT_SOURCE_DIR}/Win32/EstEIDSigningPluginBHO_p.c
    /no_robust
    ${CMAKE_CURRENT_SOURCE_DIR}/EstEIDSigningPluginBHO.idl
    VERBATIM)

install(TARGETS EstEIDSigningPluginBHO DESTINATION esteidutil/lib)