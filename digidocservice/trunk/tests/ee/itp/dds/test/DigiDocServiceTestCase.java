package ee.itp.dds.test;

import java.io.IOException;
import java.rmi.RemoteException;
import java.util.Arrays;

import javax.servlet.http.HttpServletResponse;
import javax.xml.rpc.ServiceException;
import javax.xml.rpc.holders.IntHolder;
import javax.xml.rpc.holders.StringHolder;

import org.apache.commons.httpclient.HttpClient;
import org.apache.commons.httpclient.methods.GetMethod;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.jdom.JDOMException;

import ee.itp.dds.service.DataFileData;
import ee.itp.dds.service.DigiDocServiceStub;
import ee.itp.dds.service.DigiDocService_ServiceLocator;
import ee.itp.dds.service.holders.DataFileDataHolder;
import ee.itp.dds.service.holders.SignatureModulesArrayHolder;
import ee.itp.dds.service.holders.SignedDocInfoHolder;
import ee.itp.dds.test.core.JettyServerTestServer;
import ee.itp.dds.util.SecureUtils;
import ee.itp.dds.util.TransformUtil;
import ee.sk.digidoc.Base64Util;
import ee.sk.digidoc.DataFile;

/**
 * Service main testing unit
 * 
 * @author Aleksandr Vassin
 */
public class DigiDocServiceTestCase extends JettyServerTestServer {

    protected static Log LOG = LogFactory.getLog(DigiDocServiceTestCase.class);

    private static DigiDocService_ServiceLocator locator = new DigiDocService_ServiceLocator(serviceLocation);
    private static DigiDocServiceStub binding = null;
    static {
        LOG.info("create new DigiDocServiceStub");
        try {
            binding = (DigiDocServiceStub) locator.getDigiDocService();
        } catch (ServiceException e) {
            if (e.getLinkedCause() != null)
                e.getLinkedCause().printStackTrace();
            throw new junit.framework.AssertionFailedError("JAX-RPC ServiceException caught: " + e);
        }
    }

    @Override
    protected void setUp() throws Exception {
        super.setUp();
        this.testsCount += getTestCount();
    }

    /**
     * After set up.
     */
    protected void afterSetUp() {

    }

    public void testWSDL() throws Exception {
        HttpClient client = new HttpClient();
        final GetMethod getMethod = new GetMethod(serviceLocation + "?wsdl");
        int result = client.executeMethod(getMethod);
        assertTrue(result == HttpServletResponse.SC_OK);
    }

    
    
    public void testStartSessionXML() throws Exception {
        assertNotNull("binding is null", binding);

        // Time out after a minute
        binding.setTimeout(60000);

        final String ContentType = "EMBEDDED_BASE64";
        final String Filename = "test.txt";
        final String Id = "D0";
        final String MimeType = "text/plain";
        final String FileData = "VGhpcyBpcyBhIHRlc3QgZG9jd" + "W1lbnQuDQpUaGUgY3JlYXRvciBvZiB0aGlzIGRv" + "Y3VtZW50IGRlY2xhcmVzIHRoYXQgdGhpcyBkb2N1bWVudCBpcyBtYWRlIGZvciB0"
                + "ZXN0aW5nIHB1cnBvc2VzIG9ubHku";
        final String DigestType = DataFile.DIGEST_TYPE_SHA1;
        final byte[] DigestValue = SecureUtils.hashCode(FileData.getBytes());
        final int Size = FileData.length();

        String signingProfile = "";
        String DATA_FILE_XML = "<DataFile ContentType=\"" + ContentType + "\" Filename=\"" + Filename + "\" " + "Id=\"" + Id + "\" MimeType=\"" + MimeType + "\" Size=\"" + Size + "\" "
                + "xmlns=\"http://www.sk.ee/DigiDoc/v1.3.0#\">" + FileData + "</DataFile>";

        boolean holdSession = true;
        DataFileData fileData = null;
        StringHolder status = new StringHolder();
        IntHolder sesscode = new IntHolder();
        SignedDocInfoHolder signedDocInfo = new SignedDocInfoHolder();

        // Test operation
        binding.startSession(signingProfile, DATA_FILE_XML, holdSession, fileData, status, sesscode, signedDocInfo);
        // TBD - validate results
        if (sesscode.value != 0) {

            String fileId = "D0";
            DataFileDataHolder retFile = new DataFileDataHolder();

            binding.getDataFile(sesscode.value, fileId, status, retFile);

            assertNotNull(retFile);
            assertNotNull(retFile.value);
            assertEquals(retFile.value.getFilename(), Filename);
            assertEquals(retFile.value.getContentType(), ContentType);
            assertEquals(retFile.value.getMimeType(), MimeType);
            assertEquals(retFile.value.getDfData(), FileData);
            assertEquals(retFile.value.getSize(), Size);
            assertEquals(retFile.value.getDigestType(), DigestType);
            assertTrue(Arrays.equals(Base64Util.decode(retFile.value.getDigestValue()), DigestValue));

        }// if

    }

