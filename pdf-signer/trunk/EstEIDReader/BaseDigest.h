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

#ifndef DIGEST_LIB_H_INCLUDED
#define DIGEST_LIB_H_INCLUDED

#pragma once

#include <openssl/sha.h>

class BaseDigest
{
public:    
    virtual ulong Init() = 0;
    virtual ulong Update(uchar *data_buffer, ulong data_length) = 0;
    virtual ulong Final(uchar **digest_buffer, ulong *digest_length) = 0;
    virtual const ulong GetDigestLength() = 0;
};

#endif // #ifndef DIGEST_LIB_H_INCLUDED