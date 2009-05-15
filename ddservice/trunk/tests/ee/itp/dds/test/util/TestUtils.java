package ee.itp.dds.test.util;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;

import org.jdom.Document;
import org.jdom.Element;
import org.jdom.output.XMLOutputter;

import ee.itp.dds.service.DataFileData;
import ee.itp.dds.util.FileManager;
import ee.itp.dds.util.SecureUtils;
import ee.itp.dds.util.TransformUtil;
import ee.sk.digidoc.Base64Util;
import ee.sk.digidoc.DataFile;
import ee.sk.digidoc.DigiDocException;
import ee.sk.utils.ConvertUtils;

public class TestUtils {

    public static final String FILE_TEXT = "textData.txt";
    public static final String FILE_BINARY = "binaryData.pdf";
    
    public static DataFileData buildDataFileData(String filename, String contentType) throws DigiDocException, FileNotFoundException, IOException {
            DataFileData data = new DataFileData();
            data.setFilename(filename);
            data.setContentType(contentType);
            if (contentType.equals(DataFile.CONTENT_DETATCHED)) { //only hashcode
                data.setDigestType(DataFile.DIGEST_TYPE_SHA1);
                data.setMimeType("text/plain");
                String content = FileManager.getResource(FILE_TEXT, TestUtils.class);
                data.setSize(content.length());
                data.setDigestValue(Base64Util.encode(SecureUtils.hashCode(ConvertUtils.str2data(content))));
            } else if (contentType.equals(DataFile.CONTENT_EMBEDDED)){ //Text or XML
                data.setMimeType("text/xml");
                String content = FileManager.readFile(FileManager.getResource(FILE_TEXT, TestUtils.class));
                data.setSize(content.length());
                data.setDfData(TransformUtil.escapeXml(content));
            } else if (contentType.equals(DataFile.CONTENT_EMBEDDED_BASE64)){ //binary
                data.setMimeType("application/binary");
                InputStream is = FileManager.getResourceAsinputStream(FILE_BINARY);
                try {
                    int numberBytes = is.available();
                    byte content[] = new byte[numberBytes];
                    is.read(content);                
                    data.setSize(content.length);
                    data.setDfData(Base64Util.encode(content));
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            
            return data;
    }
    
    public static String buildXmlDataFileData(String filename, String contentType) throws DigiDocException, FileNotFoundException, IOException {
        DataFileData data = buildDataFileData(filename, contentType);
        Element root = new Element("DataFile","http://www.sk.ee/DigiDoc/v1.3.0#");
        root.setAttribute("ContentType", data.getContentType());
        root.setAttribute("Filename",data.getFilename());
        root.setAttribute("MimeType",data.getMimeType());
        root.setAttribute("Size","" +data.getSize());
        if (contentType.equals(DataFile.CONTENT_DETATCHED)) { //only hashcode
            root.setAttribute("DigestType",data.getDigestType());
            root.setAttribute("DigestValue",data.getDigestValue());
        } else {
            root.setText(data.getDfData());
        }
        Document doc = new Document(root);
        return new XMLOutputter().outputString(doc);
    }    
    
    public static void main(String[] args) throws DigiDocException, FileNotFoundException, IOException {
        String z = buildXmlDataFileData("boroda.txt", DataFile.CONTENT_DETATCHED);
        System.out.println(z);
    }
    

}
