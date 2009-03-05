#------------------------------------------------------------------------------
# Desc:
# Tabs: 3
#
# $Id$
#------------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.6)
project(xpi)

# FIXME: Ugly son of a bitch!
set(CARDLIB ../../qesteidutil/trunk/cardlib)

set( CMAKE_VERBOSE_MAKEFILE TRUE)

# Define directories
# set( CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}/build)

# Build type
#if( CMAKE_BUILD_TYPE STREQUAL "Debug")
#	set( CMAKE_BUILD_TYPE Debug)
#	set( DEBUG_BUILD TRUE)
#endif( CMAKE_BUILD_TYPE STREQUAL "Debug")

# Set the project version
if( NOT PROJECT_VERSION)
	include( FindSubversion OPTIONAL)
	if( Subversion_FOUND)
		Subversion_WC_INFO( ${PROJECT_SOURCE_DIR} PROJECT)
	endif( Subversion_FOUND)
	if( NOT PROJECT_WC_REVISION)
		message( "Unable to determine subversion revision")
		set( PROJECT_WC_REVISION 999999)
	endif( NOT PROJECT_WC_REVISION)
	message( STATUS "Current subversion revision is ${PROJECT_WC_REVISION}")
	set( MAJOR 0)
	set( MINOR 1)
	set( REV ${PROJECT_WC_REVISION})
	set( PROJECT_VERSION "${MAJOR}.${MINOR}.${REV}")
endif( NOT PROJECT_VERSION)

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
)

set(CHROME_JAR "${PROJECT_BINARY_DIR}/chrome/esteid.jar")
add_custom_target( xpi-jar
	COMMAND ${RM} ${CHROME_JAR}
	COMMAND zip -pr ${CHROME_JAR} content/* skin/*
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/chrome
)

add_dependencies( xpi-jar xpi-stage)

add_custom_target( xpi ALL
	DEPENDS install.rdf
	DEPENDS esteid
	COMMAND ${RM} esteid-${PROJECT_VERSION}.xpi
	COMMAND zip -pr esteid-${PROJECT_VERSION}.xpi
                chrome/ components/ install.rdf
	VERBATIM
)
add_dependencies( xpi xpi-jar)

# XPCOM modules

# FIND Gecko SDK
INCLUDE(FindPkgConfig OPTIONAL)

IF(PKG_CONFIG_FOUND)
    INCLUDE(FindPkgConfig)
    set(pkgName libxul)

    pkg_check_modules(GECKO_SDK ${pkgName})
ENDIF(PKG_CONFIG_FOUND)

IF(NOT GECKO_SDK_FOUND)
	# FIXME: provide download location to clueless lozer
	message(FATAL_ERROR "Gecko SDK not found. Please download it.")
ENDIF(NOT GECKO_SDK_FOUND)

# Use a non-standard variable to find the real Gecko SDK directory
execute_process(
  COMMAND ${PKG_CONFIG_EXECUTABLE} "--variable=sdkdir" ${pkgName}
  OUTPUT_VARIABLE GECKO_SDK_DIR)
# Sanitize output
string(REGEX REPLACE "[\r\n]|^ +| +$" "" GECKO_SDK_DIR ${GECKO_SDK_DIR})

if(NOT GECKO_SDK_DIR)
	message(FATAL_ERROR "Unable to determine Gecko SDK path via pkg-config")
endif(NOT GECKO_SDK_DIR)

set(XPIDL ${GECKO_SDK_DIR}/bin/xpidl)
set(XPIDL_FLAGS -I${GECKO_SDK_DIR}/idl)

if(NOT EXISTS ${XPIDL})
	message(FATAL_ERROR "${GECKO_SDK_DIR}/bin/xpidl does not exist")
endif(NOT EXISTS ${XPIDL})

message( STATUS "Gecko SDK found in ${GECKO_SDK_DIR}")

add_definitions(${GECKO_SDK_CFLAGS})
link_directories(${GECKO_SDK_LIBRARY_DIRS})
include_directories(${GECKO_SDK_DIR}/include)
include_directories(${GECKO_SDK_INCLUDE_DIRS})

include_directories(${CARDLIB})

# Custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

if(UNIX)
    find_package(PCSC REQUIRED)
    include_directories(${PCSC_INCLUDE_DIR})
endif(UNIX)

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
    ${CMAKE_CURRENT_BINARY_DIR}/nsIEstEID.h

    ${CARDLIB}/CardBase.cpp
    ${CARDLIB}/CTAPIManager.cpp
    ${CARDLIB}/DynamicLibrary.cpp
    ${CARDLIB}/EstEidCard.cpp
    ${CARDLIB}/EstEidCardMaintainer.cpp
    ${CARDLIB}/PCSCManager.cpp
    ${CARDLIB}/SCError.cpp
    ${CARDLIB}/SIMCard.cpp
    ${CARDLIB}/SmartCardManager.cpp
)

set(XPCOM_CPP_FLAGS "-include xpcom-config.h -fno-rtti")
set(XPCOM_LINK_FLAGS "-Wl,--discard-all -Wl,-Bsymbolic -Wl,-z,defs -Wl,--version-script=${CMAKE_SOURCE_DIR}/components/xpcom.version_script")
# TODO: Find a better way to pass special CFLAGS to targets
add_definitions(${XPCOM_CPP_FLAGS})
add_library(esteid SHARED ${esteid_module_SRCS})
add_dependencies(esteid xpi-stage)
target_link_libraries(esteid ${GECKO_SDK_LIBRARIES})
set_target_properties(esteid PROPERTIES
				PREFIX ""
				LINK_FLAGS "${XPCOM_LINK_FLAGS}"
				SKIP_BUILD_RPATH true
				LIBRARY_OUTPUT_DIRECTORY "components")

# install(TARGETS pkcs11 DESTINATION lib)
