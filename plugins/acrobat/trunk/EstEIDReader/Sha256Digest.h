#ifndef SHA256DIGEST_LIB_H_INCLUDED
#define SHA256DIGEST_LIB_H_INCLUDED

#pragma once

#include "BaseDigest.h"

#define SHA256_DIGEST_SIZE 32

class Sha256Digest: public BaseDigest
{
public:
    Sha256Digest(void) {};
    ~Sha256Digest(void) {};
    ulong Init();
    ulong Update(uchar *data_buffer, ulong data_length);
    ulong Final(uchar **digest_buffer, ulong *digest_length);
    const ulong GetDigestLength() { return SHA256_DIGEST_SIZE; }    
protected:
    SHA256_CTX m_ctx;
};
#endif // #ifndef SHA256DIGEST_LIB_H_INCLUDED
