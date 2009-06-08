set(CONF_DIR /etc/libdigidoc++)

configure_file(libdigidoc++.conf.cmake libdigidoc++.conf)

install(FILES
    ${CMAKE_BINARY_DIR}/libdigidoc++.conf
    DESTINATION ${CONF_DIR}
)

install(DIRECTORY
    certs
    schema
    DESTINATION ${CONF_DIR}
    REGEX "\\.svn$|_svn$" EXCLUDE
)