    public void testStartSessionDataFile() throws Exception {
        assertNotNull("binding is null", binding);

        // Time out after a minute
        binding.setTimeout(60000);

        final String ContentType = DataFile.CONTENT_EMBEDDED_BASE64;
        final String Filename = "boroda.txt";
        final String Id = "D0";
        final String MimeType = "application/binary";
        final String FileData = "Test string in file";
        final String DigestType = DataFile.DIGEST_TYPE_SHA1;
        final byte[] DigestValue = SecureUtils.hashCode(FileData.getBytes());
        final int Size = FileData.length();

        String signingProfile = "";
        String DATA_FILE_XML = "";
        boolean holdSession = true;
        DataFileData fileData = new DataFileData();
        {
            fileData.setFilename(Filename);
            fileData.setDfData(FileData);
            fileData.setMimeType(MimeType);
            fileData.setContentType(ContentType);
            fileData.setSize(Size);
        }
        StringHolder status = new StringHolder();
        IntHolder sesscode = new IntHolder();
        SignedDocInfoHolder signedDocInfo = new SignedDocInfoHolder();

        // Test operation
        binding.startSession(signingProfile, DATA_FILE_XML, holdSession, fileData, status, sesscode, signedDocInfo);
        // TBD - validate results
        if (sesscode.value != 0) {

            String fileId = "D0";
            DataFileDataHolder retFile = new DataFileDataHolder();

            binding.getDataFile(sesscode.value, fileId, status, retFile);

            assertNotNull(retFile);
            assertNotNull(retFile.value);
            assertEquals(retFile.value.getFilename(), Filename);
            assertEquals(retFile.value.getContentType(), ContentType);
            assertEquals(retFile.value.getMimeType(), MimeType);
            assertEquals(retFile.value.getDfData(), FileData);
            assertEquals(retFile.value.getSize(), Size);
            assertEquals(retFile.value.getDigestType(), DigestType);
            assertTrue(Arrays.equals(Base64Util.decode(retFile.value.getDigestValue()), DigestValue));

        }// if

    }// testStartSessionDataFile

    
    
    
    public void testStartSessionDataFileHashCode() throws Exception {
      assertNotNull("binding is null", binding);

      // Time out after a minute
      binding.setTimeout(60000);

      final String ContentType = DataFile.CONTENT_DETATCHED;
      final String Filename = "boroda.txt";
      final String Id = "D0";
      final String MimeType = "application/binary";
      final String FileData = "Test string in file";
      final int Size = FileData.length();
      final String DigestType = DataFile.DIGEST_TYPE_SHA1;
      final byte[] DigestValue = SecureUtils.hashCode(FileData.getBytes());


      String signingProfile = "";
      String DATA_FILE_XML = "";
      boolean holdSession = true;
      DataFileData fileData = new DataFileData();
      {
          fileData.setFilename(Filename);
          //fileData.setDfData(FileData);
          fileData.setMimeType(MimeType);
          fileData.setContentType(ContentType);
          fileData.setSize(Size);
          fileData.setDigestType(DigestType);
          fileData.setDigestValue(Base64Util.encode(DigestValue));
      }
      StringHolder status = new StringHolder();
      IntHolder sesscode = new IntHolder();
      SignedDocInfoHolder signedDocInfo = new SignedDocInfoHolder();

      // Test operation
      binding.startSession(signingProfile, DATA_FILE_XML, holdSession, fileData, status, sesscode, signedDocInfo);
      // TBD - validate results
      if (sesscode.value != 0) {

          String fileId = "D0";
          DataFileDataHolder retFile = new DataFileDataHolder();

          binding.getDataFile(sesscode.value, fileId, status, retFile);

          assertNotNull(retFile);
          assertNotNull(retFile.value);
          assertEquals(retFile.value.getFilename(), Filename);
          assertEquals(retFile.value.getContentType(), ContentType);
          assertEquals(retFile.value.getMimeType(), MimeType);
          //assertEquals(retFile.value.getDfData(), FileData);
          assertEquals(retFile.value.getSize(), Size);
          assertEquals(retFile.value.getDigestType(), DigestType);
          assertTrue(Arrays.equals(Base64Util.decode(retFile.value.getDigestValue()), DigestValue));

      }// if

  }// testStartSessionDataFile
    
    

