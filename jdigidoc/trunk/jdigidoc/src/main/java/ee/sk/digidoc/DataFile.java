/*
 * DataFile.java
 * PROJECT: JDigiDoc
 * DESCRIPTION: Digi Doc functions for creating
 *	and reading signed documents. 
 * AUTHOR:  Veiko Sinivee, S|E|B IT Partner Estonia
 *==================================================
 * Copyright (C) AS Sertifitseerimiskeskus
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * GNU Lesser General Public Licence is available at
 * http://www.gnu.org/copyleft/lesser.html
 *==================================================
 */

package ee.sk.digidoc;
import java.io.Serializable;
import java.util.ArrayList;
import java.io.ByteArrayOutputStream;
import java.security.MessageDigest;
import java.io.DataInputStream;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.File;
import java.io.BufferedReader;
import ee.sk.utils.ConvertUtils;
import java.io.OutputStream;
import ee.sk.digidoc.factory.CanonicalizationFactory;
import ee.sk.utils.ConfigManager;
import ee.sk.utils.Base64InputStream;
//A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;
import java.io.BufferedInputStream;
import java.io.InputStreamReader;
//L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
import org.w3c.dom.Node;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.OutputKeys;
//Logging classes
import org.apache.log4j.Logger;

/**
 * Represents a DataFile instance, that either
 * contains payload data or references and external
 * DataFile.
 * @author  Veiko Sinivee
 * @version 1.0
 */
public class DataFile implements Serializable
{
    private static final long serialVersionUID = 1L;
	/** content type of the DataFile */
    private String m_contentType;
    /** filename */
    private String m_fileName;
    //A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
    /** full path in BDOC container */
    private String m_fullName;
    //L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
    /** Id attribute od this DataFile */
    private String m_id;
    /** mime type of the file */
    private String m_mimeType;
    /** file size on bytes */
    private long m_size;
    /** digest type of detatched file */
    private String m_digestType;
    /** digest value of detatched file */
    private byte[] m_digestValue;
    /** digest value of the XML form of <DataFile>
     * If read from XML file then calculated immediately
     * otherwise on demand
     */
    private byte[] m_origDigestValue;
    /** additional attributes */
    private ArrayList m_attributes;
    /** data file contents in original form */
    private byte[] m_body;
    /** initial codepage of DataFile data */
    private String m_codepage;
    /** parent object reference */
    private SignedDoc m_sigDoc;
    
    /** allowed values for content type */
    public static final String CONTENT_DETATCHED = "DETATCHED";
    public static final String CONTENT_EMBEDDED = "EMBEDDED";
    public static final String CONTENT_EMBEDDED_BASE64 = "EMBEDDED_BASE64";
    /** the only allowed value for digest type */
    public static final String DIGEST_TYPE_SHA1 = "sha1";
    private static int block_size = 2048;
    /** log4j logger */
	private transient Logger m_logger = null;
	/** temp file used to cache DataFile data if caching is enabled */
	private File m_fDfCache = null;
	//A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
	/** in BDOC's case datafile is in container */
	private ZipEntry m_ZipEntry = null;
	/** BDOC's  container */
	private ZipFile m_BDOCcontainer = null;
	/** 
     * Creates new DataFile 
     * @param id id of the DataFile
     * @param contenType DataFile content type
     * @param original file name (without path!), in BDOC case path starting from the beginning of container
     * @param mimeType contents mime type
     * @param sdoc parent object
     * @param sdoc ZipEntry zEntry
     * @param sdoc ZipFile bDOCcontainer
     * @param original full filename with path inside BDOC container
     * @throws DigiDocException for validation errors
     */
    public DataFile(String id, String contentType, String fileName, String mimeType, SignedDoc sdoc, ZipEntry zEntry, ZipFile bDOCcontainer, String fullName) 
        throws DigiDocException
    {
        this(id, contentType, fileName, mimeType, sdoc);
        m_ZipEntry=zEntry;
        m_BDOCcontainer=bDOCcontainer;
        //A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1.2
        m_fullName=ConvertUtils.data2str(fullName.getBytes(), m_codepage);
      //L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1.2
    }
//L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1	
	

    /** 
     * Creates new DataFile 
     * @param id id of the DataFile
     * @param contenType DataFile content type
     * @param fileName original file name (without path!)
     * @param mimeType contents mime type
     * @param sdoc parent object
     * @throws DigiDocException for validation errors
     */
    public DataFile(String id, String contentType, String fileName, String mimeType, SignedDoc sdoc) 
        throws DigiDocException
    {
        setId(id);
        setFileName(fileName);
        setMimeType(mimeType);
        m_sigDoc = sdoc;
        m_size = 0;
        m_digestType = null;
        m_digestValue = null;
        m_attributes = null;
        m_body = null;
        m_codepage = "UTF-8";
        m_origDigestValue = null;
        m_fDfCache = null;
        m_logger = Logger.getLogger(DataFile.class);
      //A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1.2
      //IS FIX utf8
        if(m_sigDoc != null && m_sigDoc.getFormat().equals(SignedDoc.FORMAT_BDOC)) {
        	try{
        		String simpleFileName = new File(fileName).getName();
        		String convertedStr = new String(simpleFileName.getBytes("UTF8"), "UTF8");
        		this.setFullName(convertedStr);
        	} catch(Exception ex) {
                DigiDocException.handleException(ex, DigiDocException.ERR_UTF8_CONVERT);
            }
        }
      //IS Fix check contenttype algus
        setContentType(contentType);
      //IS Fix check contenttype lopp
      //L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1.2
    }
    
    /**
     * Accessor for temp file object used to cache DataFile data
     * if caching is enabled.
     * @return temp file object used to cache DataFile data
     */
    public File getDfCacheFile() {
    	return m_fDfCache;
    }
        
    /**
     * Removes temporary DataFile cache file
     */
    public void cleanupDfCache() {
    	if(m_fDfCache != null) {
    		if(m_logger.isDebugEnabled())
    			m_logger.debug("Removing cache file for df: " + m_fDfCache.getAbsolutePath());
    		m_fDfCache.delete();
    	}
    	m_fDfCache = null;
    }
    
