#ifndef EST_EID_READER_H_INCLUDED
#define EST_EID_READER_H_INCLUDED

#pragma once

class BaseDigest;

#include "EstEIDSession.h"

enum HASH_ALGO { SHA1_ALGORITHM, SHA256_ALGORITHM };

class EstEIDReader: public EstEIDSession
{
public:
    EstEIDReader();
    int Connect(const char *module);
    int Disconnect();
    CK_RV GetSlotCount(int token_present);
    CK_RV IsMechanismSupported(ulong slot, ulong flag);
    CK_RV GetTokenInfo(ulong slot, CK_TOKEN_INFO_PTR info);    
    CK_RV Sign(ulong slot, const char *pin, 
        uchar *digest_buffer, ulong digest_length,
        uchar **sdata, ulong *slength);
    CK_RV InitDigest(size_t algorithm);
    CK_RV UpdateDigest(uchar *data_buffer, ulong data_length);
    CK_RV FinalizeDigest(uchar **digest_buffer, ulong *digest_length);
    CK_RV LocateCertificate(ulong slot, PKCS11Cert *cert);
protected:
    CK_RV GetMechanisms(CK_SLOT_ID slot, CK_MECHANISM_TYPE_PTR *pList, CK_FLAGS flags);
    CK_RV FindObject(CK_SESSION_HANDLE sess,
        CK_OBJECT_CLASS cls, 
        CK_OBJECT_HANDLE_PTR ret,
        const unsigned char *id,
        size_t id_len, 
        int obj_index);
    CK_MECHANISM_TYPE FindMechanism(CK_SLOT_ID slot, CK_FLAGS flags);    
    CK_RV GetPkcs11Attr(CK_SESSION_HANDLE session, CK_OBJECT_HANDLE o, CK_ATTRIBUTE_TYPE type, 
        void *value,
        ulong *size);
private:
    CK_SLOT_ID_PTR m_p11_slots;
    BaseDigest *m_digest;
    ulong m_digest_algorithm;
};

#endif // #ifndef EST_EID_READER_H_INCLUDED