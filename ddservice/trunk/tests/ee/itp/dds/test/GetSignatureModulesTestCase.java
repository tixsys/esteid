package ee.itp.dds.test;

import javax.xml.rpc.ServiceException;
import javax.xml.rpc.holders.IntHolder;
import javax.xml.rpc.holders.StringHolder;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import ee.itp.dds.core.SignatureModuleDesc;
import ee.itp.dds.service.DigiDocServiceStub;
import ee.itp.dds.service.DigiDocService_ServiceLocator;
import ee.itp.dds.service.ResponseStatus;
import ee.itp.dds.service.holders.SignatureModulesArrayHolder;
import ee.itp.dds.service.holders.SignedDocInfoHolder;
import ee.itp.dds.test.core.JettyServerTestServer;
import ee.itp.dds.test.util.TestUtils;
import ee.sk.digidoc.DataFile;

public class GetSignatureModulesTestCase extends JettyServerTestServer {

    protected static Log LOG = LogFactory.getLog(GetSignatureModulesTestCase.class);

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
    
    public void testGetSignatureModules() throws Exception{
      
      assertNotNull("binding is null", binding);

      // Time out after a minute
      binding.setTimeout(6000000);

      StringHolder status = new StringHolder();
      SignedDocInfoHolder signedDocInfo = new SignedDocInfoHolder();
      IntHolder sesscode = new IntHolder();
      String xml = TestUtils.buildXmlDataFileData("boroda.txt", DataFile.CONTENT_EMBEDDED);
      binding.startSession("", xml, true, null, status, sesscode, signedDocInfo);
      assertEquals(status.value, ResponseStatus.OK.name());
      SignatureModulesArrayHolder smah = new SignatureModulesArrayHolder();
      
      binding.getSignatureModules(sesscode.value,
                                  SignatureModuleDesc.Platform.WIN32_IE.name(),
                                  SignatureModuleDesc.Phase.PREPARE.name(),
                                  SignatureModuleDesc.Type.ALL.name(),
                                  status,smah);
      
      assertEquals(smah.value.length,5);
      
      
    }
    
    


}
