/*
 * libdigidocpp - a C++ library for creating and validating BDOC-1.0 documents
 *
 * Copyright (C) 2009-2010  Estonian Informatics Centre
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


// This file has platform-specific implementations.
// Treat all POSIX systems (Linux, MAC) the same way. Treat non-POSIX as Windows.

#include <algorithm>
#include <iostream>
#ifndef _WIN32
    #include <unistd.h>
#endif

// platform-specific includes
#ifdef _POSIX_VERSION
    #include <dirent.h>
    #include <sys/param.h>
    #include <sys/stat.h>
#else
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
#ifdef _POSIX_VERSION
    #define PATH_DELIMITER '/'
#else
    #define PATH_DELIMITER '\\'
#endif

std::stack<std::string> digidoc::util::File::tempFiles;

/**
 * Encodes path to compatible std lib
 * @param fileName path
 * @return encoded path
 */
std::string digidoc::util::File::encodeName(const std::string &fileName)
{ return digidoc::util::String::convertUTF8(fileName,false); }

/**
 * Decodes path from std lib path
 * @param localFileName path
 * @return decoded path
 */
std::string digidoc::util::File::decodeName(const std::string &localFileName)
{ return digidoc::util::String::convertUTF8(localFileName,true); }

/**
 * Encodes path to compatible std fstream
 * @param fileName path
 * @return encoded path
 */
#if _WIN32
std::wstring digidoc::util::File::fstreamName(const std::string &fileName)
{ return digidoc::util::String::toWideChar(CP_UTF8, fileName); }
#else
std::string digidoc::util::File::fstreamName(const std::string &fileName)
{ return digidoc::util::File::encodeName(fileName); }
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
    std::string _path = encodeName(path);
    if(stat(_path.c_str(), &fileInfo) != 0)
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
    std::string adjustedPath = encodeName(path);
#ifndef _POSIX_VERSION
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
 * @return returns file name from the file full path in UTF-8.
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
    //return path.substr(0, path.find_last_of("/\\"));
    return path.substr(0, path.find_last_of(PATH_DELIMITER));

}

/**
 * Creates full path from directory name and relative path.
 *
 * @param directory directory path.
 * @param relativePath relative path.
 * @param unixStyle when set to <code>true</code> returns path with unix path separators,
 *        otherwise returns with operating system specific path separators.
 *        Default value is <code>false</code>.
 * @return returns full path.
 */
std::string digidoc::util::File::path(const std::string& directory, const std::string& relativePath, bool unixStyle)
{
    std::string dir(directory);
    if(!dir.empty() && (dir[dir.size() - 1] == '/' || dir[dir.size() - 1] == '\\'))
    {
        dir = dir.substr(0, dir.size() - 1);
    }

    std::string path = dir + PATH_DELIMITER + relativePath;
    if(unixStyle)
    {
        std::replace(path.begin(), path.end(), '\\', '/');
    }
    else
    {
#ifdef _POSIX_VERSION
        std::replace(path.begin(), path.end(), '\\', '/');
#else
        std::replace(path.begin(), path.end(), '/', '\\');
#endif
    }

    return path;
}

/**
 * @return returns temporary filename.
 */
