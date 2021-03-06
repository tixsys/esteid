cmake_minimum_required(VERSION 2.6)
project(libdigidocpp)

# Custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

include(InstallSettings)

if(WIN32)
    set(PKCS11_MODULE "opensc-pkcs11.dll")
elseif(APPLE)
    set(PKCS11_MODULE "/Library/OpenSC/lib/opensc-pkcs11.so")
else()
    set(PKCS11_MODULE "opensc-pkcs11.so")
endif()

if(LIBDIGIDOC_MODULE)
    add_definitions(-DLIBDIGIDOC_NAME="${LIBDIGIDOC_MODULE}")
endif(LIBDIGIDOC_MODULE)

set(DIGIDOCPP_INCLUDE_DIR "${INCLUDE_INSTALL_DIR}/digidocpp")
set(DIGIDOCPP_CONFIG_DIR  "${SYSCONF_INSTALL_DIR}/digidocpp")
add_definitions(-DDIGIDOCPP_CONFIG_DIR="${DIGIDOCPP_CONFIG_DIR}")
set(DIGIDOCPP_VERSION     "0.3.0")

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

find_package(MiniZip QUIET)
if(MINIZIP_FOUND)
    message(STATUS "Found MiniZip: ${MINIZIP_LIBRARIES}")
else()
    message(STATUS "MiniZip not found; using bundled copy.")
    set(MINIZIP_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/minizip)
    # zlib is only required for building minizip
    find_package(ZLIB REQUIRED)
endif()

option(ENABLE_BINDINGS "Build bindings." TRUE)
if(ENABLE_BINDINGS)
    ## Packages required for Dynamic Language Wrappers
    find_package(SWIG)
    if(SWIG_FOUND)
        find_package(PythonLibs)
        find_package(PerlLibs)
        find_package(PHPLibs)
    endif(SWIG_FOUND)
endif()

add_subdirectory(etc)
add_subdirectory(src)

option(BUILD_TESTS "Build tests." FALSE)
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
