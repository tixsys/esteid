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
SET( UIS
	ui/MainWindow.ui
	ui/MobileDialog.ui
	ui/RegisterP12.ui
	ui/Settings.ui
	ui/SignatureDialog.ui
	)
SET( RCS
	rc/qdigidocclient.qrc
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

QT4_ADD_RESOURCES( RC_SOURCES ${COMMON_RCS} ${RCS} )
QT4_WRAP_UI( UI_HEADERS ${COMMON_UIS} ${UIS} )
QT4_WRAP_CPP( MOC_SOURCES ${COMMON_MOC_HEADERS} ${MOC_HEADERS} )

if(MSVC)
	# disable warning C4290: C++ exception specification ignored
	add_definitions( -wd4290 )
	add_definitions( -wd4996 )
endif(MSVC)
IF(WIN32)
	SET( SOURCES ${SOURCES} rc/qdigidocclient.rc )
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

if(UNIX AND NOT APPLE)
add_custom_target(
	install_client_icons
	COMMAND xdg-desktop-menu install --mode system ${CMAKE_SOURCE_DIR}/client/qdigidoc-client.desktop
	COMMAND xdg-mime install --mode system ${CMAKE_SOURCE_DIR}/client/qdigidoc-client.xml
	COMMAND xdg-icon-resource install --mode system --size 48 ${CMAKE_SOURCE_DIR}/client/images/digidoc_icon_48x48.png qdigidoc-client
	COMMAND xdg-icon-resource install --mode system --context mimetypes --size 24 ${CMAKE_SOURCE_DIR}/client/images/bdoc_file_icon_24x24.png application-x-bdoc
	COMMAND xdg-icon-resource install --mode system --context mimetypes --size 24 ${CMAKE_SOURCE_DIR}/client/images/ddoc_file_icon_24x24.png application-x-ddoc
	)

add_custom_target(
	uninstall_client_icons
	COMMAND xdg-icon-resource uninstall --mode system --context mimetypes --size 24 application-x-bdoc
	COMMAND xdg-icon-resource uninstall --mode system --context mimetypes --size 24 application-x-ddoc
	COMMAND xdg-icon-resource uninstall --mode system --size 48 qdigidoc-client
	COMMAND xdg-mime uninstall --mode system ${CMAKE_SOURCE_DIR}/client/qdigidoc-client.xml
	COMMAND xdg-desktop-menu uninstall --mode system ${CMAKE_SOURCE_DIR}/client/qdigidoc-client.desktop
	)
endif(UNIX AND NOT APPLE)
