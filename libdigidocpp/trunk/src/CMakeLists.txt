include_directories(
    ${ICONV_INCLUDE_DIR}
    ${LIBDIGIDOC_INCLUDE_DIR}
    ${LIBDL_INCLUDE_DIR}
    ${LIBP11_INCLUDE_DIR}
    ${PKCS11H_INCLUDE_DIR}
    ${MINIZIP_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
    ${OPENSSLCRYPTO_INCLUDE_DIR}
    ${XERCESC_INCLUDE_DIR}
    ${XMLSECURITYC_INCLUDE_DIR}
    ${XSD_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIR}
)


set(SCHEMA_DIR ${CMAKE_SOURCE_DIR}/etc/schema)
set(XML_DIR ${CMAKE_CURRENT_BINARY_DIR}/xml)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(COMMON_XSD_OPTS
    --type-naming ucc
    --function-naming lcc
    --generate-doxygen
    --generate-intellisense
    --root-element-first
    --output-dir ${XML_DIR}
)

# generate manifest xml parser files
add_custom_command(
    OUTPUT ${XML_DIR}/OpenDocument_manifest.cxx
    COMMAND ${CMAKE_COMMAND} -E make_directory ${XML_DIR}
    COMMAND ${XSD_EXECUTABLE} cxx-tree ${COMMON_XSD_OPTS}
                                       --generate-serialization
                                       --namespace-map urn:oasis:names:tc:opendocument:xmlns:manifest:1.0=digidoc::manifest
                                       OpenDocument_manifest.xsd
    WORKING_DIRECTORY ${SCHEMA_DIR}
)

# generate XML DSig xml parser files
add_custom_command(
    OUTPUT ${XML_DIR}/xmldsig-core-schema.cxx
    COMMAND ${CMAKE_COMMAND} -E make_directory ${XML_DIR}
    COMMAND ${XSD_EXECUTABLE} cxx-tree ${COMMON_XSD_OPTS}
                                       --generate-serialization
                                       --namespace-map http://www.w3.org/2000/09/xmldsig\#=digidoc::dsig
                                       --namespace-map http://uri.etsi.org/01903/v1.3.2\#=digidoc::xades
                                       xmldsig-core-schema.xsd
    WORKING_DIRECTORY ${SCHEMA_DIR}
)

# generate XAdES xml parser files
add_custom_command(
    OUTPUT ${XML_DIR}/XAdES.cxx
    COMMAND ${CMAKE_COMMAND} -E make_directory ${XML_DIR}
    COMMAND ${XSD_EXECUTABLE} cxx-tree ${COMMON_XSD_OPTS}
                                       --generate-serialization
                                       --namespace-map http://www.w3.org/2000/09/xmldsig\#=digidoc::dsig
                                       --namespace-map http://uri.etsi.org/01903/v1.3.2\#=digidoc::xades
                                       XAdES.xsd
    WORKING_DIRECTORY ${SCHEMA_DIR}
)

add_custom_command(
    OUTPUT ${XML_DIR}/conf.cxx
    COMMAND ${CMAKE_COMMAND} -E make_directory ${XML_DIR}
    COMMAND ${XSD_EXECUTABLE} cxx-tree ${COMMON_XSD_OPTS}
                                       conf.xsd
    WORKING_DIRECTORY ${SCHEMA_DIR}
)


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
    ADoc.cpp
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
    ${XML_DIR}/conf.cxx
    ${XML_DIR}/OpenDocument_manifest.cxx
    ${XML_DIR}/xmldsig-core-schema.cxx
    ${XML_DIR}/XAdES.cxx
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
	mark_as_advanced(CARBON_LIBRARY COREFOUNDATION_LIBRARY)
	set(EXTRA_LIBS ${CARBON_LIBRARY} ${COREFOUNDATION_LIBRARY})
endif(APPLE)

add_library(digidocpp SHARED ${digidocpp_SRCS} ${minizip_SRCS})
target_link_libraries(digidocpp
    ${ICONV_LIBRARIES}
    ${LIBP11_LIBRARIES}
    ${PKCS11H_LIBRARIES}
    ${LIBDL_LIBRARIES}
    ${OPENSSLCRYPTO_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    ${XERCESC_LIBRARIES}
    ${XMLSECURITYC_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${EXTRA_LIBS}
)
set_target_properties(digidocpp
    PROPERTIES
    VERSION ${DIGIDOCPP_VERSION}
    SOVERSION 0
    COMPILE_FLAGS "-fvisibility=hidden"
    )
install(TARGETS digidocpp DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})

add_executable(digidoc-demo main_demo.cpp)
target_link_libraries(digidoc-demo digidocpp)

add_executable(digidoc-tool bdoc_ref.cpp)
target_link_libraries(digidoc-tool digidocpp)

install( FILES
    ADoc.h
    BDocException.h
    BDoc.h
    Conf.h
    DDoc.h
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
    DESTINATION ${DIGIDOCPP_INCLUDE_DIR}
)

install( FILES
    crypto/Digest.h
    crypto/OpenSSLHelpers.h
    DESTINATION ${DIGIDOCPP_INCLUDE_DIR}/crypto
)

install( FILES
    crypto/cert/DirectoryX509CertStore.h
    crypto/cert/X509Cert.h
    crypto/cert/X509CertStore.h
    DESTINATION ${DIGIDOCPP_INCLUDE_DIR}/crypto/cert
)

install( FILES
    crypto/crypt/RSACrypt.h
    DESTINATION ${DIGIDOCPP_INCLUDE_DIR}/crypto/crypt
)

install( FILES
    crypto/ocsp/OCSPException.h
    crypto/ocsp/OCSP.h
    DESTINATION ${DIGIDOCPP_INCLUDE_DIR}/crypto/ocsp
)

install( FILES
    crypto/signer/EstEIDSigner.h
    crypto/signer/PKCS11Signer.h
    crypto/signer/RSASigner.h
    crypto/signer/Signer.h
    crypto/signer/SignException.h
    DESTINATION ${DIGIDOCPP_INCLUDE_DIR}/crypto/signer
)

install( FILES
    io/IOException.h
    io/ISerialize.h
    io/ZipSerialize.h
    DESTINATION ${DIGIDOCPP_INCLUDE_DIR}/io
)

install( FILES
    util/String.h
    DESTINATION ${DIGIDOCPP_INCLUDE_DIR}/util
)

install( FILES
    ${XML_DIR}/XAdES.hxx
    ${XML_DIR}/xmldsig-core-schema.hxx
    DESTINATION ${DIGIDOCPP_INCLUDE_DIR}/xml
)
