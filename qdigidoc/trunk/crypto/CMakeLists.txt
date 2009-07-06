SET( MOC_HEADERS
	CryptDoc.h
	KeyDialog.h
	LdapSearch.h
	MainWindow.h
	Poller.h
	SettingsDialog.h
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
	SettingsDialog.cpp
	TreeWidget.cpp
	)
QT4_WRAP_UI( UI_HEADERS
	${COMMON_UIS}
	ui/KeyAddDialog.ui
	ui/KeyDialog.ui
	ui/MainWindow.ui
	ui/SettingsDialog.ui
	)

set(QM_DIR ${CMAKE_BINARY_DIR}/crypto)
configure_file(${COMMON}/translations/common_tr.qrc.cmake ${QM_DIR}/common_tr.qrc)
configure_file(translations/tr.qrc.cmake ${QM_DIR}/tr.qrc)

QT4_ADD_RESOURCES( RC_SOURCES
	${COMMON_RCS}
	images/images.qrc
	${QM_DIR}/common_tr.qrc
	${QM_DIR}/tr.qrc
	)
QT4_ADD_TRANSLATION( QM_FILES
	${COMMON_TS}
	translations/et.ts
	translations/ru.ts
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

QT4_WRAP_CPP( MOC_SOURCES ${COMMON_MOC_HEADERS} ${MOC_HEADERS} )

IF(WIN32)
	SET( SOURCES ${SOURCES} qdigidoccrypto.rc )
ENDIF(WIN32)

ADD_EXECUTABLE(
	qdigidoccrypto
	WIN32
	MACOSX_BUNDLE
	${COMMON_SOURCES}
	${SOURCES}
	${MOC_SOURCES}
	${QM_FILES}
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
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
	${LIBDIGIDOC_LIBRARY}
	${OPENSSL_LIBRARIES}
	${OPENSSLCRYPTO_LIBRARIES}
	${LDAP_LIBRARIES}
)
install(TARGETS qdigidoccrypto DESTINATION ${BIN_INSTALL_DIR})

install(FILES qdigidoc-crypto.desktop DESTINATION ${SHARE_INSTALL_PREFIX}/applications)
install(FILES images/crypto_64.png DESTINATION ${SHARE_INSTALL_PREFIX}/pixmaps RENAME qdigidoc-crypto.png)

install(FILES qdigidoc-crypto.xml DESTINATION ${SHARE_INSTALL_PREFIX}/mime/packages)
install(FILES images/cdoc_file_icon_24x24.png DESTINATION ${SHARE_INSTALL_PREFIX}/pixmaps RENAME application-x-cdoc.png)
