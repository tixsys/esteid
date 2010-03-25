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

#include <openssl/objects.h>
#include <string.h>

#include <Conf.h>
#include <crypto/Digest.h>
#include <util/String.h>

#include "DigestTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(digidoc::DigestTest);

// Digests names.
const std::string digidoc::DigestTest::NAME_SHA1 = "SHA1";
const std::string digidoc::DigestTest::NAME_SHA256 = "SHA256";

// Digest URIs.
const std::string digidoc::DigestTest::URI_SHA1 = "http://www.w3.org/2000/09/xmldsig#sha1";
const std::string digidoc::DigestTest::URI_SHA256 = "http://www.w3.org/2001/04/xmlenc#sha256";


void digidoc::DigestTest::setUp()
{
    // SHA1
    methods.push_back(NID_sha1);
    uris.push_back(URI_SHA1);

    // SHA256
    methods.push_back(NID_sha256);
    uris.push_back(URI_SHA256);
}

void digidoc::DigestTest::testDigestFactories()
{
    // Test digest instance creation based on configuration.
    CPPUNIT_ASSERT_EQUAL(Conf::getInstance()->getDigestUri(), Digest::create()->getUri());

    // Test digest instance creation by method ID.
    for(unsigned int i = 0; i < methods.size(); i++)
    {
        CPPUNIT_ASSERT_EQUAL(methods[i], Digest::create(methods[i])->getMethod());
    }

    // Test digest instance creation by method URI.
    for(unsigned int i = 0; i < uris.size(); i++)
    {
        CPPUNIT_ASSERT_EQUAL(uris[i], Digest::create(uris[i])->getUri());
    }
}

void digidoc::DigestTest::testURIToMethod()
{
    // Test conversion from method URI to method ID.
    for(unsigned int i = 0; i < methods.size(); i++)
    {
        CPPUNIT_ASSERT_EQUAL(methods[i], Digest::toMethod(uris[i]));
    }
}

void digidoc::DigestTest::testSHA1Digest()
{
    unsigned char emptyDigest[] = {
            0xDA, 0x39, 0xA3, 0xEE, 0x5E, 0x6B, 0x4B, 0x0D,
            0x32, 0x55, 0xBF, 0xEF, 0x95, 0x60, 0x18, 0x90,
            0xAF, 0xD8, 0x07, 0x09 };

    unsigned char expectedDigest[] = {
            0xBE, 0x1B, 0xDE, 0xC0, 0xAA, 0x74, 0xB4, 0xDC,
            0xB0, 0x79, 0x94, 0x3E, 0x70, 0x52, 0x80, 0x96,
            0xCC, 0xA9, 0x85, 0xF8 };

    // Test digest getters and digest value calculation.
    testDigest(NID_sha1, NAME_SHA1, URI_SHA1, 20, emptyDigest, expectedDigest);
}

void digidoc::DigestTest::testSHA256Digest()
{
    unsigned char emptyDigest[] = {
            0xE3, 0xB0, 0xC4, 0x42, 0x98, 0xFC, 0x1C, 0x14,
            0x9A, 0xFB, 0xF4, 0xC8, 0x99, 0x6F, 0xB9, 0x24,
            0x27, 0xAE, 0x41, 0xE4, 0x64, 0x9B, 0x93, 0x4C,
            0xA4, 0x95, 0x99, 0x1B, 0x78, 0x52, 0xB8, 0x55 };

    unsigned char expectedDigest[] = {
            0x5D, 0xF6, 0xE0, 0xE2, 0x76, 0x13, 0x59, 0xD3,
            0x0A, 0x82, 0x75, 0x05, 0x8E, 0x29, 0x9F, 0xCC,
            0x03, 0x81, 0x53, 0x45, 0x45, 0xF5, 0x5C, 0xF4,
            0x3E, 0x41, 0x98, 0x3F, 0x5D, 0x4C, 0x94, 0x56 };

    // Test digest getters and digest value calculation.
    testDigest(NID_sha256, NAME_SHA256, URI_SHA256, 32, emptyDigest, expectedDigest);
}

void digidoc::DigestTest::testDigest(int method, const std::string& name, const std::string uri,
        int size, unsigned char* emptyDigest, unsigned char* expectedDigest)
{
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
}
