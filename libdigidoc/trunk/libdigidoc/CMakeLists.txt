add_subdirectory( mssp )
add_subdirectory( pkcs11 )
add_subdirectory( ts )

set( libdigidoc_SRCS
    DigiDocConfig.c
    DigiDocLib.c
    DigiDocObj.c
    DigiDocPKCS11.c
    DigiDocError.c
    DigiDocParser.c
    DigiDocDebug.c
    DigiDocSAXParser.c
    DigiDocMem.c
    DigiDocStack.c
    DigiDocEnc.c
    DigiDocEncGen.c
    DigiDocEncSAXParser.c
    DigiDocCert.c
    DigiDocConvert.c
    DigiDocGen.c
    DigiDocVerify.c
    DigiDocOCSP.c
    DigiDocDfExtract.c
)

add_definitions( -DSYSCONFDIR="${SYSCONF_INSTALL_DIR}" )
add_definitions( -DDIGIDOC_VERSION="${DIGIDOC_VERSION}" )
add_definitions( -DWITH_SOAPDEFS_H )
if( WIN32 )
    add_definitions( -DBUILDINGDLL )
endif( WIN32 )
if( MSVC )
    add_definitions(
        -D_CRT_NONSTDC_NO_DEPRECATE
        -D_CRT_SECURE_NO_DEPRECATE
        -D_CRT_SECURE_NO_WARNINGS
        -D_SCL_SECURE_NO_WARNINGS
    )
endif( MSVC )

add_library( digidoc SHARED ${libdigidoc_SRCS} )
target_link_libraries( digidoc
    ${LIBXML2_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    ${OPENSSLCRYPTO_LIBRARIES}
    ${ZLIB_LIBRARIES}
)
set_target_properties( digidoc PROPERTIES VERSION ${DIGIDOC_VERSION} SOVERSION 2 )
install( TARGETS digidoc DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS} )

add_executable( cdigidoc cdigidoc.c )
target_link_libraries( cdigidoc digidoc )
install( TARGETS cdigidoc DESTINATION ${BIN_INSTALL_DIR} )

# Include files
install( FILES
    DigiDocCert.h
    DigiDocConfig.h
    DigiDocConvert.h
    DigiDocDebug.h
    DigiDocDefs.h
    DigiDocDfExtract.h
    DigiDocEncGen.h
    DigiDocEnc.h
    DigiDocEncSAXParser.h
    DigiDocError.h
    DigiDocGen.h
    DigiDocLib.h
    DigiDocMem.h
    DigiDocObj.h
    DigiDocOCSP.h
    DigiDocParser.h
    DigiDocPKCS11.h
    DigiDocSAXParser.h
    DigiDocStack.h
    DigiDocVerify.h
    DESTINATION ${DIGIDOC_INCLUDE_DIR}
)
