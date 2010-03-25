/*
 * QDigiDocCrypt
 *
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
 *
 */

#define MAJOR_VER	0
#define MINOR_VER	1
#define RELEASE_VER	0

#ifndef BUILD_VER
#define BUILD_VER	0
#endif

#define FILE_VER	MAJOR_VER,MINOR_VER,RELEASE_VER,BUILD_VER
#define FILE_VER_DOT	MAJOR_VER.MINOR_VER.RELEASE_VER.BUILD_VER
#define VER_STR_HELPER(x)	#x
#define VER_STR(x)	VER_STR_HELPER(x)

#define ORG "Estonian ID Card"
#define APP "DigiDoc krüpto"
#define DOMAINURL "eesti.ee"
