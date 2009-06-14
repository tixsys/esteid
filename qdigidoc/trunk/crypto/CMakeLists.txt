set( COMMON ${CMAKE_SOURCE_DIR}/common )

SET( MOC_HEADERS
	${COMMON}/CertificateWidget.h
	${COMMON}/IKValidator.h
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
	${COMMON}/Common.h
	${COMMON}/SslCertificate.h
	version.h
	)
SET( SOURCES
	${HEADERS}
	${COMMON}/CertificateWidget.cpp
	${COMMON}/Common.cpp
	${COMMON}/IKValidator.cpp
	${COMMON}/SslCertificate.cpp
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
	${COMMON}/ui/CertificateWidget.ui
	ui/KeyAddDialog.ui
	ui/KeyDialog.ui
	ui/MainWindow.ui
	ui/Settings.ui
	)
SET( RCS
	${COMMON}/images/images.qrc
	${COMMON}/translations/tr.qrc
	rc/qdigidoccrypto.qrc
	)

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/crypto
	${CMAKE_BINARY_DIR}/crypto
	${OPENSSL_INCLUDE_DIR}
	${LIBDIGIDOC_INCLUDE_DIR}
	${LDAP_INCLUDE_DIR}
	)

QT4_ADD_RESOURCES( RC_SOURCES ${RCS} )
QT4_WRAP_UI( UI_HEADERS ${UIS} )
QT4_WRAP_CPP( MOC_SOURCES ${MOC_HEADERS} )

IF(WIN32)
	SET( SOURCES ${SOURCES} rc/qdigidoccrypto.rc )
ENDIF(WIN32)

ADD_EXECUTABLE( qdigidoccrypto WIN32 ${SOURCES} ${MOC_SOURCES} ${RC_SOURCES} ${UI_HEADERS} )

TARGET_LINK_LIBRARIES( qdigidoccrypto
	${QT_QTMAIN_LIBRARY}
	${QT_LIBRARIES}
	${LIBDIGIDOC_LIBRARY}
	${OPENSSL_LIBRARIES}
	${OPENSSLCRYPTO_LIBRARIES}
	${LDAP_LIBRARIES}
)

if(UNIX AND NOT APPLE)
	INSTALL(FILES QDigiDocCrypto.desktop DESTINATION share/applications)
	INSTALL(FILES images/crypto_64.png DESTINATION share/pixmaps )
endif(UNIX AND NOT APPLE)

install(TARGETS qdigidoccrypto DESTINATION bin)
