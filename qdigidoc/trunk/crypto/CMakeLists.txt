SET( MOC_HEADERS
	CryptDoc.h
	KeyDialog.h
	LdapSearch.h
	MainWindow.h
	Poller.h
	Settings.h
	TreeWidget.h
	)
SET( HEADERS
	${MOC_HEADERS}
	version.h
	)
SET( SOURCES
	${HEADERS}
	main.cpp
	CryptDoc.cpp
	KeyDialog.cpp
	LdapSearch.cpp
	MainWindow.cpp
	Poller.cpp
	Settings.cpp
	TreeWidget.cpp
	)
SET( UIS
	ui/KeyAddDialog.ui
	ui/KeyDialog.ui
	ui/MainWindow.ui
	ui/Settings.ui
	)
SET( RCS
	rc/qdigidoccrypto.qrc
	)

INCLUDE_DIRECTORIES(
	${COMMON}
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/crypto
	${CMAKE_BINARY_DIR}/crypto
	${OPENSSL_INCLUDE_DIR}
	${LIBDIGIDOC_INCLUDE_DIR}
	${LDAP_INCLUDE_DIR}
	)

if(APPLE)
    set(APP_CONTENTS_DIR "${CMAKE_SOURCE_DIR}/crypto/mac")
    
    # xCode hacks
    include_directories(/usr/local/include/../include)
    
    # Bundle resources
    file(GLOB_RECURSE RESOURCE_FILES
        ${APP_CONTENTS_DIR}/Resources/*.icns
        ${APP_CONTENTS_DIR}/Resources/**/*.strings)
    
    foreach(_file ${RESOURCE_FILES})
        get_filename_component(_file_dir ${_file} PATH)
        file(RELATIVE_PATH _file_dir ${APP_CONTENTS_DIR}/Resources ${_file_dir})
        set_source_files_properties(${_file} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/${_file_dir}" )
    endforeach(_file)
endif(APPLE)

QT4_ADD_RESOURCES( RC_SOURCES ${COMMON_RCS} ${RCS} )
QT4_WRAP_UI( UI_HEADERS ${COMMON_UIS} ${UIS} )
QT4_WRAP_CPP( MOC_SOURCES ${COMMON_MOC_HEADERS} ${MOC_HEADERS} )

IF(WIN32)
	SET( SOURCES ${SOURCES} rc/qdigidoccrypto.rc )
ENDIF(WIN32)

ADD_EXECUTABLE(
	qdigidoccrypto
	WIN32
	MACOSX_BUNDLE
	${COMMON_SOURCES}
	${SOURCES}
	${MOC_SOURCES}
	${RC_SOURCES}
	${UI_HEADERS}
	${RESOURCE_FILES}
	)

if(APPLE)
    configure_file(
        ${APP_CONTENTS_DIR}/Info.plist.cmake
        ${APP_CONTENTS_DIR}/Info.plist
        @ONLY)
    
    set_target_properties(qdigidoccrypto
    	PROPERTIES
    	MACOSX_BUNDLE_INFO_PLIST ${APP_CONTENTS_DIR}/Info.plist)
endif(APPLE)

TARGET_LINK_LIBRARIES( qdigidoccrypto
	${QT_QTMAIN_LIBRARY}
	${QT_LIBRARIES}
	${LIBDIGIDOC_LIBRARY}
	${OPENSSL_LIBRARIES}
	${OPENSSLCRYPTO_LIBRARIES}
	${LDAP_LIBRARIES}
)

install(TARGETS qdigidoccrypto DESTINATION ${BIN_INSTALL_DIR})

if(UNIX AND NOT APPLE)
add_custom_target(
	install_crypto_icons
	COMMAND xdg-desktop-menu install --mode system ${CMAKE_SOURCE_DIR}/crypto/qdigidoc-crypto.desktop
        COMMAND xdg-mime install --mode system ${CMAKE_SOURCE_DIR}/crypto/qdigidoc-crypto.xml
	COMMAND xdg-icon-resource install --mode system --size 48 ${CMAKE_SOURCE_DIR}/crypto/images/crypto_64.png qdigidoc-crypto
        COMMAND xdg-icon-resource install --mode system --context mimetypes --size 24 ${CMAKE_SOURCE_DIR}/crypto/images/cdoc_file_icon_24x24.png application-x-cdoc
        )

add_custom_target(
        uninstall_crypto_icons
        COMMAND xdg-icon-resource uninstall --mode system --context mimetypes --size 24 application-x-cdoc
	COMMAND xdg-icon-resource uninstall --mode system --size 48 qdigidoc-crypto
        COMMAND xdg-mime uninstall --mode system ${CMAKE_SOURCE_DIR}/crypto/qdigidoc-crypto.xml
	COMMAND xdg-desktop-menu uninstall --mode system ${CMAKE_SOURCE_DIR}/crypto/qdigidoc-crypto.desktop
        )
endif(UNIX AND NOT APPLE)
