cmake_minimum_required(VERSION 2.6)
project(esteidcsp)

add_definitions(
    -D_UNICODE
    -DUNICODE
    # Release builds don't compile without this definition.
    # http://social.msdn.microsoft.com/Forums/en-US/vcplus2008prerelease/thread/fa969def-911e-4760-867d-5a06a37ed048
    -D_SECURE_SCL=0
)

set(CARDLIB ${CMAKE_SOURCE_DIR}/smartcard++)

include_directories(
    ${CARDLIB}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(esteidcsp_SRCS
    csp_c_exports.cpp
    Csp.cpp
    csp.def
    CspEstEid.cpp
    Csp_Hash.cpp
    Csp_Key.cpp
    CSPTypes.cpp
    esteidcsp.cpp
    esteidcsp.rc
    precompiled.cpp
    RegKey.cpp
    ${CARDLIB}/CardBase.cpp
    ${CARDLIB}/CTAPIManager.cpp
    ${CARDLIB}/DynamicLibrary.cpp
    ${CARDLIB}/esteid/EstEidCard.cpp
    ${CARDLIB}/esteid/EstEidCardMaintainer.cpp
    ${CARDLIB}/locked_allocator.cpp
    ${CARDLIB}/PCSCManager.cpp
    ${CARDLIB}/SCError.cpp
    ${CARDLIB}/SmartCardManager.cpp
    ${CARDLIB}/utility/logger.cpp
    ${CARDLIB}/utility/monitorThread.cpp
    ${CARDLIB}/utility/pinDialog.cpp
    ${CARDLIB}/utility/threadObj.cpp
)

add_library(esteidcsp SHARED ${esteidcsp_SRCS})

find_program(CSPSIGNEXE NAMES cspsign.exe cspSign.exe PATHS ../ PATH_SUFFIXES cspdk)

if(CSPSIGNEXE)
  get_target_property(ESTEIDCSP_DLL_FULLPATH esteidcsp LOCATION)
  add_custom_command(TARGET esteidcsp POST_BUILD
    COMMAND ${CSPSIGNEXE} ARGS c ${ESTEIDCSP_DLL_FULLPATH}
    COMMAND ${CSPSIGNEXE} ARGS s ${ESTEIDCSP_DLL_FULLPATH} ${ESTEIDCSP_DLL_FULLPATH}.sig)
else(CSPSIGNEXE)
  message( STATUS "cspsign.exe not found, post-build command not added" )
endif(CSPSIGNEXE)

install(TARGETS esteidcsp DESTINATION lib)
