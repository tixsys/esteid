# - Try to find PCSC
# Once done this will define
#
#  PCSC_FOUND - system has PCSC
#  PCSC_INCLUDE_DIR - the PCSC include directory
#  PCSC_LIBRARIES - Link these to use PCSC
#  PCSC_DEFINITIONS - Compiler switches required for using PCSC

# Copyright (c) 2009, Kalev Lember, <kalev@smartlink.ee>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


if ( PCSC_INCLUDE_DIR AND PCSC_LIBRARIES )
   # in cache already
   SET(PCSC_FIND_QUIETLY TRUE)
endif ( PCSC_INCLUDE_DIR AND PCSC_LIBRARIES )

# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
if( NOT WIN32 )
  find_package(PkgConfig)

  pkg_check_modules(PC_PCSC libpcsclite)

  set(PCSC_DEFINITIONS ${PC_PCSC_CFLAGS_OTHER})
endif( NOT WIN32 )

FIND_PATH(PCSC_INCLUDE_DIR NAMES winscard.h
  PATHS
  /usr/include/PCSC
  /usr/local/include/PCSC
  ${PC_PCSC_INCLUDEDIR}
  ${PC_PCSC_INCLUDE_DIRS}
)

FIND_LIBRARY(PCSC_LIBRARIES NAMES pcsclite
  PATHS
  ${PC_PCSC_LIBDIR}
  ${PC_PCSC_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PCSC DEFAULT_MSG PCSC_INCLUDE_DIR PCSC_LIBRARIES )

# show the PCSC_INCLUDE_DIR and PCSC_LIBRARIES variables only in the advanced view
MARK_AS_ADVANCED(PCSC_INCLUDE_DIR PCSC_LIBRARIES )
