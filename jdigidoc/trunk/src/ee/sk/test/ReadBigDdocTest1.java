package ee.sk.test;
import java.util.ArrayList;
import java.util.Date;
import java.io.InputStream;
import java.io.FileOutputStream;

import ee.sk.digidoc.*;
import ee.sk.digidoc.factory.*;
import ee.sk.utils.*;

/**
 * Tests reading big digidoc document and using temporary files for caching
 * <DataFile> content instead of memory
 * @author Veiko Sinivee
 */
public class ReadBigDdocTest1 
{
	public static String config_file = null;
    public static String ddoc_file1 = null;
    
    public static void main(String[] args) 
    {
        SignedDoc sdoc = null;
        Date d1, d2;
        if(args.length < 2) {
        	System.err.println("USAGE: ReadBigDdocTest1 <config-file> <input-file>");
        	return;
        }
		config_file = args[0];
		ddoc_file1 = args[1];
        try {
        	d1 = new Date();
        	System.out.println("Reading config file: " + config_file);
            ConfigManager.init(config_file);
            
            // read from file           
            System.out.println("Reading file" + ddoc_file1);
            DigiDocFactory digFac = ConfigManager.
                instance().getDigiDocFactory();
            sdoc = digFac.readSignedDoc(ddoc_file1);
			d2 = new Date();
            System.out.println("Parsing complete, time: " + ((d2.getTime() - d1.getTime()) / 1000) + " [sek]" );
            
            for(int i = 0; i < sdoc.countDataFiles(); i++) {
				DataFile df = sdoc.getDataFile(i);
				System.out.println("DataFile: " + df.getId() + " name: " + df.getFileName() + 
						" size: " + df.getSize() + " cached: " + 
						((df.getDfCacheFile() == null) ? "in-memory" : df.getDfCacheFile().getAbsolutePath()));
				// read DF data out
				InputStream is = df.getBodyAsStream();
				FileOutputStream fos = new FileOutputStream(df.getFileName());
				int b = 0;
				while((b = is.read()) != -1)
					fos.write(b);
				//fos.write(df.getBody());
				fos.close();
			}
            
            // verify signature
            System.out.println("Verifying file" + ddoc_file1);
            ArrayList errs = sdoc.verify(true, true);
            if(errs.size() == 0)
                System.out.println("Document is OK");
            for(int j = 0; j < errs.size(); j++) 
                System.out.println((DigiDocException)errs.get(j));             
            // display signature
            for(int i = 0; i < sdoc.countSignatures(); i++) {
                Signature sig = sdoc.getSignature(i);
                System.out.print("Signature: " + sig.getId() + " - ");
                KeyInfo keyInfo = sdoc.getSignature(i).getKeyInfo();
     			String userId = keyInfo.getSubjectPersonalCode();
     			String firstName = keyInfo.getSubjectFirstName();
     			String familyName = keyInfo.getSubjectLastName();
     			//String timeStamp = sdoc.getSignature(i).getSignedProperties().getSigningTime().toString();
     			System.out.println("Signature: " + userId + "," + firstName + "," + familyName);
                errs = sig.verify(sdoc, true, true);
                if(errs.size() == 0)
                    System.out.print("OK");
                for(int j = 0; j < errs.size(); j++) 
                    System.out.println((DigiDocException)errs.get(i));                
                System.out.println("");
            }
            // cleanup
            System.out.println("Cleanup");
            sdoc.cleanupDfCache();
            
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
