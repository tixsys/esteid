#include "EstEIDIncludes.h"
#include "EstEIDReader.h"

EstEIDReader::EstEIDReader()
{
    m_module = NULL;
    m_p11 = NULL;
    m_p11_slots = NULL;
}

int EstEIDReader::Connect(const char *module)
{
    CK_RV rv;

    m_module = C_LoadModule( module, &m_p11 );
    if( m_module == NULL )
        return( 1 );

    rv = m_p11->C_Initialize( NULL );
    if( rv != CKR_OK )
    {
        C_UnloadModule( m_module );
        return( rv );
    }

    return( 0 );
}

int EstEIDReader::Disconnect()
{
    if( m_module != NULL )
        C_UnloadModule( m_module );

    return( 0 );
}

ulong EstEIDReader::GetSlotCount(int token_present)
{
    CK_RV rv;
    ulong slots = 0L;

    rv = m_p11->C_GetSlotList( token_present, m_p11_slots, &slots );
    if( rv != CKR_OK )
        return( rv );    

    return( slots );
}

ulong EstEIDReader::GetMechanisms(CK_SLOT_ID slot, CK_MECHANISM_TYPE_PTR *pList, CK_FLAGS flags)
{
    CK_ULONG	m, n, ulCount;
    CK_RV		rv;

    rv = m_p11->C_GetMechanismList( slot, *pList, &ulCount );
    *pList = (CK_MECHANISM_TYPE *)calloc( ulCount, sizeof(*pList) );
    if( *pList == NULL )
        return( 0 );

    rv = m_p11->C_GetMechanismList( slot, *pList, &ulCount );
    if( rv != CKR_OK )
    {
        free( *pList );
        return( 0 );
    }

    if( flags != (CK_FLAGS)-1 ) 
    {
        CK_MECHANISM_TYPE *mechs = *pList;
        CK_MECHANISM_INFO info;

        for (m = n = 0; n < ulCount; n++) 
        {
            rv = m_p11->C_GetMechanismInfo(slot, mechs[n], &info);
            if (rv != CKR_OK)
                continue;
            if ((info.flags & flags) == flags)
                mechs[m++] = mechs[n];
        }
        ulCount = m;
    }

    return( ulCount );
}

