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
#include "Sha1Digest.h"
#include "Sha256Digest.h"
#include "EstEIDReader.h"
#include <stdlib.h>

EstEIDReader::EstEIDReader()
{
    m_slots = NULL;
    m_digest = NULL;
    m_digest_algorithm = (ulong)0xFFFF;
}

int EstEIDReader::Connect(const char *module)
{
    CK_RV rv;

    if( Initialized() )
        return( ESTEID_OK );

    rv = Open( module );

    return( rv );
}

int EstEIDReader::Disconnect()
{
    CK_RV rv;

    if( m_slots != NULL )
    {
        delete [] m_slots;
    }

    if( m_digest != NULL )
    {
        delete m_digest;
        m_digest = NULL;
    }

    rv = Close();

    return( rv );
}

CK_RV EstEIDReader::GetSlotCount(int token_present)
{
    CK_RV rv;
    ulong slots = 0L;

    if( m_slots )
        delete [] m_slots;

    rv = m_p11->C_GetSlotList( token_present, m_slots, &slots );
    if( rv != CKR_OK )
        return( rv );

    m_slots = new CK_SLOT_ID[slots];
    if( !m_slots )
         return( ESTEID_ERR );

    rv = m_p11->C_GetSlotList( token_present, m_slots, &slots );
    if( rv != CKR_OK )
        return( rv ); 

    return( slots );
}

CK_RV EstEIDReader::GetMechanisms(CK_SLOT_ID slot, CK_MECHANISM_TYPE_PTR *pList, CK_FLAGS flags, CK_ULONG *count)
{
    CK_ULONG m, n;
    CK_RV rv;

    rv = m_p11->C_GetMechanismList( slot, *pList, count );
    if( rv != CKR_OK )
        return( rv );

    *pList = (CK_MECHANISM_TYPE *)calloc( *count, sizeof(*pList) );
    if( *pList == NULL )
        return( 0 );

    rv = m_p11->C_GetMechanismList( slot, *pList, count );
    if( rv != CKR_OK )
    {
        free( *pList );
        return( 0 );
    }

    if( flags != (CK_FLAGS)-1 ) 
    {
        CK_MECHANISM_TYPE *mechs = *pList;
        CK_MECHANISM_INFO info;

        for (m = n = 0; n < *count; n++) 
        {
            rv = m_p11->C_GetMechanismInfo( slot, mechs[n], &info );
            if( rv != CKR_OK )
                continue;
            if( (info.flags & flags) == flags )
                mechs[m++] = mechs[n];
        }
        *count = m;
    }

    return( rv );
}

CK_RV EstEIDReader::IsMechanismSupported(ulong slot, ulong flag)
{
    CK_MECHANISM_TYPE *mechs = NULL;
    CK_ULONG n, num_mechs = 0;
    CK_RV rv = ESTEID_ERR;

    rv = GetMechanisms( m_slots[slot], &mechs, -1, &num_mechs );
    if( rv != CKR_OK )
        return( rv );

    for( n = 0; n < num_mechs; n++ ) 
    {
        CK_MECHANISM_INFO info;

        rv = m_p11->C_GetMechanismInfo( m_slots[slot], mechs[n], &info );
        if( rv == CKR_OK )
        {
            if( info.flags & flag )
                break;				
        }		
    }

    if( mechs != NULL )
        free( mechs );

    return( rv );
}

CK_RV EstEIDReader::GetTokenInfo(ulong slot, CK_TOKEN_INFO_PTR info)
{
    CK_RV rv;

    rv = m_p11->C_GetTokenInfo( m_slots[slot], info );

    return( rv );
}

CK_RV EstEIDReader::FindObject(CK_SESSION_HANDLE sess, 
                             CK_OBJECT_CLASS cls, 
                             CK_OBJECT_HANDLE_PTR ret,
                             const unsigned char *id,
                             size_t id_len, 
                             int obj_index)
{
    CK_ATTRIBUTE attrs[2];
    unsigned int nattrs = 0;
    CK_ULONG count;
    CK_RV rv;
    int i;

    attrs[0].type = CKA_CLASS;
    attrs[0].pValue = &cls;
    attrs[0].ulValueLen = sizeof(cls);
    nattrs++;
    
    if( id ) 
    {
        attrs[nattrs].type = CKA_ID;
        attrs[nattrs].pValue = (void *) id;
        attrs[nattrs].ulValueLen = id_len;
        nattrs++;
    }

    rv = m_p11->C_FindObjectsInit( sess, attrs, nattrs );
    if( rv != CKR_OK )
        return( rv );

    for( i = 0; i < obj_index; i++ ) 
    {
        rv = m_p11->C_FindObjects( sess, ret, 1, &count );
        if( rv != CKR_OK )
            return( rv );
        if( count == 0 )
            goto done;
    }

    rv = m_p11->C_FindObjects( sess, ret, 1, &count );
    if( rv != CKR_OK )
        return( rv );

done:	
    if( count == 0 )
        *ret = CK_INVALID_HANDLE;
    
    m_p11->C_FindObjectsFinal( sess );

    return( count );
}

CK_RV EstEIDReader::GetPkcs11Attr(CK_SESSION_HANDLE session,
           CK_OBJECT_HANDLE obj, CK_ATTRIBUTE_TYPE type, void *value,
           ulong *size)
{
    CK_ATTRIBUTE templ;
    CK_RV rv;

    templ.type = type;
    templ.pValue = value;
    templ.ulValueLen = *size;

    rv = m_p11->C_GetAttributeValue( session, obj, &templ, 1 );
    if( rv != CKR_OK )
        return( rv );

    *size = templ.ulValueLen;
    
    return( CKR_OK );
}

