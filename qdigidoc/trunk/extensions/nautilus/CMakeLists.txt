cmake_minimum_required(VERSION 2.6)
project(NautilusQDigiDoc)

find_package(Gettext)

file(GLOB PO_FILES po/*.po)
gettext_create_translations(po/nautilus-qdigidoc.pot ALL ${PO_FILES})

install(FILES nautilus-qdigidoc.py DESTINATION ${LIB_INSTALL_DIR}/nautilus/extensions-2.0/python/)
