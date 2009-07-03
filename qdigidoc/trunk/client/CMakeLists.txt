SET( MOC_HEADERS
	DigiDoc.h
	MainWindow.h
	MobileDialog.h
	Poller.h
	PrintSheet.h
	RegisterP12.h
	Settings.h
	SignatureDialog.h
	TreeWidget.h
	)
SET( HEADERS
	${MOC_HEADERS}
	QMobileSigner.h
	version.h
	)
SET( SOURCES
	${HEADERS}
	main.cpp
	DigiDoc.cpp
	MainWindow.cpp
	MobileDialog.cpp
	Poller.cpp
	PrintSheet.cpp
	QMobileSigner.cpp
	RegisterP12.cpp
	Settings.cpp
	SignatureDialog.cpp
	TreeWidget.cpp
	)
QT4_WRAP_UI( UI_HEADERS
	${COMMON_UIS}
	ui/MainWindow.ui
	ui/MobileDialog.ui
	ui/RegisterP12.ui
	ui/Settings.ui
	ui/SignatureDialog.ui
	)

set(QM_DIR ${CMAKE_BINARY_DIR}/client)
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
	${CMAKE_SOURCE_DIR}/client
	${CMAKE_BINARY_DIR}/client
	${OPENSSL_INCLUDE_DIR}
	${XERCESC_INCLUDE_DIR}
	${XSD_INCLUDE_DIR}
	${PCSCLITE_INCLUDE_DIR}
	${LIBDIGIDOCPP_INCLUDE_DIR}
	)

if(APPLE)
    set(APP_CONTENTS_DIR "${CMAKE_SOURCE_DIR}/client/mac")
    
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

if(MSVC)
	# disable warning C4290: C++ exception specification ignored
	add_definitions( -wd4290 )
	add_definitions( -wd4996 )
endif(MSVC)
IF(WIN32)
	SET( SOURCES ${SOURCES} qdigidocclient.rc )
ENDIF(WIN32)

if(NOT "${LIBDIGIDOCPP_CONF}" STREQUAL "LIBDIGIDOCPP_CONF-NOTFOUND")
	add_definitions( -DBDOCLIB_CONF_PATH="${LIBDIGIDOCPP_CONF}" )
endif(NOT "${LIBDIGIDOCPP_CONF}" STREQUAL "LIBDIGIDOCPP_CONF-NOTFOUND")

ADD_EXECUTABLE(
	qdigidocclient
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
    
    set_target_properties(qdigidocclient
    	PROPERTIES
    	MACOSX_BUNDLE_INFO_PLIST ${APP_CONTENTS_DIR}/Info.plist)
endif(APPLE)

TARGET_LINK_LIBRARIES( qdigidocclient
	${QT_QTMAIN_LIBRARY}
	${QT_LIBRARIES}
	${LIBDIGIDOCPP_LIBRARY}
	${OPENSSL_LIBRARIES}
	${OPENSSLCRYPTO_LIBRARIES}
)
install(TARGETS qdigidocclient DESTINATION ${BIN_INSTALL_DIR})

install(FILES qdigidoc-client.desktop DESTINATION ${SHARE_INSTALL_PREFIX}/applications)
install(FILES images/digidoc_icon_48x48.png DESTINATION ${SHARE_INSTALL_PREFIX}/pixmaps RENAME qdigidoc-client.png)

install(FILES qdigidoc-client.xml DESTINATION ${SHARE_INSTALL_PREFIX}/mime/packages)
install(FILES images/bdoc_file_icon_24x24.png DESTINATION ${SHARE_INSTALL_PREFIX}/pixmaps RENAME application-x-bdoc.png)
install(FILES images/ddoc_file_icon_24x24.png DESTINATION ${SHARE_INSTALL_PREFIX}/pixmaps RENAME application-x-ddoc.png)
