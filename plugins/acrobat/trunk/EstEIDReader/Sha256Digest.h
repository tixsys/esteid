/*
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

#ifndef SHA256DIGEST_LIB_H_INCLUDED
#define SHA256DIGEST_LIB_H_INCLUDED

#pragma once

#include "BaseDigest.h"

#define SHA256_DIGEST_SIZE 32

class Sha256Digest: public BaseDigest
{
public:
    Sha256Digest(void) {};
    ~Sha256Digest(void) {};
    ulong Init();
    ulong Update(uchar *data_buffer, ulong data_length);
    ulong Final(uchar **digest_buffer, ulong *digest_length);
    const ulong GetDigestLength() { return SHA256_DIGEST_SIZE; }    
protected:
    SHA256_CTX m_ctx;
};
#endif // #ifndef SHA256DIGEST_LIB_H_INCLUDED
