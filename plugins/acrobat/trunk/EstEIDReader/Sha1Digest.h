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

#ifndef SHA1DIGEST_LIB_H_INCLUDED
#define SHA1DIGEST_LIB_H_INCLUDED

#pragma once

#include "BaseDigest.h"

#define SHA1_DIGEST_SIZE 20 

class Sha1Digest: public BaseDigest
{
public:
    Sha1Digest(void) {};
    ~Sha1Digest(void) {};
    ulong Init();
    ulong Update(uchar *data_buffer, ulong data_length);
    ulong Final(uchar **digest_buffer, ulong *digest_length);
    const ulong GetDigestLength() { return SHA1_DIGEST_SIZE; }    
protected:
    SHA_CTX m_ctx;    
};

#endif // #ifndef SHA1DIGEST_LIB_H_INCLUDED