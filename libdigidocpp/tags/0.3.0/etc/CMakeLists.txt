configure_file(digidocpp.conf.cmake ${CMAKE_CURRENT_BINARY_DIR}/digidocpp.conf)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/digidocpp.conf
    DESTINATION ${DIGIDOCPP_CONFIG_DIR}
)

install(DIRECTORY
    certs
    DESTINATION ${DIGIDOCPP_CONFIG_DIR}
    REGEX "\\.svn$|_svn$" EXCLUDE
)

add_subdirectory(schema)
