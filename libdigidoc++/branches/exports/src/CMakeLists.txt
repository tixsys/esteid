include_directories( ${ICONV_INCLUDE_DIR} )
include_directories( ${LIBDIGIDOC_INCLUDE_DIR} )
include_directories( ${MINIZIP_INCLUDE_DIR} )
include_directories( ${OPENSSL_INCLUDE_DIR} )
include_directories( ${OPENSSLCRYPTO_INCLUDE_DIR} )
include_directories( ${ZLIB_INCLUDE_DIR} )
include_directories( ${LIBP11_INCLUDE_DIR} )
include_directories( ${XERCESC_INCLUDE_DIR} )
include_directories( ${XMLSECURITYC_INCLUDE_DIR} )
include_directories( ${XSD_INCLUDE_DIR} )

set(minizip_SRCS
    minizip/zip.c
    minizip/unzip.c
    minizip/ioapi.c
)

if(WIN32)
    set(minizip_SRCS
        ${minizip_SRCS}
        minizip/iowin32.c
    )
endif(WIN32)

set(digidocpp_SRCS
    BDoc.cpp
    BDocException.cpp
    DDoc.cpp
    log.cpp
    Conf.cpp
    XmlConf.cpp
    Document.cpp
    Exception.cpp
    Signature.cpp
    SignatureBES.cpp
    SignatureTM.cpp
    SignatureMobile.cpp
    SignatureException.cpp
    WDoc.cpp
    crypto/Digest.cpp
    crypto/cert/X509Cert.cpp
    crypto/cert/X509CertStore.cpp
    crypto/cert/DirectoryX509CertStore.cpp
    crypto/ocsp/OCSP.cpp
    crypto/ocsp/OCSPException.cpp
    crypto/crypt/RSACrypt.cpp
    crypto/signer/PKCS11Signer.cpp
    crypto/signer/EstEIDSigner.cpp
    crypto/signer/RSASigner.cpp
    crypto/signer/Signer.cpp
    crypto/signer/SignException.cpp
    io/IOException.cpp
    io/ISerialize.cpp
    io/ZipSerialize.cpp
    util/String.cpp
    util/File.cpp
    util/DateTime.cpp
    xml/conf_parser.cxx
    xml/OpenDocument_manifest.cxx
    xml/xmldsig-core-schema.cxx
    xml/XAdES.cxx
)

if(ICONV_SECOND_ARGUMENT_IS_CONST)
    add_definitions( -DICONV_SECOND_ARGUMENT_IS_CONST )
endif(ICONV_SECOND_ARGUMENT_IS_CONST)

if(CMAKE_COMPILER_IS_GNUCC)
    add_definitions( -Wno-format )
endif(CMAKE_COMPILER_IS_GNUCC)

if(MSVC)
    # disable warning C4290: C++ exception specification ignored
    add_definitions( -wd4290 )
    add_definitions(
        -D_CRT_SECURE_NO_DEPRECATE
        -D_CRT_SECURE_NO_WARNINGS
        -D_SCL_SECURE_NO_WARNINGS
    )
endif(MSVC)

# xCode and static linking hacks
if(APPLE)
	include_directories(/Library/OpenSC/include)
	include_directories(/usr/local/include/../include)
	find_library(CARBON_LIBRARY Carbon)
	find_library(COREFOUNDATION_LIBRARY CoreFoundation)
	find_library(LTDL_LIBRARY ltdl)
	mark_as_advanced(CARBON_LIBRARY COREFOUNDATION_LIBRARY LTDL_LIBRARY)
	set(EXTRA_LIBS ${CARBON_LIBRARY} ${COREFOUNDATION_LIBRARY} ${LTDL_LIBRARY})
endif(APPLE)

add_library(digidocpp SHARED ${digidocpp_SRCS} ${minizip_SRCS})
target_link_libraries(digidocpp
    ${ICONV_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    ${OPENSSLCRYPTO_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${LIBP11_LIBRARIES}
    ${XERCESC_LIBRARIES}
    ${XMLSECURITYC_LIBRARIES}
    ${EXTRA_LIBS}
)
set_target_properties(digidocpp PROPERTIES
                              VERSION 0.1.0
                              SOVERSION 0
                              OUTPUT_NAME digidoc++
                      )
install(TARGETS digidocpp DESTINATION lib)

add_executable(digidoc-demo main_demo.cpp)
target_link_libraries(digidoc-demo digidocpp)
install(TARGETS digidoc-demo DESTINATION bin)

add_executable(digidoc-tool bdoc_ref.cpp)
target_link_libraries(digidoc-tool digidocpp)

install(TARGETS digidoc-tool DESTINATION bin)

install( FILES
    ADoc.h
    BDocException.h
    BDoc.h
    Conf.h
    Document.h
    Exception.h
    Exports.h
    SignatureBES.h
    SignatureException.h
    Signature.h
    SignatureTM.h
    SignatureMobile.h
    WDoc.h
    XmlConf.h
    DESTINATION include/libdigidoc++
)

install( FILES
    crypto/Digest.h
    crypto/OpenSSLHelpers.h
    DESTINATION include/libdigidoc++/crypto
)

install( FILES
    crypto/cert/DirectoryX509CertStore.h
    crypto/cert/X509Cert.h
    crypto/cert/X509CertStore.h
    DESTINATION include/libdigidoc++/crypto/cert
)

install( FILES
    crypto/crypt/RSACrypt.h
    DESTINATION include/libdigidoc++/crypto/crypt
)

install( FILES
    crypto/ocsp/OCSPException.h
    crypto/ocsp/OCSP.h
    DESTINATION include/libdigidoc++/crypto/ocsp
)

install( FILES
    crypto/signer/EstEIDSigner.h
    crypto/signer/PKCS11Signer.h
    crypto/signer/RSASigner.h
    crypto/signer/Signer.h
    crypto/signer/SignException.h
    DESTINATION include/libdigidoc++/crypto/signer
)

install( FILES
    io/IOException.h
    io/ISerialize.h
    io/ZipSerialize.h
    DESTINATION include/libdigidoc++/io
)

install( FILES
    util/File.h
    util/String.h
    DESTINATION include/libdigidoc++/util
)

install( FILES
    xml/XAdES.hxx
    xml/xmldsig-core-schema.hxx
    DESTINATION include/libdigidoc++/xml
)
