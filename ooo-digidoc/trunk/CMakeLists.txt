cmake_minimum_required(VERSION 2.6)
project(OOoDigiDocPlugin)

# Custom cmake modules
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")

#set(CMAKE_BUILD_TYPE Debug)

# Staging directory which eventually gets compressed into an .oxt file
SET(OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/oxt")
# Directory for cppumaker-generated include files
set(OUT_INC "${CMAKE_CURRENT_BINARY_DIR}/inc")

set(CP ${CMAKE_COMMAND} -E copy)
set(RM ${CMAKE_COMMAND} -E remove)

find_package(OOoSDK REQUIRED)
find_package(LibDigiDocpp REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(OpenSSLCrypto REQUIRED)
find_package(ZLIB REQUIRED)
find_package(XercesC REQUIRED)
find_package(XSD REQUIRED)

if(NOT "${LIBDIGIDOCPP_CONF}" STREQUAL "LIBDIGIDOCPP_CONF-NOTFOUND")
    add_definitions( -DBDOCLIB_CONF_PATH="${LIBDIGIDOCPP_CONF}" )
endif(NOT "${LIBDIGIDOCPP_CONF}" STREQUAL "LIBDIGIDOCPP_CONF-NOTFOUND")

# Get SVN revision number
find_package(Subversion)
if(Subversion_FOUND AND EXISTS ${PROJECT_SOURCE_DIR}/.svn)
    Subversion_WC_INFO(${PROJECT_SOURCE_DIR} PROJECT)
    message(STATUS "Current SVN revision is ${PROJECT_WC_LAST_CHANGED_REV}")
    set(BUILD_VER ${PROJECT_WC_LAST_CHANGED_REV})
else(Subversion_FOUND AND EXISTS ${PROJECT_SOURCE_DIR}/.svn)
    set(BUILD_VER 0)
endif(Subversion_FOUND AND EXISTS ${PROJECT_SOURCE_DIR}/.svn)

set(PROJECT_VERSION "1.0.${BUILD_VER}")

if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    set(PROCTYPE "x86_64")
else(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    set(PROCTYPE "x86")
endif(${CMAKE_SIZEOF_VOID_P} EQUAL 8)

if(WIN32)
    set(UNOPKG_PLATFORM "Windows")
elseif(APPLE)
    set(UNOPKG_PLATFORM "MacOSX_${PROCTYPE}")
elseif(UNIX)
    set(UNOPKG_PLATFORM "Linux_${PROCTYPE}")
endif (WIN32)

if(WIN32)
    set(URE_TYPES "${OO_SDK_URE_HOME}/misc/types.rdb")
else(WIN32)
    set(URE_TYPES "${OO_SDK_URE_HOME}/share/misc/types.rdb")
endif(WIN32)

set(OFFICE_TYPES "${OFFICE_BASE_PROGRAM_PATH}/offapi.rdb")

add_custom_command(
    OUTPUT ${OUT_INC}/com/sun/star/lang/XMultiServiceFactory.hpp
    COMMAND ${CPPUMAKER} -Gc -BUCR -O${OUT_INC} ${URE_TYPES} ${OFFICE_TYPES}
)

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
)

link_directories(
    ${OO_SDK_URE_LIB_DIR}
    ${OO_SDK_HOME_LIB_DIR}
)

if(WIN32)
    set(UNO_INCLUDES
        \"${OOSDK_INCLUDE_DIR}\"
        \"${OOSDK_INCLUDE_DIR}/stl\"
        \"${OUT_INC}\")
else(WIN32)
    set(UNO_INCLUDES
        ${OOSDK_INCLUDE_DIR}
        ${OOSDK_INCLUDE_DIR}/stl
        ${OUT_INC})
endif(WIN32)

set(BDOC_INCLUDES
    ${OPENSSL_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIR}
    ${XERCESC_INCLUDE_DIR}
    ${XSD_INCLUDE_DIR}
    ${LIBDIGIDOCPP_INCLUDE_DIR}
)

# Set different include directories for files which include UNO headers
# and for files which include libdigidocpp headers.
foreach(iDir ${UNO_INCLUDES})
    set(UNO_INCLUDE_FLAGS "${UNO_INCLUDE_FLAGS} -I${iDir}")
endforeach(iDir)
foreach(iDir ${BDOC_INCLUDES})
    set(BDOC_INCLUDE_FLAGS "${BDOC_INCLUDE_FLAGS} -I${iDir}")
endforeach(iDir)

set(UNOFILES
    ${OUT_INC}/com/sun/star/lang/XMultiServiceFactory.hpp
    MyProtocolHandler.cxx
    MyJob.cxx
    MyListener.cxx
    WriterListener.cxx
    PresentationListener.cxx
    ListenerHelper.cxx
    exports.cxx
)

set(BDOCFILES
    MyBdocBridge.cpp
)

if(WIN32)
    set(UNO_DEFINITIONS "-DWIN32 -DWNT -DCPPU_ENV=msci")
elseif(UNIX)
    set(UNO_DEFINITIONS "-DUNX -DGCC -DCPPU_ENV=gcc3 -DGXX_INCLUDE_PATH=${SDK_GXX_INCLUDE_PATH}")

    if(APPLE)
        set(UNO_DEFINITIONS "${UNO_DEFINITIONS} -DMACOSX")
    else(APPLE)
        set(UNO_DEFINITIONS "${UNO_DEFINITIONS} -DLINUX")
    endif(APPLE)
endif(WIN32)

set_source_files_properties(${UNOFILES} PROPERTIES COMPILE_FLAGS "${UNO_DEFINITIONS} ${UNO_INCLUDE_FLAGS}")
set_source_files_properties(${BDOCFILES} PROPERTIES COMPILE_FLAGS "${BDOC_DEFINITIONS} ${BDOC_INCLUDE_FLAGS}")

if(WIN32)
    # Silence Visual Studio warnings
    add_definitions(
        -wd4251 -wd4275 -wd4290 -wd4675 -wd4786 -wd4800 -D_CRT_SECURE_NO_WARNINGS
        -DWIN32_LEAN_AND_MEAN
    )
endif(WIN32)

if(UNIX)
    set(OO_SDK_MAP "$ENV{OO_SDK_MAP_SUSE}")
    if(NOT OO_SDK_MAP)
        set(OO_SDK_MAP "${OO_SDK_HOME}/settings/component.uno.map")
    endif(NOT OO_SDK_MAP)
endif(UNIX)

if(WIN32)
    set(UNO_LINK_FLAGS "/DEF:\"${OO_SDK_HOME}/settings/component.uno.def\"")
elseif(APPLE)
    set(UNO_LINK_FLAGS "-Wl,-exported_symbols_list,${OO_SDK_HOME}/settings/component.uno.map")
elseif(UNIX)
    set(UNO_LINK_FLAGS "-Wl,-z,defs -Wl,--version-script,${OO_SDK_MAP}")
endif(WIN32)

add_library(ooo_digidoc SHARED ${UNOFILES} ${BDOCFILES})
set_target_properties(ooo_digidoc PROPERTIES
                                 PREFIX ""
                                 LINK_FLAGS "${UNO_LINK_FLAGS}"
                                 SKIP_BUILD_RPATH true
)

target_link_libraries(ooo_digidoc
    ${LIBDIGIDOCPP_LIBRARY}
    ${CPPU_LIBRARY}
    ${CPPUHELPER_LIBRARY}
    ${SAL_LIBRARY}
)

# Link with stlport if it was found
if(STLPORT_FOUND AND NOT APPLE)
    target_link_libraries(ooo_digidoc ${STLPORT_LIBRARY})

    # With stlport 5.2 we also need to link with pthread library.
    if(UNIX)
        target_link_libraries(ooo_digidoc pthread)
    endif(UNIX)
endif(STLPORT_FOUND AND NOT APPLE)


# copy_files: a macro for coping files from SRCDIR to DESTDIR
macro(copy_files GLOBPAT DESTINATION)
  file(GLOB COPY_FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${GLOBPAT})

  foreach(FILENAME ${COPY_FILES})
    set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}")
    set(DST "${DESTINATION}/${FILENAME}")

    add_custom_command(
      TARGET copy-files
      COMMAND ${CP} ${SRC} ${DST}
      )
  endforeach(FILENAME)
endmacro(copy_files)

# Full path of the library, including file name
get_target_property(OUT_LIB_FILEPATH ooo_digidoc LOCATION)
# File name (including extension) of the library
get_filename_component(OUT_LIB_FILENAME ${OUT_LIB_FILEPATH} NAME)

# copy_files macro defines commands which get associated with this target
add_custom_target(copy-files
    DEPENDS ooo_digidoc
    COMMENT "Copying files"
    COMMAND ${CP} ${OUT_LIB_FILEPATH} ${OUT_DIR}/${UNOPKG_PLATFORM}/${OUT_LIB_FILENAME}
)

configure_file(manifest.xml.in ${OUT_DIR}/META-INF/manifest.xml)
configure_file(description.xml.in ${OUT_DIR}/description.xml)

copy_files(*.png ${OUT_DIR})
copy_files(*.xcu ${OUT_DIR})
copy_files(HW/*.x* ${OUT_DIR})

add_custom_target(zip-oxt ALL
    DEPENDS ${OUT_DIR}/META-INF/manifest.xml
    DEPENDS ${OUT_DIR}/description.xml
    COMMENT "Compressing oxt file"

    WORKING_DIRECTORY ${OUT_DIR}
    COMMAND ${RM}  ${CMAKE_CURRENT_BINARY_DIR}/ooo-digidoc-${PROJECT_VERSION}.oxt
    COMMAND zip -r ${CMAKE_CURRENT_BINARY_DIR}/ooo-digidoc-${PROJECT_VERSION}.oxt .
    VERBATIM
)
add_dependencies(zip-oxt copy-files)


#--------------------------------------        
# Hack for Mac linked path problem #
if(APPLE)
    add_custom_target(hackmac
        COMMENT "Running hackmac"
        COMMAND install_name_tool -change @__________________________________________________URELIB/libuno_sal.dylib.3 /Applications/OpenOffice.org.app/Contents/basis-link/ure-link/lib/libuno_sal.dylib.3 ${OUT_LIB_FILEPATH}
        COMMAND install_name_tool -change @__________________________________________________URELIB/libuno_cppu.dylib.3 /Applications/OpenOffice.org.app/Contents/basis-link/ure-link/lib/libuno_cppu.dylib.3 ${OUT_LIB_FILEPATH}
        COMMAND install_name_tool -change @__________________________________________________URELIB/libuno_cppuhelpergcc3.dylib.3 /Applications/OpenOffice.org.app/Contents/basis-link/ure-link/lib/libuno_cppuhelpergcc3.dylib.3 ${OUT_LIB_FILEPATH}
        COMMAND install_name_tool -change libdigidocpp.dylib /usr/local/lib/libdigidocpp.dylib ${OUT_LIB_FILEPATH}
    )

    add_dependencies(copy-files hackmac)
endif(APPLE)
