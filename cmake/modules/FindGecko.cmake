# - Try to find Gecko
# Once done this will define
#
#  GECKO_FOUND - System has Gecko
#  GECKO_IDL_DIR - The Gecko idl include directory
#  GECKO_SDK_DIR - The Gecko sdk directory
#  GECKO_INCLUDE_DIR - The Gecko include directory
#  GECKO_LIBRARIES - The libraries needed to use Gecko
#  GECKO_DEFINITIONS - Compiler switches required for using Gecko
#  GECKO_XPIDL_EXECUTABLE - The idl compiler xpidl coming with Gecko


IF (GECKO_IDL_DIR AND GECKO_SDK_DIR AND GECKO_INCLUDE_DIR AND GECKO_XPIDL_EXECUTABLE)
   # in cache already
   SET(Gecko_FIND_QUIETLY TRUE)
ENDIF (GECKO_IDL_DIR AND GECKO_SDK_DIR AND GECKO_INCLUDE_DIR AND GECKO_XPIDL_EXECUTABLE)

IF (NOT WIN32)
   # use pkg-config to get the directories and then use these values
   # in the FIND_PATH() and FIND_LIBRARY() calls
   FIND_PACKAGE(PkgConfig)
   PKG_CHECK_MODULES(PC_LIBXUL libxul)
   SET(GECKO_DEFINITIONS ${PC_LIBXUL_CFLAGS_OTHER})
   SET(GECKO_INCLUDE_DIR ${PC_LIBXUL_INCLUDE_DIRS})

   # Use FindPkgConfig internal function _pkgconfig_invoke to get sdkdir
   _pkgconfig_invoke(libxul "PC_LIBXUL" SDKDIR "" --variable=sdkdir)
   SET(GECKO_SDK_DIR ${PC_LIBXUL_SDKDIR})
ENDIF (NOT WIN32)

FIND_PATH(GECKO_IDL_DIR nsIDOMWindow.idl
   HINTS
   ${PC_LIBXUL_SDKDIR}/idl
   )

#FIND_LIBRARY(GECKO_LIBRARIES NAMES xul
#   HINTS
#   ${PC_LIBXUL_LIBDIR}
#   ${PC_LIBXUL_LIBRARY_DIRS}
#   )

FIND_PROGRAM(GECKO_XPIDL_EXECUTABLE xpidl
   HINTS
   ${PC_LIBXUL_LIBDIR}
   ${PC_LIBXUL_SDKDIR}/bin
   )

INCLUDE(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set GECKO_FOUND to TRUE if 
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Gecko DEFAULT_MSG GECKO_IDL_DIR GECKO_SDK_DIR GECKO_INCLUDE_DIR GECKO_XPIDL_EXECUTABLE)

MARK_AS_ADVANCED(GECKO_IDL_DIR GECKO_SDK_DIR GECKO_INCLUDE_DIR GECKO_XPIDL_EXECUTABLE)
