/*
 * esteid-csp - CSP for Estonian EID card
 *
 * Copyright (C) 2008-2010  Estonian Informatics Centre
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

#ifdef _WIN64
#define PKG_BIT " ( 64-bit )"
#else
#define PKG_BIT ""
#endif
#ifdef _DEBUG
#ifdef AVOID_SERVICE_LOAD
#define PKG_BUILD " ( debug - doesnt load in services)"
#else
#define PKG_BUILD " ( debug )"
#endif
#else
#define PKG_BUILD ""
#endif

/* Define to the full name of this package. */
#define PACKAGE_NAME "EstEID CSP" PKG_BIT PKG_BUILD

/* rc version */
#define RC_VERSION 2,2,19

/* Define to the version of this package. */
#define PACKAGE_VERSION "2.2.19"

/* Name of package */
#define PACKAGE "esteidcsp.dll"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING PACKAGE_NAME " " PACKAGE_VERSION

#define CRYPT_SIG_RESOURCE_NUMBER        0x29A
#define CRYPT_SIG_RESOURCE        TEXT("#666")
