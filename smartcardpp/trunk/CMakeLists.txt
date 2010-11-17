cmake_minimum_required(VERSION 2.6)
project(smartcardpp)

# Custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

include(InstallSettings)

set(SMARTCARDPP_VERSION "0.2.0")

if(UNIX)
    find_package(LibDL)
    find_package(PCSCLite REQUIRED)
endif(UNIX)

if(MSVC)
    add_definitions(
        # Release builds don't compile without this definition.
        # http://social.msdn.microsoft.com/Forums/en-US/vcplus2008prerelease/thread/fa969def-911e-4760-867d-5a06a37ed048
        -D_SECURE_SCL=0
    )
endif(MSVC)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LIBDL_INCLUDE_DIR}
    ${PCSCLITE_INCLUDE_DIR}
)

if(MSVC AND USE_STATIC_RUNTIME)
    message(STATUS "Building with static MSVC runtime")
    foreach(flag_var
            CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        if(${flag_var} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif(${flag_var} MATCHES "/MD")
    endforeach(flag_var)
endif(MSVC AND USE_STATIC_RUNTIME)

set(smartcard_SRCS
    CardBase.cpp
    CTAPIManager.cpp
    DynamicLibrary.cpp
    locked_allocator.cpp
    PCSCManager.cpp
    SCError.cpp
    SmartCardManager.cpp
    types.cpp
)

set(esteid_SRCS
    esteid/EstEidCard.cpp
    esteid/EstEidCardMaintainer.cpp
)

set(sim_SRCS sim/SIMCard.cpp)

# Public headers which get installed into smartcardpp/ directory
set(smartcard_HDRS
    CardBase.h
    CTAPIManager.h
    DynamicLibrary.h
    locked_allocator.h
    ManagerInterface.h
    PCSCManager.h
    PinString.h
    SCError.h
    smartcardpp.h
    SmartCardManager.h
    types.h
)

# Public headers which get installed into smartcardpp/esteid/ directory
set(esteid_HDRS
    esteid/EstEidCard.h
    esteid/EstEidCardMaintainer.h
)

# Public headers which get installed into smartcardpp/sim/ directory
set(sim_HDRS sim/SIMCard.h)

install(FILES ${smartcard_HDRS} DESTINATION ${INCLUDE_INSTALL_DIR}/smartcardpp)
install(FILES ${esteid_HDRS} DESTINATION ${INCLUDE_INSTALL_DIR}/smartcardpp/esteid)
install(FILES ${sim_HDRS} DESTINATION ${INCLUDE_INSTALL_DIR}/smartcardpp/sim)

if(WIN32 OR APPLE)
    set(LIB_TYPE STATIC)
else(WIN32 OR APPLE)
    set(LIB_TYPE SHARED)
endif(WIN32 OR APPLE)

add_library(smartcardpp ${LIB_TYPE}
    ${smartcard_SRCS}
    ${sim_SRCS}
    ${esteid_SRCS}
)

if(WIN32)
    set(WIN_LIBRARIES ws2_32)
endif(WIN32)

target_link_libraries(smartcardpp ${LIBDL_LIBRARIES} ${WIN_LIBRARIES})

set_target_properties(smartcardpp PROPERTIES VERSION ${SMARTCARDPP_VERSION} SOVERSION 0)
install(TARGETS smartcardpp DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})

add_executable(card-test test.cpp)
target_link_libraries(card-test smartcardpp)
install(TARGETS card-test DESTINATION ${BIN_INSTALL_DIR})
