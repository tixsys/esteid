#include <iostream>
#include <fstream>
#include <sstream>
#include <set>

#include "log.h"
#include "BDoc.h"
#include "Conf.h"
#include "Util.h"
#include "xml/OpenDocument_manifest.hxx"

/**
 * Mimetype prefix.
 */
const std::string digidoc::BDoc::MIMETYPE_PREFIX = "application/vnd.bdoc-";

/**
 * XML namespace of the manifest XML file.
 */
const std::string digidoc::BDoc::MANIFEST_NAMESPACE = "urn:oasis:names:tc:opendocument:xmlns:manifest:1.0";

/**
 * Initialize BDOC container.
 */
digidoc::BDoc::BDoc()
 : version("1.0")
{
}

/**
 * Initialize BDOC container. Container data is read using <code>serializer</code> implementation.
 * Container data is validated to correspond to the BDOC specification.
 *
 * @param serializer serializer implementation, used to extract data from BDOC container.
 * @throws IOException is thrown if there was a failure reading BDOC container. For example reading
 *         file from disk failed.
 * @throws BDocException is thrown if BDOC container file does not correspond to the specification.
 */
void digidoc::BDoc::readFrom(std::auto_ptr<ISerialize> serializer) throw(IOException, BDocException)
{
    DEBUG("BDoc::readFrom(serializer = 0x%X '%s')", (unsigned int)serializer.get(), serializer->getPath().c_str());

    // Set container initializer.
    this->serializer = serializer;

    // Extract container to temporary directory.
    std::string tmpPath = this->serializer->extract();

    // Read and check mimetype.
    readMimetype(tmpPath);

    // Parse manifest, load documents and signatures, also check container file listing.
    parseManifestAndLoadFiles(tmpPath);

    // Validate signatures, also documents checksums are validated.
    validateSignatures();
}

/**
 * Saves the container using the <code>serializer</code> implementation provided in
 * <code>readFrom()</code> method.
 *
 * @throws IOException is thrown if there was a failure saving BDOC container. For example added
 *         document does not exist.
 * @throws BDocException is thrown if BDoc class is not correctly initialized.
 */
void digidoc::BDoc::save() throw(IOException, BDocException)
{
    // Check that serializer is provided.
    if(serializer.get() == NULL)
    {
        THROW_BDOCEXCEPTION("You can not use save() method if you didn't open a container with readFrom() method. Use saveTo() method in case of new a BDoc container.")
    }

    // Check that at least one document and signature is added to container.
    if(documents.empty() || signatures.empty())
    {
        THROW_BDOCEXCEPTION("No documents or no signatures added to the BDoc container.")
    }

    // Create new container.
    serializer->create();

    // Create mimetype file and add it to container.
    serializer->addFile("mimetype", createMimetype());

    // Create manifest file and add it to container.
    // NB! Don't change the order of signatures in list after manifest is created,
    //     otherwise manifest has incorrect signature mimetypes.
    serializer->addFile("META-INF/manifest.xml", createManifest());

    // Add all documents to container.
    for(std::vector<Document>::const_iterator iter = documents.begin(); iter != documents.end(); iter++)
    {
        serializer->addFile(Util::File::fileName(iter->getPath()), iter->getPath());
    }

    // Add all signatures to container.
    unsigned int i = 0;
    for(std::vector<Signature>::const_iterator iter = signatures.begin(); iter != signatures.end(); iter++)
    {
        serializer->addFile(Util::String::format("META-INF/signature%u.xml", i++), iter->saveToXml());
    }

    // Save container.
    serializer->save();
}

/**
 * Saves the container using the <code>serializer</code> implementation provided.
 *
 * @param serializer serializer implementation, used to save data to BDOC container.
 * @throws IOException is thrown if there was a failure saving BDOC container. For example added
 *         document does not exist.
 * @throws BDocException is thrown if BDOC class is not correctly initialized.
 * @see save()
 */
void digidoc::BDoc::saveTo(std::auto_ptr<ISerialize> serializer) throw(IOException, BDocException)
{
    this->serializer = serializer;
    save();
}

/**
 * Adds document to the container. Documents can be removed from container only
 * after all signatures are removed.
 *
 * @param document a document, which is added to the container.
 * @throws BDocException exception is thrown if the document path is incorrect or document
 *         with same file name already exists. Also no document can be added if the
 *         container already has one or more signatures.
 */
