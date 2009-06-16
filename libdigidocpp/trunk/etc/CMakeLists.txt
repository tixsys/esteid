configure_file(digidocpp.conf.cmake digidocpp.conf)

install(FILES
    ${CMAKE_BINARY_DIR}/digidocpp.conf
    DESTINATION ${DIGIDOCPP_CONFIG_DIR}
)

install(DIRECTORY
    certs
    schema
    DESTINATION ${DIGIDOCPP_CONFIG_DIR}
    REGEX "\\.svn$|_svn$" EXCLUDE
)
