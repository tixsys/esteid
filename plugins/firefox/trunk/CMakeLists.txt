#------------------------------------------------------------------------------
# Desc:
# Tabs: 3
#
# $Id$
#------------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.6)
project(xpi)

# set( CMAKE_VERBOSE_MAKEFILE TRUE)

# Define directories
# set( CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}/build)
set(PLUGINGATE plugins/plugingate)
set(CARDLIB ${CMAKE_SOURCE_DIR}/smartcard++)
include_directories(
    ${CARDLIB}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set project version
set( MAJOR 0)
set( MINOR 4)
set( PATCH 0)

find_package(Subversion)
if(Subversion_FOUND AND EXISTS ${PROJECT_SOURCE_DIR}/.svn)
    Subversion_WC_INFO(${PROJECT_SOURCE_DIR} PROJECT)
    message(STATUS "Current subversion revision is ${PROJECT_WC_LAST_CHANGED_REV}")
    set(PATCH ${PROJECT_WC_LAST_CHANGED_REV})
else(Subversion_FOUND AND EXISTS ${PROJECT_SOURCE_DIR}/.svn)
    message( "Unable to determine subversion revision")
endif(Subversion_FOUND AND EXISTS ${PROJECT_SOURCE_DIR}/.svn)

set(PROJECT_VERSION "${MAJOR}.${MINOR}.${PATCH}")

message( STATUS "Current project version is ${PROJECT_VERSION}")
add_definitions( -DESTEID_MOZPLUG_VERSION="${PROJECT_VERSION}")

# Make life easier :P
set(MKDIR ${CMAKE_COMMAND} -E make_directory)
set(RM ${CMAKE_COMMAND} -E remove)
set(CP ${CMAKE_COMMAND} -E copy)

CONFIGURE_FILE(install.rdf.in install.rdf)

# Build the XPI
add_custom_target( xpi-stage
	COMMAND ${MKDIR} chrome
	COMMAND ${MKDIR} components
	COMMAND ${MKDIR} plugins
)

set(CHROME_JAR "${PROJECT_BINARY_DIR}/chrome/esteid.jar")
add_custom_target( xpi-jar
	COMMAND ${RM} ${CHROME_JAR}
	COMMAND zip -pr ${CHROME_JAR} content/* locale/* skin/* -x '*/.svn/*'
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/chrome
)

add_dependencies( xpi-jar xpi-stage)

add_custom_target( xpi ALL
	DEPENDS esteid
	DEPENDS npesteid
	DEPENDS ${PROJECT_BINARY_DIR}/components/nsEstEIDPrivate.js
	DEPENDS ${PROJECT_BINARY_DIR}/install.rdf
	DEPENDS chrome.manifest
	COMMAND ${RM} chrome.manifest
	COMMAND ${CP} ${PROJECT_SOURCE_DIR}/chrome.manifest .
	COMMAND ${RM} esteid-${PROJECT_VERSION}.xpi
	COMMAND zip -pr esteid-${PROJECT_VERSION}.xpi
                chrome/ components/ plugins/ install.rdf chrome.manifest
	VERBATIM
)
add_dependencies( xpi xpi-jar)
add_dependencies( xpi jsesteid)

add_custom_target(runff
	DEPENDS run-in-ff.sh
	COMMAND ${PROJECT_SOURCE_DIR}/run-in-ff.sh esteid-${PROJECT_VERSION}.xpi
)
add_dependencies(runff xpi)

add_custom_target(runtb
	DEPENDS run-in-tb.sh
	COMMAND ${PROJECT_SOURCE_DIR}/run-in-tb.sh esteid-${PROJECT_VERSION}.xpi
)
add_dependencies(runtb xpi)

# FIND Gecko SDK
INCLUDE(FindPkgConfig OPTIONAL)

IF(PKG_CONFIG_FOUND)
    # First try to find Gecko SDK using pkg-config
    set(pkgName libxul)

    pkg_check_modules(GECKO_SDK ${pkgName})
    IF(GECKO_SDK_FOUND)
        # Use a non-standard variable to find the real Gecko SDK directory
        execute_process(
          COMMAND ${PKG_CONFIG_EXECUTABLE} "--variable=sdkdir" ${pkgName}
          OUTPUT_VARIABLE GECKO_SDK_DIR)
        # Sanitize output
        string(REGEX REPLACE "[\r\n]|^ +| +$" "" GECKO_SDK_DIR
						${GECKO_SDK_DIR})
        if(NOT GECKO_SDK_DIR)
	        message(FATAL_ERROR "Unable to determine Gecko SDK path via pkg-config")
        endif(NOT GECKO_SDK_DIR)
    ENDIF(GECKO_SDK_FOUND)
ENDIF(PKG_CONFIG_FOUND)

if(NOT GECKO_SDK_FOUND)
    # If locating Gecko SDK using pkg-config failed, try some hardcoded paths
    if(NOT MSVC)
	set(${THREAD_LIBS} pthread)
    endif(NOT MSVC)
    set(NSPR_LIBRARIES plds4 plc4 nspr4 ${THREAD_LIBS} ${CMAKE_DL_LIBS})
    set(GECKO_LIBRARIES xpcomglue_s xpcom)
    if(MSVC)
        set(GECKO_LIBRARIES ${GECKO_LIBRARIES} xul)
    endif(MSVC)
    set(GECKO_SDK_LIBRARIES ${GECKO_LIBRARIES} ${NSPR_LIBRARIES} )
    FIND_PATH(GECKO_SDK_DIR include/xpcom/nsCOMPtr.h
        PATHS
	    /build/Win32/xulrunner-sdk
	    ${PROJECT_SOURCE_DIR}/xulrunner-sdk
	    ${PROJECT_SOURCE_DIR}/../xulrunner-sdk
	    ${PROJECT_BINARY_DIR}/xulrunner-sdk
	    ${PROJECT_BINARY_DIR}/../xulrunner-sdk
    )
    if(GECKO_SDK_DIR)
        set(GECKO_SDK_FOUND 1)
    endif(GECKO_SDK_DIR)
endif(NOT GECKO_SDK_FOUND)

if(NOT GECKO_SDK_FOUND)
    message(FATAL_ERROR "Could not find Gecko SDK.")
endif(NOT GECKO_SDK_FOUND)

set(XPIDL ${GECKO_SDK_DIR}/bin/xpidl)
set(XPIDL_FLAGS -I${GECKO_SDK_DIR}/idl -I${CMAKE_SOURCE_DIR}/components)

message( STATUS "Gecko SDK found in ${GECKO_SDK_DIR}")

add_definitions(${GECKO_SDK_CFLAGS})
link_directories(${GECKO_SDK_DIR}/sdk/lib)
link_directories(${GECKO_SDK_LIBRARY_DIRS})

set(GECKO_SDK_INCLUDE_DIRS
    ${GECKO_SDK_DIR}/include
    ${GECKO_SDK_DIR}/include/xpcom
    ${GECKO_SDK_DIR}/include/dom
    ${GECKO_SDK_DIR}/include/caps
    ${GECKO_SDK_DIR}/include/necko
    ${GECKO_SDK_DIR}/sdk/include
)
include_directories(${GECKO_SDK_INCLUDE_DIRS})

# Xulrunner 1.9.1 has npfunctions.h instead of npupp.h
find_file(HAVE_NPFUNCTIONS_H npfunctions.h ${GECKO_SDK_INCLUDE_DIRS})
configure_file(esteid-config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/esteid-config.h)

if(WIN32)
    add_definitions(-DXP_WIN -D_USRDLL)
else(WIN32)
    add_definitions(-DXP_UNIX)
    if(APPLE)
        add_definitions(-DXP_MACOSX)
    endif(APPLE)
endif(WIN32)

# XPCOM modules must be built with 2 byte wchar,
# no RTTI and xpcom-config.h included from every source file
# In addition, when compiling with MSVC, we link VC Runtime statically.
# TODO: generate version_script
if(MSVC)
    # Build with static MSVC runtime
    foreach(flag_var
            CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        if(${flag_var} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif(${flag_var} MATCHES "/MD")
    endforeach(flag_var)

    set(XPCOM_CPP_FLAGS "/FI xpcom-config.h /Zc:wchar_t-")
    set(XPCOM_LINK_FLAGS )
    set(NPAPI_LINK_FLAGS "/def:${CMAKE_SOURCE_DIR}/plugins/npesteid.def")
elseif(UNIX)
    set(XPCOM_CPP_FLAGS "-include xpcom-config.h -fno-rtti -fshort-wchar")
    if(NOT APPLE)
        set(LIB_LINK_FLAGS "-Wl,--discard-all -Wl,-Bsymbolic -Wl,-z,defs")
        set(XPCOM_LINK_FLAGS "${LIB_LINK_FLAGS} -Wl,--version-script=${CMAKE_SOURCE_DIR}/components/xpcom.version_script")
        set(NPAPI_LINK_FLAGS "${LIB_LINK_FLAGS} -Wl,--version-script=${CMAKE_SOURCE_DIR}/plugins/npapi.version_script")
    endif(NOT APPLE)
else(MSVC)
    message(FATAL_ERROR "Unsupported platform.")
endif(MSVC)

# Custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

if(UNIX AND NOT APPLE)
    find_package(PCSCLite REQUIRED)
    include_directories(${PCSCLITE_INCLUDE_DIR})
endif(UNIX AND NOT APPLE)

# Generate build targets for IDL files in our project
include_directories(${CMAKE_CURRENT_BINARY_DIR})
FILE(GLOB idlFiles RELATIVE ${PROJECT_SOURCE_DIR}/components components/*.idl)

FOREACH(iFile ${idlFiles})
    string(REGEX REPLACE "[.]idl$" ".h" hFile
			${CMAKE_CURRENT_BINARY_DIR}/${iFile})
    string(REGEX REPLACE "[.]idl$" ".xpt" xFile
			${CMAKE_CURRENT_BINARY_DIR}/components/${iFile})
    set(iFile ${PROJECT_SOURCE_DIR}/components/${iFile})

    add_custom_command(
        OUTPUT ${xFile}
        COMMAND ${XPIDL} ${XPIDL_FLAGS} -m typelib -e ${xFile} ${iFile}
        DEPENDS ${iFile}
    )
    add_custom_command(
        OUTPUT ${hFile}
        COMMAND ${XPIDL} ${XPIDL_FLAGS} -m header -e ${hFile} ${iFile}
        DEPENDS ${iFile}
        DEPENDS ${xFile}
    )
ENDFOREACH(iFile)

# Sources:
set(esteid_module_SRCS
    components/EstEIDModule.cpp
    components/nsEstEID.cpp
    components/EstEIDServiceBase.cpp
    components/nsEstEIDService.cpp
    components/nsEstEIDCertificate.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/nsIEstEID.h
    ${CMAKE_CURRENT_BINARY_DIR}/nsIEstEIDPrivate.h

    components/converters.cpp
    ${CARDLIB}/CardBase.cpp
    ${CARDLIB}/CTAPIManager.cpp
    ${CARDLIB}/DynamicLibrary.cpp
    ${CARDLIB}/esteid/EstEidCard.cpp
    ${CARDLIB}/esteid/EstEidCardMaintainer.cpp
    ${CARDLIB}/PCSCManager.cpp
    ${CARDLIB}/SCError.cpp
#    ${CARDLIB}/SIMCard.cpp
    ${CARDLIB}/SmartCardManager.cpp
    ${CARDLIB}/locked_allocator.cpp
)

set(esteid_plugin_SRCS
    plugins/plugin.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/nsIEstEID.h
    ${CMAKE_CURRENT_BINARY_DIR}/nsIEstEIDPrivate.h

    ${PLUGINGATE}/np_entry.cpp
    ${PLUGINGATE}/npn_gate.cpp
    ${PLUGINGATE}/npp_gate.cpp
)

set(esteid_plugin_WIN
    plugins/npesteid.rc
    plugins/npesteid.def
)

# TODO: A more generic way to list XPCOM components written in JS
set(COMP_DEST "${CMAKE_CURRENT_BINARY_DIR}/components")
add_custom_command(
    OUTPUT ${COMP_DEST}/nsEstEIDPrivate.js
    COMMAND ${CP} nsEstEIDPrivate.js ${COMP_DEST}
    DEPENDS ${COMP_DEST}/nsIEstEIDPrivate.xpt
    DEPENDS components/nsEstEIDPrivate.js
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/components
)

# TODO: Find a better way to pass special CFLAGS to targets
# add_definitions(${XPCOM_CPP_FLAGS})

set_source_files_properties(${esteid_module_SRCS} ${esteid_plugin_SRCS}
  PROPERTIES
  COMPILE_FLAGS "${XPCOM_CPP_FLAGS}")

if(WIN32)
    set(WIN_LIBRARIES ws2_32)
endif(WIN32)

add_library(esteid SHARED ${esteid_module_SRCS})
add_library(npesteid SHARED ${esteid_plugin_SRCS} ${esteid_plugin_WIN})
add_dependencies(esteid xpi-stage)
add_dependencies(npesteid xpi-stage)
target_link_libraries(esteid ${GECKO_SDK_LIBRARIES} ${WIN_LIBRARIES})
target_link_libraries(npesteid ${GECKO_SDK_LIBRARIES})
set_target_properties(esteid PROPERTIES
				PREFIX ""
				LINK_FLAGS "${XPCOM_LINK_FLAGS}"
				SKIP_BUILD_RPATH true
				LIBRARY_OUTPUT_DIRECTORY "components")
set_target_properties(npesteid PROPERTIES
				PREFIX ""
				LINK_FLAGS "${NPAPI_LINK_FLAGS}"
				SKIP_BUILD_RPATH true
				LIBRARY_OUTPUT_DIRECTORY "plugins")

# install(TARGETS pkcs11 DESTINATION lib)
