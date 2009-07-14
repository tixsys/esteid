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