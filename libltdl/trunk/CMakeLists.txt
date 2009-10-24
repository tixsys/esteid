cmake_minimum_required(VERSION 2.6)
project(libltdl)

# Upstream libtool has removed MSCV support from libltdl.
# This is a slightly modified version of GNU libltdl for
# compiling with Visual Studio compilers.

set(SOURCES 
    ltdl.c
)

add_definitions( 
    -DHAVE_CONFIG_H
    -D_CRT_SECURE_NO_DEPRECATE=1
    -D_WIN32_WINNT=0x500
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

configure_file(ltdl_config.h config.h)

add_library(libltdl STATIC ${SOURCES})
install(TARGETS libltdl DESTINATION lib)
