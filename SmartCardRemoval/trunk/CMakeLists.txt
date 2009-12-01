cmake_minimum_required(VERSION 2.6)
project(SmartCardRemoval)

add_definitions( 
    -D_UNICODE
    -DUNICODE
    -D_MERGE_PROXYSTUB
    -D_WIN_SERVICE_STARTUP_HACK
)

set(CARDLIB ${CMAKE_SOURCE_DIR}/smartcard++)

include_directories(
    ${CARDLIB}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SOURCES 
    ${CMAKE_CURRENT_BINARY_DIR}/SmartCardRemoval_i.c
    precompiled.cpp
    ProcessStarter.cpp
    SmartCardRemoval.cpp
    SmartCardRemoval.rc
    SmartCardRemoval.rgs
    utility/monitorThread.cpp
    utility/threadObj.cpp
    ${CARDLIB}/CardBase.cpp
    ${CARDLIB}/CTAPIManager.cpp
    ${CARDLIB}/DynamicLibrary.cpp
    ${CARDLIB}/esteid/EstEidCard.cpp
    ${CARDLIB}/locked_allocator.cpp
    ${CARDLIB}/PCSCManager.cpp
    ${CARDLIB}/SCError.cpp
    ${CARDLIB}/SmartCardManager.cpp
)

if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    set(MIDL_TARGET "x64")
else(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    set(MIDL_TARGET "win32")
endif(${CMAKE_SIZEOF_VOID_P} EQUAL 8)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/SmartCardRemoval.h
           ${CMAKE_CURRENT_BINARY_DIR}/SmartCardRemoval_i.c
    COMMAND Midl.Exe SmartCardRemoval.idl
                     /nologo
                     /I ${CMAKE_SOURCE_DIR}
                     /char signed
                     /env ${MIDL_TARGET}
                     /Oicf
                     /tlb SmartCardRemoval.tlb
                     /h SmartCardRemoval.h
                     /iid SmartCardRemoval_i.c
                     /proxy SmartCardRemoval_p.c
                     /no_robust
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM
)

add_executable(SmartCardRemoval WIN32 ${SOURCES})
target_link_libraries(SmartCardRemoval ws2_32)

install(TARGETS SmartCardRemoval DESTINATION bin)