void digidoc::BDoc::addDocument(const Document& document) throw(BDocException)
{
    if(signatures.size() == 0)
    {
    	// Check that document file exists.
    	if(!Util::File::fileExists(document.getPath()))
    	{
            THROW_BDOCEXCEPTION("Document file '%s' does not exist.", document.getPath().c_str())
    	}

    	// Check if document with same filename exists.
    	std::string documentFileName = Util::File::fileName(document.getPath());
        for(std::vector<Document>::const_iterator iter = documents.begin(); iter != documents.end(); iter++)
        {
        	if(documentFileName.compare(Util::File::fileName(iter->getPath())) == 0)
        	{
                THROW_BDOCEXCEPTION("Document with same file name '%s' already exists '%s'.", document.getPath().c_str(), iter->getPath().c_str())
        	}
        }

        documents.push_back(document);
    }
    else
    {
        THROW_BDOCEXCEPTION("Can not add document to container which has signatures, remove all signatures before adding new document.")
    }
}

/**
 * Returns document referenced by document id.
 *
 * @param id document id.
 * @return returns document referenced by document id.
 * @throws BDocException throws exception if the document id is incorrect.
 */
digidoc::Document digidoc::BDoc::getDocument(unsigned int id) const throw(BDocException)
{
	if(documents.size() > id)
	{
	    return documents[id];
	}

    THROW_BDOCEXCEPTION("Incorrect document id %u, there are only %u documents in container.", id, documents.size())
}

/**
 * Removes document from container by document id. Documents can be
 * removed from container only after all signatures are removed.
 *
 * @param id document's id, which will be removed.
 * @throws BDocException throws exception if the document id is incorrect or there are
 *         one or more signatures.
 */
void digidoc::BDoc::removeDocument(unsigned int id) throw(BDocException)
{
    if(signatures.size() == 0)
    {
    	if(documents.size() > id)
    	{
            documents.erase(documents.begin() + id);
    	}
    	else
    	{
            THROW_BDOCEXCEPTION("Incorrect document id %u, there are only %u documents in container.", id, documents.size())
    	}
    }
    else
    {
        THROW_BDOCEXCEPTION("Can not remove document from container which has signatures, remove all signatures before removing document.")
    }
}

/**
 * @return returns number of documents in container.
 */
unsigned int digidoc::BDoc::documentCount() const
{
    return documents.size();
}

/**
 * Adds signature to the container.
 *
 * @param signature signature, which is added to the container.
 * @throws BDocException throws exception if there are no documents in container.
 */
void digidoc::BDoc::addSignature(const Signature& signature) throw(BDocException)
{
    if(!documents.empty())
    {
        signatures.push_back(signature);
    }
    else
    {
        THROW_BDOCEXCEPTION("No documents in container, can not add signature.")
    }
}

/**
 * Returns signature referenced by signature id.
 *
 * @param id signature id.
 * @return returns signature referenced by signature id.
 * @throws BDocException throws exception if the signature id is incorrect.
 */
digidoc::Signature digidoc::BDoc::getSignature(unsigned int id) const throw(BDocException)
{
	if(signatures.size() > id)
	{
	    return signatures[id];
	}

    THROW_BDOCEXCEPTION("Incorrect signature id %u, there are only %u signatures in container.", id, signatures.size())
}

/**
 * Removes signature from container by signature id.
 *
 * @param id signature's id, which will be removed.
 * @throws BDocException throws exception if the signature id is incorrect.
 */
void digidoc::BDoc::removeSignature(unsigned int id) throw(BDocException)
{
    if(signatures.size() > id)
    {
        signatures.erase(signatures.begin() + id);
    }
    else
    {
        THROW_BDOCEXCEPTION("Incorrect signature id %u, there are only %u signatures in container.", id, signatures.size())
    }
}

/**
 * @return returns number of signatures in container.
 */
unsigned int digidoc::BDoc::signatureCount() const
{
    return signatures.size();
}

/**
 * Sets container version.
 *
 * @param version container version.
 */
void digidoc::BDoc::setVersion(const std::string& version)
{
    this->version = version;
}

/**
 * @return returns BDoc container mimetype.
 */
std::string digidoc::BDoc::getMimeType() const
{
    return MIMETYPE_PREFIX + version;
}

/**
 * Creates mimetype file and returns its path.
 *
 * @return returns path to created mimetype file.
 * @throws IOException exception is thrown if file creation failed.
 */
