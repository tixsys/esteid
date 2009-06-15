
// This file has platform-specific implementations. 
// Let's group them as necessary for us.
#if defined(__linux__) // Linux
    #define BDOC_LINUX // TODO: Consider either posix or make linux default.
#elif defined _WIN32 // Windows 32-bit
    #define BDOC_WIN32
#else
    #error Unknown platform - please implement it here
#endif


#include <algorithm>

// platform-specific includes
#if defined(BDOC_LINUX)
    #include <dirent.h>
#elif defined(BDOC_WIN32)
    #include <windows.h>
    #include <direct.h>
#endif

#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>

#include "../log.h"
#include "File.h"
#include "String.h"

// platform-specific path delimiters
#if defined(BDOC_LINUX)
    #define PATH_DELIMITER '/'
#elif defined(BDOC_WIN32)
    #define PATH_DELIMITER '\\'
#endif



/**
 * Checks whether file exists and is type of file.
 *
 * @param path path to the file, which existence is checked.
 * @return returns true if the file is a file and it exists.
 */
bool digidoc::util::File::fileExists(const std::string& path)
{
    struct stat fileInfo;
    if(stat(path.c_str(), &fileInfo) != 0)
    {
        return false;
    }

    // XXX: != S_IFREG
    if((fileInfo.st_mode & S_IFMT) == S_IFDIR)
    {
        return false;
    }

    return true;
}

/**
 * Checks whether directory exists and is type of directory.
 *
 * @param path path to the directory, which existence is checked.
 * @return returns true if the directory is a directory and it exists.
 */
bool digidoc::util::File::directoryExists(const std::string& path)
{
    struct stat fileInfo;
    std::string adjustedPath(path);
#if defined(BDOC_WIN32)
    // stat will fail on win32 if path ends with backslash
    if(!adjustedPath.empty() && (adjustedPath[adjustedPath.size() - 1] == '/' 
        || adjustedPath[adjustedPath.size() - 1] == '\\'))
    {
        adjustedPath = adjustedPath.substr(0, adjustedPath.size() - 1);
    }
    // TODO:XXX: "C:" is not a directory, so create recursively will 
    // do stack overflow in case first-dir in root doesn't exist.
#endif
    if(stat(adjustedPath.c_str(), &fileInfo) != 0)
    {
        return false;
    }

    if((fileInfo.st_mode & S_IFMT) != S_IFDIR)
    {
        return false;
    }

    return true;
}

/**
 * Parses file path and returns file name from file full path.
 *
 * @param path full path of the file.
 * @return returns file name from the file full path.
 */
std::string digidoc::util::File::fileName(const std::string& path)
{
    return path.substr(path.find_last_of("/\\") + 1);
}

/**
 * Parses file path and returns directory from file full path.
 *
 * @param path full path of the file.
 * @return returns directory part of the file full path.
 */
std::string digidoc::util::File::directory(const std::string& path)
{
    return path.substr(0, path.find_last_of("/\\"));
}

/**
 * Creates full path from directory name and relative path.
 *
 * @param directory directory path.
 * @param relativePath relative path.
 * @return returns full path.
 */
std::string digidoc::util::File::path(const std::string& directory, const std::string& relativePath)
{
    std::string dir(directory);
    if(!dir.empty() && (dir[dir.size() - 1] == '/' || dir[dir.size() - 1] == '\\'))
    {
        dir = dir.substr(0, dir.size() - 1);
    }

    std::string path = dir + PATH_DELIMITER + relativePath;
#if defined(BDOC_LINUX)
    std::replace(path.begin(), path.end(), '\\', '/');
#elif defined(BDOC_WIN32)
    std::replace(path.begin(), path.end(), '/', '\\');
#endif
    return path;
}

/**
 * @return returns temporary filename.
 */
