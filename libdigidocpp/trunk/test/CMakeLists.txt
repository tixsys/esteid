find_package(CppUnit REQUIRED)

include_directories(
    ${MINIZIP_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
)

set(unittests_SRCS
    src/crypto/DigestTest.cpp
    src/io/ZipSerializeTest.cpp
    src/main_unittest.cpp
    src/SignatureBESTest.cpp
)

if(WIN32)
    add_definitions(
      -DNDEBUG
      -DWIN32
      -DXSEC_NO_XALAN
      -D_CONSOLE
      -D_CRT_SECURE_NO_DEPRECATE
      -D_CRT_SECURE_NO_WARNINGS
    )

    target_link_libraries( unittests libeay32 ssleay32 zlib xerces-c_2D xsec_1D TestRunnerd cppunitd )
endif(WIN32)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

add_executable( unittests WIN32 ${unittests_SRCS} )
target_link_libraries( unittests cppunit digidocpp ${OPENSSL_LIBRARIES} )

add_custom_target( test-stage
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/test.py ${CMAKE_CURRENT_BINARY_DIR}
)
add_dependencies( unittests test-stage )

add_test(PythonTest test.py)
