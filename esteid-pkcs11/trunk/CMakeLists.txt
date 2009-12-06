cmake_minimum_required(VERSION 2.6)
project(esteid-pkcs11)

# Custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
include(InstallSettings)

if(UNIX)
    find_package(PCSCLite)
endif(UNIX)
find_package(SmartCardpp REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PCSCLITE_INCLUDE_DIR}
    ${SMARTCARDPP_INCLUDE_DIR}
)

set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-Bsymbolic -Wl,-z,defs")

set(SOURCES
    esteidpkcs11.cpp
    esteidpkcs11.rc
    PKCS11Context.cpp
    precompiled.cpp
    utility/logger.cpp
    utility/asnCertificate.cpp
    utility/asnObject.cpp
)

add_library(esteid-pkcs11 SHARED ${SOURCES})
set_target_properties(esteid-pkcs11 PROPERTIES PREFIX "")
target_link_libraries(esteid-pkcs11 ${SMARTCARDPP_LIBRARIES})
install(TARGETS esteid-pkcs11 DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
