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