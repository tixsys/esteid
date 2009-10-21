configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/digidoc.conf.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/digidoc.conf
    @ONLY
)
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/digidoc.conf DESTINATION ${SYSCONF_INSTALL_DIR} )


set( digidoc_CERTS
    eid-sk-2007.pem
    eid-sk-2007-responder.pem
    esteid-sk-2007.pem
    esteid-sk-2007-responder.pem
    eid-sk-2007.pem
    eid-sk.pem
    ocsp_hash_resp1.pem
    sk-eid-ocsp-responder-2006.cer
    sk-esteid-ocsp-responder-2005.pem
    sk-esteid-ocsp-responder.pem
    sk-esteid.pem
    sk-klass3-ocsp-responder-2006.cer
    sk-klass3-ocsp-responder-2009.pem
    sk-klass3-ocsp-responder.pem
    sk-klass3.pem
    sk-root.pem
    sk-test-ocsp-responder-2005.pem
    sk-test-ocsp-responder.pem
    sk-test.pem
    test_ocsp_020305-040305.pem
    test_ocsp_040305-080308.pem
    test_ocsp_2006.cer
)
install(FILES ${digidoc_CERTS} DESTINATION ${DIGIDOC_CERTS_PATH})
