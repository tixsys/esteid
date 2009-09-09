
if(WIN32)
  set(PKCS_HEADERS pkcs11/)
elseif(UNIX)
  if (APPLE)
    set(PKCS_HEADERS /System/Library/Frameworks/PCSC.framework/Versions/A/Headers/)
  else (APPLE)
    set(PKCS_HEADERS /usr/include/PCSC/)
  endif(APPLE)
endif(WIN32)

include_directories (../../ ../../src ../ ${PKCS_HEADERS})

set(LIBPKCS_HEADERS PKCS11Context.h precompiled.h)
set(LIBPKCS_SOURCES esteidpkcs11.cpp PKCS11Context.cpp precompiled.cpp)

link_directories (${ESTEIDUTIL_BINARY_DIR}/smartcard++/ ${ESTEIDUTIL_BINARY_DIR}/src/utility) 
link_libraries (smartcard++ utility)

if(UNIX)
  link_libraries (dl)
endif(UNIX)

add_library (esteidpkcs11 SHARED ${LIBPKCS_SOURCES} ${LIBPKCS_HEADERS}) 

install(TARGETS esteidpkcs11 DESTINATION esteidutil/lib)