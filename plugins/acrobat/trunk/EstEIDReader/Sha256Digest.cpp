/*
 * Copyright (C) 2009  Manuel Matonin <manuel@smartlink.ee>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */ 

#include "EstEIDIncludes.h"
#include "Sha256Digest.h"

ulong Sha256Digest::Init()
{
    memset( &m_ctx, 0, sizeof(m_ctx) );

    if( !SHA256_Init(&m_ctx) )
        return( ESTEID_ERR );

    return( ESTEID_OK );
}
 
ulong Sha256Digest::Update(uchar *data_buffer, ulong data_length)
{
    if( !SHA256_Update(&m_ctx, data_buffer, data_length) )
        return( ESTEID_ERR );

    return( ESTEID_OK );
}
 
ulong Sha256Digest::Final(uchar **digest_buffer, ulong *digest_length)
{
    if( !SHA256_Final(*digest_buffer, &m_ctx) )
        return( ESTEID_ERR );    

    *digest_length = SHA256_DIGEST_SIZE;

    return( ESTEID_OK );
}