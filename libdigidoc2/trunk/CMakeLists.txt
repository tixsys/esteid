cmake_minimum_required( VERSION 2.6 )
project( libdigidoc2 )

# Custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

set( SYSCONF_INSTALL_DIR
    "${CMAKE_INSTALL_PREFIX}/etc"
    CACHE PATH "Configuration files directory"
)
set( SHARE_INSTALL_PREFIX
    "${CMAKE_INSTALL_PREFIX}/share"
    CACHE PATH "Base directory for files which go to share/"
)

set( PKCS11_PROVIDER "/usr/lib/opensc-pkcs11.so.1" )
set( DIGIDOC_CONF_FILE "${SYSCONF_INSTALL_DIR}/digidoc.conf" )
set( DIGIDOC_CERTS_PATH "${SHARE_INSTALL_PREFIX}/libdigidoc2/certs" )
set( DIGIDOC_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include/libdigidoc2" )
set( DIGIDOC_VERSION "2.6.0" )

find_package( LibXml2 REQUIRED )
find_package( OpenSSL REQUIRED )
find_package( OpenSSLCrypto REQUIRED )
find_package( ZLIB REQUIRED )

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/libdigidoc2.pc.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/libdigidoc2.pc
    @ONLY
)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LIBXML2_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
    ${OPENSSLCRYPTO_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIR}
)

add_subdirectory( etc )
add_subdirectory( libdigidoc )

install( FILES ${CMAKE_CURRENT_BINARY_DIR}/libdigidoc2.pc DESTINATION lib/pkgconfig )