    /**
     * Accessor for body attribute. 
     * Note that the body is normally NOT LOADED
     * from file and this attribute is empty!
     * @return value of body attribute
     */
    public byte[] getBody() 
    	throws DigiDocException
    {
    	if(m_fDfCache != null) {
    		try {
    			byte[] data = SignedDoc.readFile(m_fDfCache);
    			if(m_contentType.equals(CONTENT_EMBEDDED_BASE64))
    	        	data = Base64Util.decode(data);
    			return data;
    		} catch(Exception ex) {
    			DigiDocException.handleException(ex, DigiDocException.ERR_READ_FILE);
    		}
    	}
        return m_body;
    }
    
    /**
     * Mutator for body attribute. For
     * any bigger files don't use this method!
     * If you are using very small messages onthe other hand
     * then this might speed things up.
     * This method should not be publicly used to assign
     * data to body. If you do then you must also set the
     * initial codepage and size of body!
     * @param data new value for body attribute
     */    
    public void setBody(byte[] data) 
    	throws DigiDocException
    {
    	try {
    		m_body = data;
    		if(data != null) {
    			m_size = data.length;
    			storeInTempFile();
    		}
    	} catch(IOException ex) {
            DigiDocException.handleException(ex, DigiDocException.ERR_WRITE_FILE);
        }
    }
    
    /**
     * Sets DataFile contents from an input stream.
     * This method allways uses temporary files to read out
     * the input stream first in order to determine the
     * size of data. Caller can close the stream after
     * invoking this method because data has been copied.
     * Data is not yet converted to base64 (if required)
     * nor is the hash code calculated at this point.
     * Please not that data is stored in original binary format,
     * so getBody() etc. will not deliver correct result
     * until digidoc has been actually written to disk and read
     * in again.
     * @param is input stream delivering the data
     */
    public void setBodyFromStream(InputStream is)
    	throws DigiDocException
    {
    	// copy data to temp file
    	try {
    		File fCacheDir = new File(ConfigManager.instance().
        			getStringProperty("DIGIDOC_DF_CACHE_DIR", System.getProperty("java.io.tmpdir")));
        	String dfId = new Long(System.currentTimeMillis()).toString();
        	m_fDfCache = File.createTempFile(dfId, ".df", fCacheDir);
        	FileOutputStream fos = new FileOutputStream(m_fDfCache);
        	m_body = null;
        	byte[] data = new byte[2048];
        	int nRead = 0;
        	m_size = 0;
        	do {
        		nRead = is.read(data);
        		if(nRead > 0) {
        			fos.write(data, 0, nRead);
        			m_size += nRead;
        		}
        	} while(nRead > 0);
        	fos.close();
        	if(m_logger.isDebugEnabled())
        		m_logger.debug("DF: " + m_id + " size: " + m_size + " cache-file: " + m_fDfCache.getAbsolutePath());
    	} catch(IOException ex) {
    		DigiDocException.handleException(ex, DigiDocException.ERR_WRITE_FILE);
    	}
    }
    
    /**
     * Accessor for body attribute. 
     * Returns the body as a string. Takes in
     * account the initial codepage. usable
     * only for EMBEDDED type of documents or
     * if body is stored in Base64 then you have to be
     * sure that the converted data is textual and
     * can be returned as a String after decoding.
     * @return body as string
     */
    public String getBodyAsString() 
        throws DigiDocException
    {
        String str = null;
        if(m_fDfCache != null) {
    		try {
    			byte[] data = SignedDoc.readFile(m_fDfCache);
    			if(m_contentType.equals(CONTENT_EMBEDDED))
    	            str = ConvertUtils.data2str(data, m_codepage);
    	        if(m_contentType.equals(CONTENT_EMBEDDED_BASE64))
    	        	str = ConvertUtils.data2str(Base64Util.decode(data), m_codepage);
    		} catch(Exception ex) {
    			DigiDocException.handleException(ex, DigiDocException.ERR_READ_FILE);
    		}
    	} else {
          if(m_contentType.equals(CONTENT_EMBEDDED))
            str = ConvertUtils.data2str(m_body, m_codepage);
          if(m_contentType.equals(CONTENT_EMBEDDED_BASE64))
        	str = ConvertUtils.data2str(Base64Util.decode(m_body), m_codepage);
    	}
        return str;
    }

    /**
     * Accessor for body attribute. 
     * Returns the body as a byte array. If body contains
     * embedded base64 data then this is decoded first
     * and decoded actual payload data returned.
     * @return body as a byte array
     */
    public byte[] getBodyAsData() 
    	throws DigiDocException
    {
    	byte[] data = null;
    	if(m_fDfCache != null) {
    		try {
    			data = SignedDoc.readFile(m_fDfCache);
    			if(m_contentType.equals(CONTENT_EMBEDDED_BASE64))
    	        	data = Base64Util.decode(data);
    		} catch(Exception ex) {
    			DigiDocException.handleException(ex, DigiDocException.ERR_READ_FILE);
    		}
    	} else {
          if(m_contentType.equals(CONTENT_EMBEDDED))
        	data = m_body;
          if(m_contentType.equals(CONTENT_EMBEDDED_BASE64))
        	data = Base64Util.decode(m_body);
    	}
        return data;
    }

