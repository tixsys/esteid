# - Find LibDigidDoc2
# Find the native LibDigidDoc2 includes and library
#
#  LIBDIGIDOC2_INCLUDE_DIR - where to find winscard.h, wintypes.h, etc.
#  LIBDIGIDOC2_LIBRARIES   - List of libraries when using LibDigidDoc2.
#  LIBDIGIDOC2_FOUND       - True if LibDigidDoc2 found.


IF (LIBDIGIDOC2_INCLUDE_DIR)
  # Already in cache, be silent
  SET(LIBDIGIDOC2_FIND_QUIETLY TRUE)
ENDIF (LIBDIGIDOC2_INCLUDE_DIR)

FIND_PATH(LIBDIGIDOC2_INCLUDE_DIR libdigidoc/DigiDocDefs.h
  /usr/include/libdigidoc2
  /usr/include
  /usr/local/include/libdigidoc2
  /usr/local/include
)
FIND_LIBRARY(LIBDIGIDOC2_LIBRARY NAMES digidoc2)

# handle the QUIETLY and REQUIRED arguments and set LIBDIGIDOC2_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibDigidDoc2 DEFAULT_MSG LIBDIGIDOC2_LIBRARY LIBDIGIDOC2_INCLUDE_DIR)

IF(LIBDIGIDOC2_FOUND)
  SET( LIBDIGIDOC2_LIBRARIES ${LIBDIGIDOC2_LIBRARY} )
ELSE(LIBDIGIDOC2_FOUND)
  SET( LIBDIGIDOC2_LIBRARIES )
ENDIF(LIBDIGIDOC2_FOUND)

MARK_AS_ADVANCED(LIBDIGIDOC2_LIBRARY LIBDIGIDOC2_INCLUDE_DIR)
