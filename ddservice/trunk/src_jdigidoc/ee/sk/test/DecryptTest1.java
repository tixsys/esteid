package ee.sk.test;
import ee.sk.digidoc.*;
import ee.sk.utils.*;
import ee.sk.xmlenc.factory.*;
import java.io.*;

public class DecryptTest1 {

	public static void main(String[] args)
	{
		try {
			ConfigManager.init("jdigidoc.cfg");

		    EncryptedStreamParser ddStreamParser = new EncryptedStreamSAXParser();
		    ddStreamParser.init(); // must register provider
		    /*
		    System.out.println("Decrypting file - valid.cdoc" );
		    ByteArrayOutputStream bos = new ByteArrayOutputStream();
		    ddStreamParser.decryptStreamUsingRecipientNameAndKey(
		    		new FileInputStream("valid.cdoc"), bos, 
		    		SignedDoc.hex2bin("6fa1322c6ae89b80705138b2ae6809e2"), "hansatest");
		    System.out.println("Decrypted: " + bos.size() + " bytes");
		    System.out.println("DATA:\n---\n" + new String(bos.toByteArray()) + "\n---\n");
		    */
		    System.out.println("Decrypting file - invalid.cdoc" );
		    ByteArrayOutputStream bos = new ByteArrayOutputStream();
		    ddStreamParser.decryptStreamUsingRecipientNameAndKey(
		    		new FileInputStream("invalid.cdoc"), bos,
		    		SignedDoc.hex2bin("d7ce0e1045ad9e839305813fcb1ceecf"), "hansatest");
		    System.out.println("Decrypted: " + bos.size() + " bytes");
		    System.out.println("DATA:\n---\n" + new String(bos.toByteArray()) + "\n---\n");
		    	    
		    System.out.println("Done");
		} catch(Exception ex) {
			System.err.println("Error: " + ex);
			ex.printStackTrace();
		}
	}
}