    /**
     * Accessor for body attribute. 
     * Returns the body as an input stream. If body contains
     * embedded base64 data then this is decoded first
     * and decoded actual payload data returned.
     * @return body as a byte array
     */
    public InputStream getBodyAsStream() 
    	throws DigiDocException
    {
    	InputStream strm = null;
    	if(m_fDfCache != null) {
    		try {
    			if(m_contentType.equals(CONTENT_EMBEDDED))
    				strm = new FileInputStream(m_fDfCache);
    			if(m_contentType.equals(CONTENT_EMBEDDED_BASE64))
    	        	strm = new Base64InputStream(new FileInputStream(m_fDfCache));
    		} catch(Exception ex) {
    			DigiDocException.handleException(ex, DigiDocException.ERR_READ_FILE);
    		}
    	} else {
    		//A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
    		if(m_ZipEntry!=null && m_BDOCcontainer !=null){
    			try{
    				strm =  m_BDOCcontainer.getInputStream(m_ZipEntry);
    			}catch(Exception ex) {
    	    		DigiDocException.handleException(ex, DigiDocException.ERR_READ_FILE);
    	    	}
    		} else {
    		//L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
    			if(m_contentType.equals(CONTENT_EMBEDDED))
    				strm = new ByteArrayInputStream(m_body);
    			if(m_contentType.equals(CONTENT_EMBEDDED_BASE64))
    				strm = new ByteArrayInputStream(Base64Util.decode(m_body));
    		}
    	}
        return strm;
    }
    
    /**
     * Checks if this DataFile object schould use a temp file
     * to store it's data because of memory cache size limitation
     * @return true if this object schould use temp file
     */
    public boolean schouldUseTempFile()
    {
    	long lMaxDfCached = ConfigManager.instance().
			getLongProperty("DIGIDOC_MAX_DATAFILE_CACHED", Long.MAX_VALUE);
    	return (m_size > lMaxDfCached);
    }
    
    /**
     * Helper method to enable temporary cache file for this DataFile
     * @return new temporary file object
     * @throws IOException
     */
    public File createCacheFile()
		throws IOException
    {
    	m_fDfCache = null;
    	if(schouldUseTempFile()) {
    		File fCacheDir = new File(ConfigManager.instance().
        			getStringProperty("DIGIDOC_DF_CACHE_DIR", System.getProperty("java.io.tmpdir")));
        	String dfId = new Long(System.currentTimeMillis()).toString();
        	m_fDfCache = File.createTempFile(dfId, ".df", fCacheDir);
    	}
    	return m_fDfCache;
    }
    
    /**
     * Helper method to store body in file if it exceeds the 
     * memory cache limit
     * @throws IOException
     */
    private void storeInTempFile()
    	throws IOException
    {
    	File f = createCacheFile();
    	if(f != null) {
    		FileOutputStream fos = new FileOutputStream(f);
    		fos.write(m_body);
    		fos.close();
    		// remove memory cache if stored in file
    		m_body = null;
    	}
    }
    
    /**
     * Use this method to assign data directly to body.
     * If you do this then the input file will not be read.
     * This also sets the initial size and codepage for you
     * @param data new value for body attribute
     */    
    public void setBody(byte[] data, String codepage) 
    	throws DigiDocException
    {
    	try {
          m_body = data;
          m_codepage = codepage;
          m_size = m_body.length;
          // check if data must be stored in file instead
          storeInTempFile();
    	} catch(IOException ex) {
            DigiDocException.handleException(ex, DigiDocException.ERR_WRITE_FILE);
        }
    }
    
    /**
     * Use this method to assign data directly to body.
     * Input data is an XML subtree
     * @param xml xml subtree containing input data
     * @param codepage input data's original codepage
     */    
    public void setBody(Node xml)
        throws DigiDocException
    {
        try {
            ByteArrayOutputStream bos = new ByteArrayOutputStream();
            TransformerFactory tFactory = TransformerFactory.newInstance();
            Transformer transformer = tFactory.newTransformer();
            transformer.setOutputProperty(OutputKeys.OMIT_XML_DECLARATION, "yes");
            transformer.setOutputProperty(OutputKeys.ENCODING, "UTF-8");
            DOMSource source = new DOMSource(xml);
            StreamResult result = new StreamResult(bos);
            transformer.transform(source, result);
            m_body = bos.toByteArray();  
            // DOM library always outputs in UTF-8
            m_codepage = "UTF-8";
            m_size = m_body.length;
            // check if data must be stored in file instead
            storeInTempFile();
        } catch(Exception ex) {
            DigiDocException.handleException(ex, DigiDocException.ERR_XML_CONVERT);
        }
    }
    
    /**
     * Accessor for initialCodepage attribute. 
     * @return value of initialCodepage attribute
     */
    public String getInitialCodepage() {
        return m_codepage;
    }
    
    /**
     * Mutator for initialCodepage attribute. 
     * If you use setBody() or assign data from a file
     * which is not in UTF-8 and then use CONTENT_EMBEDDED
     * then you must use this method to tell the library
     * in which codepage your data is so that we
     * can convert it to UTF-8.
     * @param data new value for initialCodepage attribute
     */    
    public void setInitialCodepage(String data) 
    {
        m_codepage = data;
    }

    /**
     * Accessor for contentType attribute
     * @return value of contentType attribute
     */
    public String getContentType() {
        return m_contentType;
    }
    
    /**
     * Mutator for contentType attribute
     * @param str new value for contentType attribute
     * @throws DigiDocException for validation errors
     */    
    public void setContentType(String str) 
        throws DigiDocException
    {
        DigiDocException ex = validateContentType(str);
        if(ex != null)
            throw ex;
        m_contentType = str;
    }
    
    /**
     * Helper method to validate a content type
     * @param str input data
     * @return exception or null for ok
     */
    private DigiDocException validateContentType(String str)
    {
        DigiDocException ex = null;
        if(str == null || 
           (!str.equals(CONTENT_DETATCHED) && 
            !str.equals(CONTENT_EMBEDDED) &&
            !str.equals(CONTENT_EMBEDDED_BASE64)))
            ex = new DigiDocException(DigiDocException.ERR_DATA_FILE_CONTENT_TYPE, 
                "Currently supports only content types: DETATCHED, EMBEDDED and EMBEDDED_BASE64", null);
      //IS Fix check contenttype algus
        else if ((m_sigDoc.getFormat().equals(SignedDoc.FORMAT_BDOC)) &&
        		(!str.equals(CONTENT_EMBEDDED))){
        	ex = new DigiDocException(DigiDocException.ERR_DATA_FILE_CONTENT_TYPE, 
                    "BDOC supports only content type: EMBEDDED", null);
        }
      //IS Fix check contenttype lopp
        return ex;
    }

