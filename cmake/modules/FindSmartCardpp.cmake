# - Find smartcardpp
# Find the native SMARTCARDPP includes and library
#
#  SMARTCARDPP_INCLUDE_DIR - where to find smartcardpp.h, etc.
#  SMARTCARDPP_LIBRARIES   - List of libraries when using smartcardpp.
#  SMARTCARDPP_FOUND       - True if smartcardpp found.


IF (SMARTCARDPP_INCLUDE_DIR)
  # Already in cache, be silent
  SET(SMARTCARDPP_FIND_QUIETLY TRUE)
ENDIF (SMARTCARDPP_INCLUDE_DIR)

FIND_PATH(SMARTCARDPP_INCLUDE_DIR smartcardpp.h
  PATHS
  /usr/local/include/smartcardpp
  /usr/include/smartcardpp
)

SET(SMARTCARDPP_NAMES smartcardpp)
FIND_LIBRARY(SMARTCARDPP_LIBRARY NAMES ${SMARTCARDPP_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set SMARTCARDPP_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SmartCardpp DEFAULT_MSG SMARTCARDPP_LIBRARY SMARTCARDPP_INCLUDE_DIR)

IF(SMARTCARDPP_FOUND)
  SET( SMARTCARDPP_LIBRARIES ${SMARTCARDPP_LIBRARY} )
ELSE(SMARTCARDPP_FOUND)
  SET( SMARTCARDPP_LIBRARIES )
ENDIF(SMARTCARDPP_FOUND)

MARK_AS_ADVANCED( SMARTCARDPP_LIBRARY SMARTCARDPP_INCLUDE_DIR )
