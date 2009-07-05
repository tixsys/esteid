#include "../minizip/zip.h"
#include "../minizip/unzip.h"

#include <iostream>
#include "../log.h"
#include "../util/File.h"
#include "../util/String.h"
#include "ZipSerialize.h"


namespace digidoc
{
static void extractCurrentFile(unzFile zipFile, const std::string& directory) throw(IOException);
}

/**
 * Initializes ZIP file serializer.
 *
 * @param path
 */
digidoc::ZipSerialize::ZipSerialize(const std::string& path)
 : ISerialize(path)
{
}

/**
 * Extracts all files from ZIP file to a temporary directory on disk.
 *
 * @return returns path, where files from ZIP file were extracted.
 * @throws IOException throws exception if there were errors during
 *         extracting files to disk.
 */
std::string digidoc::ZipSerialize::extract() throw(IOException)
{
    DEBUG("ZipSerialize::extract()");

    // Open ZIP file for extracting.
    std::string fileName = digidoc::util::File::encodeName(path);
    unzFile zipFile = unzOpen(fileName.c_str());
    if(zipFile == NULL)
    {
        THROW_IOEXCEPTION("Failed to open ZIP file '%s'.", path.c_str());
    }

    DEBUG("Opened ZIP file '%s' for extracting.", path.c_str());

    // Go to first file inside ZIP container.
    int unzResult = unzGoToFirstFile(zipFile);
    if(unzResult != UNZ_OK)
    {
        unzClose(zipFile);
        THROW_IOEXCEPTION("Failed to go to the first file inside ZIP container. ZLib error: %d", unzResult);
    }

    // Create temporary folder for extracted files.
    std::string tmpDirectory = util::File::createTempDirectory();
    DEBUG("Extracting ZIP file to temporary directory '%s'", tmpDirectory.c_str());


    // Extract all files inside the ZIP container.
    for( ;; )
    {
        try
        {
            extractCurrentFile(zipFile, tmpDirectory);
        }
        catch(const IOException& e)
        {
            unzClose(zipFile);
            throw e;
        }

        // Go to the next file inside ZIP container.
        unzResult = unzGoToNextFile(zipFile);
        if(unzResult == UNZ_END_OF_LIST_OF_FILE)
        {
            break;
        }
        else if(unzResult != UNZ_OK)
        {
            unzClose(zipFile);
            THROW_IOEXCEPTION("Failed to go to the next file inside ZIP container. ZLib error: %d", unzResult);
        }
    }


    // Close ZIP file.
    unzResult = unzClose(zipFile);
    if(unzResult != UNZ_OK)
    {
        THROW_IOEXCEPTION("Failed to close ZIP file. ZLib error: %d", unzResult);
    }

    DEBUG("Successfully closed ZIP file '%s'.", path.c_str());
    return tmpDirectory;
}

/**
 * Extracts current file from ZIP file to directory pointed in <code>directory</code> parameter.
 *
 * @param zipFile pointer to opened ZIP file.
 * @param directory directory where current file from ZIP should be extracted.
 * @throws IOException throws exception if the extraction of the current file fails from ZIP
 *         file or creating new file to disk failed.
 */
namespace digidoc
{
void extractCurrentFile(unzFile zipFile, const std::string& directory) throw(IOException)
{
    DEBUG("ZipSerialize::extractCurrentFile(zipFile = 0x%X, directory = '%s')", (unsigned int)zipFile, directory.c_str());

    // Get filename of the current file inside ZIP container.
    // XXX: How long must the filename buffer be?
    char fileNameBuf[2048];
    int unzResult = unzGetCurrentFileInfo(zipFile, NULL, fileNameBuf, 2048, NULL, 0, NULL, 0);
    if(unzResult != UNZ_OK)
    {
        THROW_IOEXCEPTION("Failed to get filename of the current file inside ZIP container. ZLib error: %d", unzResult);
    }
    std::string fileName(fileNameBuf);

    // make sure the destination directory exists
    std::string path = digidoc::util::File::encodeName(util::File::path(directory, fileName));
    std::string subDirectory = util::File::directory(path);
    util::File::createDirectory(subDirectory);

    // Extract current file inside ZIP container.
    unzResult = unzOpenCurrentFile(zipFile);
    if(unzResult != UNZ_OK)
    {
        THROW_IOEXCEPTION("Failed to open current file inside ZIP container. ZLib error: %d", unzResult);
    }
    std::cout << "Extracted file " << fileName << " into the temp directory." << std::endl;

	if(fileName[fileName.size()-1] == '/')
	{
		return;
	}

    // Create new file to write the extracted data to.
    DEBUG("Extracting file '%s' to '%s'.", fileName.c_str(), path.c_str());
    FILE* pFile = fopen(path.c_str(), "wb");
    if(pFile == NULL)
    {
        unzCloseCurrentFile(zipFile);
        THROW_IOEXCEPTION("Failed to create new file '%s'.", path.c_str());
    }

    // Extract current file inside ZIP container.
    char buf[10240];
    for( ;; )
    {
        unzResult = unzReadCurrentFile(zipFile, buf, 10240);

        if(unzResult > 0)
        {
            // Write bytes to file.
            int bytesWritten = fwrite(buf, 1, unzResult, pFile);
            if(bytesWritten != unzResult)
            {
                unzCloseCurrentFile(zipFile);
                THROW_IOEXCEPTION("Failed to write %d bytes to file '%s', could only write %d bytes.", unzResult, path.c_str(), bytesWritten);
            }
        }
        else if(unzResult == UNZ_EOF)
        {
            break;
        }
        else
        {
            unzCloseCurrentFile(zipFile);
            THROW_IOEXCEPTION("Failed to read bytes from current file inside ZIP container. ZLib error: %d", unzResult);
        }
    }

    DEBUG("Finished writing data to file '%s', closing file.", path.c_str());

    // Close file.
    if(fclose(pFile) != 0)
    {
        unzCloseCurrentFile(zipFile);
        THROW_IOEXCEPTION("Failed to close file '%s'.", path.c_str());
    }

    // Close current file in ZIP container.
    unzResult = unzCloseCurrentFile(zipFile);
    if(unzResult != UNZ_OK)
    {
        THROW_IOEXCEPTION("Failed to close current file inside ZIP container. ZLib error: %d", unzResult);
    }
}
}