std::string digidoc::util::File::tempFileName()
{
#ifdef _POSIX_VERSION
    char fileName[L_tmpnam];
    tmpnam(fileName);
#else
    // requires TMP environment variable to be set
    char* fileName = _tempnam(NULL, NULL); // TODO: static buffer, not thread-safe
    if ( fileName == NULL )
    {
        THROW_IOEXCEPTION("Failed to create a temporary file name.");
    }
#endif
    tempFiles.push(fileName);
    return decodeName(fileName);
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
        THROW_IOEXCEPTION("Can not create directory with no name.");
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

    std::string _path = encodeName(path);
#ifdef _POSIX_VERSION
    umask(0);
    int result = mkdir(_path.c_str(), 0700);
    DEBUG("Created directory '%s' with result = %d", _path.c_str(), result);
    createFailed = (result != 0);
#else
    int result = _mkdir(path.c_str());
    createFailed = (result != 0);
    if ( createFailed )
    {
        DEBUG("Creating directory '%s' failed with errno = %d", _path.c_str(), errno);
    }
    else
    {
        DEBUG("Created directory '%s'", _path.c_str());
    }
#endif

    if(createFailed || !directoryExists(path))
    {
        THROW_IOEXCEPTION("Failed to create directory '%s'", path.c_str());
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
    std::string _path = encodeName(path);
    if(stat(_path.c_str(), &fileInfo) != 0)
    {
        THROW_IOEXCEPTION("File '%s' does not exist.", _path.c_str());
    }

    if((fileInfo.st_mode & S_IFMT) == S_IFDIR)
    {
        THROW_IOEXCEPTION("'%s' is not a file.", _path.c_str());
    }

    if (fileInfo.st_size < 0)
    {
        THROW_IOEXCEPTION("Failed to get size for file '%s'.", _path.c_str());
    }

    return static_cast<unsigned long>(fileInfo.st_size);
}

/**
 * Returns list of files (and empty directories, if <code>listEmptyDirectories</code> is set)
 * found in the directory <code>directory</code>.
 *
 * @param directory full path of the directory.
 * @param relative should the returned file list be relative to the <code>directory</code>.
 *        Default value is <code>false</code>, meaning returned file listing has full
 *        file paths.
 * @param listEmptyDirectories should the returned file list contain also empty directories.
 *        Default value is <code>false</code>, meaning no empty directories are returned.
 * @param unixStyle should the returned file path entries in the file list use the unix style
 *        path separators. Default value is <code>false</code>, meaning operating system
 *        specific path separators are used.
 * @throws IOException throws exception if the directory listing failed.
 */
std::vector<std::string> digidoc::util::File::listFiles(const std::string& directory, bool relative,
		bool listEmptyDirectories, bool unixStyle) throw(IOException)
{
    // Why, oh why does this function use vectors, not std::lists? Does anybody need to do heavy random access on the file names?
	std::string pathDelim; pathDelim += PATH_DELIMITER;
	if(unixStyle)
	{
	    pathDelim = "/";
	}
    std::vector<std::string> files;

    // List entries in sub folders.
    std::vector<std::string> subDirectories = getDirSubElements(directory, true, false, unixStyle);
    std::vector<std::string>::const_iterator subDirectory = subDirectories.begin();
    for ( ; subDirectory != subDirectories.end(); subDirectory++ )
    {
        // Get files in sub directory.
        std::vector<std::string> list = listFiles(path(directory, *subDirectory), relative, listEmptyDirectories, unixStyle);

        if(!list.empty())
        {
            for(std::vector<std::string>::const_iterator iter = list.begin(); iter != list.end(); iter++)
            {
                if(relative)
                    files.push_back(path(*subDirectory, *iter, unixStyle));
                else
                	files.push_back(*iter);
            }
        }
        else
        {
        	if(listEmptyDirectories)
        	{
                if(relative)
                    files.push_back(std::string(*subDirectory) + pathDelim);
                else
                    files.push_back(path(directory, *subDirectory, unixStyle) + pathDelim);
        	}
        }
    }

    // List files directly in current directory
    std::vector<std::string> dirFiles = getDirSubElements(directory, relative, true, unixStyle);
    files.insert(files.begin(), dirFiles.begin(), dirFiles.end());

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
        THROW_IOEXCEPTION("Source file '%s' does not exist.", srcPath.c_str());
    }

    if(!overwrite && fileExists(destPath))
    {
        THROW_IOEXCEPTION("Destination file exists '%s' can not copy to there. Overwrite flag is set to false.", destPath.c_str());
    }

    // Copy file.
    std::ifstream ifs(fstreamName(srcPath).c_str(), std::ios::binary);
    std::ofstream ofs(fstreamName(destPath).c_str(), std::ios::binary | std::ios::trunc);

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
        THROW_IOEXCEPTION("Source file '%s' does not exist.", srcPath.c_str());
    }

    if(!overwrite && fileExists(destPath))
    {
        THROW_IOEXCEPTION("Destination file exists '%s' can not move to there. Overwrite flag is set to false.", destPath.c_str());
    }

    std::string _srcPath = encodeName(srcPath);
    std::string _destPath = encodeName(destPath);
    int result = rename(_srcPath.c_str(), _destPath.c_str());
    if ( result != 0 )
    {
		// -=K=-: copy and remove source should work as move between different partitions
		copyFile( srcPath, destPath, overwrite );
		result = remove( _srcPath.c_str() );
    }
	if ( result != 0 )
	{
		// -=K=-: suceeded to copy, failed to remove. Should we throw or warn?
		WARN( "Failed to remove source file '%s' when moving it to '%s'.", _srcPath.c_str(), _destPath.c_str() );
	}

}

