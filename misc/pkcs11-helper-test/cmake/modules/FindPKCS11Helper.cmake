# - Find pkcs11-helper
# Find the native PKCS11HELPER includes and library
#
#  PKCS11HELPER_INCLUDE_DIR - where to find pkcs11.h, etc.
#  PKCS11HELPER_LIBRARIES   - List of libraries when using pkcs11-helper.
#  PKCS11HELPER_FOUND       - True if pkcs11-helper found.


IF (PKCS11HELPER_INCLUDE_DIR)
  # Already in cache, be silent
  SET(PKCS11HELPER_FIND_QUIETLY TRUE)
ENDIF (PKCS11HELPER_INCLUDE_DIR)

FIND_PATH(PKCS11HELPER_INCLUDE_DIR pkcs11-helper-1.0/pkcs11.h)

SET(PKCS11HELPER_NAMES pkcs11-helper libpkcs11-helper)
FIND_LIBRARY(PKCS11HELPER_LIBRARY NAMES ${PKCS11HELPER_NAMES})

# handle the QUIETLY and REQUIRED arguments and set PKCS11HELPER_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PKCS11Helper DEFAULT_MSG PKCS11HELPER_LIBRARY PKCS11HELPER_INCLUDE_DIR)

IF(PKCS11HELPER_FOUND)
  SET( PKCS11HELPER_LIBRARIES ${PKCS11HELPER_LIBRARY} )
ELSE(PKCS11HELPER_FOUND)
  SET( PKCS11HELPER_LIBRARIES )
ENDIF(PKCS11HELPER_FOUND)

MARK_AS_ADVANCED( PKCS11HELPER_LIBRARY PKCS11HELPER_INCLUDE_DIR )