    /**
     * Accessor for fileName attribute
     * @return value of fileName attribute
     */
    public String getFileName() {
        return m_fileName;
    }
    
    /**
     * Mutator for fileName attribute
     * @param str new value for fileName attribute
     * @throws DigiDocException for validation errors
     */    
    public void setFileName(String str) 
        throws DigiDocException
    {
        DigiDocException ex = validateFileName(str);
        if(ex != null)
            throw ex;
        m_fileName = str;
    }
    
    /**
     * Helper method to validate a file name
     * @param str input data
     * @return exception or null for ok
     */
    private DigiDocException validateFileName(String str)
    {
        DigiDocException ex = null;
        if(str == null)
            ex = new DigiDocException(DigiDocException.ERR_DATA_FILE_FILE_NAME, 
                "Filename is a required attribute", null);
        return ex;
    }

    /**
     * Accessor for id attribute
     * @return value of id attribute
     */
    public String getId() {
        return m_id;
    }
    
    /**
     * Mutator for id attribute
     * @param str new value for id attribute
     * @throws DigiDocException for validation errors
     */    
    public void setId(String str) 
        throws DigiDocException
    {
        DigiDocException ex = validateId(str, false);
        if(ex != null)
            throw ex;
        m_id = str;
    }
    
    /**
     * Helper method to validate an id
     * @param str input data
     * @param bStrong flag that specifies if Id atribute value is to
     * be rigorously checked (according to digidoc format) or only
     * as required by XML-DSIG
     * @return exception or null for ok
     */
    private DigiDocException validateId(String str, boolean bStrong)
    {
        DigiDocException ex = null;
        if (m_sigDoc != null && !m_sigDoc.getFormat().equals(SignedDoc.FORMAT_BDOC)){
        	if(str == null)
        		ex = new DigiDocException(DigiDocException.ERR_DATA_FILE_ID, 
        				"Id is a required attribute", null);
        	if(str != null && bStrong && 
        		(str.charAt(0) != 'D' || !Character.isDigit(str.charAt(1))))
        		ex = new DigiDocException(DigiDocException.ERR_DATA_FILE_ID, 
                    "Id attribute value has to be in form D<number>", null);
        }
        return ex;
    }

    /**
     * Accessor for mimeType attribute
     * @return value of mimeType attribute
     */
    public String getMimeType() {
        return m_mimeType;
    }
    
    /**
     * Mutator for mimeType attribute
     * @param str new value for mimeType attribute
     * @throws DigiDocException for validation errors
     */    
    public void setMimeType(String str) 
        throws DigiDocException
    {
        DigiDocException ex = validateMimeType(str);
        if(ex != null)
            throw ex;
        m_mimeType = str;
    }
    
    /**
     * Helper method to validate a mimeType
     * @param str input data
     * @return exception or null for ok
     */
    private DigiDocException validateMimeType(String str)
    {
        DigiDocException ex = null;
        if(str == null)
            ex = new DigiDocException(DigiDocException.ERR_DATA_FILE_MIME_TYPE, 
                "MimeType is a required attribute", null);
        return ex;
    }
    
    /**
     * Accessor for size attribute
     * @return value of size attribute
     */
    public long getSize() {
        return m_size;
    }
    
    /**
     * Mutator for size attribute
     * @param l new value for size attribute
     * @throws DigiDocException for validation errors
     */    
    public void setSize(long l) 
        throws DigiDocException
    {
        DigiDocException ex = validateSize(l);
        if(ex != null)
            throw ex;
        m_size = l;
    }
    
    /**
     * Helper method to validate a mimeType
     * @param l input data
     * @return exception or null for ok
     */
    private DigiDocException validateSize(long l)
    {
        DigiDocException ex = null;
        if (m_sigDoc != null && !m_sigDoc.getFormat().equals(SignedDoc.FORMAT_BDOC)){
        if(l <= 0)
            ex = new DigiDocException(DigiDocException.ERR_DATA_FILE_SIZE, 
                "Size must be greater than zero", null);
        }
        return ex;
    }

    /**
     * Accessor for digestType attribute
     * @return value of digestType attribute
     */
    public String getDigestType() {
        return m_digestType;
    }
    
    /**
     * Mutator for digestType attribute
     * @param str new value for digestType attribute
     * @throws DigiDocException for validation errors
     */    
    public void setDigestType(String str) 
        throws DigiDocException
    {
        DigiDocException ex = validateDigestType(str);
        if(ex != null)
            throw ex;
        m_digestType = str;
    }
    
    /**
     * Helper method to validate a digestType
     * @param str input data
     * @return exception or null for ok
     */
    private DigiDocException validateDigestType(String str)
    {
        DigiDocException ex = null;
        if(str != null && !str.equals(DIGEST_TYPE_SHA1))
            ex = new DigiDocException(DigiDocException.ERR_DATA_FILE_DIGEST_TYPE, 
                "The only supported digest type is sha1", null);
        return ex;
    }

    /**
     * Accessor for digestValue attribute
     * @return value of digestValue attribute
     */
    public byte[] getDigestValue() 
        throws DigiDocException
    {
        return m_digestValue;
    }
    
    /**
     * Mutator for digestValue attribute
     * @param data new value for digestValue attribute
     * @throws DigiDocException for validation errors
     */    
    public void setDigestValue(byte[] data) 
        throws DigiDocException
    {
        DigiDocException ex = validateDigestValue(data);
        if(ex != null)
            throw ex;
        m_digestValue = data;
    }
    
    /**
     * Accessor for digest attribute
     * @return value of digest attribute
     */
    public byte[] getDigest() 
        throws DigiDocException
    {
        if(m_origDigestValue == null)
            calculateFileSizeAndDigest(null);
        return m_origDigestValue;
    }
    
    /**
     * Mutator for digest attribute
     * @param data new value for digest attribute
     * @throws DigiDocException for validation errors
     */    
    public void setDigest(byte[] data) 
        throws DigiDocException
    {
        DigiDocException ex = validateDigestValue(data);
        if(ex != null)
            throw ex;
        m_origDigestValue = data;
    }
    
