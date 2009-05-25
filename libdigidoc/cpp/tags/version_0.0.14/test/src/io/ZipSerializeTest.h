#if !defined(__ZIPSERIALIZETEST_H_INCLUDED__)
#define __ZIPSERIALIZETEST_H_INCLUDED__

#include <cppunit/extensions/HelperMacros.h>

namespace digidoc
{
    class ZipSerializeTest : public CppUnit::TestCase
    {

        CPPUNIT_TEST_SUITE(ZipSerializeTest);
        CPPUNIT_TEST(testPath);
        CPPUNIT_TEST(testExtractZip);
        CPPUNIT_TEST(testCreateZip);
        CPPUNIT_TEST_SUITE_END();

      public:
          virtual void setUp();

      protected:
          void testPath();
          void testExtractZip();
          void testCreateZip();

      private:
          void testZip(int method, const std::string& name, const std::string uri,
                  int size, unsigned char* emptyDigest, unsigned char* expectedDigest);

    };
}

#endif // !defined(__ZIPSERIALIZETEST_H_INCLUDED__)
