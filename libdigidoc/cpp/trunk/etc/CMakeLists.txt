set(CONF_DIR /etc/libdigidoc)

configure_file(bdoclib.conf.cmake bdoclib.conf)

install(FILES
    ${CMAKE_BINARY_DIR}/bdoclib.conf
    DESTINATION ${CONF_DIR}
)

install(DIRECTORY
    certs
    schema
    DESTINATION ${CONF_DIR}
)
