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
    DEPENDS ${SCHEMA_DIR}/OpenDocument_manifest.xsd
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
    DEPENDS ${SCHEMA_DIR}/xmldsig-core-schema.xsd
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
    DEPENDS ${SCHEMA_DIR}/XAdES.xsd
)

add_custom_command(
    OUTPUT ${XML_DIR}/conf.cxx
    COMMAND ${CMAKE_COMMAND} -E make_directory ${XML_DIR}
    COMMAND ${XSD_EXECUTABLE} cxx-tree ${COMMON_XSD_OPTS}
                                       --generate-serialization
                                       conf.xsd
    WORKING_DIRECTORY ${SCHEMA_DIR}
    DEPENDS ${SCHEMA_DIR}/conf.xsd
)


if(NOT MINIZIP_FOUND)
    # Use bundled copy of minizip if not found in system
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
endif(NOT MINIZIP_FOUND)

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
    SignatureAttributes.cpp
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

if(WIN32)
    add_definitions( -DWIN32_LEAN_AND_MEAN )
endif(WIN32)

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
    ${MINIZIP_LIBRARIES}
    ${OPENSSLCRYPTO_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    ${XERCESC_LIBRARIES}
    ${XMLSECURITYC_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${EXTRA_LIBS}
)
if(UNIX)
    set(VISIBILITY_FLAG "-fvisibility=hidden")
endif(UNIX)

set_target_properties(digidocpp
    PROPERTIES
    VERSION ${DIGIDOCPP_VERSION}
    SOVERSION 0
    COMPILE_FLAGS "${VISIBILITY_FLAG}"
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
    Signature.h
    SignatureAttributes.h
    SignatureBES.h
    SignatureException.h
    SignatureMobile.h
    SignatureTM.h
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
    ${XML_DIR}/conf.hxx
    DESTINATION ${DIGIDOCPP_INCLUDE_DIR}/xml
)

## Generate Dynamic Language Wrappers
macro(DO_SWIG_MODULE language)
  string(TOUPPER "${language}" lang_ucase)
  message(STATUS "Building ${language} bindings")

  # Ugly hack to make target names differ so CMake will be happy
  set (module_name ${SWIG_MODULE_NAME}__${lang_ucase}__)
  set(MODULE_INCLUDE_DIRS
      ${${lang_ucase}_INCLUDE_DIRS}
      ${CMAKE_CURRENT_BINARY_DIR}/${language}
      ${CMAKE_CURRENT_SOURCE_DIR})
  # Another ugly hack to add includes only to module
  # sources not the entire build. We can't just use include_directories :(
  foreach(it ${MODULE_INCLUDE_DIRS})
      set(module_include_flags "${module_include_flags} -I${it}")
  endforeach(it)
  set_source_files_properties(${SWIG_FILES} PROPERTIES CPLUSPLUS ON)
  set(mod_outdir "${CMAKE_CURRENT_BINARY_DIR}/${language}")
  set(CMAKE_SWIG_OUTDIR "${mod_outdir}")
  SWIG_ADD_MODULE(${module_name} ${language} ${SWIG_FILES})
  SWIG_LINK_LIBRARIES(${module_name} ${${lang_ucase}_LIBRARIES} digidocpp)
  string(REPLACE "__${lang_ucase}__" "" mod_outname
         "${SWIG_MODULE_${module_name}_REAL_NAME}")
  set_target_properties(${SWIG_MODULE_${module_name}_REAL_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${mod_outdir}"
    COMPILE_FLAGS "${module_include_flags}"
    PREFIX ""
    OUTPUT_NAME "${mod_outname}")
  set(${lang_ucase}_WRAPPER ${CMAKE_CURRENT_BINARY_DIR}/${language}/${SWIG_MODULE_NAME}.${SWIG_${lang_ucase}_EXTRA_FILE_EXTENSION})
  list(APPEND wrappers_DEPS ${SWIG_MODULE_${module_name}_REAL_NAME})
  list(APPEND wrappers_SRCS ${${lang_ucase}_WRAPPER})
  install(TARGETS ${SWIG_MODULE_${module_name}_REAL_NAME} 
    LIBRARY DESTINATION "${${lang_ucase}_EXTENSION_DIR}")
endmacro(DO_SWIG_MODULE language)

if(SWIG_FOUND)
  include(${SWIG_USE_FILE})
  # TODO: Send this fix to upstream
  SET(SWIG_PERL_EXTRA_FILE_EXTENSION "pm")
  SET(SWIG_PHP_EXTRA_FILE_EXTENSION "php")

  set (SWIG_FILES        digidoc.i)
  set (SWIG_MODULE_NAME  "digidoc")

  # Python
  if(PYTHONLIBS_FOUND)
    # Pull in KDE crap for PYTHON_SITE_PACKAGES_DIR and PYTHON_INSTALL
    find_package(PythonLibrary QUIET)
    include(PythonMacros)
    set(PYTHON_EXTENSION_DIR "${PYTHON_SITE_PACKAGES_DIR}")

    # CMake 2.6
    if(NOT DEFINED PYTHON_INCLUDE_DIRS)
      set(PYTHON_INCLUDE_DIRS "${PYTHON_INCLUDE_PATH}")
    endif(NOT DEFINED PYTHON_INCLUDE_DIRS)

    DO_SWIG_MODULE(python)
    PYTHON_INSTALL(${PYTHON_WRAPPER} ${PYTHON_EXTENSION_DIR})
    # This is a braindead hack! PYTHON_INSTALL is just broken
    add_dependencies(compile_python_files swig-wrappers)
  endif(PYTHONLIBS_FOUND)

  # Perl
  if(PERLLIBS_FOUND)
    set(PERL_EXTENSION_DIR "${PERL_VENDORARCH}/auto")
    DO_SWIG_MODULE(perl)
    install(FILES ${PERL_WRAPPER} DESTINATION "${PERL_VENDORLIB}")
  endif(PERLLIBS_FOUND)

  # PHP
  if(PHPLIBS_FOUND)
    if(NOT PHP_WRAPPER_DIR)
      set(PHP_WRAPPER_DIR "${PHP_INSTALL_PREFIX}/share/php")
    endif(NOT PHP_WRAPPER_DIR)
    DO_SWIG_MODULE(php)
    set(PHP_INIFILE ${CMAKE_CURRENT_BINARY_DIR}/php/${SWIG_MODULE_NAME}.ini)
    file(WRITE ${PHP_INIFILE} "extension=${SWIG_MODULE_NAME}.so\n")
    install(FILES ${PHP_WRAPPER} DESTINATION "${PHP_WRAPPER_DIR}")
    install(FILES ${PHP_INIFILE} DESTINATION "${SYSCONF_INSTALL_DIR}/php.d")
  endif(PHPLIBS_FOUND)

  add_custom_target(swig-wrappers ALL
    DEPENDS ${wrappers_DEPS}
    SOURCES ${wrapper_SRCS})

endif(SWIG_FOUND)