ulong EstEIDReader::IsMechanismSupported(ulong slot, ulong flag)
{
    CK_MECHANISM_TYPE *mechs = NULL;
    CK_ULONG n, num_mechs = 0;
    CK_RV rv = 1;

    num_mechs = GetMechanisms( slot, &mechs, -1 );

    for( n = 0; n < num_mechs; n++ ) 
    {
        CK_MECHANISM_INFO info;

        rv = m_p11->C_GetMechanismInfo( slot, mechs[n], &info );
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

ulong EstEIDReader::GetTokenInfo(ulong slot, CK_TOKEN_INFO_PTR info)
{
    CK_RV rv;

    rv = m_p11->C_GetTokenInfo( slot, info );
    
    return( rv );
}

int EstEIDReader::FindObject(CK_SESSION_HANDLE sess, 
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
        return( 0 );

    for( i = 0; i < obj_index; i++ ) 
    {
        rv = m_p11->C_FindObjects( sess, ret, 1, &count );
        if( rv != CKR_OK )
            return( 0 );
        if( count == 0 )
            goto done;
    }

    rv = m_p11->C_FindObjects( sess, ret, 1, &count );
    if( rv != CKR_OK )
        return( 0 );

done:	
    if( count == 0 )
        *ret = CK_INVALID_HANDLE;
    
    m_p11->C_FindObjectsFinal( sess );

    return( count );
}

CK_OBJECT_HANDLE EstEIDReader::LocateCertificate(CK_SESSION_HANDLE hSession, 
                   CK_BYTE_PTR certData, CK_ULONG_PTR certLen, 
                   char idData[20][20], CK_ULONG idLen[20],
                   int* pSelKey)
{
    CK_OBJECT_HANDLE Objects[10];
    CK_ULONG ulObjectCount = sizeof(Objects)/sizeof(CK_OBJECT_HANDLE), i, j;
    CK_BYTE buf1[20];
    CK_OBJECT_HANDLE hCert = CK_INVALID_HANDLE;
    CK_RV rv;

    // Set up a template to search for Certificate token objects
    // in the given session and thus slot
    CK_OBJECT_CLASS ObjClass = CKO_CERTIFICATE;	
    CK_ATTRIBUTE Template1[] = {
        { CKA_CLASS,     &ObjClass,  sizeof(ObjClass) },
        { CKA_ID,	     (void*)0,  0 }
    };
    CK_ATTRIBUTE Template3[] = {
        { CKA_CLASS,     &ObjClass,  sizeof(ObjClass) },
    };
    CK_ATTRIBUTE Template2[] = {
        { CKA_VALUE,      (void*)0,	*certLen  },
        { CKA_ID,	     (void*)0,  0 }
    };
    CK_ULONG ulCount = 0;

    if( idLen )
        ulCount = sizeof(Template1)/sizeof(CK_ATTRIBUTE);
    else
        ulCount = sizeof(Template3)/sizeof(CK_ATTRIBUTE);
    *certLen = 0;

    for( j = 0; j < 20 && !(*certLen); j++ ) 
    {
        if(idLen && idLen[j]) {
            memset(buf1, 0, sizeof(buf1));
            memcpy(buf1, idData[j], idLen[j]);
            Template1[1].pValue = buf1;
            Template1[1].ulValueLen = idLen[j];
        }
        rv = m_p11->C_FindObjectsInit( hSession, (idLen ? Template1 : Template3), 1 ); 
        if( rv == CKR_OK )
        {
            rv =  m_p11->C_FindObjects( hSession, Objects,ulObjectCount, &ulObjectCount );
            if( rv == CKR_OK )
            {
                // pick the first cert that is valid
                // list and ignore any other
                for(i = 0; i < ulObjectCount; i++) 
                {
                    hCert = Objects[i];
                    memset(certData, 0, *certLen);
                    ulCount = sizeof(Template2) / sizeof(CK_ATTRIBUTE);
                    // get cert length
                    *certLen = 0;
                    Template2[1].pValue = buf1;
                    Template2[1].ulValueLen = sizeof(buf1);
                    rv =  m_p11->C_GetAttributeValue( hSession, hCert, Template2, ulCount );                    
                    if( rv == CKR_OK && (!idLen ||
                        (idLen && !memcmp(idData[j], buf1, idLen[1]))) ) 
                    {
                            *certLen = Template2[0].ulValueLen;
                            // now get cert data
                            Template2[0].pValue = certData;
                            rv =  m_p11->C_GetAttributeValue( hSession, hCert, Template2, ulCount );                            
                            if( *certLen > 0 && pSelKey )
                            {
                                *pSelKey = j;
                                break; // found it
                            }

                    } // if rv == CKR_OK
                } // for i < ulObjectCount
            } // if rv
        } // if rv
        rv =  m_p11->C_FindObjectsFinal( hSession );
    } // for j
    if( hCert == CK_INVALID_HANDLE )
        *certLen = 0;
    
    return( hCert );
}

CK_RV EstEIDReader::LocatePrivateKey(CK_SESSION_HANDLE hSession, char idData[20][20], CK_ULONG idLen[20], CK_OBJECT_HANDLE_PTR hKeys)
{
    CK_OBJECT_HANDLE Objects[10];
    CK_RV rv;
    CK_ULONG ulObjectCount = sizeof(Objects)/sizeof(CK_OBJECT_HANDLE), i;
    CK_OBJECT_HANDLE hPrivateKey = CK_INVALID_HANDLE;
    // Set up a template to search for all Private Key tokens 
    // Given the session context, that is associated with
    // one slot we will find only one object
    CK_OBJECT_CLASS ObjClass = CKO_PRIVATE_KEY;
    char buf1[20];
    CK_ATTRIBUTE Template1[] = {
        { CKA_CLASS,            &ObjClass,  sizeof(ObjClass)    }
    };	
    CK_ATTRIBUTE Template2[] = {
        { CKA_ID,      (void*)0,	idLen[0] }
    };
    CK_ULONG ulCount = sizeof(Template1) / sizeof(CK_ATTRIBUTE);

    rv = m_p11->C_FindObjectsInit( hSession,Template1,ulCount );
    if(rv==CKR_OK) 
    {
        // Get list of object handles
        rv = m_p11->C_FindObjects( hSession,Objects,ulObjectCount, &ulObjectCount );
        if( rv == CKR_OK )
        {
            // get labels of all possible private keys
            for(i = 0; i < ulObjectCount; i++) 
            {
                hKeys[i] = Objects[i];
                ulCount = sizeof(Template2) / sizeof(CK_ATTRIBUTE);
                // get key id length
                rv = m_p11->C_GetAttributeValue( hSession, hKeys[i], Template2, ulCount );
                if( rv == CKR_OK ) 
                {
                    idLen[i] = Template2[0].ulValueLen;
                    // now get key id data
                    Template2[0].pValue = buf1;
                    memset(buf1, 0, sizeof(buf1));
                    rv = m_p11->C_GetAttributeValue( hSession, hKeys[i], Template2, ulCount );                    
                    memcpy( idData[i], buf1, idLen[i] );
                }
            } // for i < ulObjectsCount
        }
    }
    
    rv = m_p11->C_FindObjectsFinal( hSession );
    return rv;  
}

ulong EstEIDReader::Sign(ulong slot, const char *pin, uchar *data, ulong dlength, 
                         uchar **hdata, ulong *hlength,
                         uchar **sdata, ulong *slength)
{    
    CK_MECHANISM mech;
    CK_RV rv;
    CK_SESSION_HANDLE session = CK_INVALID_HANDLE;
    CK_OBJECT_HANDLE key = CK_INVALID_HANDLE;
    
    int flags = CKF_SERIAL_SESSION;
    flags |= CKF_RW_SESSION;
    
    rv = m_p11->C_OpenSession( slot, flags, NULL, NULL, &session );
    if( rv != CKR_OK )
        return( ESTEID_ERR );

    rv = m_p11->C_Login( session, CKU_USER, (CK_UTF8CHAR *) pin, pin == NULL ? 0 : strlen(pin) );
    if( rv != CKR_OK )
        return( ESTEID_ERR );

    if( !FindObject(session, CKO_PRIVATE_KEY, &key, NULL, 0, 0) )
    {
        m_p11->C_Logout( session );
        return( ESTEID_ERR );
    }

    //rv = LocatePrivateKey( session, keyId, keyIdLen, hKeys );
    
    // get SHA1 digest    
    if( *hlength >= SHA_DIGEST_LENGTH )
        *hlength = SHA_DIGEST_LENGTH;

    SHA1( data, dlength, *hdata );    

    memset( &mech, 0, sizeof(mech) );
    mech.mechanism = CKM_SHA1_RSA_PKCS;

    rv = m_p11->C_SignInit( session, &mech, key );
    if( rv != CKR_OK )
    {
        m_p11->C_Logout( session );
        return( ESTEID_ERR );
    }

    rv = m_p11->C_SignUpdate( session, *hdata, *hlength );    
    if( rv != CKR_OK )
    {
        m_p11->C_Logout( session );
        return( ESTEID_ERR );
    }

    rv = m_p11->C_SignFinal( session, *sdata, slength );
    if( rv != CKR_OK )
    {
        m_p11->C_Logout( session );
        return( ESTEID_ERR );
    }

    rv = m_p11->C_Logout( session );
    if( rv != CKR_OK )
        return( ESTEID_ERR );

    return( ESTEID_OK ); // Ok
}