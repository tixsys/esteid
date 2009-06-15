cmake_minimum_required(VERSION 2.6)
project(libdigidoc++)

# Custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

set( PKCS11_PROVIDER
    "opensc-pkcs11.so"
    CACHE FILEPATH "pkcs11 module"
)

if(LIBDIGIDOC_PROVIDER)
    add_definitions(-DLIBDIGIDOC_NAME="${LIBDIGIDOC_PROVIDER}")
endif(LIBDIGIDOC_PROVIDER)

find_package(Iconv REQUIRED)
find_package(LibDigiDoc REQUIRED)
find_package(LibP11 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(OpenSSLCrypto REQUIRED)
find_package(XercesC REQUIRED)
find_package(XmlSecurityC REQUIRED)
find_package(XSD REQUIRED)
find_package(ZLIB REQUIRED)

if(UNIX)
	find_package(LibDL REQUIRED)
endif(UNIX)

set(MINIZIP_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/minizip)

add_subdirectory(etc)
add_subdirectory(src)

#add_subdirectory(test)
#ENABLE_TESTING()
