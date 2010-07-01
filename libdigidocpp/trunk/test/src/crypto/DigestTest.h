/*
 * libdigidocpp - a C++ library for creating and validating BDOC-1.0 documents
 *
 * Copyright (C) 2009-2010  Estonian Informatics Centre
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

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
                  unsigned int size, unsigned char* emptyDigest, unsigned char* expectedDigest);

          static const std::string NAME_SHA1;
          static const std::string NAME_SHA256;

          std::vector<int> methods;
          std::vector<std::string> uris;

    };
}

#endif // !defined(__DIGESTTEST_H_INCLUDED__)