    public void testCloseSession() throws Exception {
        assertNotNull("binding is null", binding);

        // Time out after a minute
        binding.setTimeout(60000);

        // Test operation
        String value = null;
        // value =
        binding.closeSession(0);
        // TBD - validate results
    }

    
    public void testCreateSignedDoc()  {
        assertNotNull("binding is null", binding);

        // Time out after a minute
        binding.setTimeout(60000);

        // Test operation
        try {
          binding.createSignedDoc(0, new String(), new String(), new StringHolder(), new SignedDocInfoHolder());
        } catch (RemoteException e) {
          assertEquals("WRONG_SESSION_CODE", binding.getFaultString());
        }
        // TBD - validate results
    }

    public void testAddDataFile()  {
        assertNotNull("binding is null", binding);

        // Time out after a minute
        binding.setTimeout(60000);

        // Test operation
        try {
          binding.addDataFile(0, new String(), new String(), new String(), 0, new String(), new String(), new String(), new StringHolder(), new SignedDocInfoHolder());
        } catch (RemoteException e) {
          assertEquals("WRONG_SESSION_CODE", binding.getFaultString());

        }
        // TBD - validate results
    }

    public void testRemoveDataFile()  {
        assertNotNull("binding is null", binding);

        // Time out after a minute
        binding.setTimeout(60000);

        // Test operation
        try {
          binding.removeDataFile(0, "D0", new StringHolder(), new SignedDocInfoHolder());
        } catch (RemoteException e) {
          assertEquals("WRONG_SESSION_CODE", binding.getFaultString());

        }
        // TBD - validate results
    }

    public void testGetSignedDoc()  {
        assertNotNull("binding is null", binding);

        // Time out after a minute
        binding.setTimeout(60000);

        // Test operation
        try {
          binding.getSignedDoc(0, new StringHolder(), new StringHolder());
        } catch (RemoteException e) {
          assertEquals("WRONG_SESSION_CODE", binding.getFaultString());

        }
        // TBD - validate results
    }

    public void testGetSignedDocInfo()  {
        assertNotNull("binding is null", binding);

        // Time out after a minute
        binding.setTimeout(60000);

        // Test operation
        try {
          binding.getSignedDocInfo(0, new StringHolder(), new SignedDocInfoHolder());
        } catch (RemoteException e) {
          assertEquals("WRONG_SESSION_CODE", binding.getFaultString());

        }
        // TBD - validate results
    }

    public void testGetDataFile()  {
        assertNotNull("binding is null", binding);

        // Time out after a minute
        binding.setTimeout(60000);

        // Test operation
        try {
          binding.getDataFile(0, "D0", new StringHolder(), new DataFileDataHolder());
        } catch (RemoteException e) {
          assertEquals("WRONG_SESSION_CODE", binding.getFaultString());

        }
        // TBD - validate results
    }

    public void testGetSignersCertificate()  {
        assertNotNull("binding is null", binding);

        // Time out after a minute
        binding.setTimeout(60000);

        // Test operation
        try {
          binding.getSignersCertificate(0, new String(), new StringHolder(), new StringHolder());
        } catch (RemoteException e) {
          assertEquals("WRONG_SESSION_CODE", binding.getFaultString());
        }
        // TBD - validate results
    }

    public void testGetNotarysCertificate()  {
        assertNotNull("binding is null", binding);

        // Time out after a minute
        binding.setTimeout(60000);

        // Test operation
        try {
          binding.getNotarysCertificate(0, new String(), new StringHolder(), new StringHolder());
        } catch (RemoteException e) {
          assertEquals("WRONG_SESSION_CODE", binding.getFaultString());

        }
        // TBD - validate results
    }

    public void testGetNotary()  {
        assertNotNull("binding is null", binding);

        // Time out after a minute
        binding.setTimeout(60000);

        // Test operation
        try {
          binding.getNotary(0, new String(), new StringHolder(), new StringHolder());
        } catch (RemoteException e) {
          assertEquals("WRONG_SESSION_CODE", binding.getFaultString());

        }
        // TBD - validate results
    }

    public void testGetTSACertificate()  {
        assertNotNull("binding is null", binding);

        // Time out after a minute
        binding.setTimeout(60000);

        // Test operation
        try {
          binding.getTSACertificate(0, new String(), new StringHolder(), new StringHolder());
        } catch (RemoteException e) {
          assertEquals("WRONG_SESSION_CODE", binding.getFaultString());

        }
        // TBD - validate results
    }

