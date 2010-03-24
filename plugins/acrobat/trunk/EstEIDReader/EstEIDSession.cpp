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
#include "EstEIDSession.h"

EstEIDSession::EstEIDSession(void)
{
    m_module = NULL;
    m_p11 = NULL;
    m_initialized = 0;
}

CK_RV EstEIDSession::Open(const char *module)
{
    CK_RV rv = ESTEID_ERR;

    m_module = C_LoadModule( module, &m_p11 );
    if( m_module == NULL )
        return( ESTEID_ERR );

    rv = m_p11->C_Initialize( NULL );
    if( rv != CKR_OK )
    {
        m_p11->C_Finalize( NULL );
        C_UnloadModule( m_module );
        m_module = NULL;
        return( rv );
    }

    m_initialized = 1;

    return( rv );
}

CK_RV EstEIDSession::Close()
{
    CK_RV rv = ESTEID_ERR;

    if( m_p11 != NULL )
    {
        rv = m_p11->C_Finalize( NULL );
        C_UnloadModule( m_module );
    }

    m_initialized = 0;

    return( rv );
}