std::string digidoc::util::File::tempFileName()
{
#if defined(BDOC_LINUX)
    char fileName[L_tmpnam];
    tmpnam(fileName);
    return fileName;
#elif defined(BDOC_WIN32)
    // requires TMP environment variable to be set
    char* fileName = _tempnam(NULL, NULL); // TODO: static buffer, not thread-safe
    if ( fileName == NULL )
    {
        THROW_IOEXCEPTION("Failed to create a temporary file name.")
    }
    return fileName;
#endif
}

/**
 * @return returns temporary directory name.
 */
std::string digidoc::util::File::tempDirectory()
{
    return tempFileName() + PATH_DELIMITER;
}

/**
 * Creates directory recursively. Also access rights can be omitted. Defaults are 700 in unix.
 *
 * @param path full path of the directory created.
 * @param mode directory access rights, optional parameter, default value 0700 (owner: rwx, group: ---, others: ---)
 * @throws IOException exception is thrown if the directory creation failed.
 */
void digidoc::util::File::createDirectory(const std::string& path) throw(IOException)
{
    if(path.empty())
    {
        THROW_IOEXCEPTION("Can not create directory with no name.")
    }

    if(directoryExists(path))
    {
        return;
    }

    std::string parentDir(path);
    if(parentDir[parentDir.size() - 1] == '/' || parentDir[parentDir.size() - 1] == '\\')
    {
        parentDir = parentDir.substr(0, parentDir.size() - 1);
    }
    parentDir = parentDir.substr(0, parentDir.find_last_of("/\\"));

    if(!directoryExists(parentDir))
    {
        createDirectory(parentDir);
    }

    bool createFailed = true;

#if defined(BDOC_LINUX)
    umask(0);
    int result = mkdir(path.c_str(), 0700);
    DEBUG("Created directory '%s' with result = %d", path.c_str(), result)
    createFailed = (result != 0);
#elif defined(BDOC_WIN32)
    int result = _mkdir(path.c_str());
    createFailed = (result != 0);
    if ( createFailed )
    {
        DEBUG("Creating directory '%s' failed with errno = %d", path.c_str(), errno)
    }
    else
    {
        DEBUG("Created directory '%s'", path.c_str())
    }
#endif

    if(createFailed || !directoryExists(path))
    {
        THROW_IOEXCEPTION("Failed to create directory '%s'", path.c_str())
    }
}

/**
 * Creates temporary directory.
 *
 * @return returns the created temporary directory.
 * @throws IOException throws exception if the directory creation failed.
 * @see createDirectory(const std::string& path)
 */
std::string digidoc::util::File::createTempDirectory() throw(IOException)
{
    std::string directory = tempDirectory();
    createDirectory(directory);
    return directory;
}

/**
 * Checks that file is a file and it exists, if the file exists returns its size in bytes.
 *
 * @param path full path of the file.
 * @return size of the file in bytes.
 * @throws IOException throws exception if the file does not exists.
 */
unsigned long digidoc::util::File::fileSize(const std::string& path) throw(IOException)
{
    struct stat fileInfo;
    if(stat(path.c_str(), &fileInfo) != 0)
    {
        THROW_IOEXCEPTION("File '%s' does not exist.", path.c_str());
    }

    if((fileInfo.st_mode & S_IFMT) == S_IFDIR)
    {
        THROW_IOEXCEPTION("'%s' is not a file.", path.c_str());
    }

    if (fileInfo.st_size < 0)
    {
        THROW_IOEXCEPTION("Failed to get size for file '%s'.", path.c_str());
    }

    return static_cast<unsigned long>(fileInfo.st_size);
}

/**
 * Creates list of files and empty folders in directory. The listing is suitable for
 * manifest.xml validation where:
 *  -# Unix-style path separator '/' is used
 *  -# files must be validated
 *  -# empty directories must be validated
 *  -# the non-empty directory entry itself is of no interest
 *
 * @param directory full path of the directory.
 * @param relative should the returned file list be relative to the <code>directory</code>.
 * @throws IOException throws exception if the directory listing failed.
 */
