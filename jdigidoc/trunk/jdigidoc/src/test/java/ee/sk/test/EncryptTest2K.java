package ee.sk.test;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.FileOutputStream;

import ee.sk.digidoc.SignedDoc;
import ee.sk.utils.ConfigManager;
import ee.sk.xmlenc.*;
import ee.sk.xmlenc.factory.*;
import java.security.cert.X509Certificate;

public class EncryptTest2K {
	public static void main(String[] args)
	{
		try {
			ConfigManager.init("jdigidoc.cfg");

			X509Certificate  cert = SignedDoc.readCertificate("veiko-auth.cer");

		    EncryptedData encData = new EncryptedData(null, null, null, 
		    		EncryptedData.DENC_XMLNS_XMLENC, EncryptedData.DENC_ENC_METHOD_AES128);

		    encData.addEncryptedKey(new EncryptedKey("ID0", "testCert", 
		    		EncryptedData.DENC_ENC_METHOD_RSA1_5, null, null, cert));

		    StringBuffer sb = new StringBuffer();
		    while(sb.length() < 2048)
		      sb.append("12345678"); // selle stringi pikkus oleks hea mingi 2kordne panna
		    
		    ByteArrayInputStream bis = new ByteArrayInputStream(sb.toString().getBytes());
		    //ByteArrayOutputStream bos = new ByteArrayOutputStream();

		    encData.addProperty(EncryptedData.ENCPROP_FILENAME, "test2K.txt");
		    encData.addProperty(EncryptedData.ENCPROP_ORIG_MIME, "text/plain");
		    System.out.println("Encrypting 2KB - test2K.txt");
		    //encData.encryptStream(bis, bos, EncryptedData.DENC_COMPRESS_ALLWAYS);
		    //int index = bos.toString().indexOf("<denc:CipherValue></denc:CipherValue>");
		    //System.out.println("End idx: " + index);
		    System.out.println("Writing file: test2k.cdoc");
		    FileOutputStream fos = new FileOutputStream("test2k.cdoc");
		    encData.encryptStream(bis, fos, EncryptedData.DENC_COMPRESS_ALLWAYS);
		    fos.close();
		    	    
		    System.out.println("Done");
		} catch(Exception ex) {
			System.err.println("Error: " + ex);
			ex.printStackTrace();
		}
	}
}