    /**
     * Helper method to validate a digestValue
     * @param str input data
     * @return exception or null for ok
     */
    private DigiDocException validateDigestValue(byte[] data)
    {
        DigiDocException ex = null;
        if(data != null && data.length != SignedDoc.SHA1_DIGEST_LENGTH)
            ex = new DigiDocException(DigiDocException.ERR_DATA_FILE_DIGEST_VALUE, 
                "SHA1 digest value must be 20 bytes", null);
        return ex;
    }
    
    /**
     * Returns the count of attributes
     * @return count of attributes
     */
    public int countAttributes()
    {
        return ((m_attributes == null) ? 0 : m_attributes.size());
    }
  
    /**
     * Adds a new DataFileAttribute object
     * @param attr DataFileAttribute object to add
     */
    public void addAttribute(DataFileAttribute attr) 
    {
        if(m_attributes == null)
            m_attributes = new ArrayList();
        m_attributes.add(attr);
    }
    
    /**
     * Returns the desired DataFileAttribute object
     * @param idx index of the DataFileAttribute object
     * @return desired DataFileAttribute object
     */
    public DataFileAttribute getAttribute(int idx) {
        return (DataFileAttribute)m_attributes.get(idx);
    }
    
    /**
     * Helper method to validate the whole
     * DataFile object
     * @param bStrong flag that specifies if Id atribute value is to
     * be rigorously checked (according to digidoc format) or only
     * as required by XML-DSIG
     * @return a possibly empty list of DigiDocException objects
     */
    public ArrayList validate(boolean bStrong)
    {
        ArrayList errs = new ArrayList();
        DigiDocException ex = validateContentType(m_contentType);
        if(ex != null)
            errs.add(ex);
        ex = validateFileName(m_fileName);
        if(ex != null)
            errs.add(ex);
        ex = validateId(m_id, bStrong);
        if(ex != null)
            errs.add(ex);
        ex = validateMimeType(m_mimeType);
        if(ex != null)
            errs.add(ex);
        ex = validateSize(m_size);
        if(ex != null)
            errs.add(ex);
        ex = validateDigestType(m_digestType);
        if(ex != null)
            errs.add(ex);
        ex = validateDigestValue(m_digestValue);
        if(ex != null)
            errs.add(ex);
        for(int i = 0; i < countAttributes(); i++) {
            DataFileAttribute attr = getAttribute(i);
            ArrayList e = attr.validate();
            if(!e.isEmpty())
                errs.addAll(e);
        }
        return errs;
    }
    
    /*private void debugWriteFile(String name, String data)
    {
        try {
            String str = "C:\\veiko\\work\\sk\\JDigiDoc\\" + name;
            System.out.println("Writing debug file: " + str);
            FileOutputStream fos = new FileOutputStream(str);
            fos.write(data.getBytes());
            fos.close();
        } catch(Exception ex) {
            System.out.println("Error: " + ex);
            ex.printStackTrace(System.out);
        }
    }*/
    
    /** 
     * Helper method to calculate original digest
     * for base64 encoded content. Since such content
     * is decoded the whitespace around it is thrown
     * away. So we must calculate in beforehand
     * @param origBody original base64 body with any whitespace
     */
    /*public void calcOrigDigest(String origBody)
        throws DigiDocException
    {
        //System.out.println("calculateFileSizeAndDigest(" + getId() + ")");
        try {
            
            ByteArrayOutputStream sbDig = new ByteArrayOutputStream();
            byte[] tmp = null;
            tmp = xmlHeader();
            sbDig.write(tmp);
            tmp = origBody.getBytes();
            sbDig.write(tmp);
            tmp = xmlTrailer();
            sbDig.write(tmp);
            //debugWriteFile(getId() + "-body1.xml", sbDig.toString());
            CanonicalizationFactory canFac = ConfigManager.
                    instance().getCanonicalizationFactory();
            tmp = canFac.canonicalize(sbDig.toByteArray(), 
                    SignedDoc.CANONICALIZATION_METHOD_20010315);
            //debugWriteFile(getId() + "-body2.xml", new String(tmp));
            MessageDigest sha = MessageDigest.getInstance("SHA-1");
            sha.update(tmp);
            byte[] digest = sha.digest();
            setDigest(digest);
            //System.out.println("DataFile: \'" + getId() + "\' length: " +
            //        tmp.length + " digest: " + Base64Util.encode(digest));
        } catch(Exception ex) {
            DigiDocException.handleException(ex, DigiDocException.ERR_READ_FILE);
        }
    }*/
    
	/**
	 * Helper method to canonicalize a piece of xml
	 * @param xml data to be canonicalized
	 * @return canonicalized xml
	 */
	private byte[] canonicalizeXml(byte[] data) {
		try {				 
			CanonicalizationFactory canFac = ConfigManager.
				instance().getCanonicalizationFactory();
			byte[] tmp = canFac.canonicalize(data, 
					SignedDoc.CANONICALIZATION_METHOD_20010315);
			return tmp;
	   } catch(Exception ex) {
		   System.out.println("Canonicalizing exception: " + ex);
	   }
	   return null;
	}
	
