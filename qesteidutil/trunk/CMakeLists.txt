cmake_minimum_required(VERSION 2.6)
project(qesteidutil)

# Custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

find_package(Qt4 REQUIRED)
if(UNIX)
    find_package(PCSCLite REQUIRED)
endif(UNIX)

set(qesteidutil_MOC_HDRS
    jscardmanager.h
    jscertdata.h
    jsesteidcard.h
    jsextender.h
    mainwindow.h
)

set(cardlib_SRCS
    cardlib/CardBase.cpp
    cardlib/CTAPIManager.cpp
    cardlib/DynamicLibrary.cpp
    cardlib/EstEidCard.cpp
    cardlib/PCSCManager.cpp
    cardlib/SCError.cpp
    cardlib/SmartCardManager.cpp
)

set(qesteidutil_SRCS
    jscardmanager.cpp
    jscertdata.cpp
    jsesteidcard.cpp
    jsextender.cpp
    main.cpp
    mainwindow.cpp
)

set(qesteidutil_RCS qesteidutil.qrc)

set(QT_USE_QTNETWORK true)
set(QT_USE_QTWEBKIT true)

include(${QT_USE_FILE})

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${PCSCLITE_INCLUDE_DIR}
)

QT4_ADD_RESOURCES(qesteidutil_RCC_SRCS ${qesteidutil_RCS})
QT4_WRAP_CPP(qesteidutil_MOC_SRCS ${qesteidutil_MOC_HDRS})

add_executable(qesteidutil WIN32
    ${cardlib_SRCS}
    ${qesteidutil_SRCS}
    ${qesteidutil_MOC_SRCS}
    ${qesteidutil_RCC_SRCS}
)

target_link_libraries(qesteidutil
    ${QT_LIBRARIES}
)

install(TARGETS qesteidutil DESTINATION bin)
