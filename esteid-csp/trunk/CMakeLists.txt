cmake_minimum_required(VERSION 2.6)
project(esteidcsp)

# Custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
include(InstallSettings)
find_package(SmartCardpp REQUIRED)

add_definitions(
    -D_UNICODE
    -DUNICODE
    # Release builds don't compile without this definition.
    # http://social.msdn.microsoft.com/Forums/en-US/vcplus2008prerelease/thread/fa969def-911e-4760-867d-5a06a37ed048
    -D_SECURE_SCL=0
)

if(USE_STATIC_RUNTIME)
    message(STATUS "Building with static MSVC runtime")
    foreach(flag_var
            CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        if(${flag_var} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif(${flag_var} MATCHES "/MD")
    endforeach(flag_var)
endif(USE_STATIC_RUNTIME)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${SMARTCARDPP_INCLUDE_DIR}
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
    utility/logger.cpp
    utility/monitorThread.cpp
    utility/pinDialog.cpp
    utility/threadObj.cpp
)

add_library(esteidcsp SHARED ${esteidcsp_SRCS})
target_link_libraries(esteidcsp ${SMARTCARDPP_LIBRARIES} ws2_32)

find_program(CSPSIGNEXE NAMES cspsign.exe cspSign.exe PATHS ../ PATH_SUFFIXES cspdk)

if(CSPSIGNEXE)
  get_target_property(ESTEIDCSP_DLL_FULLPATH esteidcsp LOCATION)
  add_custom_command(TARGET esteidcsp POST_BUILD
    COMMAND ${CSPSIGNEXE} ARGS c ${ESTEIDCSP_DLL_FULLPATH}
    COMMAND ${CSPSIGNEXE} ARGS s ${ESTEIDCSP_DLL_FULLPATH} ${ESTEIDCSP_DLL_FULLPATH}.sig)
else(CSPSIGNEXE)
  message( STATUS "cspsign.exe not found, post-build command not added" )
endif(CSPSIGNEXE)

install(TARGETS esteidcsp DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
