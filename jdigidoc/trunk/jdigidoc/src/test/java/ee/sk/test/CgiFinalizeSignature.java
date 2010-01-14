package ee.sk.test;
import ee.sk.digidoc.*;

import java.io.*;
import java.security.cert.X509Certificate;
import ee.sk.digidoc.factory.*;
import ee.sk.utils.*;

import java.util.*;



/**
 * CgiFinalizeSignature demonstrates creating a new signature with JDigiDoc
 * library in a CGI-like way, e.g. reading incomplete ddoc from file,
 * reading signature value from file, finalizing signature and writing it to 
 * a final ddoc file
 * @author  Veiko Sinivee
 * @version 1.0
 */
public class CgiFinalizeSignature {

	public static String config_file = null;
   	public static String ddoc_in = null;
   	public static String ddoc_out = null;
    public static String sigvalfile = null;
	public static String sigId = null;
	
    public static void main(String[] args) 
    {
        SignedDoc sdoc = null;
		if(args.length < 5) {
			System.err.println("USAGE: CgiFinalizeSignature <config-file> <ddoc-in> <sig-id> <sign-value-file> <ddoc-out>");
			return;
		} 
		config_file = args[0];
		ddoc_in = args[1];
		sigId = args[2];
		sigvalfile = args[3];
		ddoc_out = args[4];
		
        try {
			System.out.println("Reading config file: " + config_file);
			ConfigManager.init(config_file);
			// read incomplete digidoc
            System.out.println("Reading digidoc: " + ddoc_in);
			DigiDocFactory digFac = ConfigManager.
            	instance().getDigiDocFactory();
			sdoc = digFac.readSignedDoc(ddoc_in);
            
            // add a Signature
            System.out.println("Finalize signature");
            byte[] sigval = SignedDoc.readFile(new File(sigvalfile));
            // let's assume signature value was stored in base64 format
            if(sigval != null && sigval.length > 128) // if encoded ?
            	sigval = Base64Util.decode(sigval);
            // actually web signature modules deliver it in hex format
            //sigval = SignedDoc.hex2bin(new String(sigval));
            
            System.out.println("Final signature value " + ((sigval == null) ? 0 : sigval.length) + " bytes");
			
            // check for decode errors
            if(sigval != null && sigval.length == 128 && sigId != null) {
            	Signature sig = sdoc.findSignatureById(sigId);
            	System.out.println("Finalize signature");
            	sig.setSignatureValue(sigval);
                // get confirmation
            	System.out.println("Get confirmation");
            	sig.getConfirmation();
            	System.out.println("Confirmation OK!");
            }
            
            // write it in a file
            System.out.println("Writing in file: " + ddoc_out);
            sdoc.writeToFile(new File(ddoc_out));  
            System.out.println("Done!");
             
            
        } catch(DigiDocException ex) {
            System.err.println(ex);
            ex.printStackTrace(System.err);
        } catch(Exception ex) {
            System.err.println(ex);
            ex.printStackTrace(System.err);
        }
    }
    
}
