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