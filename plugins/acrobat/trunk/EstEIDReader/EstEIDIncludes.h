#ifndef EST_EID_INCLUDES_H_INCLUDED
#define EST_EID_INCLUDES_H_INCLUDED

#pragma once

#ifndef ulong
#define ulong unsigned long
#endif

#ifndef uchar
#define uchar unsigned char
#endif

#ifndef WIN32
#define HANDLE void*
#endif

#define ESTEID_OK 0
#define ESTEID_ERR 1

#include <string.h>
#include <opensc/pkcs11.h>

#if defined(__cplusplus)
extern "C" {
#endif

void *C_LoadModule(const char *name, CK_FUNCTION_LIST_PTR_PTR);
CK_RV C_UnloadModule(void *module);

struct PKCS11Cert
{
    uchar *certBuffer;
    ulong certBufferLength;
    uchar *certLabel;
    ulong certLabelLength;
};

#if defined(__cplusplus)
}
#endif

#endif // #ifndef EST_EID_INCLUDES_H_INCLUDED