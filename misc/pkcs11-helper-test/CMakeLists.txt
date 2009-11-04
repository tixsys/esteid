cmake_minimum_required(VERSION 2.6)
project(pkcs11-helper-test)

# Custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

find_package(PKCS11Helper REQUIRED)

if(WIN32)
    set(PKCS11_PROVIDER "opensc-pkcs11.dll")
elseif(APPLE)
    set(PKCS11_PROVIDER "/Library/OpenSC/lib/opensc-pkcs11.so")
elseif(UNIX)
    set(PKCS11_PROVIDER "opensc-pkcs11.so")
endif(WIN32)

add_definitions(-DPKCS11_PROVIDER="${PKCS11_PROVIDER}")

include_directories(${PKCS11HELPER_INCLUDE_DIR})

add_executable(pkcs11-helper-test test.c)
target_link_libraries(pkcs11-helper-test ${PKCS11HELPER_LIBRARIES})

install(TARGETS pkcs11-helper-test DESTINATION bin)
