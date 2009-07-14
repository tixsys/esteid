#ifndef DIGEST_LIB_H_INCLUDED
#define DIGEST_LIB_H_INCLUDED

#pragma once

class BaseDigest
{
public:    
    virtual ulong Init() = 0;
    virtual ulong Update(uchar *data_buffer, ulong data_length) = 0;
    virtual ulong Final(uchar **digest_buffer, ulong *digest_length) = 0;
    virtual const ulong GetDigestLength() = 0;
};

#endif // #ifndef DIGEST_LIB_H_INCLUDED