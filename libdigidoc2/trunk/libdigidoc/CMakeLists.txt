add_subdirectory( mssp )
add_subdirectory( pkcs11 )
add_subdirectory( ts )

set( libdigidoc2_SRCS
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

add_library( digidoc2 SHARED ${libdigidoc2_SRCS} )
target_link_libraries( digidoc2
    ${LIBXML2_LIBRARIES}
    ${OPENSSL_LIBRARIES}
)
set_target_properties( digidoc2 PROPERTIES VERSION ${DIGIDOC_VERSION} SOVERSION 2 )
install( TARGETS digidoc2 DESTINATION lib )

add_executable( cdigidoc cdigidoc.c )
target_link_libraries( cdigidoc digidoc2 )
install( TARGETS cdigidoc DESTINATION bin )

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
    DESTINATION ${DIGIDOC_INCLUDE_DIR}/libdigidoc
)

install( FILES
    ts/DigiDocTS.h
    DESTINATION ${DIGIDOC_INCLUDE_DIR}/libdigidoc/ts
)
