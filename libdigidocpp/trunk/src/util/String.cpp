#include <memory.h>
#include <stdarg.h>
#include <stdlib.h>
#include <iostream>
#include <iconv.h>
#include "../log.h"
#include "String.h"

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
std::string digidoc::util::String::convertUTF8(const std::string& str_in, bool to_UTF)
{
    std::string charset("");
    iconv_t ic_descr;

    try
    {
        charset.append(getSystemEncoding());  
    
        if( (charset.compare("UTF-8") == 0) || (charset.compare("utf-8") == 0) )
        {
	    	// no conversion needed for UTF-8 
		    return str_in;
	    }
          
        if(charset.compare("C") == 0)
        {
            INFO("System locale is C, continuing without conversion.");
            charset = "LATIN1";
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
    }

    catch(std::exception& e)
    {
        INFO("Could not get the iconv descriptor for converting to/from charset %s, continuing without conversion.", charset.c_str());
        return str_in;
    }

    if( ic_descr == (iconv_t)(-1))
    {
        INFO("Could not get the iconv descriptor for converting to/from charset %s, continuing without conversion.", charset.c_str());
        return str_in; 
    }

    char outbuf[MAX_LANG_LENGTH] = {0};
    ICONV_CONST char* inptr = (ICONV_CONST char*)str_in.c_str();
    char* outptr( outbuf );
    size_t inleft = str_in.size();
    size_t outleft = MAX_LANG_LENGTH-1;
    size_t conversion_result = iconv(ic_descr, &inptr, &inleft, &outptr, &outleft);
    iconv_close(ic_descr);

    if(conversion_result == -1)
    {    
        THROW_BDOCEXCEPTION("Failed to convert to/from UTF-8.");
    }

    return std::string(outbuf);
}


/**
 * Helper method for getting the platform encoding.
 *
 * @return Returns the platform encoding read from the LANG environment variable. 
 */
std::string digidoc::util::String::getSystemEncoding()
{
    std::string encoding("");
    const char *env_lang = getenv("LANG");

    if(env_lang)
    {
        encoding.append(env_lang);
    }
    else
    {
        encoding.append("C");
        INFO("Empty LANG environment variable, continuing without conversion to/from UTF-8.");
        return encoding;
    }

    if(encoding.length() == 0)
    {   
        encoding.append("C");
        INFO("Empty LANG environment variable, continuing without conversion to/from UTF-8.");
        return encoding;
    }

    // Is LANG in a format such as en_US.ISO-8859-1 ?
    size_t locale_start = encoding.rfind(".");

    if(locale_start != std::string::npos)
    {
        size_t newline = encoding.find("\n");
        encoding = encoding.substr(locale_start+1, newline);
    }

    // else just return what was read from LANG
    return encoding;
}


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
