#ifndef EST_EID_INCLUDES_H_INCLUDED
#define EST_EID_INCLUDES_H_INCLUDED

#pragma once

#ifndef ulong
#define ulong unsigned long
#endif

#ifndef uchar
#define uchar unsigned char
#endif

#define ESTEID_OK 0
#define ESTEID_ERR 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <opensc/pkcs11.h>
#include <opensc/pkcs11-opensc.h>
#include <compat_getpass.h>

#ifdef ENABLE_OPENSSL
#include "openssl/evp.h"
#include "openssl/x509.h"
#include "openssl/rsa.h"
#include "openssl/bn.h"
#include "openssl/err.h"
#endif

#if defined(__cplusplus)
extern "C" {
#endif

//extern "C"
//{
    void *C_LoadModule(const char *name, CK_FUNCTION_LIST_PTR_PTR);
    CK_RV C_UnloadModule(void *module);
//}

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