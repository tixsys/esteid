/*
 * esteid-browser-plugin - a browser plugin for Estonian EID card
 *
 * Copyright (C) 2010  Estonian Informatics Centre
 * Copyright (C) 2010  Smartlink OÜ
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

#include "converter.h"
#include "debug.h"
#include <iconv.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef ICONV_SECOND_ARGUMENT_IS_CONST
    #define ICONV_CONST const
#else
    #define ICONV_CONST
#endif

#if defined (__APPLE__) && defined (__LP64__)
#define LIBICONV_PLUG 1
#endif

std::string Converter::iconvConvert(const std::string & str_in, const char *tocode, const char *fromcode)
{
    iconv_t iso_utf;
    ICONV_CONST char *inptr;
    char *outptr;
    size_t inbytes, outbytes, result;
    char outbuf[128];
    std::string out;

    iso_utf = iconv_open(tocode, fromcode);
    if (iso_utf == (iconv_t)-1) {
        ESTEID_DEBUG("error in iconv_open");
        return str_in;
    }

    inptr = (ICONV_CONST char*)str_in.c_str();
    inbytes = str_in.size();

    outptr = outbuf;
    outbytes = sizeof(outbuf) - 1;

    result = iconv(iso_utf, &inptr, &inbytes, &outptr, &outbytes);
    if (result == (size_t)-1) {
        ESTEID_DEBUG("error converting with iconv");
        return str_in;
    }
    *outptr = '\0';

    iconv_close(iso_utf);

    out += outbuf;
    return out;
}


std::string Converter::CP1252_to_UTF8(const std::string & str_in)
{
    return iconvConvert(str_in, "UTF-8", "CP1252");
}


#ifdef _WIN32
std::string Converter::wstring_to_string(const std::wstring & in)
{
    int len = WideCharToMultiByte(CP_UTF8, 0, in.c_str(), -1, 0, 0, 0, 0);
    char *buf = new char[len];

    WideCharToMultiByte(CP_UTF8, 0, in.c_str(), -1, buf, len, 0, 0);
    std::string out(buf);
    delete[] buf;

    return out;
}


std::wstring Converter::string_to_wstring(const std::string & in)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, in.c_str(), -1, 0, 0);
    wchar_t *buf = new wchar_t[len];

    MultiByteToWideChar(CP_UTF8, 0, in.c_str(), -1, buf, len);
    std::wstring out(buf);
    delete[] buf;

    return out;
}
#endif //_WIN32
