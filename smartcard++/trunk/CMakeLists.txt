include_directories (../ ./ ../.. ../../src)

if(UNIX)
  include_directories (/usr/include/PCSC/)
endif(UNIX)

set(LIBSMARTCARD_HEADERS 
  smartcard++.h 	CardBase.h 
  common.h 		CTAPIManager.h 
  DynamicLibrary.h 	helperMacro.h 
  ManagerInterface.h 	PCSCManager.h 
  precompiled.h 	SCError.h 
  SmartCardManager.h 	locked_allocator.h 
  types.h)
            
set(LIBSMARTCARD_SOURCES 
  CardBase.cpp 		CTAPIManager.cpp 
  DynamicLibrary.cpp 	PCSCManager.cpp 
  SCError.cpp 		SmartCardManager.cpp 
  locked_allocator.cpp 	types.cpp)

set(LIBESTEID_HEADERS esteid/EstEidCard.h esteid/EstEidCardMaintainer.h)
set(LIBESTEID_SOURCES esteid/EstEidCard.cpp esteid/EstEidCardMaintainer.cpp)

set(LIBSIM_HEADERS sim/SIMCard.h)
set(LIBSIM_SOURCES sim/SIMCard.cpp)

add_library (sim STATIC ${LIBSIM_SOURCES} ${LIBSIM_HEADERS}) 
add_library (esteid STATIC ${LIBESTEID_SOURCES} ${LIBESTEID_HEADERS})

if(WIN32)
  add_library (smartcard++ STATIC ${LIBSMARTCARD_SOURCES} ${LIBSMARTCARD_HEADERS}) 
elseif(UNIX)
  add_library (smartcard++ SHARED ${LIBSMARTCARD_SOURCES} ${LIBSMARTCARD_HEADERS}) 
endif(WIN32)

target_link_libraries (smartcard++ esteid sim)

if(UNIX)
  target_link_libraries (smartcard++ dl)
endif(UNIX)

install(TARGETS smartcard++ sim esteid DESTINATION esteidutil/lib)