std::vector<std::string> digidoc::util::File::listFiles(const std::string& directory, bool relative) throw(IOException)
{
    std::vector<std::string> files;

    // First, recurse through subfolders
    std::vector<std::string> subDirectories = getDirSubElements(directory, relative, false);

    std::vector<std::string>::const_iterator subDirectory = subDirectories.begin();
    for ( ; subDirectory != subDirectories.end(); subDirectory++ )
    {
        DEBUG("Listing dir contents: subdirectory '%s'", subDirectory->c_str())

        // Get files in sub directory.
        std::vector<std::string> list = listFiles(path(directory, *subDirectory), true);

        if(!list.empty())
        {
            for(std::vector<std::string>::const_iterator iter = list.begin(); iter != list.end(); iter++)
            {
                if(relative)
                    files.push_back(pathAsUnix(*subDirectory, *iter));
                else
                    files.push_back(pathAsUnix(directory, pathAsUnix(*subDirectory, *iter)));
            }
        }
        else
        {
            if(relative)
                files.push_back(std::string(*subDirectory) + "/");
            else
                files.push_back(pathAsUnix(directory, *subDirectory) + "/");
        }
    }

    // Second, list files directly in current directory

    std::vector<std::string> dirFiles = getDirSubElements(directory, relative, true);

    std::vector<std::string>::const_iterator dirFile = dirFiles.begin();
    for ( ; dirFile != dirFiles.end(); dirFile++ )
    {
        DEBUG("Listing dir contents: file '%s'", dirFile->c_str())

        if(relative)
            files.push_back(*dirFile);
        else
            files.push_back(pathAsUnix(directory, *dirFile));
    }


    // Third, ignore anything else besides files and directories

    return files;
}

/**
 * Copies file from source <code>srcPath</code> to destination <code>destPath</code>.
 *
 * @param srcPath source full file path.
 * @param destPath destination full file path.
 * @param overwrite whether to overwrite existing file.
 * @throws IOException exception is thrown if overwrite flag is set to false and destination file already exists.
 *         Or the file copy operation failed.
 */
void digidoc::util::File::copyFile(const std::string& srcPath, const std::string& destPath, bool overwrite) throw(IOException)
{
    if(!fileExists(srcPath))
    {
        THROW_IOEXCEPTION("Source file '%s' does not exist.", srcPath.c_str())
    }

    if(!overwrite && fileExists(destPath))
    {
        THROW_IOEXCEPTION("Destination file exists '%s' can not copy to there. Overwrite flag is set to false.", destPath.c_str())
    }

    // Copy file.
    std::ifstream ifs(srcPath.c_str(), std::ios::binary);
    std::ofstream ofs(destPath.c_str(), std::ios::binary | std::ios::trunc);

    ofs << ifs.rdbuf();

    ifs.close();
    ofs.close();

    if(ifs.fail() || ofs.fail())
    {
        THROW_IOEXCEPTION("Failed to copy file '%s' to '%s'.", srcPath.c_str(), destPath.c_str());
    }
}

/**
 * Moves file from source <code>srcPath</code> to destination <code>destPath</code>.
 *
 * @param srcPath source full file path.
 * @param destPath destination full file path.
 * @param overwrite whether to overwrite existing file.
 * @throws IOException exception is thrown if overwrite flag is set to false and destination file already exists.
 *         Or the file move operation failed.
 */
void digidoc::util::File::moveFile(const std::string& srcPath, const std::string& destPath, bool overwrite) throw(IOException)
{
    if(!fileExists(srcPath))
    {
        THROW_IOEXCEPTION("Source file '%s' does not exist.", srcPath.c_str())
    }

    if(!overwrite && fileExists(destPath))
    {
        THROW_IOEXCEPTION("Destination file exists '%s' can not move to there. Overwrite flag is set to false.", destPath.c_str())
    }

    int result = rename(srcPath.c_str(), destPath.c_str());
    if(result != 0)
    {
        THROW_IOEXCEPTION("Failed to move file '%s' to '%s'.", srcPath.c_str(), destPath.c_str())
    }
}