/**
 * Lists files or sub directories in directory. If flag <code>filesOnly</code> is set
 * to <code>true</code> returns ONLY files, otherwise returns ONLY directories.
 *
 * @param directory full path of the directory.
 * @param relative should the returned file list be relative to the <code>directory</code>.
 * @param filesOnly if set to <code>true</code>, returns only files, otherwise returns only directories.
 * @param unixStyle should the returned file path entries in the file list use the unix style
 *        path separators.
 * @throws IOException throws exception if the directory listing failed.
 */
std::vector<std::string> digidoc::util::File::getDirSubElements(const std::string& directory,
		bool relative, bool filesOnly, bool unixStyle) throw(IOException)
{
    std::vector<std::string> files;

#ifdef _POSIX_VERSION

    std::string _directory = encodeName(directory);
    DIR* pDir = opendir(_directory.c_str());
    if(!pDir)
    {
        THROW_IOEXCEPTION("Failed to open directory '%s'", _directory.c_str());
    }

    char fullPath[MAXPATHLEN];
    struct stat info;
    dirent* entry;
    while((entry = readdir(pDir)) != NULL)
    {
        if(std::string(".").compare(entry->d_name) == 0
        || std::string("..").compare(entry->d_name) == 0)
        {
            continue;
        }

        sprintf(fullPath, "%s/%s", _directory.c_str(), entry->d_name);
        lstat(fullPath, &info);

        if((!filesOnly && (entry->d_type == 0x04||S_ISDIR(info.st_mode))) // Directory
        || (filesOnly && (entry->d_type == 0x08||S_ISREG(info.st_mode)))) // File
        {
            if(relative)
                files.push_back(decodeName(entry->d_name));
            else
                files.push_back(path(directory, decodeName(entry->d_name), unixStyle));
        }
    }

    closedir(pDir);

#else

    WIN32_FIND_DATA findFileData;
    HANDLE hFind = NULL;

    try
    {
        if ( directory.size() > MAX_PATH )
        {
            // MSDN: "Relative paths are limited to MAX_PATH characters." - can this be true?
            THROW_IOEXCEPTION("Directory path '%s' exceeds the limit %d", directory.c_str(), MAX_PATH);
        }

        std::string findPattern = encodeName(directory) + "\\*";
        hFind = ::FindFirstFile(findPattern.c_str(), &findFileData);
        if (hFind == INVALID_HANDLE_VALUE)
        {
            THROW_IOEXCEPTION("Listing contents of directory '%s' failed with error %d", directory.c_str(), ::GetLastError());
        }

        do
        {
            std::string fileName(findFileData.cFileName);
            if ( fileName == "." || fileName == ".." )
            {
                continue; // skip those too
            }

            if(!filesOnly && (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // Directory
            || filesOnly && !(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) // File
            {
                if(relative)
                    files.push_back(decodeName(fileName));
                else
                    files.push_back(path(directory, decodeName(fileName), unixStyle));
            }
        } while ( ::FindNextFile(hFind, &findFileData) != FALSE );

        // double-check for errors
        if ( ::GetLastError() != ERROR_NO_MORE_FILES )
        {
            THROW_IOEXCEPTION("Listing contents of directory '%s' failed with error %d", directory.c_str(), ::GetLastError());
        }

        ::FindClose(hFind);
    }
    catch (...)
    {
        ::FindClose(hFind);
        throw;
    }


#endif

    return files;
}


/**
 * Constructs the full file path in the format "file:///fullpath" in URI encoding. 
 *
 * @param fullDirectory full directory path to the relativeFilePath
 * @param relativeFilePath file name to be appended to the full path
 * @return full file path in the format "file:///fullpath" in URI encoding.
 */

std::string digidoc::util::File::fullPathUrl(const std::string& fullDirectory, const std::string& relativeFilePath)
{
    std::string result(fullDirectory);
    // Under windows replace the path delimiters
#ifndef _POSIX_VERSION
    std::replace(result.begin(), result.end(), '\\', '/');
    return digidoc::util::String::toUriFormat("file:///" + result + "/" + relativeFilePath);
#endif
    return digidoc::util::String::toUriFormat("file://" + result + "/" + relativeFilePath);
}

/**
 * Deletes the filesystem object named <code>fname</code>. Mysteriously fails to delete empty directories
 * on Windows, so use removeDirectory instead.
 *
 * @param fname full file or directory fname.
 */
void digidoc::util::File::removeFile(const std::string& fname)
{
    std::string _fname = encodeName(fname);
    int result = remove( _fname.c_str() );
    if ( result != 0 )
    {
        WARN( "Tried to remove the temporary file or directory '%s', but failed.", _fname.c_str() );
    }
}

/**
 * Deletes the directory named <code>dname</code>. The directory must be empty.
 *
 * @param fname full file or directory fname.
 */
void digidoc::util::File::removeDirectory(const std::string& dname)
{
    std::string _dname = encodeName(dname);
#ifndef _POSIX_VERSION
	if (!RemoveDirectory(_dname.c_str()))
#else
	if (remove( _dname.c_str() ) != 0)
#endif
	{
		WARN( "Tried to remove the temporary directory '%s', but failed.", _dname.c_str() );
    }
}

/**
 * Tries to delete this directory along with its contents, recursively.
 *
 * @param dname full directory name
 */
void digidoc::util::File::removeDirectoryRecursively(const std::string& dname) throw(IOException)
{
    // First delete recursively all subdirectories
    std::vector<std::string> subDirs = getDirSubElements(dname, false, false, false);
    for (std::vector<std::string>::reverse_iterator it = subDirs.rbegin(); it != subDirs.rend(); it++)
    {
        removeDirectoryRecursively(*it);
    }

    // Then delete all files inside this directory
    std::vector<std::string> subFiles = getDirSubElements(dname, false, true, false);
    for (std::vector<std::string>::reverse_iterator it = subFiles.rbegin(); it != subFiles.rend(); it++)
    {
        DEBUG( "Deleting the temporary file '%s'", it->c_str() );
        removeFile(*it);
    }

    // Then delete the directory itself. It should now be empty.
    DEBUG( "Deleting the temporary directory '%s'", dname.c_str() );
    removeDirectory(dname);
}

/**
 * Tries to delete all temporary files and directories whose names were handled out with tempFileName, tempDirectory and createTempDirectory.
 * The deletion of directories is recursive.
 */
void digidoc::util::File::deleteTempFiles() throw(IOException)
{
    while (!tempFiles.empty())
    {
        if ( directoryExists(tempFiles.top()) )
            removeDirectoryRecursively(tempFiles.top());
        else
            removeFile(tempFiles.top());
        tempFiles.pop();
    }
}

