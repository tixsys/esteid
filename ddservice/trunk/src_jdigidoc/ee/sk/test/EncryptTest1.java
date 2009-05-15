package ee.sk.test;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import ee.sk.digidoc.SignedDoc;
import ee.sk.utils.ConfigManager;
import ee.sk.xmlenc.*;
import ee.sk.xmlenc.factory.*;
import java.security.cert.X509Certificate;



public class EncryptTest1 {

	public static void main(String[] args)
	{
		try {
			ConfigManager.init("jdigidoc.cfg");

			X509Certificate  cert = SignedDoc.readCertificate("veiko-auth.cer");

		    EncryptedData encData = new EncryptedData(null, null, null, 
		    		EncryptedData.DENC_XMLNS_XMLENC, EncryptedData.DENC_ENC_METHOD_AES128);

		    encData.addEncryptedKey(new EncryptedKey("ID0", "testCert", 
		    		EncryptedData.DENC_ENC_METHOD_RSA1_5, null, null, cert));

		    FileInputStream fis = new FileInputStream("testfile.txt");
		    FileOutputStream fos = new FileOutputStream("testfile.cdoc");

		    encData.addProperty(EncryptedData.ENCPROP_FILENAME, "testfile.txt");
		    encData.addProperty(EncryptedData.ENCPROP_ORIG_MIME, "text/plain");

		    encData.encryptStream(fis, fos, EncryptedData.DENC_COMPRESS_ALLWAYS);

		    fis.close();
		    fos.close();
		    	    
		    System.out.println("Done");
		} catch(Exception ex) {
			System.err.println("Error: " + ex);
			ex.printStackTrace();
		}
	}
}
