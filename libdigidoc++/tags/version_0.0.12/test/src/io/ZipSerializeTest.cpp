#include <openssl/objects.h>
#include <string.h>

#include <io/ZipSerialize.h>

#include "ZipSerializeTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(digidoc::ZipSerializeTest);

extern std::string TEST_DATA_PATH;

void digidoc::ZipSerializeTest::setUp()
{
}

void digidoc::ZipSerializeTest::testPath()
{
    printf("\n\n'%s'\n\n", TEST_DATA_PATH.c_str());


	// Extract ja save on vaja mõlemad testida, tühjade ja mitte eksisteerivata jne pathidega

	// 1. empty path
	// 2. non existing path
	// 3. dir
	// 4. non zip file (oli juba extracti juures)
	// 5. õigusi pole
	// 6. relative path
	// 7. full path
}

void digidoc::ZipSerializeTest::testExtractZip()
{
	// EXTRACT
	// 1. Empty ZIP
	// 2. Empty file
	// 3. Non Zip file
	// 4. one file in zip root
	// 5. multiple files in ZIP root
	// 6. no files in root, but multiple files in directories (multiple levels of directories)
	// 7. several files in root and in different directories
	// 8. one empty dir in root
	// 9. multiple dirs in root, that have multiple levels

}

void digidoc::ZipSerializeTest::testCreateZip()
{
	// CREATE
	// 1. empty ZIP (should fail?)
	// 2. with one file
	// 3. with multiple files in root
	// 4. with one empty dir
	// 5. with one empty file
	// 6. no files in root, but multiple files in directories (multiple levels of directories)
	// 7. several files in root and in different directories
	// 8. multiple dirs in root, that have multiple levels
}





void digidoc::ZipSerializeTest::testZip(int method, const std::string& name, const std::string uri,
        int size, unsigned char* emptyDigest, unsigned char* expectedDigest)
{
	/*
    std::auto_ptr<Digest> calc = Digest::create(method);

    // Test digest getters.
    CPPUNIT_ASSERT_EQUAL(method, calc->getMethod());
    CPPUNIT_ASSERT_EQUAL(name, calc->getName());
    CPPUNIT_ASSERT_EQUAL(uri, calc->getUri());
    CPPUNIT_ASSERT_EQUAL(size, calc->getSize());

    // Test empty digest calculation by calling finalize without update.
    std::string msg = util::String::format("Empty %s digest does not match.", name.c_str());
    CPPUNIT_ASSERT_MESSAGE(msg, memcmp(emptyDigest, &calc->getDigest()[0], size) == 0);

    // Test empty digest calculation, calculate from buffer with size 0;
    calc = Digest::create(method);
    calc->update(emptyDigest, 0);
    CPPUNIT_ASSERT_MESSAGE(msg, memcmp(emptyDigest, &calc->getDigest()[0], size) == 0);

    // Test that digest update fails after finalize is called.
    msg = util::String::format("%s digest update did not throw an exception after finalize.", name.c_str());
    CPPUNIT_ASSERT_THROW_MESSAGE(msg, calc->update(emptyDigest, size), IOException);

    // Test that calculating digest from NULL pointer buffer fails.
    calc = Digest::create(method);
    msg = util::String::format("%s digest calculation from NULL pointer buffer did not throw an exception.", name.c_str());
    CPPUNIT_ASSERT_THROW_MESSAGE(msg, calc->update(NULL, size), IOException);

    // Test digest calculation, calculate digest from empty digest value.
    calc = Digest::create(method);
    calc->update(emptyDigest, size);
    msg = util::String::format("%s digest does not match.", name.c_str());
    CPPUNIT_ASSERT_MESSAGE(msg, memcmp(expectedDigest, &calc->getDigest()[0], size) == 0);
    */
}
