#if !defined(__ISERIALIZE_H_INCLUDED__)
#define __ISERIALIZE_H_INCLUDED__

#include "IOException.h"

namespace digidoc
{
    /**
     * Container serializer interface used to (de)serialize BDoc container.
     *
     * @author Janari Põld
     */
    class ISerialize
    {

      public:
          ISerialize(const std::string& path);
          std::string getPath();
          virtual std::string extract() throw(IOException) = 0;
          virtual void create() = 0;
          virtual void addFile(const std::string& containerPath, const std::string& path) = 0;
          virtual void save() throw(IOException) = 0;

      protected:
          std::string path;

    };
}

#endif // !defined(__ISERIALIZE_H_INCLUDED__)
