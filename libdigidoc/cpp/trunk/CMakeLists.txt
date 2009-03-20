cmake_minimum_required(VERSION 2.6)
project(libdigidoc)

# Custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

find_package(OpenSSL REQUIRED)
find_package(OpenSSLCrypto REQUIRED)
find_package(ZLIB REQUIRED)
find_package(LibP11 REQUIRED)
find_package(XercesC REQUIRED)
find_package(XmlSecurityC REQUIRED)
find_package(XSD REQUIRED)
find_package(CppUnit)

set(MINIZIP_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/minizip)

add_subdirectory(etc)
add_subdirectory(src)

# Disable tests if CppUnit wasn't found.
if(CPPUNIT_FOUND)
    add_subdirectory(test)
    enable_testing()
endif(CPPUNIT_FOUND)
