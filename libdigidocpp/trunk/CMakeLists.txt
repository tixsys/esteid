cmake_minimum_required(VERSION 2.6)
project(libdigidocpp)

# Custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

include(InstallSettings)

if(WIN32)
    set(PKCS11_MODULE "opensc-pkcs11.dll")
else(WIN32)
    set(PKCS11_MODULE "opensc-pkcs11.so")
endif(WIN32)

if(LIBDIGIDOC_MODULE)
    add_definitions(-DLIBDIGIDOC_NAME="${LIBDIGIDOC_MODULE}")
endif(LIBDIGIDOC_MODULE)

set(DIGIDOCPP_INCLUDE_DIR "${INCLUDE_INSTALL_DIR}/digidocpp")
set(DIGIDOCPP_CONFIG_DIR  "${SYSCONF_INSTALL_DIR}/digidocpp")
add_definitions(-DDIGIDOCPP_CONFIG_DIR="${DIGIDOCPP_CONFIG_DIR}")
set(DIGIDOCPP_VERSION     "0.2.0")

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/libdigidocpp.pc.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/libdigidocpp.pc
    @ONLY
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libdigidocpp.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)

if(UNIX AND NOT APPLE)
    find_package(Iconv REQUIRED)
endif(UNIX AND NOT APPLE)
find_package(LibDigiDoc REQUIRED)
if(UNIX)
    find_package(LibDL)
endif(UNIX)
find_package(LibP11 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(OpenSSLCrypto REQUIRED)
#find_package(PKCS11H REQUIRED)
find_package(XercesC REQUIRED)
find_package(XmlSecurityC REQUIRED)
find_package(XSD REQUIRED)
find_package(ZLIB REQUIRED)

find_package(MiniZip QUIET)
if(MINIZIP_FOUND)
    message(STATUS "Found MiniZip: ${MINIZIP_LIBRARIES}")
else(MINIZIP_FOUND)
    message(STATUS "MiniZip not found; using bundled copy.")
    set(MINIZIP_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/minizip)
endif(MINIZIP_FOUND)

## Packages required for Dynamic Language Wrappers
find_package(SWIG QUIET)
if(SWIG_FOUND)
    find_package(PythonLibs QUIET)
    find_package(PerlLibs QUIET)
    find_package(PHPLibs QUIET)
endif(SWIG_FOUND)

add_subdirectory(etc)
add_subdirectory(src)

#add_subdirectory(test)
#ENABLE_TESTING()
