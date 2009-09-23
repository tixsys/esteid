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
set(DIGIDOCPP_VERSION     "0.1.0")

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/libdigidocpp.pc.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/libdigidocpp.pc
    @ONLY
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libdigidocpp.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)

find_package(Iconv REQUIRED)
find_package(LibDigiDoc REQUIRED)
find_package(LibP11 REQUIRED)
#find_package(PKCS11H REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(OpenSSLCrypto REQUIRED)
find_package(XercesC REQUIRED)
find_package(XmlSecurityC REQUIRED)
find_package(XSD REQUIRED)
find_package(ZLIB REQUIRED)

if(UNIX)
	find_package(LibDL)
endif(UNIX)

set(MINIZIP_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/minizip)

add_subdirectory(etc)
add_subdirectory(src)

#add_subdirectory(test)
#ENABLE_TESTING()
