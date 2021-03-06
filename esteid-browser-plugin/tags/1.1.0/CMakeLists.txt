set(ESTEID_BROWSER_PLUGIN_VERSION "1.1.0")

# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

# Options which affect the way the project is built
set(ENABLE_DEBUG FALSE
    CACHE BOOL "Set to TRUE to enable debug print statements (default: FALSE)"
)
option(ENABLE_OLD_APIS "Support OLD API-s (default: TRUE)" TRUE)

find_package(Subversion)
if(Subversion_FOUND AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.svn)
    Subversion_WC_INFO(${CMAKE_CURRENT_SOURCE_DIR} PROJECT)
    set(SVN_REV ${PROJECT_WC_LAST_CHANGED_REV})
    message(STATUS "Current subversion revision is ${SVN_REV}")
else()
    message( "Unable to determine subversion revision")
endif()

include(${CMAKE_DIR}/common.cmake)

set (PROJNAME ${PLUGIN_NAME})

browserplugin_project(${PROJNAME})

# Custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
include(InstallSettings)

find_package(Iconv REQUIRED)
find_package(OpenSSLCrypto REQUIRED)
find_package(SmartCardpp REQUIRED)

if(NOT WITH_SYSTEM_BOOST)
    message(FATAL_ERROR "Firebreath needs to be built with WITH_SYSTEM_BOOST enabled")
endif()

if(WIN32)
    # FindBoost.cmake isn't up to date wrt new boost versions
    set(Boost_ADDITIONAL_VERSIONS "1.44" "1.44.0" "1.43" "1.43.0" "1.42" "1.42.0")
    # boost::signals is only needed on Windows
    set(EXTRA_BOOST_COMPONENTS "signals")
endif(WIN32)
find_package(Boost COMPONENTS filesystem system thread ${EXTRA_BOOST_COMPONENTS} REQUIRED)

if(UNIX AND NOT APPLE)
    if(NOT GTKMM_INCLUDE_DIRS)
        pkg_check_modules(GTKMM REQUIRED gtkmm-2.4)
    endif()
endif(UNIX AND NOT APPLE)

# Utility classes (FIXME: Move those to smartcardpp)
set(UTIL_DIR utility)
set(UTILITY_SOURCES
    ${UTIL_DIR}/threadObj.cpp
    ${UTIL_DIR}/converters.cpp
    ${UTIL_DIR}/logger.cpp
)

# Pthread is needed to link utility classes on UNIX-like OS-es
if(UNIX)
  link_libraries (pthread)
endif(UNIX)

if(ICONV_SECOND_ARGUMENT_IS_CONST)
    add_definitions( -DICONV_SECOND_ARGUMENT_IS_CONST )
endif(ICONV_SECOND_ARGUMENT_IS_CONST)

if(ENABLE_DEBUG)
    message(STATUS "Debug print statements are enabled")
    add_definitions(-DDEBUG)
endif(ENABLE_DEBUG)

if(ENABLE_OLD_APIS)
    message(STATUS "Support for OLD API-s is enabled")
    add_definitions(-DSUPPORT_OLD_APIS)
endif(ENABLE_OLD_APIS)


include_directories(
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GTKMM_INCLUDE_DIRS}
    ${ICONV_INCLUDE_DIR}
    ${OPENSSLCRYPTO_INCLUDE_DIR}
    ${SMARTCARDPP_INCLUDE_DIR}
    ${UTIL_DIR}
)

if( APPLE )
    include_directories( /usr/local/include/../include )
endif( APPLE )

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp
    *.h
    *.cmake
    )

# Generated files are stored in ${GENERATED} by the project configuration
SET_SOURCE_FILES_PROPERTIES(
    ${GENERATED}
    PROPERTIES
        GENERATED 1
    )

SOURCE_GROUP(Generated FILES
    ${GENERATED}
    )

SET( SOURCES
    ${GENERAL}
    ${GENERATED}
    ${UTILITY_SOURCES}
    )

# This will include Win/projectDef.cmake, Linux/projectDef.cmake, etc
include_platform()

add_subdirectory(Mozilla)
