# - Find OOo SDK
# Find OOSDK includes and executable
#
#  OOSDK_INCLUDE_DIR - Where to find OOSDK include sub-directory.
#  OOSDK_EXECUTABLE  - OOSDK compiler.
#  OOSDK_FOUND       - True if OOSDK found.


IF (OOSDK_INCLUDE_DIR)
  # Already in cache, be silent.
  SET(OOSDK_FIND_QUIETLY TRUE)
ENDIF (OOSDK_INCLUDE_DIR)

#FIND_PATH(OOSDK_INCLUDE_DIR cppuhelper/bootstrap.hxx "${OOO_SDK_DIR}/include")
#SET(OOSDK_NAMES libsofficeapp)
#FIND_LIBRARY(OOSDK_LIBRARY NAMES ${OOSDK_NAMES} PATHS ${OFFICE_BASE_PROGRAM_PATH})
SET(OOSDK_LIBRARY "${OFFICE_BASE_PROGRAM_PATH}/libsofficeapp.so")
 
#SET(OOSDK_SEARCH_DIR "${OOO_SDK_DIR}/lib")
SET(OOSDK_SEARCH_DIR "${OO_SDK_URE_HOME}/lib")

message( STATUS "searching libraries in ${OOSDK_SEARCH_DIR}")

SET(CPPU_NAMES icppu libuno_cppu cppu)
FIND_LIBRARY(CPPU_LIBRARY NAMES ${CPPU_NAMES} PATHS ${OOSDK_SEARCH_DIR})

SET(CPPUHELPER_NAMES libuno_cppuhelpergcc3 icppuhelper cppuhelper)
FIND_LIBRARY(CPPUHELPER_LIBRARY NAMES ${CPPUHELPER_NAMES} PATHS ${OOSDK_SEARCH_DIR})

SET(REG_NAMES ireg libreg reg)
FIND_LIBRARY(REG_LIBRARY NAMES ${REG_NAMES} PATHS ${OOSDK_SEARCH_DIR})

SET(RMCXT_NAMES irmcxt librmcxt.so.3 rmcxt)
FIND_LIBRARY(RMCXT_LIBRARY NAMES ${RMCXT_NAMES} PATHS ${OOSDK_SEARCH_DIR})

SET(SAL_NAMES isal libuno_sal sal)
FIND_LIBRARY(SAL_LIBRARY NAMES ${SAL_NAMES} PATHS ${OOSDK_SEARCH_DIR})

SET(SALHELPER_NAMES isalhelper libuno_salhelpergcc3 salhelper)
FIND_LIBRARY(SALHELPER_LIBRARY NAMES ${SALHELPER_NAMES} PATHS ${OOSDK_SEARCH_DIR})

SET(STORE_NAMES istore libstore store)
FIND_LIBRARY(STORE_LIBRARY NAMES ${STORE_NAMES} PATHS ${OOSDK_SEARCH_DIR})

SET(STLPORT_NAMES istlport_vc71 libstlport_gcc stlport_vc71)
FIND_LIBRARY(STLPORT_LIBRARY NAMES ${STLPORT_NAMES} PATHS ${OOSDK_SEARCH_DIR})        

#------------------------------------
SET(CPPU_LIBRARY "${OOSDK_SEARCH_DIR}/libuno_cppu.so")
SET(CPPUHELPER_LIBRARY "${OOSDK_SEARCH_DIR}/libuno_cppuhelpergcc3.so")
SET(REG_LIBRARY "${OOSDK_SEARCH_DIR}/libreg.so.3")
SET(RMCXT_LIBRARY "${OOSDK_SEARCH_DIR}/librmcxt.so.3")
SET(SAL_LIBRARY "${OOSDK_SEARCH_DIR}/libuno_sal.so")
SET(SALHELPER_LIBRARY "${OOSDK_SEARCH_DIR}/libuno_salhelpergcc3.so")
SET(STORE_LIBRARY "${OOSDK_SEARCH_DIR}/libstore.so.3")
SET(STLPORT_LIBRARY "${OOSDK_SEARCH_DIR}/libstlport_gcc.so")
#------------------------------------

# Handle the QUIETLY and REQUIRED arguments and set OOSDK_FOUND to
# TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OOSDK DEFAULT_MSG OOSDK_LIBRARY OOSDK_INCLUDE_DIR)

IF(OOSDK_FOUND)
  SET( OOSDK_LIBRARIES ${CPPU_LIBRARY} ${CPPUHELPER_LIBRARY} ${REG_LIBRARY} ${RMCXT_LIBRARY} ${SAL_LIBRARY} ${SALHELPER_LIBRARY} ${STORE_LIBRARY} ${STLPORT_LIBRARY})
ELSE(OOSDK_FOUND)
  SET( OOSDK_LIBRARIES )
ENDIF(OOSDK_FOUND)

MARK_AS_ADVANCED(OOSDK_LIBRARY OOSDK_INCLUDE_DIR)
