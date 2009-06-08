cmake_minimum_required( VERSION 2.6 )
set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" )

project( qdigidoccrypto )

find_package( Qt4 REQUIRED )
find_package( OpenSSL REQUIRED )
find_package( OpenSSLCrypto REQUIRED )
find_package( LibDigiDoc REQUIRED )
find_package( Ldap REQUIRED )

SET( MOC_HEADERS
	src/CertificateWidget.h
	src/CryptDoc.h
	src/KeyDialog.h
	src/LdapSearch.h
	src/MainWindow.h
	src/Poller.h
	src/Settings.h
	)
SET( HEADERS
	${MOC_HEADERS}
	src/Common.h
	src/SslCertificate.h
	src/version.h
	)
SET( SOURCES
	${HEADERS}
	src/main.cpp
	src/CertificateWidget.cpp
	src/Common.cpp
	src/CryptDoc.cpp
	src/KeyDialog.cpp
	src/LdapSearch.cpp
	src/MainWindow.cpp
	src/Poller.cpp
	src/Settings.cpp
	src/SslCertificate.cpp
	)
SET( UIS
	src/ui/CertificateWidget.ui
	src/ui/KeyAddDialog.ui
	src/ui/KeyDialog.ui
	src/ui/MainWindow.ui
	src/ui/Settings.ui
	)
SET( RCS src/rc/qdigidoccrypto.qrc )

SET( QT_USE_QTNETWORK true )
INCLUDE( ${QT_USE_FILE} )
ADD_DEFINITIONS(-DQT_NO_KEYWORDS)

find_package(Subversion)
if (Subversion_FOUND)
  if (EXISTS ${PROJECT_SOURCE_DIR}/.svn)
	  Subversion_WC_INFO(${PROJECT_SOURCE_DIR} PROJECT)
	  message(STATUS "Current SVN revision is ${PROJECT_WC_REVISION}")
	  add_definitions(-DBUILD_VER=${PROJECT_WC_REVISION})
	  set(SVN_BUILD ${PROJECT_WC_REVISION})
  else (EXISTS ${PROJECT_SOURCE_DIR}/.svn)
	  add_definitions(-DBUILD_VER=0)
  endif (EXISTS ${PROJECT_SOURCE_DIR}/.svn)
else (Subversion_FOUND)
  add_definitions(-DBUILD_VER=0)
endif (Subversion_FOUND)

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_BINARY_DIR}
	${OPENSSL_INCLUDE_DIR}
	${LIBDIGIDOC_INCLUDE_DIR}
	${LDAP_INCLUDE_DIR}
	)

# xCode hacks
if(APPLE)
	include_directories(/usr/local/include/../include)
endif(APPLE)

QT4_ADD_RESOURCES( RC_SOURCES ${RCS} )
QT4_WRAP_UI( UI_HEADERS ${UIS} )
QT4_WRAP_CPP( MOC_SOURCES ${MOC_HEADERS} )

IF(WIN32)
	SET( SOURCES ${SOURCES} src/rc/qdigidoccrypto.rc )
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
	INSTALL(FILES src/images/crypto_icon_48x48.png DESTINATION share/pixmaps )
endif(UNIX AND NOT APPLE)

install(TARGETS qdigidoccrypto DESTINATION bin)