    public void testGetTimestamp()  {
        assertNotNull("binding is null", binding);

        // Time out after a minute
        binding.setTimeout(60000);

        // Test operation
        try {
          binding.getTimestamp(0, new String(), new StringHolder(), new StringHolder());
        } catch (RemoteException e) {
          assertEquals("WRONG_SESSION_CODE", binding.getFaultString());

        }
        // TBD - validate results
    }

    public void testGetCRL()  {
        assertNotNull("binding is null", binding);

        // Time out after a minute
        binding.setTimeout(60000);

        // Test operation
        try {
          binding.getCRL(0, new String(), new StringHolder(), new StringHolder());
        } catch (RemoteException e) {
          assertEquals("WRONG_SESSION_CODE", binding.getFaultString());

        }
        // TBD - validate results
    }

    public void testGetSignatureModules()  {
        assertNotNull("binding is null", binding);
        binding.setTimeout(60000);
        try {
          binding.getSignatureModules(0, new String(), new String(), new String(), new StringHolder(), new SignatureModulesArrayHolder());
        } catch (RemoteException e) {
          assertEquals("WRONG_SESSION_CODE", binding.getFaultString());

        }
    }

    public void testPrepareSignature()  {
        assertNotNull("binding is null", binding);

        // Time out after a minute
        binding.setTimeout(60000);

        // Test operation
        try {
          binding.prepareSignature(0, new String(), new String(), new String(), new String(), new String(), new String(), new String(), new String(), new StringHolder(), new StringHolder(),
                  new StringHolder());
        } catch (RemoteException e) {
          assertEquals("WRONG_SESSION_CODE", binding.getFaultString());

        }
        // TBD - validate results
    }

    public void testFinalizeSignature()  {
        assertNotNull("binding is null", binding);

        // Time out after a minute
        binding.setTimeout(60000);

        // Test operation
        try {
          binding.finalizeSignature(0, new String(), new String(), new StringHolder(), new SignedDocInfoHolder());
        } catch (RemoteException e) {
          assertEquals("WRONG_SESSION_CODE", binding.getFaultString());

        }
        // TBD - validate results
    }

    public void testRemoveSignature()  {
        assertNotNull("binding is null", binding);

        // Time out after a minute
        binding.setTimeout(60000);

        // Test operation
        try {
          binding.removeSignature(0, new String(), new StringHolder(), new SignedDocInfoHolder());
        } catch (RemoteException e) {
          assertEquals("WRONG_SESSION_CODE", binding.getFaultString());

        }
        // TBD - validate results
    }

    public void testGetVersion() throws Exception {
        assertNotNull("binding is null", binding);

        // Time out after a minute
        binding.setTimeout(60000);

        // Test operation
        binding.getVersion(new StringHolder(), new StringHolder());
        // TBD - validate results
    }

    public void testGetStatusInfo()  {
        assertNotNull("binding is null", binding);

        // Time out after a minute
        binding.setTimeout(60000);

        // Test operation
        try {
          binding.getStatusInfo(0, true, true, new StringHolder(), new StringHolder(), new SignedDocInfoHolder());
        } catch (RemoteException e) {
          assertEquals("WRONG_SESSION_CODE", binding.getFaultString());
        }
        // TBD - validate results
    }

    public void testConverter() throws JDOMException, IOException {

        String DATA_FILE_XML = "<DataFile ContentType=\"EMBEDDED_BASE64\" Filename=\"test.txt\" " + "Id=\"D0\" MimeType=\"text/plain\" Size=\"117\" "
                + "xmlns=\"http://www.sk.ee/DigiDoc/v1.3.0#\">VGhpcyBpcyBhIHRlc3QgZG9jd" + "W1lbnQuDQpUaGUgY3JlYXRvciBvZiB0aGlzIGRv"
                + "Y3VtZW50IGRlY2xhcmVzIHRoYXQgdGhpcyBkb2N1bWVudCBpcyBtYWRlIGZvciB0" + "ZXN0aW5nIHB1cnBvc2VzIG9ubHku</DataFile>";

        DataFileData obj = TransformUtil.convertToDataFile(DATA_FILE_XML);
        assertEquals(obj.getFilename(), "test.txt");
        assertEquals(obj.getMimeType(), "text/plain");
        assertEquals(obj.getContentType(), "EMBEDDED_BASE64");
        assertEquals(obj.getSize(), 117);

    }// converterTest

}
