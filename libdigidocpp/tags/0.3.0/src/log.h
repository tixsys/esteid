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

#if !defined(__LOG_H_INCLUDED__)
#define __LOG_H_INCLUDED__
#pragma once

#include <stdio.h>

#undef LOG_LEVEL_ERROR
#undef LOG_LEVEL_WARN
#undef LOG_LEVEL_INFO
#undef LOG_LEVEL_DEBUG
#undef LOG_USE_COLORS

#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_WARN  1
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_DEBUG 0
#define LOG_USE_COLORS  0

#undef ERR
#undef WARN
#undef INFO
#undef DEBUG

// for silencing warnings in do/while(0) multiline macros on MS compilers
// while(0) causes warning C4127 "conditional expression is constant"
// throwing from cycle causes warning C4702: "unreachable code" about while(0) being out of reach
namespace fix
{
    inline bool never() {return false;}  // use as while(fix::never())
    inline bool always() {return true;}  // use as if(fix::always()) throw ...
}

#if defined (LOG_LEVEL_ERROR) && LOG_LEVEL_ERROR > 0
#   if defined (LOG_USE_COLORS) && LOG_USE_COLORS > 0
#       define ERR(...) do {printf("\033[31mERROR [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\033[0m\n");} while(0)
#   else
#       define ERR(...) do { printf("ERROR [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\n");} while(fix::never())
#   endif
#else
#   define ERR(...)
#endif


#if defined (LOG_LEVEL_WARN) && LOG_LEVEL_WARN > 0
#   if defined (LOG_USE_COLORS) && LOG_USE_COLORS > 0
#       define WARN(...) do {printf("\033[33mWARN  [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\033[0m\n");} while(0)
#   else
#       define WARN(...) do {printf("WARN  [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\n");} while(fix::never())
#   endif
#else
#   define WARN(...)
#endif


#if defined (LOG_LEVEL_INFO) && LOG_LEVEL_INFO > 0
#   if defined (LOG_USE_COLORS) && LOG_USE_COLORS > 0
#       define INFO(...) do {printf("\033[36mINFO  [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\033[0m\n");} while(0)
#   else
#       define INFO(...) do {printf("INFO  [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\n");} while(fix::never())
#   endif
#       else
#   define INFO(...)
#endif


#if defined (LOG_LEVEL_DEBUG) && LOG_LEVEL_DEBUG > 0
#   if defined (LOG_USE_COLORS) && LOG_USE_COLORS > 0
//#       define DEBUG(...) do {printf("\033[32mDEBUG [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\033[0m\n");} while(0)
#       define DEBUG(...) do {printf("DEBUG [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\n");} while(0)
#   else
#       define DEBUG(...) do {printf("DEBUG [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\n");} while(0)
#   endif
#else
#   define DEBUG(...)
#endif


#if defined (LOG_LEVEL_DEBUG) && LOG_LEVEL_DEBUG > 0
#   define DEBUGMEM(msg, ptr, size) dbgPrintfMemImpl(msg, ptr, size, __FILE__, __LINE__)
    void dbgPrintfMemImpl(char const * msg, void const * ptr, int size, char const* file, int line);
    void HD(unsigned char *p,int n);
#else
#   define DEBUGMEM(msg, ptr, size)
#endif

#endif // !defined(__LOG_H_INCLUDED__)