	/**
	 * Helper method for using an optimization for base64 data's
	 * conversion and digest calculation. We use data blockwise to
	 * conserve memory
	 * @param os output stream to write data
	 * @param digest existing sha1 digest to be updated
	 * @param b64leftover leftover base64 data from previous block
	 * @param b64left leftover data length
	 * @param data new binary data
	 * @param dLen number of used bytes in data
	 * @param bLastBlock flag last block
	 * @return length of leftover bytes from this block
	 * @throws DigiDocException
	 */
	private int calculateAndWriteBase64Block(OutputStream os, MessageDigest digest, 
				byte[] b64leftover, int b64left, byte[] data, int dLen, boolean bLastBlock)
		throws DigiDocException
	{
		byte[] b64input = null;
		int b64Used, nLeft = 0, nInLen = 0;
        StringBuffer b64data = new StringBuffer();
        
        if(m_logger.isDebugEnabled())
        	m_logger.debug("os: " + ((os != null) ? "Y" :"N") +
        			" b64left: " + b64left + " input: " + dLen + " last: " + (bLastBlock ? "Y" : "N"));
        try {
        	// use data from the last block
        	if(b64left > 0) {
        		if(dLen > 0) {
        			b64input = new byte[dLen + b64left];
        			nInLen = b64input.length;
        			System.arraycopy(b64leftover, 0, b64input, 0, b64left);
        			System.arraycopy(data, 0, b64input, b64left, dLen);
        			if(m_logger.isDebugEnabled())
        				m_logger.debug("use left: " + b64left + " from 0 and add " + dLen);
        		} else {
        			b64input = b64leftover;
        			nInLen = b64left;
        			if(m_logger.isDebugEnabled())
        				m_logger.debug("use left: " + b64left + " with no new data");
        		}
        	} else {
        		b64input = data;
        		nInLen = dLen;
        		if(m_logger.isDebugEnabled())
        			m_logger.debug("use: " + nInLen + " from 0");
        	}
        	// encode full rows
        	b64Used = Base64Util.encodeToBlock(b64input, nInLen, b64data, bLastBlock);
        	nLeft = nInLen - b64Used;
        	// use the encoded data
        	byte[] encdata = b64data.toString().getBytes();
        	if(os != null)
        		os.write(encdata);
        	digest.update(encdata);
        	// now copy not encoded data back to buffer
        	if(m_logger.isDebugEnabled())
            	m_logger.debug("Leaving: " + nLeft + " of: " + b64input.length);
        	if(nLeft > 0)
        		System.arraycopy(b64input, b64input.length - nLeft, b64leftover, 0, nLeft);
        } catch(Exception ex) {
            DigiDocException.handleException(ex, DigiDocException.ERR_READ_FILE);
        }
        if(m_logger.isDebugEnabled())
        	m_logger.debug("left: " + nLeft + " bytes for the next run");
        return nLeft;
	}
    
