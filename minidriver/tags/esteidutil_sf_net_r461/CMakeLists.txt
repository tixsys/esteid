include_directories (../.. ../../src ../ ../../..)

set(ESTEIDCM_HEADERS
  esteidcm.h	precompiled.h
  resource.h	version.h)

set(ESTEIDCM_SOURCES
  esteidcm.rc	esteidcm.cpp
  esteidcm.def 	notimplemented.cpp
  precompiled.cpp) 

link_directories (${ESTEIDUTIL_BINARY_DIR}/smartcard++/) 
link_libraries (smartcard++)

add_library (esteidcm SHARED ${ESTEIDCM_SOURCES} ${ESTEIDCM_HEADERS}) 

install(TARGETS esteidcm DESTINATION esteidutil/lib)