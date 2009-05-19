cmake_minimum_required(VERSION 2.6)
project(qesteidutil)

# Custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

find_package(Qt4 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(OpenSSLCrypto REQUIRED)
if(UNIX)
    find_package(PCSCLite REQUIRED)
endif(UNIX)

set(qesteidutil_MOC_HDRS
    CertificateWidget.h
    DiagnosticsDialog.h
    Settings.h
    SettingsDialog.h
    jscardmanager.h
    jscertdata.h
    jsesteidcard.h
    jsextender.h
    mainwindow.h
    sslConnect.h
)

set(qesteidutil_HDRS
    SslCertificate.h
)

set(cardlib_SRCS
    cardlib/CardBase.cpp
    cardlib/CTAPIManager.cpp
    cardlib/DynamicLibrary.cpp
    cardlib/EstEidCard.cpp
    cardlib/PCSCManager.cpp
    cardlib/SCError.cpp
    cardlib/SmartCardManager.cpp
)

set(qesteidutil_SRCS
    CertificateWidget.cpp
    DiagnosticsDialog.cpp
    SettingsDialog.cpp
    jscardmanager.cpp
    jscertdata.cpp
    jsesteidcard.cpp
    jsextender.cpp
    main.cpp
    mainwindow.cpp
    SslCertificate.cpp
    sslConnect.cpp
)

set( UIS
    CertificateWidget.ui
    DiagnosticsDialog.ui
    SettingsDialog.ui
)

set(qesteidutil_RCS qesteidutil.qrc)

set(QT_USE_QTNETWORK true)
set(QT_USE_QTWEBKIT true)

include(${QT_USE_FILE})

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${PCSCLITE_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
)

QT4_ADD_RESOURCES(qesteidutil_RCC_SRCS ${qesteidutil_RCS})
QT4_WRAP_UI( UI_HEADERS ${UIS} )
QT4_WRAP_CPP(qesteidutil_MOC_SRCS ${qesteidutil_MOC_HDRS})

IF(WIN32)
	SET( qesteidutil_SRCS ${qesteidutil_SRCS} qesteidutil.rc )
ENDIF(WIN32)

add_executable(qesteidutil WIN32
    ${cardlib_SRCS}
    ${qesteidutil_SRCS}
    ${qesteidutil_HDRS}
    ${qesteidutil_MOC_SRCS}
    ${qesteidutil_RCC_SRCS}
    ${qesteidutil_MOC_HDRS}
    ${UI_HEADERS}
)

target_link_libraries(qesteidutil
    ${QT_QTMAIN_LIBRARY}
    ${QT_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    ${OPENSSLCRYPTO_LIBRARIES}
)

if(UNIX AND NOT APPLE)
        INSTALL(FILES QEstEIDUtil.desktop DESTINATION share/applications)
        INSTALL(FILES html/images/id_icon_48x48.png DESTINATION share/pixmaps )
endif(UNIX AND NOT APPLE)

install(TARGETS qesteidutil DESTINATION bin)
