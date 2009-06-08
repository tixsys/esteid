#include <memory.h>
#include <stdarg.h>

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
 * @param str string that is converted so std::auto_ptr<char>.
 * @returns string converted to self destroying char*.
 */
std::auto_ptr<char> digidoc::util::String::pointer(const std::string& str)
{
    std::auto_ptr<char> s(new char[str.size() + 1]);
    strcpy(s.get(), str.c_str());
    return s;
}
