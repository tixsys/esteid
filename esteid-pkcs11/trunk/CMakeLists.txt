cmake_minimum_required(VERSION 2.6)
project(esteidpkcs11)

# Custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

set(CARDLIB smartcard++)

set(smartcard_SRCS
    ${CARDLIB}/CardBase.cpp
    ${CARDLIB}/CTAPIManager.cpp
    ${CARDLIB}/DynamicLibrary.cpp
    ${CARDLIB}/esteid/EstEidCard.cpp
    ${CARDLIB}/locked_allocator.cpp
    ${CARDLIB}/PCSCManager.cpp
    ${CARDLIB}/SCError.cpp
    ${CARDLIB}/SmartCardManager.cpp
    ${CARDLIB}/types.cpp
    ${CARDLIB}/utility/logger.cpp
    ${CARDLIB}/utility/asnCertificate.cpp
    ${CARDLIB}/utility/asnObject.cpp
)

if(UNIX AND NOT APPLE)
    find_package(PCSCLite REQUIRED)
    include_directories(${PCSCLITE_INCLUDE_DIR})
endif(UNIX AND NOT APPLE)

include_directories(
    pkcs11
    ${CMAKE_SOURCE_DIR}
)


set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-Bsymbolic -Wl,-z,defs")

set(LIBPKCS_HEADERS PKCS11Context.h precompiled.h)
set(LIBPKCS_SOURCES esteidpkcs11.cpp PKCS11Context.cpp precompiled.cpp)

link_directories (${ESTEIDUTIL_BINARY_DIR}/smartcard++/ ${ESTEIDUTIL_BINARY_DIR}/src/utility) 
# link_libraries (smartcard++ utility)

if(UNIX)
  link_libraries (dl)
endif(UNIX)

add_library (esteidpkcs11 SHARED ${LIBPKCS_SOURCES} ${LIBPKCS_HEADERS} ${smartcard_SRCS}) 

install(TARGETS esteidpkcs11 DESTINATION esteidutil/lib)
