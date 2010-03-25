/*
 * ooo-digidoc - OpenOffice.org digital signing extension
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
 */

#include <stdio.h>
#include <stdarg.h>

#define DEBUG 0

#ifdef _WIN32
#define DEBUG_FILE "c:\\OOoDebug.txt"
#define UNO_URL_HEAD "file:///"
#define SLASH "/"

#else
#define UNO_URL_HEAD "file://"
#define SLASH ""
#define DEBUG_FILE "/tmp/OOoDebug.txt"

#endif

#if DEBUG
#define PRINT_DEBUG(...)									\
	do {													\
			FILE *fp;										\
			fp = fopen(DEBUG_FILE, "a");					\
			fprintf(fp, "[%s:%d] in Function: %s\n",__FILE__, __LINE__, __FUNCTION__);	\
			fprintf(fp, __VA_ARGS__);						\
			fprintf(fp, "\n\n");							\
			fclose(fp);										\
        } while (0)											
#else
#define PRINT_DEBUG(...)           do { } while (0)
#endif

