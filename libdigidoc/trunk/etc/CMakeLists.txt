configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/digidoc.conf.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/digidoc.conf
    @ONLY
)
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/digidoc.conf DESTINATION ${SYSCONF_INSTALL_DIR} )

install(DIRECTORY
    certs
    DESTINATION ${DIGIDOC_DATA_DIR}
    REGEX "\\.svn$|_svn$" EXCLUDE
)
