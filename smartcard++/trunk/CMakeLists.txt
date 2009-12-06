cmake_minimum_required(VERSION 2.6)
project(smartcard++)

# Custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

include(InstallSettings)

set(SMARTCARDPP_VERSION "0.1.0")

if(UNIX)
    include_directories(/usr/include/PCSC/)
endif(UNIX)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

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

# Public headers which get installed into smartcard++/ directory
set(smartcard_HDRS
    CardBase.h
    common.h
    CTAPIManager.h
    DynamicLibrary.h
    helperMacro.h
    internal-pcsc22.h
    locked_allocator.h
    ManagerInterface.h
    PCSCManager.h
    PinString.h
    SCError.h
    smartcard++.h
    SmartCardManager.h
    types.h
)

# Public headers which get installed into smartcard++/esteid/ directory
set(esteid_HDRS
    esteid/EstEidCard.h
    esteid/EstEidCardMaintainer.h
)

# Public headers which get installed into smartcard++/sim/ directory
set(sim_HDRS sim/SIMCard.h)

set(GENERATED_HEADERS "")
macro(stage_smartcard_headers HEADERS)
    foreach(H ${HEADERS})
        set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/${H}")
        set(DST "${CMAKE_CURRENT_BINARY_DIR}/smartcard++/${H}")

        add_custom_command(
            OUTPUT ${DST}
            COMMAND ${CMAKE_COMMAND} -E copy ${SRC} ${DST}
            DEPENDS ${SRC}
        )

        set(GENERATED_HEADERS ${GENERATED_HEADERS} ${DST})
    endforeach(H)
endmacro(stage_smartcard_headers)

stage_smartcard_headers("${smartcard_HDRS}")
stage_smartcard_headers("${esteid_HDRS}")
stage_smartcard_headers("${sim_HDRS}")

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/smartcard++ DESTINATION ${INCLUDE_INSTALL_DIR})

if(WIN32)
    set(LIB_TYPE STATIC)
else(WIN32)
    set(LIB_TYPE SHARED)
endif(WIN32)

add_library(smartcard++ ${LIB_TYPE}
    ${GENERATED_HEADERS}
    ${smartcard_SRCS}
    ${sim_SRCS}
    ${esteid_SRCS}
)

if(WIN32)
    target_link_libraries(smartcard++ ws2_32)
else(WIN32)
    target_link_libraries(smartcard++ dl)
endif(WIN32)

set_target_properties(smartcard++ PROPERTIES VERSION ${SMARTCARDPP_VERSION} SOVERSION 0)
install(TARGETS smartcard++ DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})

add_executable(card-test test.cpp)
target_link_libraries(card-test smartcard++)
install(TARGETS card-test DESTINATION ${BIN_INSTALL_DIR})
