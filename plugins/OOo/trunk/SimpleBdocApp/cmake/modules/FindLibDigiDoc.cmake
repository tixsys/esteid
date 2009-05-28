# - Find LibDigiDoc
# Find the native LibDigiDoc includes and library
#
#  LIBDIGIDOC_INCLUDE_DIR - where to find winscard.h, wintypes.h, etc.
#  LIBDIFIDOC_CONF        - where is bdoclib.conf file
#  LIBDIGIDOC_LIBRARIES   - List of libraries when using LibDigiDoc.
#  LIBDIGIDOC_FOUND       - True if LibDigiDoc found.


IF (LIBDIGIDOC_INCLUDE_DIR)
  # Already in cache, be silent
  SET(LIBDIGIDOC_FIND_QUIETLY TRUE)
ENDIF (LIBDIGIDOC_INCLUDE_DIR)

FIND_PATH(LIBDIGIDOC_INCLUDE_DIR digidoc/BDoc.h /usr/include /usr/local/include)
FIND_FILE(LIBDIGIDOC_CONF bdoclib.conf /etc/libdigidoc)
FIND_LIBRARY(LIBDIGIDOC_LIBRARY NAMES digidoc)

# handle the QUIETLY and REQUIRED arguments and set LIBDIGIDOC_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibDigiDoc DEFAULT_MSG LIBDIGIDOC_LIBRARY LIBDIGIDOC_INCLUDE_DIR)

IF(LIBDIGIDOC_FOUND)
  SET( LIBDIGIDOC_LIBRARIES ${LIBDIGIDOC_LIBRARY} )
ELSE(LIBDIGIDOC_FOUND)
  SET( LIBDIGIDOC_LIBRARIES )
ENDIF(LIBDIGIDOC_FOUND)

MARK_AS_ADVANCED(LIBDIGIDOC_LIBRARY LIBDIGIDOC_INCLUDE_DIR LIBDIGIDOC_CONF)

