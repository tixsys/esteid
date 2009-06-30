#ifdef WIN32
#include <windows.h>
#endif

#include "EstEIDIncludes.h"
#include "EstEID.h"
#include "EstEIDReader.h"

ESTEID_DECLARE_FUNCTION(HANDLE, OpenReader(char *module))
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

ESTEID_DECLARE_FUNCTION(void, CloseReader(HANDLE h))
{
    if( h )
    {
        EstEIDReader *obj = ((EstEIDReader*)h);
        obj->Disconnect();
        delete obj;
    }
}

ESTEID_DECLARE_FUNCTION(ulong, GetSlotCount(HANDLE h, int token_present))
{
    if( h )
    {
        EstEIDReader *obj = ((EstEIDReader*)h);
        return( obj->GetSlotCount(token_present) );
    }

    return( 0 );
}

ESTEID_DECLARE_FUNCTION(ulong, IsMechanismSupported(HANDLE h, ulong slot, ulong flag))
{
    if( h )
    {
        EstEIDReader *obj = ((EstEIDReader*)h);
        return( obj->IsMechanismSupported(slot, flag) );
    }

    return( ESTEID_ERR );
}

ESTEID_DECLARE_FUNCTION(ulong, GetTokenInfo(HANDLE h, ulong slot, CK_TOKEN_INFO_PTR info))
{
    if( h )
    {
        EstEIDReader *obj = ((EstEIDReader*)h);
        return( obj->GetTokenInfo(slot, info) );
    }

    return( ESTEID_ERR );
}

ESTEID_DECLARE_FUNCTION(ulong, Sign(HANDLE h, ulong slot, const char *pin, uchar *data, ulong dlength, 
                        void **hdata, ulong *hlength,
                        void **sdata, ulong *slength))
{
    if( h )
    {
        EstEIDReader *obj = ((EstEIDReader*)h);
        return( obj->Sign(slot, pin, data, dlength, (uchar**)hdata, hlength, (uchar**)sdata, slength) );
    }

    return( ESTEID_ERR );
}