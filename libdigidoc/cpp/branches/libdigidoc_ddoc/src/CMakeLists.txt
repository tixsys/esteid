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

set(libdigidoc_SRCS
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

# FIXME: to compile libdigidoc as a shared dll, we need to either
# create a .def file or use __declspec(dllexport) export directives.
if(WIN32)
    set(DIGIDOC_LIB_TYPE STATIC)
else(WIN32)
    set(DIGIDOC_LIB_TYPE SHARED)
endif(WIN32)

add_library(digidoc ${DIGIDOC_LIB_TYPE} ${libdigidoc_SRCS} ${minizip_SRCS})
target_link_libraries(digidoc
    ${EXTERNAL_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    ${OPENSSLCRYPTO_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${LIBP11_LIBRARIES}
    ${XERCESC_LIBRARIES}
    ${XMLSECURITYC_LIBRARIES}
)
set_target_properties(digidoc PROPERTIES
                              VERSION 0.0.8
                              SOVERSION 0
                      )
install(TARGETS digidoc DESTINATION lib)

add_executable(digidoc-demo main_demo.cpp)
target_link_libraries(digidoc-demo digidoc)
install(TARGETS digidoc-demo DESTINATION bin)

add_executable(digidoc-tool bdoc_ref.cpp)
target_link_libraries(digidoc-tool digidoc)

install(TARGETS digidoc-tool DESTINATION bin)

install( FILES
    BDocException.h
    BDoc.h
    Conf.h
    Document.h
    Exception.h
    SignatureBES.h
    SignatureException.h
    Signature.h
    SignatureTM.h
    XmlConf.h
    DESTINATION include/digidoc
)

install( FILES
    crypto/Digest.h
    crypto/OpenSSLHelpers.h
    DESTINATION include/digidoc/crypto
)

install( FILES
    crypto/cert/DirectoryX509CertStore.h
    crypto/cert/X509Cert.h
    crypto/cert/X509CertStore.h
    DESTINATION include/digidoc/crypto/cert
)

install( FILES
    crypto/crypt/RSACrypt.h
    DESTINATION include/digidoc/crypto/crypt
)

install( FILES
    crypto/ocsp/OCSPException.h
    crypto/ocsp/OCSP.h
    DESTINATION include/digidoc/crypto/ocsp
)

install( FILES
    crypto/signer/EstEIDSigner.h
    crypto/signer/PKCS11Signer.h
    crypto/signer/RSASigner.h
    crypto/signer/Signer.h
    crypto/signer/SignException.h
    DESTINATION include/digidoc/crypto/signer
)

install( FILES
    io/IOException.h
    io/ISerialize.h
    io/ZipSerialize.h
    DESTINATION include/digidoc/io
)

install( FILES
    minizip/unzip.h
    minizip/ioapi.h
    DESTINATION include/digidoc/minizip
)


install( FILES
    util/File.h
    util/String.h
    DESTINATION include/digidoc/util
)

install( FILES
    xml/OpenDocument_manifest.hxx
    xml/XAdES.hxx
    xml/xmldsig-core-schema.hxx
    DESTINATION include/digidoc/xml
)