    /** 
     * Calculates the DataFiles size and digest
     * Since it calculates the digest of the external file
     * then this is only useful for detatched files
     * @throws DigiDocException for all errors
     */
    public void calculateFileSizeAndDigest(OutputStream os) throws DigiDocException
    {
    	if(m_logger.isDebugEnabled()){
    		m_logger.debug("calculateFileSizeAndDigest(" + getId() + ")");
    	}
    	//A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
    	// A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1.1

    	MessageDigest sha = null;
    	try {
    		sha = MessageDigest.getInstance("SHA-1");
    		// Inga <2008 aprill> digesti arvutamine
    		
    		//IS DF 
    		if (m_sigDoc.getFormat().equals(SignedDoc.FORMAT_BDOC)){
    			
    			byte[] tmp2 = getBytesFromFile();
 				if(tmp2 != null && tmp2.length > 0) {
 					if(tmp2[0] == '<')
 						tmp2 = canonicalizeXml(tmp2);
 					if(tmp2 != null && tmp2.length > 0) {
 						sha.update(tmp2);  
 						if(os != null){
 							os.write(tmp2);
 						}
 					}
 				}
 				byte[] digest = sha.digest();
 	            setDigest(digest);
				return;
    		}
		} catch(Exception ex) {
            DigiDocException.handleException(ex, DigiDocException.ERR_READ_FILE);
        }
		// L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1.1
    	//L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
		boolean bUse64ByteLines = true;
        String use64Flag = ConfigManager.instance().getProperty("DATAFILE_USE_64BYTE_LINES");
        if(use64Flag != null && use64Flag.equalsIgnoreCase("FALSE"))
        	bUse64ByteLines = false;
        try {
        	// if DataFile's digest has already been initialized
        	// and body in memory, e.g. has been read from digidoc
        	// then write directly to output stream and don't calculate again
        	if(m_origDigestValue != null && m_body != null) {
        		os.write(xmlHeader());
        		os.write(m_body);
        		os.write(xmlTrailer());
        		return;
        	}
        	// else calculate again
            if(m_contentType.equals(CONTENT_DETATCHED) &&
                m_digestValue == null) {
                setDigestType(DIGEST_TYPE_SHA1);
                setDigestValue(calculateDetatchedFileDigest());
            }
            //A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
            BufferedInputStream bis =null;
            
            if(m_body == null && !m_contentType.equals(CONTENT_DETATCHED)) {
            	if(m_fDfCache != null) {
            		bis =  new BufferedInputStream( new FileInputStream(m_fDfCache));
            		//setSize(m_fDfCache.length());
            	} else {
					// Lauri Lüüs: bug when cache is located in other directory or changed the original DIGIDOC_DF_CACHE_DIR
					File fname = new File(m_fileName);
					if(!fname.exists())
						fname=new File(ConfigManager.instance().getStringProperty("DIGIDOC_DF_CACHE_DIR", System.getProperty("java.io.tmpdir")), m_fileName);
            		bis =  new BufferedInputStream( new FileInputStream(fname));
            		setSize(fname.length());
            	} 
            }
            //L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
            String longFileName = m_fileName;
            m_fileName = new File(m_fileName).getName();
			//A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
			byte[] tmp1=null,tmp2=null,tmp3=null;
			ByteArrayOutputStream sbDig = null;
			//L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
			sbDig = new ByteArrayOutputStream();
			sbDig.write(xmlHeader());
			// add trailer and canonicalize
			tmp3 = xmlTrailer();
			sbDig.write(tmp3);
			tmp1 = canonicalizeXml(sbDig.toByteArray());
			// now remove the end tag again and calculate digest of the start tag only
			tmp2 = new byte[tmp1.length - tmp3.length];
			System.arraycopy(tmp1, 0, tmp2, 0, tmp2.length);
			sha.update(tmp2);
			if(os != null)
				os.write(tmp2);
			// reset the collecting buffer and other temp buffers
			sbDig = new ByteArrayOutputStream();
			tmp1 = tmp2 = tmp3 = null;
		    // content must be read from file
            if(m_body == null && !m_contentType.equals(CONTENT_DETATCHED)) {
                byte[] buf = new byte[block_size]; 
                byte[] b64leftover = null;
                int fRead = 0, b64left = 0;
                ByteArrayOutputStream content = null;
                if(m_contentType.equals(CONTENT_EMBEDDED_BASE64)) {
                	// optimization for 64 char base64 lines
                	// convert to base64 online at a time to conserve memory
                	// VS: DF temp file base64 decoding fix
                	if(m_fDfCache == null) {
                		if(bUse64ByteLines)
                			b64leftover = new byte[65];
                		else
                			content = new ByteArrayOutputStream();
                	}
                }
                //A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
                while((fRead = bis.read(buf)) > 0 || b64left > 0) { // read input file
				//L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
                	if(m_logger.isDebugEnabled())
                    	m_logger.debug("read: " + fRead + " bytes of input data");
                    if(m_contentType.equals(CONTENT_EMBEDDED_BASE64)) {
                    	// VS: DF temp file base64 decoding fix
                    	if(m_fDfCache != null) {
                    		os.write(buf, 0, fRead);
                    		// VS: bug fix on 14.05.2008
                    		sha.update(buf, 0, fRead);
                    	} else {
                    		if(bUse64ByteLines) { // 1 line base64 optimization
                    			b64left = calculateAndWriteBase64Block(os, sha, b64leftover, 
                    				b64left, buf, fRead, fRead < block_size);                    		
                    		} else { // no optimization
                    			content.write(buf, 0, fRead);
                    		}
                    	}
                    } else {
                        if(fRead < buf.length) {
                            tmp2= new byte[fRead];
                            System.arraycopy(buf, 0, tmp2, 0, fRead);
                            tmp1 = ConvertUtils.data2utf8(tmp2, m_codepage);
                        }
                        else
                            tmp1 = ConvertUtils.data2utf8(buf, m_codepage);
                        sbDig.write(tmp1);
                    }                    
                } // end reading input file
                if(m_contentType.equals(CONTENT_EMBEDDED_BASE64)) {
                	// VS: DF temp file base64 decoding fix
                	if(!bUse64ByteLines && m_fDfCache == null)
                		sbDig.write(Base64Util.encode(content.toByteArray(), 0).getBytes());
                    content = null;
                }
            } else { // content allready in memeory
                    if(m_body != null) {
                    	if(bUse64ByteLines && m_contentType.equals(CONTENT_EMBEDDED_BASE64)) {
                    		calculateAndWriteBase64Block(os, sha, null, 0, m_body, m_body.length, true);
                    		m_body = Base64Util.encode(m_body).getBytes();
                    	} else {
                    		if(m_contentType.equals(CONTENT_EMBEDDED_BASE64))
                    			tmp1 = Base64Util.encode(m_body).getBytes();
                    		else
                    			tmp1 = ConvertUtils.data2utf8(m_body, m_codepage);
                    		sbDig.write(tmp1);
                    	}
                    }
            }
            tmp1 = null;
            //A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
            if(bis != null)
                bis.close(); 
            //L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1    
            // don't need to canonicalize base64 content !
			if(m_contentType.equals(CONTENT_EMBEDDED_BASE64)) {
				// VS: DF temp file base64 decoding fix
            	if(!bUse64ByteLines && m_fDfCache == null) {
					tmp2 = sbDig.toByteArray();
					if(tmp2 != null && tmp2.length > 0) {
						sha.update(tmp2);
						if(os != null)
							os.write(tmp2);
					}
				}
			} else {
				// canonicalize body
				tmp2 = sbDig.toByteArray();
				if(tmp2 != null && tmp2.length > 0) {
					//System.out.println("Body: \"" + tmp2 + "\"");
					if(tmp2[0] == '<')
						tmp2 = canonicalizeXml(tmp2);
					if(tmp2 != null && tmp2.length > 0) {
						sha.update(tmp2);  // crash
						if(os != null)
							os.write(tmp2);
					}
				}
			}
			tmp2 = null;
			sbDig = null;
			// trailer			
            tmp1 = xmlTrailer();
			sha.update(tmp1);
			if(os != null)
				 os.write(tmp1);
            // now calculate the digest
            byte[] digest = sha.digest();
            setDigest(digest);
            if(m_logger.isDebugEnabled())
            	m_logger.debug("DataFile: \'" + getId() + "\' length: " +
                    getSize() + " digest: " + Base64Util.encode(digest));
            m_fileName = longFileName;
        } catch(Exception ex) {
            DigiDocException.handleException(ex, DigiDocException.ERR_READ_FILE);
        }
    }
    
    public byte[] calculateDetatchedFileDigest()
        throws DigiDocException
    {
        byte[] digest = null;
        try {
            //System.out.println("calculateDetatchedFileDigest(" + getId() + ")");
            FileInputStream is = new FileInputStream(m_fileName);
            setSize(is.available());
            MessageDigest sha = MessageDigest.getInstance("SHA-1");
            byte[] buf = new byte[block_size]; // use 2KB bytes to avoid base64 problems
            int fRead = 0;
            while((fRead = is.read(buf)) == block_size) {
                sha.update(buf);
            }
            byte[] buf2 = new byte[fRead];
            System.arraycopy(buf, 0, buf2, 0, fRead);
            sha.update(buf2);
            is.close();
            digest = sha.digest();
            //System.out.println("DataFile: \'" + getId() + 
            //    "\' digest: " + Base64Util.encode(digest));
        } catch(Exception ex) {
            DigiDocException.handleException(ex, DigiDocException.ERR_READ_FILE);
        }
        return digest;
    }
    
    /** 
     * Writes the DataFile to an outout file
     * @param fos output stream
     * @throws DigiDocException for all errors
     */
    public void writeToFile(OutputStream fos)
        throws DigiDocException
    {
        //System.out.println("writeToFile(" + getId() + ")");
        // for detatched files just read them in
        // calculate digests and store a reference to them
        try {
           calculateFileSizeAndDigest(fos);
        } catch(DigiDocException ex) {
           throw ex;
        } catch(Exception ex) {
           DigiDocException.handleException(ex, DigiDocException.ERR_READ_FILE);
        }
    }
    
