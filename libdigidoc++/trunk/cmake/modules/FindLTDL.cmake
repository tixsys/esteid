# - Find ltdl
# Find the native LTDL includes and library
#
#  LTDL_INCLUDE_DIR - where to find dlfcn.h, etc.
#  LTDL_LIBRARIES   - List of libraries when using ltdl.
#  LTDL_FOUND       - True if ltdl found.


IF (LTDL_INCLUDE_DIR)
  # Already in cache, be silent
  SET(LTDL_FIND_QUIETLY TRUE)
ENDIF (LTDL_INCLUDE_DIR)

FIND_PATH(LTDL_INCLUDE_DIR dlfcn.h)

SET(LTDL_NAMES dl ltdl)
FIND_LIBRARY(LTDL_LIBRARY NAMES ${LTDL_NAMES})

# handle the QUIETLY and REQUIRED arguments and set LTDL_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LTDL DEFAULT_MSG LTDL_LIBRARY LTDL_INCLUDE_DIR)

IF(LTDL_FOUND)
  SET( LTDL_LIBRARIES ${LTDL_LIBRARY} )
ELSE(LTDL_FOUND)
  SET( LTDL_LIBRARIES )
ENDIF(LTDL_FOUND)

MARK_AS_ADVANCED( LTDL_LIBRARY LTDL_INCLUDE_DIR )
