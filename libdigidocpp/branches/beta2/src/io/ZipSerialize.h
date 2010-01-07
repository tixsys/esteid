#if !defined(__ZIPSERIALIZE_H_INCLUDED__)
#define __ZIPSERIALIZE_H_INCLUDED__

#include "ISerialize.h"

namespace digidoc
{
    /**
     * ZIP file implementation of the ISerialize interface. Saves files to ZIP file
     * and extracts the ZIP file on demand. Uses ZLib to implement ZIP file operations.
     *
     * @author Janari Põld
     */
    class EXP_DIGIDOC ZipSerialize : public ISerialize
    {

      public:
          ZipSerialize(const std::string& path);
          std::string extract() throw(IOException);
          void create();
          void addFile(const std::string& containerPath, const std::string& path);
          void save() throw(IOException);

      protected:
          struct FileEntry { std::string containerPath; std::string path; };
          std::vector<FileEntry> filesAdded;

    };
}

#endif // !defined(__ZIPSERIALIZE_H_INCLUDED__)
