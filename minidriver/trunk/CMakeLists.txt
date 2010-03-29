cmake_minimum_required(VERSION 2.6)
project(esteidcm)

# Custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
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

# esteidcm64.dll on x64
# esteidcm.dll on Win32
if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    set(esteidcm_NAME "esteidcm64")
else()
    set(esteidcm_NAME "esteidcm")
endif()

configure_file(esteidcm.def.in ${CMAKE_CURRENT_BINARY_DIR}/esteidcm.def @ONLY)

set(esteidcm_SRCS
    esteidcm.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/esteidcm.def
    esteidcm.rc
    notimplemented.cpp
    precompiled.cpp
)

add_library(esteidcm MODULE ${esteidcm_SRCS}) 
set_target_properties(esteidcm PROPERTIES OUTPUT_NAME ${esteidcm_NAME})

target_link_libraries(esteidcm ${SMARTCARDPP_LIBRARIES} ws2_32)
install(TARGETS esteidcm DESTINATION bin)
