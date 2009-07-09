set( MOC_HEADERS
        CertificateWidget.h
        ComboBox.h
        Common.h
        IKValidator.h
        PinDialog.h
        Settings.h
	sslConnect.h
        )
QT4_WRAP_UI( UI_HEADERS
	ui/CertificateWidget.ui
	)
set( HEADERS
        ${MOC_HEADERS}
	${UI_HEADERS}
        SslCertificate.h
        )
QT4_WRAP_CPP( MOC_SOURCES ${MOC_HEADERS} )
set( SOURCES
        ${HEADERS}
	${MOC_SOURCES}
        CertificateWidget.cpp
        ComboBox.cpp
        Common.cpp
        IKValidator.cpp
        PinDialog.cpp
        SslCertificate.cpp
	sslConnect.cpp
        )

INCLUDE_DIRECTORIES(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
	${OPENSSL_INCLUDE_DIR}
	)

ADD_LIBRARY(
        qdigidoccommon
        STATIC
        ${SOURCES}
        )
