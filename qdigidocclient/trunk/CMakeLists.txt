cmake_minimum_required( VERSION 2.6 )
set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" )

project( qdigidocclient )

find_package( Qt4 REQUIRED )
find_package( OpenSSL REQUIRED )
if( WIN32 )
	find_package( OpenSSLCrypto REQUIRED )
	find_package( XercesC REQUIRED )
	find_package( XmlSecurityC REQUIRED )
	find_package( XSD REQUIRED )
endif( WIN32 )
if( UNIX )
	find_package( PCSCLite REQUIRED )
endif( UNIX )
find_package( LibDigiDoc++ REQUIRED )

SET( MOC_HEADERS
	common/CertificateWidget.h
	src/DigiDoc.h
	src/IKValidator.h
	src/MainWindow.h
	src/MobileDialog.h
	src/Poller.h
	src/PrintSheet.h
	src/Settings.h
	src/SignatureDialog.h
	src/TreeWidget.h
	)
SET( HEADERS
	${MOC_HEADERS}
	common/SslCertificate.h
	src/QMobileSigner.h
	src/version.h
	)
SET( SOURCES
	${HEADERS}
	common/CertificateWidget.cpp
	common/SslCertificate.cpp
	src/main.cpp
	src/DigiDoc.cpp
	src/IKValidator.cpp
	src/MainWindow.cpp
	src/MobileDialog.cpp
	src/Poller.cpp
	src/PrintSheet.cpp
	src/QMobileSigner.cpp
	src/Settings.cpp
	src/SignatureDialog.cpp
	src/TreeWidget.cpp
	)
SET( UIS
	common/ui/CertificateWidget.ui
	src/ui/MainWindow.ui
	src/ui/MobileDialog.ui
	src/ui/Settings.ui
	src/ui/SignatureDialog.ui
	)
SET( RCS src/rc/qdigidocclient.qrc )

SET( QT_USE_QTNETWORK true )
SET( QT_USE_QTWEBKIT true )
SET( QT_USE_QTXML true )

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
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_BINARY_DIR}
	${OPENSSL_INCLUDE_DIR}
	${XERCESC_INCLUDE_DIR}
	${XSD_INCLUDE_DIR}
	${PCSCLITE_INCLUDE_DIR}
	${LIBDIGIDOC++_INCLUDE_DIR}
	)

# xCode hacks
if(APPLE)
	include_directories(/usr/local/include/../include)
endif(APPLE)

QT4_ADD_RESOURCES( RC_SOURCES ${RCS} )
QT4_WRAP_UI( UI_HEADERS ${UIS} )
QT4_WRAP_CPP( MOC_SOURCES ${MOC_HEADERS} )

if(MSVC)
	# disable warning C4290: C++ exception specification ignored
	add_definitions( -wd4290 )
	add_definitions( -wd4996 )
endif(MSVC)
IF(WIN32)
	SET( SOURCES ${SOURCES} src/rc/qdigidocclient.rc )
ENDIF(WIN32)

if(NOT "${LIBDIGIDOC++_CONF}" STREQUAL "LIBDIGIDOC++_CONF-NOTFOUND")
	add_definitions( -DBDOCLIB_CONF_PATH="${LIBDIGIDOC++_CONF}" )
endif(NOT "${LIBDIGIDOC++_CONF}" STREQUAL "LIBDIGIDOC++_CONF-NOTFOUND")

ADD_EXECUTABLE( qdigidocclient WIN32 ${SOURCES} ${MOC_SOURCES} ${RC_SOURCES} ${UI_HEADERS} )

TARGET_LINK_LIBRARIES( qdigidocclient
	${QT_QTMAIN_LIBRARY}
	${QT_LIBRARIES}
	${LIBDIGIDOC++_LIBRARY}
	${OPENSSL_LIBRARIES}
	${OPENSSLCRYPTO_LIBRARIES}
	${XERCESC_LIBRARIES}
	${XMLSECURITYC_LIBRARIES}
)

if(UNIX AND NOT APPLE)
	INSTALL(FILES QDigiDocClient.desktop DESTINATION share/applications)
	INSTALL(FILES src/images/digidoc_icon_48x48.png DESTINATION share/pixmaps )
endif(UNIX AND NOT APPLE)

install(TARGETS qdigidocclient DESTINATION bin)
