set(SERVICES_INSTALL_DIR "${SHARE_INSTALL_PREFIX}/kde4/services" CACHE PATH "Install dir for KDE service (desktop, protocol, ...) files")

install(FILES qdigidoc-signer.desktop DESTINATION "${SERVICES_INSTALL_DIR}")
