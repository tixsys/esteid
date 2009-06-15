#if !defined(__DIGESTTEST_H_INCLUDED__)
#define __DIGESTTEST_H_INCLUDED__

#include <cppunit/extensions/HelperMacros.h>

namespace digidoc
{
    class DigestTest : public CppUnit::TestCase
    {

        CPPUNIT_TEST_SUITE(DigestTest);
        CPPUNIT_TEST(testDigestFactories);
        CPPUNIT_TEST(testURIToMethod);
        CPPUNIT_TEST(testSHA1Digest);
        CPPUNIT_TEST(testSHA256Digest);
        CPPUNIT_TEST_SUITE_END();

      public:
          virtual void setUp();

      protected:
          void testDigestFactories();
          void testURIToMethod();
          void testSHA1Digest();
          void testSHA256Digest();

      private:
          void testDigest(int method, const std::string& name, const std::string uri,
                  int size, unsigned char* emptyDigest, unsigned char* expectedDigest);

          static const std::string NAME_SHA1;
          static const std::string NAME_SHA256;

          static const std::string URI_SHA1;
          static const std::string URI_SHA256;

          std::vector<int> methods;
          std::vector<std::string> uris;

    };
}

#endif // !defined(__DIGESTTEST_H_INCLUDED__)
