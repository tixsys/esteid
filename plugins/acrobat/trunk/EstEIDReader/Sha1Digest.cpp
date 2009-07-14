#include "EstEIDIncludes.h"
#include "Sha1Digest.h"

ulong Sha1Digest::Init()
{
    memset( &m_ctx, 0, sizeof(m_ctx) );

    if( !SHA1_Init(&m_ctx) )
        return( ESTEID_ERR );

    return( ESTEID_OK );
}
 
ulong Sha1Digest::Update(uchar *data_buffer, ulong data_length)
{
    if( !SHA1_Update(&m_ctx, data_buffer, data_length) )
        return( ESTEID_ERR );

    return( ESTEID_OK );
}
 
ulong Sha1Digest::Final(uchar **digest_buffer, ulong *digest_length)
{
    if( !SHA1_Final(*digest_buffer, &m_ctx) )
        return( ESTEID_ERR );    

    *digest_length = SHA1_DIGEST_SIZE;

    return( ESTEID_OK );
}