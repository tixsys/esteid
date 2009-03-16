package ee.sk.test;
import java.io.File;
import java.io.InputStream;
import java.io.FileOutputStream;
import java.io.FileInputStream;

import ee.sk.digidoc.*;
import ee.sk.digidoc.factory.*;
import ee.sk.utils.*;

/**
 * Tests creating a big digidoc file and using temporary files for
 * caching <DataFile> contents instead of memory. This test
 * uses setBodyFromStream()
 * @author Veiko Sinivee
 */
public class WriteBigDdocTest2 {
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
            sdoc = new SignedDoc(SignedDoc.FORMAT_DIGIDOC_XML, SignedDoc.VERSION_1_3);
            // another way to efficiently generate a digidoc is to not use setBodyFromStream()
            // Data is copied immediately to temporary file but please note that data is
            // kept in orignal binary format and not converted to base64 so getBody()
            // will not work during composing digidoc.
            DataFile df = sdoc.addDataFile(new File(in_file1), in_mime1, DataFile.CONTENT_EMBEDDED_BASE64);
            FileInputStream fis = new FileInputStream(in_file1);
            df.setBodyFromStream(fis);
            fis.close(); // data has been copied
            
            // write it in a file
            System.out.println("Writing in file: " + ddoc_file1);
            sdoc.writeToFile(new File(ddoc_file1));
            sdoc.cleanupDfCache(); // must cleanup here if we used setBodyFromStream()
            l2 = System.currentTimeMillis();
            System.out.println("Writing complete, time: " + ((l2 - l1) / 1000) + " [sek]" );
            
            DigiDocFactory digFac = ConfigManager.
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
