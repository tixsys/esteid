include_directories( ${CPPUNIT_INCLUDE_DIR} )
include_directories( ${MINIZIP_INCLUDE_DIR} )
include_directories( ${OPENSSL_INCLUDE_DIR} )
include_directories( ${ZLIB_INCLUDE_DIR} )
include_directories( ${XSD_INCLUDE_DIR} )

set(unittests_SRCS
    src/crypto/DigestTest.cpp
    src/io/ZipSerializeTest.cpp
    src/main_unittest.cpp
    src/SignatureBESTest.cpp
)

if(MSVC)
    # disable warning C4290: C++ exception specification ignored
    add_definitions( -wd4290 )
    add_definitions(
        -D_CRT_SECURE_NO_DEPRECATE
        -D_CRT_SECURE_NO_WARNINGS
        -D_SCL_SECURE_NO_WARNINGS
    )
endif(MSVC)

include_directories(
    ../src
)

add_executable( unittests ${unittests_SRCS} )
target_link_libraries( unittests digidoc ${CPPUNIT_LIBRARIES} )

add_custom_target( test-stage
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/test.py ${CMAKE_CURRENT_BINARY_DIR}
)
add_dependencies( unittests test-stage )

add_test(PythonTest test.py)
