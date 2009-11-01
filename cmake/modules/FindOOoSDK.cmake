# - Find OOo SDK
# Find OOSDK includes and executable
#
#  OOSDK_INCLUDE_DIR - Where to find OOSDK include sub-directory.
#  OOSDK_EXECUTABLE  - OOSDK compiler.
#  OOSDK_FOUND       - True if OOSDK found.


IF (CPPUMAKER)
  # Already in cache, be silent.
  SET(CPPUMAKER_FIND_QUIETLY TRUE)
ENDIF (CPPUMAKER)

FIND_PROGRAM(CPPUMAKER NAMES cppumaker PATHS "${OO_SDK_HOME}/bin")
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CppuMaker DEFAULT_MSG CPPUMAKER)
MARK_AS_ADVANCED(CPPUMAKER)


IF (OOSDK_INCLUDE_DIR)
  # Already in cache, be silent.
  SET(OOSDK_FIND_QUIETLY TRUE)
ENDIF (OOSDK_INCLUDE_DIR)

IF (WIN32)
	SET(OOSDK_SEARCH_DIR "${OO_SDK_HOME}/lib")
ENDIF (WIN32)

IF (UNIX)
	SET(OOSDK_SEARCH_DIR "${OO_SDK_URE_HOME}/lib")
ENDIF (UNIX)

IF (APPLE)
	SET(OOSDK_SEARCH_DIR "/Users/mark/OpenOffice.org3.1_SDK/macosx/lib")
ENDIF (APPLE)

message( STATUS "searching libraries in ${OOSDK_SEARCH_DIR}")

SET(CPPU_NAMES icppu uno_cppu cppu libuno_cppu)
FIND_LIBRARY(CPPU_LIBRARY NAMES ${CPPU_NAMES} PATHS ${OOSDK_SEARCH_DIR} ENV LD_LIBRARY_PATH)

SET(CPPUHELPER_NAMES uno_cppuhelpergcc3 icppuhelper cppuhelper libuno_cppuhelpergcc3)
FIND_LIBRARY(CPPUHELPER_LIBRARY NAMES ${CPPUHELPER_NAMES} PATHS ${OOSDK_SEARCH_DIR} ENV LD_LIBRARY_PATH)

SET(SAL_NAMES isal uno_sal sal libuno_sal)
FIND_LIBRARY(SAL_LIBRARY NAMES ${SAL_NAMES} PATHS ${OOSDK_SEARCH_DIR} ENV LD_LIBRARY_PATH)

SET(SALHELPER_NAMES isalhelper uno_salhelpergcc3 salhelper libuno_salhelpergcc3)
FIND_LIBRARY(SALHELPER_LIBRARY NAMES ${SALHELPER_NAMES} PATHS ${OOSDK_SEARCH_DIR} ENV LD_LIBRARY_PATH)

SET(STLPORT_NAMES istlport_vc71 stlport_gcc stlport_vc71 libstlport_gcc)
FIND_LIBRARY(STLPORT_LIBRARY NAMES ${STLPORT_NAMES} PATHS ${OOSDK_SEARCH_DIR} ENV LD_LIBRARY_PATH)

IF (APPLE)
#	SET(CPPU_LIBRARY "${OOSDK_SEARCH_DIR}/libuno_cppu.dylib.3")
#	SET(CPPUHELPER_LIBRARY "${OOSDK_SEARCH_DIR}/libuno_cppuhelpergcc3.dylib.3")
#	SET(SAL_LIBRARY "${OOSDK_SEARCH_DIR}/libuno_sal.dylib.3")
#	SET(SALHELPER_LIBRARY "${OOSDK_SEARCH_DIR}/libuno_salhelpergcc3.dylib.3")
	SET(STLPORT_LIBRARY "/usr/local/lib/libstlport.dylib")
ENDIF (APPLE)

# Handle the QUIETLY and REQUIRED arguments and set OOSDK_FOUND to
# TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OOSDK DEFAULT_MSG OOSDK_LIBRARY OOSDK_INCLUDE_DIR)

IF(OOSDK_FOUND)
  SET( OOSDK_LIBRARIES ${CPPU_LIBRARY} ${CPPUHELPER_LIBRARY} ${SAL_LIBRARY} ${SALHELPER_LIBRARY} ${STLPORT_LIBRARY})
ELSE(OOSDK_FOUND)
  SET( OOSDK_LIBRARIES )
ENDIF(OOSDK_FOUND)

MARK_AS_ADVANCED(OOSDK_LIBRARY OOSDK_INCLUDE_DIR)
