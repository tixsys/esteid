cmake_minimum_required( VERSION 2.6 )
set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" )

project( qdigidoccrypto )

find_package( Qt4 REQUIRED )
find_package( OpenSSL REQUIRED )
find_package( LibDigiDoc2 REQUIRED )
find_package( Ldap REQUIRED )

SET( HEADERS
	src/CryptDoc.h
	src/KeyDialog.h
	src/LdapSearch.h
	src/MainWindow.h
	src/Settings.h
	)
SET( SOURCES
	${HEADERS}
	src/main.cpp
	src/CryptDoc.cpp
	src/KeyDialog.cpp
	src/LdapSearch.cpp
	src/MainWindow.cpp
	src/Settings.cpp
	)
SET( UIS
	src/ui/KeyAddDialog.ui
	src/ui/KeyDialog.ui
	src/ui/MainWindow.ui
	src/ui/Settings.ui
	)
SET( RCS src/rc/qdigidoccrypto.qrc )

SET( QT_USE_QTNETWORK true )
INCLUDE( ${QT_USE_FILE} )
ADD_DEFINITIONS(-DQT_NO_KEYWORDS)

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}
	${CMAKE_BINARY_DIR}
	${OPENSSL_INCLUDE_DIR}
	${LIBDIGIDOC2_INCLUDE_DIR}
	${LDAP_INCLUDE_DIR}
	)

QT4_ADD_RESOURCES( RC_SOURCES ${RCS} )
QT4_WRAP_UI( UI_HEADERS ${UIS} )
QT4_WRAP_CPP( MOC_SOURCES ${HEADERS} )

ADD_EXECUTABLE( qdigidoccrypto WIN32 ${SOURCES} ${MOC_SOURCES} ${RC_SOURCES} ${UI_HEADERS} )

TARGET_LINK_LIBRARIES( qdigidoccrypto
	${QT_QTMAIN_LIBRARY}
	${QT_LIBRARIES}
	${OPENSSL_LIBRARIES}
	${LIBDIGIDOC2_LIBRARY}
	${LDAP_LIBRARIES}
)

if(UNIX AND NOT APPLE)
	INSTALL(FILES QDigiDocCrypto.desktop DESTINATION share/applications)
endif(UNIX AND NOT APPLE)

install(TARGETS qdigidoccrypto DESTINATION bin)
