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

#ifndef EST_EID_H_INCLUDED
#define EST_EID_H_INCLUDED

#pragma once

#ifdef ESTEID_READER_EXPORTS
/* Specified that the function is an exported DLL entry point. */
#define ESTEID_EXPORT_SPEC extern "C" __declspec(dllexport)
#else
#define ESTEID_EXPORT_SPEC extern "C"
#endif

ESTEID_EXPORT_SPEC HANDLE OpenReader(char *module);
ESTEID_EXPORT_SPEC void CloseReader(HANDLE h);
ESTEID_EXPORT_SPEC ulong GetSlotCount(HANDLE h, int token_present);
ESTEID_EXPORT_SPEC ulong IsMechanismSupported(HANDLE h, ulong slot, ulong flag);
ESTEID_EXPORT_SPEC ulong GetTokenInfo(HANDLE h, ulong slot, CK_TOKEN_INFO_PTR info);
ESTEID_EXPORT_SPEC ulong GetSlotInfo(HANDLE h, ulong slot, CK_SLOT_INFO_PTR info);
ESTEID_EXPORT_SPEC ulong Sign(HANDLE h, ulong slot, const char *pin, 
                              void *digest_buffer, ulong digest_length,
                              void **signature_buffer, ulong *signature_length);
ESTEID_EXPORT_SPEC ulong InitDigest(HANDLE h, size_t algorithm);
ESTEID_EXPORT_SPEC ulong UpdateDigest(HANDLE h, uchar *data_buffer, ulong data_length);
ESTEID_EXPORT_SPEC ulong FinalizeDigest(HANDLE h, uchar **digest_buffer, ulong *digest_length);
ESTEID_EXPORT_SPEC ulong LocateCertificate(HANDLE h, ulong slot, PKCS11Cert *cert);

#endif // #ifndef EST_EID_H_INCLUDED





