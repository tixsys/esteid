package ee.sk.test;
import java.io.File;
import java.io.FileOutputStream;
import ee.sk.digidoc.SignedDoc;
import ee.sk.digidoc.Base64Util;

/**
 * Tests decoding base64 
 * @author veiko
 */
public class DecodeBase64Test1 {
	public static String in_file1 = null;
    public static String out_file1 = null;
    
    public static void main(String[] args) 
    {
        if(args.length < 2) {
        	System.err.println("USAGE: DecodeBase64Test1 <input-file-b64> <output-file-bin>");
        	return;
        }
        in_file1 = args[0];
        out_file1 = args[1];
		try {
        	// compose a digidoc file           
            System.out.println("Reading file" + in_file1);
            byte[] b64data = SignedDoc.readFile(new File(in_file1));
            // decode
            byte[] binData = Base64Util.decode(b64data);
            System.out.println("b64: " + b64data.length + " decoded " + binData.length);
            
            System.out.println("Writing file" + out_file1);
            FileOutputStream fos = new FileOutputStream(out_file1);
			fos.write(binData);
			fos.close();
			
            System.out.println("Done!");            
        } catch(Exception ex) {
            System.err.println(ex);
            ex.printStackTrace(System.err);
        }
    }
    
}
