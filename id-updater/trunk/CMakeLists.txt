cmake_minimum_required(VERSION 2.6)
project(id-updater)

find_package(Qt4 4.4.3 COMPONENTS QtCore QtGui QtNetwork QtXml REQUIRED)
include(${QT_USE_FILE})

add_definitions(
    -D_CRT_SECURE_NO_WARNINGS
    -DUNICODE
)

set(UPDATER_URL "http://ftp.id.eesti.ee/pub/id/updater/" CACHE STRING "URL to download manifest.xml" )
add_definitions(-DUPDATER_URL="${UPDATER_URL}")

if(MSVC)
    # http://www.qtcentre.org/forum/f-installation-5/t-wchar-to-qstring-giving-error-in-vs2005-4625.html
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t-")
endif(MSVC)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(CARDLIB ${CMAKE_SOURCE_DIR}/smartcard++)

set(updater_SRCS
    idupdater.cpp
    InstallChecker.cpp
    main.cpp
    ProcessStarter.cpp
    ScheduledUpdateTask.cpp
    ${CARDLIB}/DynamicLibrary.cpp
    idupdater.rc
)
set(updater_MOC_HDRS idupdater.h)
set(updater_UIS idupdater.ui)
set(updater_RCS idupdater.qrc)

QT4_WRAP_CPP(updater_MOC_SRCS ${updater_MOC_HDRS})
QT4_WRAP_UI(updater_UIS_H ${updater_UIS})
QT4_ADD_RESOURCES(updater_RCC_SRCS ${updater_RCS})

add_executable(id-updater WIN32
    ${updater_SRCS}
    ${updater_MOC_SRCS}
    ${updater_UIS_H}
    ${updater_RCC_SRCS}
)
target_link_libraries(id-updater
    ${QT_QTMAIN_LIBRARY}
    ${QT_LIBRARIES}
)

install(TARGETS id-updater DESTINATION bin)
