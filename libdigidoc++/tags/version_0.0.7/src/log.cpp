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