std::string digidoc::BDoc::createMimetype() throw(IOException)
{
    DEBUG("BDoc::createMimetype()");

    // Create mimetype file.
    std::string fileName = Util::File::tempFileName();
    std::ofstream ofs(fileName.c_str());
    ofs << getMimeType();
    ofs.close();

    if(ofs.fail())
    {
        THROW_IOEXCEPTION("Failed to create mimetype file to '%s'.", fileName.c_str());
    }

    return fileName;
}

/**
 * Creates BDoc container manifest file and returns its path.
 *
 * @return returns created manifest file path.
 * @throws IOException exception is thrown if manifest file creation failed.
 */
std::string digidoc::BDoc::createManifest() throw(IOException)
{
    DEBUG("digidoc::BDoc::createManifest()")

    std::string fileName = Util::File::tempFileName();

    try
    {
        manifest::Manifest manifest;

        // Add container file entry with mimetype.
        manifest.file_entry().push_back(manifest::File_entry("/", getMimeType()));

        // Add documents with mimetypes.
        for(std::vector<Document>::const_iterator iter = documents.begin(); iter != documents.end(); iter++)
        {
            manifest::File_entry fileEntry(Util::File::fileName(iter->getPath()), iter->getMediaType());
            manifest.file_entry().push_back(fileEntry);
        }

        // Add signatures with mimetypes.
        unsigned int i = 0;
        for(std::vector<Signature>::const_iterator iter = signatures.begin(); iter != signatures.end(); iter++)
        {
            manifest::File_entry fileEntry(Util::String::format("META-INF/signature%u.xml", i++), iter->getMediaType());
            manifest.file_entry().push_back(fileEntry);
        }

        // Serialize XML to file.
        xml_schema::NamespaceInfomap map;
        map["manifest"].name = MANIFEST_NAMESPACE;
        DEBUG("Serializing manifest XML to '%s'", fileName.c_str())
        std::ofstream ofs(fileName.c_str());
        manifest::manifest(ofs, manifest, map);
        //manifest::manifest(ofs, manifest, map, "", xml_schema::Flags::dont_pretty_print);
        ofs.close();

        if(ofs.fail())
        {
            THROW_IOEXCEPTION("Failed to create manifest XML file to '%s'.", fileName.c_str());
        }
    }
    catch(const xml_schema::Exception& e)
    {
        std::ostringstream oss;
        oss << e;
        THROW_IOEXCEPTION("Failed to create manifest XML file. Error: %s", oss.str().c_str())
    }

    return fileName;
}

/**
 * Reads and parses container mimetype. Checks that the mimetype is supported.
 *
 * @param path path to container directory.
 * @throws IOException exception is thrown if there was error reading mimetype file from disk.
 * @throws BDocException exception is thrown if the parsed mimetype is incorrect.
 */
void digidoc::BDoc::readMimetype(std::string path) throw(IOException, BDocException)
{
    DEBUG("BDoc::readMimetype(path = '%s')", path.c_str())

    // Read mimetype from file.
    std::string fileName = Util::File::path(path, "mimetype");
    std::ifstream ifs(fileName.c_str());
    std::string mimetype;
    ifs >> mimetype;
    ifs.close();

    if(ifs.fail())
    {
        THROW_IOEXCEPTION("Failed to read mimetype file from '%s'.", fileName.c_str());
    }

    // Check that mimetype version is correct.
    DEBUG("mimetype = '%s'", mimetype.c_str())
    if(getMimeType().compare(mimetype) != 0)
    {
        THROW_BDOCEXCEPTION("Incorrect mimetype '%s', expecting '%s'", mimetype.c_str(), getMimeType().c_str());
    }
}

/**
 * Parses manifest file and checks that files described in manifest exist, also
 * checks that no extra file do exist that are not described in manifest.xml.
 *
 * @param path directory on disk of the BDOC container.
 * @throws IOException exception is thrown if the manifest.xml file parsing failed.
 * @throws BDocException
 */