CK_RV EstEIDReader::Sign(ulong slot, const char *pin, 
                         uchar *digest_buffer, ulong digest_length,
                         uchar **signature_buffer, ulong *signature_length)
{    
    CK_MECHANISM mech;
    CK_RV rv;
    CK_SESSION_HANDLE session = CK_INVALID_HANDLE;
    CK_OBJECT_HANDLE key = CK_INVALID_HANDLE, cert = CK_INVALID_HANDLE;    
    
    int flags = CKF_SERIAL_SESSION;
    flags |= CKF_RW_SESSION;
    
    rv = m_p11->C_OpenSession( m_slots[slot], flags, NULL, NULL, &session );
    if( rv != CKR_OK )
        return( rv );

    rv = m_p11->C_Login( session, CKU_USER, (CK_UTF8CHAR *) pin, pin == NULL ? 0 : strlen(pin) );
    if( rv != CKR_OK )
    {
        m_p11->C_CloseSession( session );        
        return( rv );
    }

    rv = FindObject( session, CKO_PRIVATE_KEY, &key, NULL, 0, 0 );
    if( key == CK_INVALID_HANDLE )
    {
        m_p11->C_Logout( session );
        m_p11->C_CloseSession( session );
        return( rv );
    }

    memset( &mech, 0, sizeof(mech) );
    mech.mechanism = m_digest_algorithm;

    rv = m_p11->C_SignInit( session, &mech, key );
    if( rv != CKR_OK )
    {
        m_p11->C_Logout( session );
        m_p11->C_CloseSession( session );
        return( rv );
    }

    rv = m_p11->C_SignUpdate( session, digest_buffer, digest_length );
    if( rv != CKR_OK )
    {
        m_p11->C_Logout( session );
        m_p11->C_CloseSession( session );
        return( rv );
    }

    rv = m_p11->C_SignFinal( session, *signature_buffer, signature_length );
    if( rv != CKR_OK )
    {
        m_p11->C_Logout( session );
        m_p11->C_CloseSession( session );
        return( rv );
    }

    rv = m_p11->C_Logout( session );
    if( rv != CKR_OK )
        return( rv );

    rv = m_p11->C_CloseSession( session );
    if( rv != CKR_OK )
        return( rv );

    return( ESTEID_OK ); // Ok
}

CK_RV EstEIDReader::InitDigest(size_t algorithm)
{
    switch( algorithm )
    {
    case SHA1_ALGORITHM:
        m_digest_algorithm = CKM_SHA1_RSA_PKCS; 
        if( !m_digest )
            m_digest = new Sha1Digest();                
        break;
    case SHA256_ALGORITHM:
        m_digest_algorithm = CKM_SHA256_RSA_PKCS;
        if( !m_digest )
            m_digest = new Sha256Digest();
        break;
    default: 
        return( ESTEID_ERR );
    }
    
    if( !m_digest ) // out of memory ?
        return( ESTEID_ERR );

    return( m_digest->Init() );
}
 
CK_RV EstEIDReader::UpdateDigest(uchar *data_buffer, ulong data_length)
{
    if( !m_digest )
        return( ESTEID_ERR );

    return( m_digest->Update(data_buffer, data_length) ); 
}

CK_RV EstEIDReader::FinalizeDigest(uchar **digest_buffer, ulong *digest_length)
{
    if( !m_digest )
        return( ESTEID_ERR );

    return( m_digest->Final(digest_buffer, digest_length) ); 
}

CK_RV EstEIDReader::LocateCertificate(ulong slot, PKCS11Cert *cert)
{
    CK_RV rv;
    CK_SESSION_HANDLE session = CK_INVALID_HANDLE;
    CK_OBJECT_HANDLE obj = CK_INVALID_HANDLE; 
    CK_CERTIFICATE_TYPE cert_type;
    ulong size;
    int flags = CKF_SERIAL_SESSION;    

    rv = m_p11->C_OpenSession( m_slots[slot], flags, NULL, NULL, &session );
    if( rv != CKR_OK )
        return( rv );

    rv = FindObject( session, CKO_CERTIFICATE, &obj, NULL, 0, 0 );
    if( obj == CK_INVALID_HANDLE )
    {
        m_p11->C_CloseSession( session );
        return( rv ? rv : ESTEID_ERR );
    }

    size = sizeof(cert_type);
    rv = GetPkcs11Attr( session, obj, CKA_CERTIFICATE_TYPE, &cert_type, &size );
    if( cert_type != CKC_X_509 ) 
    {
        m_p11->C_CloseSession( session );
        return( rv ? rv : ESTEID_ERR );
    }

    memset( cert->certLabel, 0, cert->certLabelLength );
    rv = GetPkcs11Attr( session, obj, CKA_LABEL, cert->certLabel, &cert->certLabelLength );
    if( rv != CKR_OK ) 
    {
        m_p11->C_CloseSession( session );
        return( rv );
    }

    memset( cert->certBuffer, 0, cert->certBufferLength );
    rv = GetPkcs11Attr( session, obj, CKA_VALUE, cert->certBuffer, &cert->certBufferLength );
    if( rv != CKR_OK )
    {
        m_p11->C_CloseSession( session );
        return( rv );
    }

    rv = m_p11->C_CloseSession( session );
    if( rv != CKR_OK )
        return( rv );

    return( ESTEID_OK ); // Ok
}