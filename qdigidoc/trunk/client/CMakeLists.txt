set( COMMON ${CMAKE_SOURCE_DIR}/common )

SET( MOC_HEADERS
	${COMMON}/CertificateWidget.h
	${COMMON}/Common.h
	${COMMON}/IKValidator.h
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
	${COMMON}/SslCertificate.h
	QMobileSigner.h
	version.h
	)
SET( SOURCES
	${HEADERS}
	${COMMON}/CertificateWidget.cpp
	${COMMON}/Common.cpp
	${COMMON}/SslCertificate.cpp
	${COMMON}/IKValidator.cpp
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
	${COMMON}/ui/CertificateWidget.ui
	ui/MainWindow.ui
	ui/MobileDialog.ui
	ui/RegisterP12.ui
	ui/Settings.ui
	ui/SignatureDialog.ui
	)
SET( RCS
	${COMMON}/images/images.qrc
	${COMMON}/translations/tr.qrc
	rc/qdigidocclient.qrc
	)

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/client
	${CMAKE_BINARY_DIR}/client
	${OPENSSL_INCLUDE_DIR}
	${XERCESC_INCLUDE_DIR}
	${XSD_INCLUDE_DIR}
	${PCSCLITE_INCLUDE_DIR}
	${LIBDIGIDOCPP_INCLUDE_DIR}
	)

QT4_ADD_RESOURCES( RC_SOURCES ${RCS} )
QT4_WRAP_UI( UI_HEADERS ${UIS} )
QT4_WRAP_CPP( MOC_SOURCES ${MOC_HEADERS} )

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

ADD_EXECUTABLE( qdigidocclient WIN32 ${SOURCES} ${MOC_SOURCES} ${RC_SOURCES} ${UI_HEADERS} )

TARGET_LINK_LIBRARIES( qdigidocclient
	${QT_QTMAIN_LIBRARY}
	${QT_LIBRARIES}
	${LIBDIGIDOCPP_LIBRARY}
	${OPENSSL_LIBRARIES}
	${OPENSSLCRYPTO_LIBRARIES}
)

if(UNIX AND NOT APPLE)
	INSTALL(FILES qdigidoc.desktop DESTINATION ${SHARE_INSTALL_PREFIX}/applications)

	# Install icons
	foreach(RES 48x48)
		install(
			FILES images/digidoc_icon_${RES}.png
			DESTINATION ${SHARE_INSTALL_PREFIX}/icons/hicolor/${RES}/apps/
			RENAME qdigidoc.png
		)
	endforeach(RES)
endif(UNIX AND NOT APPLE)

install(TARGETS qdigidocclient DESTINATION ${BIN_INSTALL_DIR})