    /**
     * Helper method to replace '&' by '&amp;' in file names
     * @param filename original file name
     * @return fixed file name
     */
    private String fixFileName(String fileName)
    {
    	StringBuffer sb = new StringBuffer();
    	for(int i = 0; (fileName != null) && (i < fileName.length()); i++) {
    		char ch = fileName.charAt(i);
    		if(ch == '&')
    			sb.append("&amp;");
    		else
    			sb.append(ch);
    	}
    	return sb.toString();
    }

    /**
     * Helper method to create the xml header
     * @return xml header
     */
    private byte[] xmlHeader()
        throws DigiDocException
    {
        StringBuffer sb = new StringBuffer("<DataFile");
        if(m_codepage != null && !m_codepage.equals("UTF-8")) {
            sb.append(" Codepage=\"");
            sb.append(m_codepage);
            sb.append("\"");
        }
        sb.append(" ContentType=\"");
        sb.append(m_contentType);        
        sb.append("\" Filename=\"");
        // we write only file name not path to file
        String fileName = new File(m_fileName).getName();
        sb.append(fixFileName(fileName));
        sb.append("\" Id=\"");
        sb.append(m_id);
        sb.append("\" MimeType=\"");
        sb.append(m_mimeType);
        sb.append("\" Size=\"");
        sb.append(new Long(m_size).toString());
        sb.append("\"");
        if(m_digestType != null && m_digestValue != null) {
            sb.append(" DigestType=\"");
            sb.append(m_digestType);
            sb.append("\" DigestValue=\"");
            sb.append(Base64Util.encode(m_digestValue, 0));
            sb.append("\"");
        }
        for(int i = 0; i < countAttributes(); i++) {
            DataFileAttribute attr = getAttribute(i);
            sb.append(" ");
            sb.append(attr.toXML());
        }
        // namespace
        if(m_sigDoc != null && 
        	m_sigDoc.getVersion().equals(SignedDoc.VERSION_1_3)) {
        	sb.append(" xmlns=\"");
        	sb.append(SignedDoc.xmlns_digidoc);
        	sb.append("\"");
        }
        sb.append(">");
        return ConvertUtils.str2data(sb.toString(), "UTF-8");
    }
    
    /**
     * Helper method to create the xml trailer
     * @return xml trailer
     */
    private byte[] xmlTrailer()
        throws DigiDocException
    {
        return ConvertUtils.str2data("</DataFile>", "UTF-8");
    }
    
    /**
     * Converts the DataFile to XML form
     * @return XML representation of DataFile
     */
    public byte[] toXML()
       throws DigiDocException
    {
        ByteArrayOutputStream sb = new ByteArrayOutputStream();
        try {
        sb.write(xmlHeader());
        if(m_body != null) {
            //if(m_contentType.equals(CONTENT_EMBEDDED_BASE64))
            //    sb.write(Base64Util.encode(m_body).getBytes());
            if(m_contentType.equals(CONTENT_EMBEDDED) ||
               m_contentType.equals(CONTENT_EMBEDDED_BASE64))
                sb.write(m_body);
        }
        sb.write(xmlTrailer());
        } catch(Exception ex) {
            DigiDocException.handleException(ex, DigiDocException.ERR_ENCODING);
        }
        return sb.toByteArray();
    }

    /**
     * Returns the stringified form of DataFile
     * @return DataFile string representation
     */
    public String toString() 
    {
        String str = null;
        try {
            str = new String(toXML(), "UTF-8");
        } catch(Exception ex) {}
        return str;
    }
    
    //A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
    /**
     * Returns ZipEntry  of  the DataFile
     * @return ZipEntry
     */

	public ZipEntry getZipEntry() {
		return m_ZipEntry;
	}
	/**
     * Sets ZipEntry  of  the DataFile
     * @return ZipEntry
     */

	public void setZipEntry(ZipEntry zipEntry) {
		m_ZipEntry = zipEntry;
	}
	/**
     * Returns BDOC container  of  the DataFile
     * @return ZipEntry
     */

	public ZipFile getBDOCcontainer() {
		return m_BDOCcontainer;
	}
	/**
     * Sets BDOC container  of  the DataFile
     * @return ZipEntry
     */

	public void setBDOCcontainer(ZipFile ccontainer) {
		m_BDOCcontainer = ccontainer;
	}
	
	/** returns full path in BDOC container */
	public String getFullName() {
		return m_fullName;
	}
	/** sets full path in BDOC container */
	public void setFullName(String name) {
		m_fullName = name;
	}  
	
	// A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1.1
	//A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1.2
	//IS Fix reading files algus
	public byte[] getBytesFromFile() throws DigiDocException {
		byte[] bytes = null;
		try{
		if (getBody() != null){
			bytes = getBodyAsData();
        }
		else if ((getZipEntry() != null) && (getBDOCcontainer() != null)){
	        DataInputStream dis = new DataInputStream(getBDOCcontainer().getInputStream(getZipEntry()));
	        bytes = new byte[dis.available()];
	        dis.readFully(bytes);
	        dis.close();
		}	
        else if (getDfCacheFile() != null){
			bytes = SignedDoc.readFile(m_fDfCache);
        }
        else if (getFileName() != null){
			bytes = SignedDoc.readFile(new File(getFileName()));
        }
        else {
        	throw new DigiDocException(DigiDocException.ERR_DATA_FILE_FILE_NAME,
                   	"No file specified!", null);
        }
	} catch(Exception ex) {
		DigiDocException.handleException(ex, DigiDocException.ERR_READ_FILE);
	}
	return bytes;
	}
	//IS Fix reading files lopp
	//L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1.2
	// L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1.1
	//L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
	
	/**
	 * Beacause Logger is not serialzable we'll fix this
	 * @author Lauri Lüüs
	 */
	 private void readObject(java.io.ObjectInputStream in) throws IOException, ClassNotFoundException{
		 in.defaultReadObject();
		 this.m_logger=Logger.getLogger(DataFile.class);
	 }
	
}
