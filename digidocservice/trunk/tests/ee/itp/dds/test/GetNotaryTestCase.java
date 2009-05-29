package ee.itp.dds.test;

import java.net.URL;
import java.security.Security;
import java.security.cert.X509Certificate;

import javax.xml.rpc.ServiceException;
import javax.xml.rpc.holders.IntHolder;
import javax.xml.rpc.holders.StringHolder;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import ee.itp.dds.service.DigiDocServiceStub;
import ee.itp.dds.service.DigiDocService_ServiceLocator;
import ee.itp.dds.service.ResponseStatus;
import ee.itp.dds.service.holders.SignedDocInfoHolder;
import ee.itp.dds.test.core.JettyServerTestServer;
import ee.itp.dds.test.util.TestUtils;
import ee.itp.dds.util.FileManager;
import ee.sk.digidoc.DataFile;
import ee.sk.digidoc.SignedDoc;
import ee.sk.digidoc.factory.SignatureFactory;
import ee.sk.utils.ConfigManager;

public class GetNotaryTestCase extends JettyServerTestServer {
    
    protected static Log LOG = LogFactory.getLog(GetNotaryTestCase.class);

    private static DigiDocService_ServiceLocator locator = new DigiDocService_ServiceLocator(serviceLocation);
    private static DigiDocServiceStub binding = null;
    static {
        LOG.info("Create new DigiDocServiceStub");
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
        Security.addProvider(new org.bouncycastle.jce.provider.BouncyCastleProvider());
        URL url = FileManager.getResourceAsUrl("jdigidoc.cfg", GetNotaryTestCase.class);
        ConfigManager.init(url.getFile());
        this.testsCount += getTestCount();
    }

    /**
     * After set up.
     */
    protected void afterSetUp() {

    }    
        
    public void _testGetNotary() throws Exception {
        //TODO: Config first DigiDoc for run this test
        assertNotNull("binding is null", binding);
        binding.setTimeout(600000);
        
        StringHolder status = new StringHolder();
        SignedDocInfoHolder signedDocInfo = new SignedDocInfoHolder();
        IntHolder sesscode = new IntHolder();
        
        String xml = TestUtils.buildXmlDataFileData("boroda.txt", DataFile.CONTENT_EMBEDDED);
        
        binding.startSession("", xml, true, null, status, sesscode, signedDocInfo);
        assertEquals(status.value, ResponseStatus.OK.name());

        // Do card login and get certificate
        SignatureFactory sigFac = ConfigManager.instance().getSignatureFactory();
        X509Certificate cert = sigFac.getCertificate(0, "dds123");
        
        StringHolder signatureId = new StringHolder();
        StringHolder signedInfoDigest = new StringHolder();
        
        binding.prepareSignature(sesscode.value, SignedDoc.bin2hex(cert.getEncoded()), 
                                  "0", "Role1", "City1", "State1", "55555", "Country1",
                                  "", status, signatureId,signedInfoDigest);

        assertEquals(status.value, ResponseStatus.OK.name());
        
        byte[] sigval = sigFac.sign( SignedDoc.hex2bin(signedInfoDigest.value), 0, "dds123");
        
        binding.finalizeSignature(sesscode.value, signatureId.value, SignedDoc.bin2hex(sigval), 
                                    status, signedDocInfo);

        assertEquals(status.value, ResponseStatus.OK.name());

        StringHolder ocspData = new StringHolder();
        binding.getNotary(sesscode.value , "S1", status, ocspData );

        assertEquals(status.value, ResponseStatus.OK.name());
    
    }    
    
    

}