/**
 * Dedicated method to only help listFiles method in manifest-file handling.
 * 
 * @param directory full path of the directory.
 * @param relative should the returned file list be relative to the <code>directory</code>.
 * @param getFiles delivers only file elements in <code>directory</code> when true. Delivers 
 *        only subdirectories when false.
 * @throws IOException throws exception if the directory listing failed.
 */
std::vector<std::string> digidoc::util::File::getDirSubElements(const std::string& directory, bool relative, bool getFiles) throw(IOException)
{
    std::vector<std::string> files;

#if defined(BDOC_LINUX)

    DIR* pDir = opendir(directory.c_str());
    if(!pDir)
    {
        THROW_IOEXCEPTION("Failed to open directory '%s'", directory.c_str())
    }

    dirent* entry;
    while((entry = readdir(pDir)) != NULL)
    {
        if(std::string(".").compare(entry->d_name) == 0
        || std::string("..").compare(entry->d_name) == 0)
        {
            continue;
        }

        DEBUG("entry->d_name = '%s', entry->d_type = %u", entry->d_name, entry->d_type)

        if(entry->d_type == 0x04) // Directory
        {
            if ( getFiles )
                continue; // don't add directories when caller requests for files

            if(relative)
                files.push_back(std::string(entry->d_name) + "/");
            else
                files.push_back(pathAsUnix(directory, entry->d_name) + "/");

            continue;
        }
        else if(entry->d_type == 0x08) // File
        {
            if ( !getFiles )
                continue; // don't add files when caller requests for directories

            if(relative)
                files.push_back(entry->d_name);
            else
                files.push_back(pathAsUnix(directory, entry->d_name));
        }
    }

    closedir(pDir);

#elif defined (BDOC_WIN32)

    WIN32_FIND_DATA findFileData;
    HANDLE hFind = NULL;

    try
    {
        if ( directory.size() > MAX_PATH )
        {
            // MSDN: "Relative paths are limited to MAX_PATH characters." - can this be true?
            THROW_IOEXCEPTION("Directory path '%s' exceeds the limit %d", directory.c_str(), MAX_PATH)
        }

        std::string findPattern(directory + "\\*");
        hFind = ::FindFirstFile(findPattern.c_str(), &findFileData);
        if (hFind == INVALID_HANDLE_VALUE)
        {
            THROW_IOEXCEPTION("Listing contents of directory '%s' failed with error %d", directory.c_str(), ::GetLastError())
        }

        do
        {
            if ( getFiles && (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
                continue; // don't add directories when caller asks for files
            else if ( ! getFiles && !(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
                continue; // don't add files when caller asks for directories
            // else: proceed adding it

            std::string fileName(findFileData.cFileName);
            if ( fileName == "." || fileName == ".." )
            {
                continue; // skip those too
            }

            if(relative)
                files.push_back(fileName);
            else
                files.push_back(pathAsUnix(directory, fileName));

        } while ( ::FindNextFile(hFind, &findFileData) != FALSE );

        // double-check for errors
        if ( ::GetLastError() != ERROR_NO_MORE_FILES )
        {
            THROW_IOEXCEPTION("Listing contents of directory '%s' failed with error %d", directory.c_str(), ::GetLastError())
        }

        ::FindClose(hFind);
    }
    catch (...)
    {
        ::FindClose(hFind);
    }


#endif

    return files;
}

/**
 * Creates full path from directory name and relative path in a unix form ('/'-delimited).
 *
 * @param directory directory path.
 * @param relativePath relative path.
 * @return returns full path.
 */
std::string digidoc::util::File::pathAsUnix(const std::string& directory, const std::string& relativePath)
{
    std::string dir(directory);
    if(!dir.empty() && (dir[dir.size() - 1] == '/'))
    {
        dir = dir.substr(0, dir.size() - 1);
    }

    std::string path = dir + '/' + relativePath;
    std::replace(path.begin(), path.end(), '\\', '/'); // replace just in case
    return path;
}

