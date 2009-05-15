#include <memory.h>
#include <stdarg.h>
#include <stdlib.h>
#include <iostream>
#include "iconv.h"
#include "../log.h"
#include "String.h"


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


/**
 * Helper method for converting from non-UTF-8 encoded strings to UTF-8.
 * Supported LANG values for Linux: see /usr/share/i18n/SUPPORTED.
 *
 * @param str_in The string to be converted.
 * @return Returns the input string in UTF-8.
 */
std::string digidoc::util::String::convertUTF8(const std::string& str_in, bool to_UTF)
{
    std::string charset(getSystemEncoding());
    // XXX do we need additional formatting of the charset string? (iso8859-1 vs. ISO-8859-1)

    iconv_t ic_descr;
    if( (charset.compare("UTF-8") == 0) || (charset.compare("utf-8") == 0) )
    {
		// no conversion needed
		return str_in;
	}

    if(to_UTF)
    {
        //direction of conversion: from platform specific encoding to UTF-8
        ic_descr = iconv_open("UTF-8", charset.c_str()); // to_encoding, from_encoding
    }
    else
    {
        //direction of conversion: from UTF-8 to platform specific encoding
        ic_descr = iconv_open(charset.c_str(), "UTF-8");
    }

    if( ic_descr == (iconv_t)(-1))
    {
        THROW_BDOCEXCEPTION("Unsupported character encoding.");
    }

    char inbuf[MAX_LANG_LENGTH] = {0}; 
    char outbuf[MAX_LANG_LENGTH] = {0};
    strncpy( inbuf, str_in.c_str(), MAX_LANG_LENGTH-1 );
    char* inptr( inbuf );
    char* outptr( outbuf );
    size_t inleft = strlen( inbuf ) + 1;
    size_t outleft = MAX_LANG_LENGTH-1;
    size_t conversion_result = iconv(ic_descr, &inptr, &inleft, &outptr, &outleft);

    if(conversion_result == -1)
    { 
        THROW_BDOCEXCEPTION("Failed to convert to/from UTF-8.");
    }
    return std::string(outbuf);
}


/**
 * Helper method for getting the platform specific encoding.
 * This is necessary for converting data that is written to XML into UTF-8.
 * 
 * @return Returns the system encoding read from the LANG environment variable. 
 */
std::string digidoc::util::String::getSystemEncoding()
{
    std::string encoding(getenv("LANG"));   
    size_t locale_start = encoding.rfind(".");

    if(locale_start != std::string::npos)
    {
        size_t newline = encoding.find("\n");
        encoding = encoding.substr(locale_start+1, newline);
    }
    else
    {
        THROW_BDOCEXCEPTION("Failed to extract the charset."); 
    }
    return encoding;
}


/**
 * Helper method for converting strings with non-ascii characters to the URI format (%HH for each non-ascii character).
 *
 * @param str_in the string to be converted
 * @return the string converted to the URI format
 */
 
std::string digidoc::util::String::toUriFormat(const std::string& str_in)
{
    char dst[1024] = {0}; 
    std::string legal_chars = "-_.!~*'()";

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
    std::cout << "url string: " << std::string(dst) << std::endl;
    return std::string(dst);
}
