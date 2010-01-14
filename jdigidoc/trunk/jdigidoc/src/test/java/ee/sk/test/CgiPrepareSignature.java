package ee.sk.test;
import ee.sk.digidoc.*;
import java.io.*;
import java.security.cert.X509Certificate;
import ee.sk.digidoc.factory.*;
import ee.sk.utils.*;
import java.util.*;



/**
 * CgiPrepareSignature demonstrates creating a new signature with JDigiDoc
 * library in a CGI-like way, e.g. receiving signers cert from
 * a file written by another programm, displaying hash-to-be-signed
 * and writing incomplete signature in a ddoc file
 * @author  Veiko Sinivee
 * @version 1.0
 */
public class CgiPrepareSignature {

	public static String config_file = null;
   	public static String ddocfile = null;
    public static String inputfile = null;
    public static String mime = "application/binary";
	public static String outputfile = null;
    public static String certfile = null;
	
	
    public static void main(String[] args) 
    {
        SignedDoc sdoc = null;
		if(args.length < 4) {
			System.err.println("USAGE: CgiPrepareSignature <config-file> <input-file> <certfile> <outputfile>");
			return;
		} 
		config_file = args[0];
		inputfile = args[1];
		certfile = args[2];
		ddocfile = args[3];
		
        try {
			System.out.println("Reading config file: " + config_file);
			ConfigManager.init(config_file);
			// create a new SignedDoc 
            sdoc = new SignedDoc(SignedDoc.FORMAT_DIGIDOC_XML, SignedDoc.VERSION_1_3);
			System.out.println("Adding file: " + inputfile);
            DataFile df = sdoc.addDataFile(new File(inputfile), mime, DataFile.CONTENT_EMBEDDED_BASE64);
            // read certificate from file
            System.out.println("Read cert from: " + certfile);
            X509Certificate cert = SignedDoc.readCertificate(certfile);
            // add a Signature
            System.out.println("Prepare signature");
            Signature sig = sdoc.prepareSignature(cert, null, null);
            byte[] sidigest = sig.calculateSignedInfoDigest();
            System.out.println("Final hash to sign: " + Base64Util.encode(sidigest) );
			// write it in a file
            System.out.println("Writing in file: " + ddocfile);
            sdoc.writeToFile(new File(ddocfile));  
            
            
            /*byte[] sigval = sigFac.sign(sidigest, 0, pin);
            System.out.println("Finalize signature");
            sig.setSignatureValue(sigval);
             
            // get confirmation
            System.out.println("Get confirmation");
            sig.getConfirmation();
            System.out.println("Confirmation OK!");
            //System.out.println("Signature: " + sig);
             */
            
			
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
