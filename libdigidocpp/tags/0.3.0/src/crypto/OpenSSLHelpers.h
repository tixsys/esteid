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

#if !defined(__OPENSSLHELPERS_H_INCLUDED__)
#define __OPENSSLHELPERS_H_INCLUDED__

#include <openssl/bio.h>
#include <openssl/x509.h>

#define DECLARE_OPENSSL_RELEASE_STRUCT(type) \
struct type##_scope \
{ \
    type** p; \
    type##_scope(type** p) { this->p = p; } \
    ~type##_scope() { if(p && *p) { type##_free(*p); *p = NULL; } } \
};

#define DECLARE_OPENSSL_RELEASE_STACK(type) \
struct type##Stack_scope \
{ \
    STACK_OF(type)** p; \
    type##Stack_scope(STACK_OF(type)** p) { this->p = p; } \
    ~type##Stack_scope() { if(p && *p) { sk_##type##_free(*p); *p = NULL; } } \
};

DECLARE_OPENSSL_RELEASE_STRUCT(BIO)
DECLARE_OPENSSL_RELEASE_STRUCT(X509)
DECLARE_OPENSSL_RELEASE_STRUCT(RSA)
DECLARE_OPENSSL_RELEASE_STRUCT(X509_STORE)
DECLARE_OPENSSL_RELEASE_STRUCT(X509_STORE_CTX)

DECLARE_OPENSSL_RELEASE_STACK(X509)


#endif // !defined(__OPENSSLHELPERS_H_INCLUDED__)