void digidoc::BDoc::parseManifestAndLoadFiles(std::string path) throw(IOException, BDocException)
{
    DEBUG("BDoc::readManifest(path = '%s')", path.c_str())

    try
    {
    	// Parse manifest file.
        std::string fileName = Util::File::path(path, "META-INF/manifest.xml");
        xml_schema::Properties properties;
        properties.schema_location(MANIFEST_NAMESPACE, Conf::getInstance()->getManifestXsdPath());
        std::auto_ptr<manifest::Manifest> manifest(manifest::manifest(fileName, 0, properties));

        // Extract and validate file list from manifest.
        std::set<std::string> manifestFiles;
        bool mimetypeChecked = false;
        for(manifest::Manifest::File_entrySequence::const_iterator iter = manifest->file_entry().begin(); iter != manifest->file_entry().end(); iter++)
        {
            DEBUG("full_path = '%s', media_type = '%s'", iter->full_path().c_str(), iter->media_type().c_str())

            // Check container mimetype.
            if(std::string("/").compare(iter->full_path()) == 0)
            {
                if(mimetypeChecked)
                {
                    THROW_BDOCEXCEPTION("Manifest has more than one container media type defined.");
                }

                if(getMimeType().compare(iter->media_type()) != 0)
                {
                    THROW_BDOCEXCEPTION("Manifest has incorrect BDCO container media type defined '%s', expecting '%s'.", iter->media_type().c_str(), getMimeType().c_str());
                }

                DEBUG("BDOC mimetype OK")
                mimetypeChecked = true;
                continue;
            }

            // Check that file reference is not defined already and add relative file reference to set.
            if(manifestFiles.find(iter->full_path()) != manifestFiles.end())
            {
                THROW_BDOCEXCEPTION("Manifest multiple entries defined for file '%s'.", iter->media_type().c_str());
            }
            manifestFiles.insert(iter->full_path());

            // Add document to documents list.
            if(iter->full_path().find_first_of("/") == std::string::npos)
            {
            	if(!Util::File::fileExists(Util::File::path(path, iter->full_path())))
            	{
                    THROW_BDOCEXCEPTION("File described in manifest '%s' does not exist in BDOC container.", iter->full_path().c_str())
            	}

                documents.push_back(Document(Util::File::path(path, iter->full_path()), iter->media_type()));
                continue;
            }

            // Add signature to signatures list.
            DEBUG("%s :: %u", iter->full_path().c_str(), iter->full_path().find_first_of("META-INF/"))
            if(iter->full_path().find_first_of("META-INF/") == 0)
            {
                std::string fileName(iter->full_path().substr(9));
                DEBUG("signature filename :: '%s'", fileName.c_str())

                if(fileName.find_first_of("/") != std::string::npos)
                {
                    THROW_BDOCEXCEPTION("Unexpected file described in manifest '%s'.", iter->full_path().c_str());
                }

            	if(!Util::File::fileExists(Util::File::path(Util::File::path(path, "META-INF"), fileName)))
            	{
                    THROW_BDOCEXCEPTION("File described in manifest '%s' does not exist in BDOC container.", iter->full_path().c_str())
            	}

                signatures.push_back(Signature(Util::File::path(Util::File::path(path, "META-INF"), fileName), iter->media_type()));
                continue;
            }

            // Found unexpected file description in manifest.
            THROW_BDOCEXCEPTION("Unexpected file description found in container manifest.")
        }

        if(!mimetypeChecked)
        {
            THROW_BDOCEXCEPTION("Manifest file does not have BDOC media type described.");
        }

        // Check that there are no unexpected files in container.
        std::vector<std::string> containerFiles = Util::File::listFiles(path);
        for(std::vector<std::string>::const_iterator iter = containerFiles.begin(); iter != containerFiles.end(); iter++)
        {
        	std::string containerFile = *iter;
        	if(std::string("mimetype").compare(containerFile) == 0
            || std::string("META-INF/manifest.xml").compare(containerFile) == 0)
            {
                continue;
            }

            std::replace(containerFile.begin(), containerFile.end(), '\\', '/');

            if(manifestFiles.find(containerFile) == manifestFiles.end())
            {
                THROW_BDOCEXCEPTION("File '%s' found in BDOC container is not described in manifest.", containerFile.c_str())
            }
        }
    }
    catch(const xml_schema::Exception& e)
    {
        std::ostringstream oss;
        oss << e;
        THROW_IOEXCEPTION("Failed to parse manifest XML: %s", oss.str().c_str())
    }
}

/**
 *
 * throws BDocException
 */
void digidoc::BDoc::validateSignatures() throw(BDocException)
{
    DEBUG("BDoc::validateSignatures()")
    // TODO: validate all signatures and file checksums.
    // Failide olemasolu on juba kontrollitud.
}
