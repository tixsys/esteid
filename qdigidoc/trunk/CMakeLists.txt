cmake_minimum_required( VERSION 2.6 )
project( qdigidoc )

set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" )

include( InstallSettings )

find_package( Ldap REQUIRED )
find_package( LibDigiDoc REQUIRED )
find_package( LibDigiDocpp REQUIRED )
find_package( OpenSSL REQUIRED )
find_package( OpenSSLCrypto REQUIRED )
find_package( Qt4 REQUIRED )
find_package( XercesC REQUIRED )
find_package( XSD REQUIRED )

set( QT_MIN_VERSION 4.4.0 )
set( QT_USE_QTNETWORK true )
set( QT_USE_QTWEBKIT true )
set( QT_USE_QTXML true )

include( ${QT_USE_FILE} )
add_definitions( -DQT_NO_KEYWORDS )

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

set( COMMON ${CMAKE_SOURCE_DIR}/common )
set( COMMON_MOC_HEADERS
	${COMMON}/CertificateWidget.h
	${COMMON}/Common.h
	${COMMON}/IKValidator.h
	${COMMON}/PinDialog.h
	)
set( COMMON_HEADERS
	${COMMON_MOC_HEADERS}
	${COMMON}/SslCertificate.h
	)
set( COMMON_SOURCES
	${COMMON_HEADERS}
	${COMMON}/CertificateWidget.cpp
	${COMMON}/Common.cpp
	${COMMON}/IKValidator.cpp
	${COMMON}/PinDialog.cpp
	${COMMON}/SslCertificate.cpp
	)
set( COMMON_UIS
	${COMMON}/ui/CertificateWidget.ui
	)
set( COMMON_RCS
	${COMMON}/images/images.qrc
	${COMMON}/translations/tr.qrc
	)

add_subdirectory( client )
add_subdirectory( crypto )
