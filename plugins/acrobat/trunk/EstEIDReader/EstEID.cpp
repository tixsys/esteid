#ifdef WIN32
#include <windows.h>
#endif

#include "EstEIDIncludes.h"
#include "EstEID.h"
#include "EstEIDReader.h"

ESTEID_EXPORT_SPEC HANDLE OpenReader(char *module)
{
    EstEIDReader *h = new EstEIDReader();

    if( h == NULL )
        return( NULL );

    if( h->Connect(module) )
    {
        delete h;
        return( NULL );
    }    

    return( h );
}

ESTEID_EXPORT_SPEC void CloseReader(HANDLE h)
{
    if( h )
    {
        EstEIDReader *obj = ((EstEIDReader*)h);
        obj->Disconnect();
        delete obj;
    }
}

ESTEID_EXPORT_SPEC ulong GetSlotCount(HANDLE h, int token_present)
{
    if( h )
    {
        EstEIDReader *obj = ((EstEIDReader*)h);
        return( obj->GetSlotCount(token_present) );
    }

    return( 0 );
}

ESTEID_EXPORT_SPEC ulong IsMechanismSupported(HANDLE h, ulong slot, ulong flag)
{
    if( h )
    {
        EstEIDReader *obj = ((EstEIDReader*)h);
        return( obj->IsMechanismSupported(slot, flag) );
    }

    return( ESTEID_ERR );
}

ESTEID_EXPORT_SPEC ulong GetTokenInfo(HANDLE h, ulong slot, CK_TOKEN_INFO_PTR info)
{
    if( h )
    {
        EstEIDReader *obj = ((EstEIDReader*)h);
        return( obj->GetTokenInfo(slot, info) );
    }

    return( ESTEID_ERR );
}

ESTEID_EXPORT_SPEC ulong Sign(HANDLE h, ulong slot, const char *pin, 
                        void *digest_buffer, ulong digest_length,
                        void **signature_buffer, ulong *signature_length)
{
    if( h )
    {
        EstEIDReader *obj = ((EstEIDReader*)h);
        return( obj->Sign(slot, pin, (uchar*)digest_buffer, digest_length, 
            (uchar**)signature_buffer, signature_length) );
    }

    return( ESTEID_ERR );
}

ESTEID_EXPORT_SPEC ulong InitDigest(HANDLE h, size_t algorithm)
{
    if( h )
    {
        EstEIDReader *obj = ((EstEIDReader*)h);
        return( obj->InitDigest(algorithm) );
    }

    return( ESTEID_ERR );
}

ESTEID_EXPORT_SPEC ulong UpdateDigest(HANDLE h, uchar *data_buffer, ulong data_length)
{
    if( h )
    {
        EstEIDReader *obj = ((EstEIDReader*)h);
        return( obj->UpdateDigest(data_buffer, data_length) );
    }

    return( ESTEID_ERR );
}

ESTEID_EXPORT_SPEC ulong FinalizeDigest(HANDLE h, uchar **digest_buffer, ulong *digest_length) 
{
    if( h )
    {
        EstEIDReader *obj = ((EstEIDReader*)h);
        return( obj->FinalizeDigest(digest_buffer, digest_length) );

    }

    return( ESTEID_ERR );
}

ESTEID_EXPORT_SPEC ulong LocateCertificate(HANDLE h, ulong slot, PKCS11Cert *cert)
{
    if( h )
    {
        EstEIDReader *obj = ((EstEIDReader*)h);
        return( obj->LocateCertificate(slot, cert) );
    }

     return( ESTEID_ERR );
}