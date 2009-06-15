set(CONF_DIR /etc/digidocpp)

configure_file(digidocpp.conf.cmake digidocpp.conf)

install(FILES
    ${CMAKE_BINARY_DIR}/digidocpp.conf
    DESTINATION ${CONF_DIR}
)

install(DIRECTORY
    certs
    schema
    DESTINATION ${CONF_DIR}
    REGEX "\\.svn$|_svn$" EXCLUDE
)
