cmake_minimum_required(VERSION 2.6)
project(esteidcm)

add_definitions(
    -D_UNICODE
    -DUNICODE
    -D_CRT_SECURE_NO_DEPRECATE
)

# Build with static MSVC runtime
foreach(flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    if(${flag_var} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
    endif(${flag_var} MATCHES "/MD")
endforeach(flag_var)

set(CARDLIB ${CMAKE_SOURCE_DIR}/smartcardpp)

include_directories(
    ${CARDLIB}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(esteidcm_SRCS
    esteidcm.cpp
    esteidcm.def
    esteidcm.rc
    notimplemented.cpp
    precompiled.cpp
    ${CARDLIB}/CardBase.cpp
    ${CARDLIB}/DynamicLibrary.cpp
    ${CARDLIB}/esteid/EstEidCard.cpp
    ${CARDLIB}/locked_allocator.cpp
    ${CARDLIB}/PCSCManager.cpp
    ${CARDLIB}/SCError.cpp
)

add_library(esteidcm SHARED ${esteidcm_SRCS}) 
install(TARGETS esteidcm DESTINATION lib)
