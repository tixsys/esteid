#if !defined(__SIGNATUREBESTEST_H_INCLUDED__)
#define __SIGNATUREBESTEST_H_INCLUDED__

#include <cppunit/extensions/HelperMacros.h>

namespace digidoc
{
    class SignatureBESTest : public CppUnit::TestCase
    {

        CPPUNIT_TEST_SUITE(SignatureBESTest);
        //CPPUNIT_TEST(testFailingSignatures);
        CPPUNIT_TEST_SUITE_END();

      public:
          virtual void setUp();

      protected:
          void testFailingSignatures();

    };
}

#endif // !defined(__SIGNATUREBESTEST_H_INCLUDED__)
