/*
 * libdigidocpp - a C++ library for creating and validating BDOC-1.0 documents
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

#include "log.h"
#include <stdio.h>

#if defined (LOG_LEVEL_DEBUG) && LOG_LEVEL_DEBUG > 0
void dbgPrintfMemImpl(char const * msg, void const * ptr, int size, char const * file, int line)
{
    printf("%s:%d - ", file, line);
    printf("%s{",msg);

    HD((unsigned char*)ptr, size);
    printf("}:%d\n", size);
}

/**
 * HexDump
 */
void HD(unsigned char *p,int n)
{
int i=0,j;
unsigned char *s=p;
  while (n--) {
    if (!(i&15)) {
      if (i) {
        s-=16; printf("  ");
        for (j=0;j<16;j++) {
          printf("%c",(*s>=' ' && *s<=0x7f)?*s:'.'); s++;
        }
      }
      printf("\n"); i=0;
    }
    printf("%02x ",*s++); i++;
  }
  if (i) {
    j=16-i; printf("%*s",j*3+2,"");
    j=i; s-=j;
    while (j--) {
      printf("%c",(*s>=' ' && *s<=0x7f)?*s:'.'); s++;
    }
  }
  printf("\n");
}
#endif
