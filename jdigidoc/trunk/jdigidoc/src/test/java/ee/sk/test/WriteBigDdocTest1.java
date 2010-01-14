package ee.sk.test;
import java.io.File;
import java.io.InputStream;
import java.io.FileOutputStream;

import ee.sk.digidoc.*;
import ee.sk.digidoc.factory.*;
import ee.sk.utils.*;

/**
 * Tests creating a big digidoc file and using temporary files for
 * caching <DataFile> contents instead of memory
 * @author Veiko Sinivee
 */
public class WriteBigDdocTest1 {
	public static String config_file = null;
    public static String in_file1 = null;
    public static String in_mime1 = null;
    public static String ddoc_file1 = null;
    
    public static void main(String[] args) 
    {
        SignedDoc sdoc = null;
        long l1, l2;
        if(args.length < 4) {
        	System.err.println("USAGE: WriteBigDdocTest1 <config-file> <input-file> <mime-type> <output-ddoc>");
        	return;
        }
		config_file = args[0];
		in_file1 = args[1];
		in_mime1 = args[2];
		ddoc_file1 = args[3];
        try {
        	l1 = System.currentTimeMillis();
        	System.out.println("Reading config file: " + config_file);
            ConfigManager.init(config_file);
            
            // compose a digidoc file           
            System.out.println("Reading file" + ddoc_file1);
            //A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
            sdoc = new SignedDoc(SignedDoc.FORMAT_BDOC, SignedDoc.BDOC_VERSION_1_0);
            //L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
            // one way to efficiently generate a digidoc is to not use setBody()
            // in any form and just pass in the File object to read input data.
            // In this case the file is read block by block and written to
            // digidoc output file
            //A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
            DataFile df = sdoc.addDataFile(new File(in_file1), in_mime1, DataFile.CONTENT_EMBEDDED);
            //L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
            
            // write it in a file
            System.out.println("Writing in file: " + ddoc_file1);
            sdoc.writeToFile(new File(ddoc_file1));
            l2 = System.currentTimeMillis();
            System.out.println("Writing complete, time: " + ((l2 - l1) / 1000) + " [sek]" );
            
            //L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
            /*DigiDocFactory digFac = ConfigManager.
                instance().getDigiDocFactory();
            l1 = System.currentTimeMillis();
            sdoc = digFac.readSignedDoc(ddoc_file1);
            l2 = System.currentTimeMillis();
            System.out.println("Parsing complete, time: " + ((l2 - l1) / 1000) + " [sek]" );
            
            for(int i = 0; i < sdoc.countDataFiles(); i++) {
				df = sdoc.getDataFile(i);
				System.out.println("DataFile: " + df.getId() + " name: " + df.getFileName() + 
						" size: " + df.getSize() + " cached: " + 
						((df.getDfCacheFile() == null) ? "in-memory" : df.getDfCacheFile().getAbsolutePath()));
				// read DF data out
				InputStream is = df.getBodyAsStream();
				FileOutputStream fos = new FileOutputStream(df.getFileName());
				int b = 0;
				while((b = is.read()) != -1)
					fos.write(b);
				fos.close();
			}
            // cleanup
            System.out.println("Cleanup");
            sdoc.cleanupDfCache();*/
            
            //L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
            
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
