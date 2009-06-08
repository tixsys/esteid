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
          typedef std::vector<Exception> Causes;

          Exception(const std::string& file, int line, const std::string& msg);
          Exception(const std::string& file, int line, const std::string& msg, const Exception& cause);
          std::string getMsg() const;
          bool hasCause() const;
          Causes getCauses() const;
          void addCause(const Exception& cause)
          {
              this->causes.push_back(cause);
          }

      protected:
          std::string file;
          int line;
          std::string msg;
          Causes causes;

    };
}

#endif // !defined(__EXCEPTION_H_INCLUDED__)
