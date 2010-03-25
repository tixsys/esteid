/*
 * Copyright (C) 2009-2010  Estonian Informatics Centre
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

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
    CK_RV GetSlotInfo(ulong slot, CK_SLOT_INFO_PTR info);
    CK_RV Sign(ulong slot, const char *pin, 
        uchar *digest_buffer, ulong digest_length,
        uchar **sdata, ulong *slength);
    CK_RV InitDigest(size_t algorithm);
    CK_RV UpdateDigest(uchar *data_buffer, ulong data_length);
    CK_RV FinalizeDigest(uchar **digest_buffer, ulong *digest_length);
    CK_RV LocateCertificate(ulong slot, PKCS11Cert *cert);
protected:
    CK_RV GetMechanisms(CK_SLOT_ID slot, CK_MECHANISM_TYPE_PTR *pList, CK_FLAGS flags, CK_ULONG *count);
    CK_RV FindObject(CK_SESSION_HANDLE sess,
        CK_OBJECT_CLASS cls, 
        CK_OBJECT_HANDLE_PTR ret,
        const unsigned char *id,
        size_t id_len, 
        int obj_index);    
    CK_RV GetPkcs11Attr(CK_SESSION_HANDLE session, CK_OBJECT_HANDLE o, CK_ATTRIBUTE_TYPE type, 
        void *value,
        ulong *size);
private:
    CK_SLOT_ID_PTR m_slots;
    BaseDigest *m_digest;
    ulong m_digest_algorithm;
};

#endif // #ifndef EST_EID_READER_H_INCLUDED