/**
 * Reinitializes ZipSerializer for new ZIP file creation. This method should be called before
 * creating new ZIP file.
 *
 * @see addFile(const std::string& containerPath, const std::string& path)
 * @see save()
 */
void digidoc::ZipSerialize::create()
{
    filesAdded.clear();
}

/**
 * Add new file to ZIP container. The file is actually archived to ZIP container after <code>save()</code>
 * method is called.
 *
 * @param containerPath file path inside ZIP file.
 * @param path full path of the file that should be added to ZIP file.
 * @see create()
 * @see save()
 */
void digidoc::ZipSerialize::addFile(const std::string& containerPath, const std::string& path)
{
    DEBUG("ZipSerialize::addFile(containerPath = '%s', path = '%s')", containerPath.c_str(), path.c_str());

    FileEntry fileEntry;
    fileEntry.containerPath = containerPath;
    fileEntry.path = path;
    filesAdded.push_back(fileEntry);
}

/**
 * Creates new ZIP file and adds all the added files to the ZIP file.
 *
 * @throws IOException throws exception if the file to be added did not exists or creating new
 *         ZIP file failed.
 * @see create()
 * @see addFile(const std::string& containerPath, const std::string& path)
 */
void digidoc::ZipSerialize::save() throw(IOException)
{
    DEBUG("ZipSerialize::save()");

    // No files added to ZIP.
    if(filesAdded.empty())
    {
        THROW_IOEXCEPTION("No files added to ZipSerialize, can not create ZIP file.");
    }

    // Create new ZIP file.
    std::string fileName = digidoc::util::File::encodeName(util::File::tempFileName());
    zipFile zipFile = zipOpen(fileName.c_str(), APPEND_STATUS_CREATE);
    DEBUG("Created ZIP file: fileName = '%s', zipFile = 0x%X", fileName.c_str(), (unsigned int)zipFile);
    if(zipFile == NULL)
    {
        THROW_IOEXCEPTION("Failed to create ZIP file '%s'.", path.c_str());
    }

    // Add files to ZIP.
    for(std::vector<FileEntry>::const_iterator iter = filesAdded.begin(); iter != filesAdded.end(); iter++)
    {
        // Open file to archive.
        std::string path = digidoc::util::File::encodeName(iter->path);
        FILE* pFile = fopen(path.c_str(), "rb");
        if(pFile == NULL)
        {
            zipClose(zipFile, NULL);
            THROW_IOEXCEPTION("Failed to open file '%s'.", path.c_str());
        }

        // Create new file inside ZIP container.
        int zipResult = zipOpenNewFileInZip(zipFile, iter->containerPath.c_str(), NULL, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
        if(zipResult != ZIP_OK)
        {
            zipClose(zipFile, NULL);
            THROW_IOEXCEPTION("Failed to create new file inside ZIP container. ZLib error: %d", zipResult);
        }

        DEBUG("Archiving file '%s' to ZIP file '%s' with path '%s'.", iter->path.c_str(), fileName.c_str(), iter->containerPath.c_str());

        // Archive file.
        char buf[10240];
        for( ;; )
        {
            unsigned int bytesRead = fread(buf, 1, 10240, pFile);
            if(bytesRead == 0)
            {
                break;
            }

            zipResult = zipWriteInFileInZip(zipFile, buf, bytesRead);
            if(zipResult != ZIP_OK)
            {
                fclose(pFile);
                zipCloseFileInZip(zipFile);
                zipClose(zipFile, NULL);
                THROW_IOEXCEPTION("Failed to write bytes to current file inside ZIP container. ZLib error: %d", zipResult);
            }
        }

        // Close file.
        if(fclose(pFile) != 0)
        {
            zipCloseFileInZip(zipFile);
            zipClose(zipFile, NULL);
            THROW_IOEXCEPTION("Failed to close file '%s'.", iter->path.c_str());
        }

        // Close created file inside ZIP container.
        zipResult = zipCloseFileInZip(zipFile);
        if(zipResult != ZIP_OK)
        {
            zipClose(zipFile, NULL);
            THROW_IOEXCEPTION("Failed to close current file inside ZIP container. ZLib error: %d", zipResult);
        }
    }


    // Close ZIP file.
    int zipResult = zipClose(zipFile, NULL);
    if(zipResult != ZIP_OK)
    {
        THROW_IOEXCEPTION("Failed to close ZIP file. ZLib error: %d", zipResult);
    }

    DEBUG("Successfully closed ZIP file '%s'.", fileName.c_str());

    // Move created temporary ZIP file to file 'path'.
    util::File::moveFile(fileName, path);

    // Clear added files list.
    filesAdded.clear();
}
