#if !defined(__EXCEPTION_H_INCLUDED__)
#define __EXCEPTION_H_INCLUDED__

#include <string>
#include <vector>

namespace digidoc
{
    /**
     * Base exception class of the digidoc implementation. Digidoc library
     * should never throw exceptions other type than instance of this class
     * or class inherited from this class like IOException or BDocException.
     *
     * @author Janari Põld
     */
    class Exception
    {

      public:
          Exception(const std::string& file, int line, const std::string& msg);
          Exception(const std::string& file, int line, const std::string& msg, const Exception& cause);
          std::string getMsg() const;
          bool hasCause() const;
          Exception getCause() const;

      protected:
          std::string file;
          int line;
          std::string msg;
          std::vector<Exception> causes;

    };
}

#endif // !defined(__EXCEPTION_H_INCLUDED__)
