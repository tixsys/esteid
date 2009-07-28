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
ESTEID_EXPORT_SPEC ulong Sign(HANDLE h, ulong slot, const char *pin, 
                              void *digest_buffer, ulong digest_length,
                              void **signature_buffer, ulong *signature_length);
ESTEID_EXPORT_SPEC ulong InitDigest(HANDLE h, size_t algorithm);
ESTEID_EXPORT_SPEC ulong UpdateDigest(HANDLE h, uchar *data_buffer, ulong data_length);
ESTEID_EXPORT_SPEC ulong FinalizeDigest(HANDLE h, uchar **digest_buffer, ulong *digest_length);
ESTEID_EXPORT_SPEC ulong LocateCertificate(HANDLE h, ulong slot, PKCS11Cert *cert);

#endif // #ifndef EST_EID_H_INCLUDED





