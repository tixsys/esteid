cmake_minimum_required(VERSION 2.6)
project(esteidcm)

# Custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
find_package(SmartCardpp REQUIRED)
find_package(Perl)

# Get current date
if(PERL_FOUND)
    execute_process(COMMAND "${PERL_EXECUTABLE}" "-le" "@T=localtime; print sprintf \"%02d/%02d/%04d\",$T[4]+1,$T[3],$T[5]+1900"
    OUTPUT_VARIABLE BUILD_DATE OUTPUT_STRIP_TRAILING_WHITESPACE)
else()
    message(STATUS "Warning: Perl not found; using dummy date.")
    set(BUILD_DATE "03/29/2010")
endif()
message(STATUS "Build date is: ${BUILD_DATE}")

set(MAJOR 1)
set(MINOR 1)
set(PATCH 0)
set(BUILD_REV 0)

# Get SVN revision number
find_package(Subversion)
if(Subversion_FOUND AND EXISTS ${PROJECT_SOURCE_DIR}/.svn)
    Subversion_WC_INFO(${PROJECT_SOURCE_DIR} PROJECT)
    message(STATUS "Current SVN revision is ${PROJECT_WC_LAST_CHANGED_REV}")
    set(BUILD_REV  ${PROJECT_WC_LAST_CHANGED_REV})
endif(Subversion_FOUND AND EXISTS ${PROJECT_SOURCE_DIR}/.svn)

set(PROJECT_VERSION "${MAJOR}.${MINOR}.${PATCH}.${BUILD_REV}")
message(STATUS "Project version is ${PROJECT_VERSION}")

add_definitions(
    -D_UNICODE
    -DUNICODE
    -D_CRT_SECURE_NO_DEPRECATE
)

# Build with static MSVC runtime
foreach(flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    if(${flag_var} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
    endif(${flag_var} MATCHES "/MD")
endforeach(flag_var)

include_directories(
    ${SMARTCARDPP_INCLUDE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

# esteidcm64.dll on x64
# esteidcm.dll on Win32
if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    set(esteidcm_NAME "esteidcm64")
else()
    set(esteidcm_NAME "esteidcm")
endif()

configure_file(esteidcm.def.in ${CMAKE_CURRENT_BINARY_DIR}/esteidcm.def @ONLY)
configure_file(version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h @ONLY)
configure_file(install/${esteidcm_NAME}.inf.in ${CMAKE_CURRENT_BINARY_DIR}/esteidcm.inf @ONLY)

set(esteidcm_SRCS
    esteidcm.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/esteidcm.def
    esteidcm.rc
    notimplemented.cpp
    precompiled.cpp
)

add_library(esteidcm MODULE ${esteidcm_SRCS}) 
set_target_properties(esteidcm PROPERTIES OUTPUT_NAME ${esteidcm_NAME})
target_link_libraries(esteidcm ${SMARTCARDPP_LIBRARIES} ws2_32)

install(TARGETS esteidcm DESTINATION bin)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/esteidcm.inf DESTINATION bin)
