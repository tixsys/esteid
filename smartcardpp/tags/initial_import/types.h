#ifndef _smartcard_pp_types_h
#define _smartcard_pp_types_h

#ifndef UNUSED_ARG
#define UNUSED_ARG(x) (void)x
#endif

typedef unsigned int uint;
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;
typedef unsigned short ushort;
/// shorthand for std::vector<unsigned char>, used everywhere
typedef	std::vector<byte> ByteVec;
/// ostream printer
std::ostream& operator<<(std::ostream &out,const ByteVec &vec);


#endif
