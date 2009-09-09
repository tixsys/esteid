include_directories (../.. ../../src ../ ../../..)

set(ESTEIDCSP_SOURCES
  csp_c_exports.cpp	Csp.cpp
  csp.def		CSPErrors.cpp
  CspEstEid.cpp		Csp_Hash.cpp
  Csp_Key.cpp		CSPTypes.cpp
  esteidcsp.cpp		esteidcsp.rc
  precompiled.cpp	RegKey.cpp)

set(ESTEIDCSP_HEADERS
  CSPErrors.h		CSPEstEid.h
  CSPTypes.h		precompiled.h
  RegKey.h		resource.h
  Setup.h		utility.h
  Csp.h)

add_definitions(-D_WINDOWS  -D_USRDLL -DESTEIDCSP_EXPORTS -D_SCL_SECURE_NO_WARNINGS -D_UNICODE -DUNICODE)

link_directories (${ESTEIDUTIL_BINARY_DIR}/smartcard++/ ${ESTEIDUTIL_BINARY_DIR}/src/utility) 
link_libraries (smartcard++ utility)

add_library (esteidcsp SHARED ${ESTEIDCSP_SOURCES} ${ESTEIDCSP_HEADERS}) 

find_program(CSPSIGNEXE NAMES cspsign.exe cspSign.exe PATHS ../../../../ PATH_SUFFIXES cspdk)

if(CSPSIGNEXE)
  get_target_property(ESTEIDCSP_DLL_FULLPATH esteidcsp LOCATION)
  add_custom_command(TARGET esteidcsp POST_BUILD
    COMMAND ${CSPSIGNEXE} ARGS c ${ESTEIDCSP_DLL_FULLPATH}
    COMMAND ${CSPSIGNEXE} ARGS s ${ESTEIDCSP_DLL_FULLPATH} ${ESTEIDCSP_DLL_FULLPATH}.sig)
else(CSPSIGNEXE)
  message( STATUS "cspsign.exe not found, post-build command not added" )
endif(CSPSIGNEXE)

install(TARGETS esteidcsp DESTINATION esteidutil/lib)