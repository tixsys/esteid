package ee.itp.dds.test;

import javax.xml.rpc.ServiceException;
import javax.xml.rpc.holders.IntHolder;
import javax.xml.rpc.holders.StringHolder;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import ee.itp.dds.service.DataFileData;
import ee.itp.dds.service.DigiDocServiceStub;
import ee.itp.dds.service.DigiDocService_ServiceLocator;
import ee.itp.dds.service.holders.DataFileDataHolder;
import ee.itp.dds.service.holders.SignedDocInfoHolder;
import ee.itp.dds.test.core.JettyServerTestServer;
import ee.sk.digidoc.SignedDoc;

public class ComplexTest1 extends JettyServerTestServer {

    protected static Log LOG = LogFactory.getLog(ComplexTest1.class);
    //private static DigiDocService_ServiceLocator locator = new DigiDocService_ServiceLocator("http://www.sk.ee:8095/DigiDocService");
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
    
    /**
     * 1. Create new session without data 
     * 2. Creating new digiDoc (Container) 
     * 3. Adding and verify 2 files 
     * 4. Deleting and verify first file 
     * 5. Close session 
     * 6. Verify that session was closed
     * 
     */
    public void testBigComplexTest() throws Exception{
      
      assertNotNull("binding is null", binding);

      // Time out after a minute
      binding.setTimeout(6000000);

      String signingProfile = "";
      String DATA_FILE_XML = "";
      boolean holdSession = true;
      DataFileData fileData = null;
      StringHolder status = new StringHolder();
      IntHolder sesscode = new IntHolder();
      SignedDocInfoHolder signedDocInfo = new SignedDocInfoHolder();

      // Test operation
      binding.startSession(signingProfile, DATA_FILE_XML, holdSession, fileData, status, sesscode, signedDocInfo);

      // Test operation
      binding.createSignedDoc(sesscode.value, SignedDoc.FORMAT_DIGIDOC_XML, SignedDoc.VERSION_1_4, status, signedDocInfo);//TODO 1_4

      addFile1(sesscode, status, signedDocInfo);
      addFile2(sesscode, status, signedDocInfo);

      String fileId = "D0";
      SignedDocInfoHolder retInfo = new SignedDocInfoHolder();
      binding.removeDataFile(sesscode.value, fileId, status, retInfo);

      DataFileDataHolder retFile = new DataFileDataHolder();
      try {
        binding.getDataFile(sesscode.value, fileId, status, retFile);
      } catch (Exception e) {
        //assertNotNull(retFile);
      }
      assertTrue(binding.hasResponseFaultString());
      assertEquals(binding.getFaultString(),"101");

      binding.closeSession(sesscode.value);
      try {
        binding.createSignedDoc(sesscode.value, SignedDoc.FORMAT_DIGIDOC_XML, SignedDoc.VERSION_1_4, status, signedDocInfo);
      } catch (Exception e) {
        assertEquals("101", binding.getFaultString() );
      }
      
      try {
        binding.closeSession(1234);
      } catch (Exception e) {
        assertEquals(true, binding.hasResponseFaultString());
      }
      
      
    }//testBigComplexTest
    
    
    private void addFile1(IntHolder sesscode, StringHolder status, SignedDocInfoHolder signedDocInfo) throws Exception {

      final String contentType = "EMBEDDED_BASE64";
      final String filename = "test_file_1.txt";
      final String id = "D0";
      final String mimeType = "application/binary";
      final String digestType = "";
      final String digestValue = "";
      final String fileData = "Test string in file 1";
      final int Size = fileData.length();

      // Test operation
      binding.addDataFile(sesscode.value, filename, mimeType, contentType, Size, digestType, digestValue, fileData, status, signedDocInfo);

      DataFileDataHolder retFile = new DataFileDataHolder();
      String fileId = "D0";

      binding.getDataFile(sesscode.value, fileId, status, retFile);

      assertNotNull(retFile);
      assertNotNull(retFile.value);
      assertEquals(retFile.value.getFilename(), filename);
      assertEquals(retFile.value.getContentType(), contentType);
      assertEquals(retFile.value.getMimeType(), mimeType);
      //assertEquals(retFile.value.getDfData(), fileData);
      //assertEquals(retFile.value.getSize(), Size);

  }

    
  private void addFile2(IntHolder sesscode, StringHolder status, SignedDocInfoHolder signedDocInfo) throws Exception {

      final String contentType = "EMBEDDED_BASE64";
      final String filename = "test_file_2.txt";
      final String id = "D1";
      final String mimeType = "application/binary";
      final String digestType = "";
      final String digestValue = "";
      final String fileData = "Test string in file 2";
      final int size = fileData.length();

      // Test operation
      binding.addDataFile(sesscode.value, filename, mimeType, contentType, size, digestType, digestValue, fileData, status, signedDocInfo);

      DataFileDataHolder retFile = new DataFileDataHolder();
      String fileId = "D1";

      binding.getDataFile(sesscode.value, fileId, status, retFile);

      assertNotNull(retFile);
      assertNotNull(retFile.value);
      assertEquals(retFile.value.getFilename(), filename);
      assertEquals(retFile.value.getContentType(), contentType);
      assertEquals(retFile.value.getMimeType(), mimeType);
      //assertEquals(retFile.value.getDfData(), fileData);
      //assertEquals(retFile.value.getSize(), size);

  }
    

}
