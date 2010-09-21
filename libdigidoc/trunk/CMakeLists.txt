cmake_minimum_required( VERSION 2.6 )
project( libdigidoc )

# Custom cmake modules
set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" )

include( InstallSettings )

if( WIN32 )
    set( DIGIDOC_CONF_NAME "digidoc.ini" )
    set( PKCS11_MODULE "opensc-pkcs11.dll" )
elseif( APPLE )
    set( DIGIDOC_CONF_NAME "digidoc.conf" )
    set( PKCS11_MODULE "/Library/OpenSC/lib/opensc-pkcs11.so" )
else()
    set( DIGIDOC_CONF_NAME "digidoc.conf" )
    set( PKCS11_MODULE "opensc-pkcs11.so" )
endif()

set( DIGIDOC_DATA_DIR "${SHARE_INSTALL_PREFIX}/libdigidoc")
set( DIGIDOC_INCLUDE_DIR "${INCLUDE_INSTALL_DIR}/libdigidoc" )
set( DIGIDOC_VERSION "2.6.0" )

find_package( LibXml2 REQUIRED )
find_package( OpenSSL REQUIRED )
find_package( OpenSSLCrypto REQUIRED )
find_package( ZLIB REQUIRED )
find_package( Iconv REQUIRED )
if(UNIX)
    find_package( LibDL )
endif(UNIX)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/libdigidoc.pc.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/libdigidoc.pc
    @ONLY
)
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/libdigidoc.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ICONV_INCLUDE_DIR}
    ${LIBDL_INCLUDE_DIR}
    ${LIBXML2_INCLUDE_DIR}
    ${OPENSSLCRYPTO_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIR}
)

if( APPLE )
    include_directories( /Library/OpenSC/include )
    include_directories( /usr/local/include/../include )
endif( APPLE )

add_subdirectory( etc )
add_subdirectory( libdigidoc )
