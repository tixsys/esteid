#include "String.h"

#include "../log.h"

#include <memory.h>
#include <stdlib.h>
#include <errno.h>

#if defined(_WIN32)
#include <windows.h>
#elif !defined(__APPLE__)
#include <iconv.h>
#endif

#ifdef ICONV_SECOND_ARGUMENT_IS_CONST
     #define ICONV_CONST const
#else
     #define ICONV_CONST
#endif


/**
 * Formats string, use same syntax as <code>printf()</code> function.
 * Example implementation from:
 * http://www.senzee5.com/2006/05/c-formatting-stdstring.html
 *
 * @param fmt format of the string. Uses same formating as <code>printf()</code> function.
 * @param ... parameters for the string format.
 * @return returns formatted string.
 */
std::string digidoc::util::String::format(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    std::string s = formatArgList(fmt, args);
    va_end(args);
    return s;
}

/**
 * Helper method for string formatting.
 *
 * @param fmt format of the string. Uses same formating as <code>printf()</code> function.
 * @param args parameters for the string format.
 * @return returns formatted string.
 * @see Util::String::format(const char* fmt, ...)
 */
std::string digidoc::util::String::formatArgList(const char* fmt, va_list args)
{
    if(!fmt)
    {
        return "";
    }

    int result = -1;
    int length = 256;
    char* buffer = 0;
    while(result == -1)
    {
        if (buffer) delete [] buffer;
        buffer = new char [length + 1];
        memset(buffer, 0, length + 1);
        result = vsnprintf(buffer, length, fmt, args);
        length *= 2;
    }

    std::string s(buffer);
    delete [] buffer;
    return s;
}

#ifdef _WIN32
std::string digidoc::util::String::toMultiByte(int format, const std::wstring &in)
{
    int len = WideCharToMultiByte(format, 0, in.data(), in.size(), 0, 0, 0, 0);
    char *conv = (char*)malloc(sizeof(char)*len);
    memset(conv, 0, len);
    len = WideCharToMultiByte(format, 0, in.data(), in.size(), conv, len, 0, 0);
    std::string out(conv, len);
    free(conv);
    return out;
}

std::wstring digidoc::util::String::toWideChar(int format, const std::string &in);
{
    int len = MultiByteToWideChar(format, 0, in.data(), in.size(), 0, 0);
    wchar_t *conv = (wchar_t*)malloc(sizeof(wchar_t)*len);
    memset(conv, 0, len);
    len = MultiByteToWideChar(format, 0, in.data(), in.size(), conv, len);
    std::wstring out(conv, len);
    free(conv);
    return out;
}
#endif

/**
 * Helper method for converting from non-UTF-8 encoded strings to UTF-8.
 * Supported LANG values for Linux: see /usr/share/i18n/SUPPORTED.
 * Supported encodings for libiconv: see iconv --list .
 * 
 * Note! If non-ASCII characters are used we assume a proper LANG value!!! 
 *
 * @param str_in The string to be converted.
 * @return Returns the input string in UTF-8.
 */
#ifdef _WIN32
std::string digidoc::util::String::convertUTF8(const std::string &in, bool to_UTF)
{
    int inFormat = to_UTF ? CP_ACP : CP_UTF8;
    int outFormat = to_UTF ? CP_UTF8 : CP_ACP;
    return toMultiByte(outFormat, toWideChar(inFormat, in));
}
#else
std::string digidoc::util::String::convertUTF8(const std::string& str_in, bool to_UTF)
{
#if defined(__APPLE__)
    return str_in;
#else
    std::string charset("ISO8859-1");
    const char *env_lang = getenv("LANG");
    if(env_lang && strcmp(env_lang, "C") != 0)
    {
        charset = env_lang;
        size_t locale_start = charset.rfind(".");
        if(locale_start != std::string::npos)
            charset = charset.substr(locale_start+1);
    }

    // no conversion needed for UTF-8
    if((charset.compare("UTF-8") == 0) || (charset.compare("utf-8") == 0))
        return str_in;

    iconv_t ic_descr = iconv_t(-1);
    try
    {
        ic_descr = to_UTF ? iconv_open("UTF-8", charset.c_str()) : iconv_open(charset.c_str(), "UTF-8");
    }
    catch(std::exception &) {}

    if( ic_descr == iconv_t(-1))
    {
        INFO("Could not get the iconv descriptor for converting to/from charset %s, "
             "continuing without conversion.", charset.c_str());
        return str_in; 
    }

    ICONV_CONST char* inptr = (ICONV_CONST char*)str_in.c_str();
    size_t inleft = str_in.size();

    std::string out;
    char outbuf[64];
    char* outptr;
    size_t outleft;

    while(inleft > 0)
    {
        outbuf[0] = '\0';
        outptr = (char *)outbuf;
        outleft = sizeof(outbuf) - sizeof(outbuf[0]);

        size_t result = iconv(ic_descr, &inptr, &inleft, &outptr, &outleft);
        if(result == size_t(-1))
        {
            switch(errno)
            {
            case E2BIG: break;
            case EILSEQ:
            case EINVAL:
            default:
                iconv_close(ic_descr);
                return str_in;
                break;
            }
        }
        *outptr = '\0';
        out += outbuf;
    }
    iconv_close(ic_descr);

    return out;
#endif
}
#endif

/**
 * Helper method for converting strings with non-ascii characters to the URI format (%HH for each non-ascii character).
 * 
 * Not converting:
 * (From RFC 2396 "URI Generic Syntax")
 * reserved = ";" | "/" | "?" | ":" | "@" | "&" | "=" | "+" | "$" | ","
 * mark     = "-" | "_" | "." | "!" | "~" | "*" | "'" | "(" | ")"
 * @param str_in the string to be converted
 * @return the string converted to the URI format
 */
 
std::string digidoc::util::String::toUriFormat(const std::string& str_in)
{
    char dst[1024] = {0}; 

    std::string legal_chars = "-_.!~*'();/?:@&=+$,";
    for(size_t i=0, j=0; i<str_in.length(); i++)
    {
        if( (!(isalnum((str_in.c_str())[i])) ) && ( legal_chars.find((str_in.c_str())[i]) == std::string::npos) )
        {
            strncpy(&dst[j], "%", 1);
            j++;
            char const to_convert = (str_in.c_str())[i];
            sprintf(&dst[j], "%x", (to_convert&0x000000ff));
            j+=2;
        }
        else
        {
            sprintf(&dst[j], "%c", (str_in.c_str())[i]);
            j++;
        }
    }
    return std::string(dst);
}
