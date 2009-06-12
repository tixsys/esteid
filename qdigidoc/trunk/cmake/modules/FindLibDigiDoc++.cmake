# - Find LibDigiDoc++
# Find the native LibDigiDoc++ includes and library
#
#  LIBDIGIDOC++_INCLUDE_DIR - where to find winscard.h, wintypes.h, etc.
#  LIBDIFIDOC_CONF        - where is bdoclib.conf file
#  LIBDIGIDOC++_LIBRARIES   - List of libraries when using LibDigiDoc++.
#  LIBDIGIDOC++_FOUND       - True if LibDigiDoc++ found.


IF (LIBDIGIDOC++_INCLUDE_DIR)
  # Already in cache, be silent
  SET(LIBDIGIDOC++_FIND_QUIETLY TRUE)
ENDIF (LIBDIGIDOC++_INCLUDE_DIR)

FIND_PATH(LIBDIGIDOC++_INCLUDE_DIR libdigidoc++/BDoc.h /usr/include /usr/local/include)
FIND_FILE(LIBDIGIDOC++_CONF libdigidoc++.conf /etc/libdigidoc++)
FIND_LIBRARY(LIBDIGIDOC++_LIBRARY NAMES digidoc++)

# handle the QUIETLY and REQUIRED arguments and set LIBDIGIDOC++_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibDigiDoc++ DEFAULT_MSG LIBDIGIDOC++_LIBRARY LIBDIGIDOC++_INCLUDE_DIR)

IF(LIBDIGIDOC++_FOUND)
  SET( LIBDIGIDOC++_LIBRARIES ${LIBDIGIDOC++_LIBRARY} )
ELSE(LIBDIGIDOC++_FOUND)
  SET( LIBDIGIDOC++_LIBRARIES )
ENDIF(LIBDIGIDOC++_FOUND)

MARK_AS_ADVANCED(LIBDIGIDOC++_LIBRARY LIBDIGIDOC++_INCLUDE_DIR LIBDIGIDOC++_CONF)
