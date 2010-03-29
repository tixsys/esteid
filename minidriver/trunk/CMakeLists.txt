cmake_minimum_required(VERSION 2.6)
project(esteidcm)

# Custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
include(InstallSettings)
find_package(SmartCardpp REQUIRED)

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

include_directories(${SMARTCARDPP_INCLUDE_DIR})

set(esteidcm_SRCS
    esteidcm.cpp
    esteidcm.def
    esteidcm.rc
    notimplemented.cpp
    precompiled.cpp
)

add_library(esteidcm MODULE ${esteidcm_SRCS}) 
# Rename to esteidcm64.dll on x64
if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    set_target_properties(esteidcm PROPERTIES OUTPUT_NAME esteidcm64)
endif(${CMAKE_SIZEOF_VOID_P} EQUAL 8)

target_link_libraries(esteidcm ${SMARTCARDPP_LIBRARIES} ws2_32)
install(TARGETS esteidcm DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
