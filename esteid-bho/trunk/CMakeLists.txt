cmake_minimum_required(VERSION 2.6)
project(EstEIDSigningPluginBHO)

add_definitions(
    -D_UNICODE
    -DUNICODE
    -D_MERGE_PROXYSTUB

    # Release builds don't compile without this definition.
    # http://social.msdn.microsoft.com/Forums/en-US/vcplus2008prerelease/thread/fa969def-911e-4760-867d-5a06a37ed048
    -D_SECURE_SCL=0
)

set(CARDLIB ${CMAKE_SOURCE_DIR}/smartcard++)

include_directories(
    ${CARDLIB}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(bho_SRCS 
    ${CMAKE_CURRENT_BINARY_DIR}/EstEIDSigningPluginBHO_i.c
    dlldatax.c
    dllmain.cpp
    EstEIDSigningBHO.cpp
    EstEIDSigningBHO.rgs
    EstEIDSigningPluginBHO.cpp
    EstEIDSigningPluginBHO.def
    EstEIDSigningPluginBHO.rc
    EstEIDSigningPluginBHO.rgs
    precompiled.cpp
    SmartCardCertificate.cpp
    SmartCardCertificate.rgs
    SmartCardSigner.cpp
    SmartCardSigner.rgs
    ${CARDLIB}/CardBase.cpp
    ${CARDLIB}/CTAPIManager.cpp
    ${CARDLIB}/DynamicLibrary.cpp
    ${CARDLIB}/esteid/EstEidCard.cpp
    ${CARDLIB}/locked_allocator.cpp
    ${CARDLIB}/PCSCManager.cpp
    ${CARDLIB}/SCError.cpp
    ${CARDLIB}/SmartCardManager.cpp
    ${CARDLIB}/utility/converters.cpp
    ${CARDLIB}/utility/monitorThread.cpp
    ${CARDLIB}/utility/pinDialog.cpp
    ${CARDLIB}/utility/threadObj.cpp
)

if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    set(MIDL_TARGET "x64")
else(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    set(MIDL_TARGET "win32")
endif(${CMAKE_SIZEOF_VOID_P} EQUAL 8)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/EstEIDSigningPluginBHO_i.h
           ${CMAKE_CURRENT_BINARY_DIR}/EstEIDSigningPluginBHO_i.c
           ${CMAKE_CURRENT_BINARY_DIR}/EstEIDSigningPluginBHO_p.c
    COMMAND Midl.Exe EstEIDSigningPluginBHO.idl
                     /nologo
                     /I ${CMAKE_SOURCE_DIR}
                     /char signed
                     /env ${MIDL_TARGET}
                     /Oicf
                     /tlb EstEIDSigningPluginBHO.tlb
                     /h EstEIDSigningPluginBHO_i.h
                     /iid EstEIDSigningPluginBHO_i.c
                     /proxy EstEIDSigningPluginBHO_p.c
                     /no_robust
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM
)

add_library(EstEIDSigningPluginBHO SHARED ${bho_SRCS})

install(TARGETS EstEIDSigningPluginBHO DESTINATION lib)
