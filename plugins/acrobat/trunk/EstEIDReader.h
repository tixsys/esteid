#pragma once

#include "EstEIDIncludes.h"

class EstEIDReader
{
public:
    EstEIDReader();
    int Connect(const char *module);
    int Disconnect();
    ulong GetSlotCount(int token_present);
    ulong IsMechanismSupported(ulong slot, ulong flag);
    ulong GetTokenInfo(ulong slot, CK_TOKEN_INFO_PTR info);
    ulong Sign(ulong slot, const char *pin, uchar *data, ulong dlength, 
        uchar **hdata, ulong *hlength,
        uchar **sdata, ulong *slength);
protected:
    ulong GetMechanisms(CK_SLOT_ID slot, CK_MECHANISM_TYPE_PTR *pList, CK_FLAGS flags);
    int FindObject(CK_SESSION_HANDLE sess, 
                             CK_OBJECT_CLASS cls, 
                             CK_OBJECT_HANDLE_PTR ret,
                             const unsigned char *id,
                             size_t id_len, 
                             int obj_index);
    CK_RV LocatePrivateKey(CK_SESSION_HANDLE hSession, char idData[20][20], CK_ULONG idLen[20], CK_OBJECT_HANDLE_PTR hKeys);
    CK_OBJECT_HANDLE LocateCertificate(CK_SESSION_HANDLE hSession, CK_BYTE_PTR certData, CK_ULONG_PTR certLen, 
				   char idData[20][20], CK_ULONG idLen[20], int* pSelKey);
private:
    void *m_module;
    CK_FUNCTION_LIST_PTR m_p11;
    CK_SLOT_ID_PTR m_p11_slots;
};