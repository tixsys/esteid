cmake_minimum_required( VERSION 2.6 )
set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" )

project( qdigidocclient )

find_package( Qt4 REQUIRED )
find_package( OpenSSL REQUIRED )
if( UNIX )
	find_package( PCSCLite REQUIRED )
ENDIF( UNIX )
find_package( LibDigiDoc REQUIRED )

SET( HEADERS
	src/DigiDoc.h
	src/MainWindow.h
	src/Settings.h
	)
SET( SOURCES
	${HEADERS}
	src/main.cpp
	src/DigiDoc.cpp
	src/MainWindow.cpp
	src/Settings.cpp
	)
SET( UIS
	src/ui/MainWindow.ui
	src/ui/Settings.ui
	)
SET( RCS src/rc/qdigidocclient.qrc )

SET( QT_USE_QTNETWORK true )
INCLUDE( ${QT_USE_FILE} )
ADD_DEFINITIONS(-DQT_NO_KEYWORDS)

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}
	${CMAKE_BINARY_DIR}
	${OPENSSL_INCLUDE_DIR}
	${PCSCLITE_INCLUDE_DIR}
	${LIBDIGIDOC_INCLUDE_DIR}
	)

QT4_ADD_RESOURCES( RC_SOURCES ${RCS} )
QT4_WRAP_UI( UI_HEADERS ${UIS} )
QT4_WRAP_CPP( MOC_SOURCES ${HEADERS} )

if(MSVC)
    # disable warning C4290: C++ exception specification ignored
    add_definitions( -wd4290 )
endif(MSVC)

ADD_EXECUTABLE( qdigidocclient WIN32 ${SOURCES} ${MOC_SOURCES} ${RC_SOURCES} ${UI_HEADERS} )

TARGET_LINK_LIBRARIES( qdigidocclient ${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES} ${LIBDIGIDOC_LIBRARY} ${OPENSSL_LIBRARIES} )
