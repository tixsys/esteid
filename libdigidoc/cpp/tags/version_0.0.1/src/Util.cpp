#include <algorithm>
#include <memory.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdarg.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "log.h"
#include "Util.h"


/**
 * Formats string, use same syntax as <code>printf()</code> function.
 * Example implementation from:
 * http://www.senzee5.com/2006/05/c-formatting-stdstring.html
 *
 * @param fmt format of the string. Uses same formating as <code>printf()</code> function.
 * @param ... parameters for the string format.
 * @return returns formatted string.
 */
std::string digidoc::Util::String::format(const char* fmt, ...)
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
std::string digidoc::Util::String::formatArgList(const char* fmt, va_list args)
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
std::auto_ptr<char> digidoc::Util::String::pointer(const std::string& str)
{
    std::auto_ptr<char> s(new char[str.size() + 1]);
    strcpy(s.get(), str.c_str());
    return s;
}

/**
 * Checks whether file exists and is type of file.
 *
 * @param path path to the file, which existence is checked.
 * @return returns true if the file is a file and it exists.
 */
bool digidoc::Util::File::fileExists(const std::string& path)
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
bool digidoc::Util::File::directoryExists(const std::string& path)
{
    struct stat fileInfo;
    if(stat(path.c_str(), &fileInfo) != 0)
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
std::string digidoc::Util::File::fileName(const std::string& path)
{
    return path.substr(path.find_last_of("/\\") + 1);
}

/**
 * Parses file path and returns directory from file full path.
 *
 * @param path full path of the file.
 * @return returns directory part of the file full path.
 */
std::string digidoc::Util::File::directory(const std::string& path)
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
std::string digidoc::Util::File::path(const std::string& directory, const std::string& relativePath)
{
    std::string dir(directory);
    if(!dir.empty() && (dir[dir.size() - 1] == '/' || dir[dir.size() - 1] == '\\'))
    {
        dir = dir.substr(0, dir.size() - 1);
    }

    // TODO: not windows compatible.
    std::string path = dir + "/" + relativePath;
    std::replace(path.begin(), path.end(), '\\', '/');
    return path;
}

/**
 * @return returns temporary filename.
 */
std::string digidoc::Util::File::tempFileName()
{
    char fileName[L_tmpnam];
    tmpnam(fileName);
    return fileName;
}

/**
 * @return returns temporary directory name.
 */
std::string digidoc::Util::File::tempDirectory()
{
    // TODO: not windows compatible.
    return tempFileName() + "/";
}

/**
 * Creates directory recursively. Also access rights can be omitted. Defaults are 700 in unix.
 *
 * @param path full path of the directory created.
 * @param mode directory access rights, optional parameter, default value 0700 (owner: rwx, group: ---, others: ---)
 * @throws IOException exception is thrown if the directory creation failed.
 */
void digidoc::Util::File::createDirectory(const std::string& path, __mode_t mode) throw(IOException)
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
        createDirectory(parentDir, mode);
    }

    // TODO: not windows compatible. http://www.daniweb.com/forums/thread19976.html
    umask(0);
    int result = mkdir(path.c_str(), mode);
    DEBUG("Created directory '%s' with result = %d", path.c_str(), result)
    if(result != 0 || !directoryExists(path))
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
std::string digidoc::Util::File::createTempDirectory() throw(IOException)
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
unsigned long digidoc::Util::File::fileSize(const std::string& path) throw(IOException)
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

    // TODO: not windows compatible.
    // TODO: type conversion from __SLONGWORD_TYPE ?
    return fileInfo.st_size;
}

/**
 * Creates list of files and empty folders in directory.
 *
 * @param directory full path of the directory.
 * @param relative should the returned file list be relative to the <code>directory</code>.
 * @throws IOException throws exception if the directory listing failed.
 */
std::vector<std::string> digidoc::Util::File::listFiles(const std::string& directory, bool relative) throw(IOException)
{
    DIR* pDir = opendir(directory.c_str());
    if(!pDir)
    {
        THROW_IOEXCEPTION("Failed to open directory '%s'", directory.c_str())
    }

    std::vector<std::string> files;
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
            // Get files in sub directory.
            std::vector<std::string> list = listFiles(path(directory, entry->d_name), true);

            if(!list.empty())
            {
                for(std::vector<std::string>::const_iterator iter = list.begin(); iter != list.end(); iter++)
                {
                    if(relative)
                        files.push_back(path(entry->d_name, *iter));
                    else
                        files.push_back(path(directory, path(entry->d_name, *iter)));
                }
            }
            else
            {
                // TODO: not windows compatible.
                if(relative)
                    files.push_back(std::string(entry->d_name) + "/");
                else
                    files.push_back(path(directory, entry->d_name) + "/");
            }

            continue;
        }
        else if(entry->d_type == 0x08) // File
        {
            if(relative)
                files.push_back(entry->d_name);
            else
                files.push_back(path(directory, entry->d_name));
        }
    }

    closedir(pDir);

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
void digidoc::Util::File::copyFile(const std::string& srcPath, const std::string& destPath, bool overwrite) throw(IOException)
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
void digidoc::Util::File::moveFile(const std::string& srcPath, const std::string& destPath, bool overwrite) throw(IOException)
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
