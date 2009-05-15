package ee.itp.dds.util;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.MalformedURLException;
import java.net.URL;

import javax.imageio.ImageIO;

public class FileManager {

	public static String getResource(String filename, Class cls) {
		URL url = cls.getClassLoader().getResource(filename);
		if (url == null)
			throw new RuntimeException("Resource not found:" + filename);
		return url.getFile();
	}

	public static URL getResourceAsUrl(String filename, Class cls) {
		return cls.getClassLoader().getResource(filename);
	}

	public static InputStream getResourceAsinputStream(String filename) {
		return FileManager.class.getClassLoader().getResourceAsStream(filename);
	}

	public static byte[] loadFile(String filename) throws IOException {
		InputStream is = getInputSteam(filename);
		byte[] bytes = new byte[(int) new File(filename).length()];
		is.read(bytes);
		is.close();
		return bytes;
	}

	public static void saveFile(byte[] ret, String filename)
			throws FileNotFoundException, IOException {
		OutputStream os = new FileOutputStream(new File(filename));
		os.write(ret);
		os.close();
	}

	public static InputStream getInputSteam(String filename)
			throws FileNotFoundException {
		return (new FileInputStream(new File(filename)));
	}

	public static String readFile(String filename)
			throws FileNotFoundException, IOException {
		FileReader is = new FileReader(filename);
		char[] chars = new char[(int) new File(filename).length()];
		is.read(chars);
		is.close();
		return String.valueOf(chars).trim();
	}

	public static String removeCharacter(String sss, String chr) {
		int k = sss.indexOf(chr);
		if (k == -1)
			return sss;
		String lll = sss.substring(0, k) + sss.substring(k + 1);
		return lll;
	}

	public static String readFileAsResource(String resource, Class cls)
			throws FileNotFoundException, IOException {
		String filename = getResource(resource, cls);
		System.out.println("Loading:" + filename);
		return readFile(filename);
	}

	public static void saveFileNoErorr(byte[] bytes, String string) {
		try {
			saveFile(bytes, string);
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public static String getProjectRoot(Class baseClass) {
		String baseClassName = baseClass.getName().replace('.', '/') + ".class";
		URL url = baseClass.getClassLoader().getResource(baseClassName);
		int i = url.getPath().indexOf("WEB-INF");
		if (i != -1)
			return url.getPath().substring(0, i);
		i = url.getPath().indexOf("classes-test");
		if (i != -1)
			return url.getPath().substring(0, i);
		i = url.getPath().indexOf("classes-src");
		if (i != -1)
			return url.getPath().substring(0, i);

		throw new RuntimeException("Cant calculate project root bases on URL"
				+ url);
	}
	
    public static InputStream getNoImageInputStream( String text ) {
        try {
            BufferedImage bi = new BufferedImage(120,40,BufferedImage.TYPE_INT_RGB);
            Graphics g = bi.getGraphics();

            g.setColor(Color.WHITE);
            g.fillRect(0,0,120,40);
            g.setColor(Color.BLACK);
            g.setFont( new Font("Serif",Font.BOLD,22));
            g.drawString( text,5,35);

            File tempImg = File.createTempFile("tempImg","jpg");
            tempImg.deleteOnExit();
            ImageIO.write(bi,"jpeg",tempImg);

            return new FileInputStream(tempImg);

        } catch (Exception e) {
            throw new RuntimeException(e);
        }

    }	

    public static byte[] getBytesFromFile( File file ) {

        long length = file.length();
        byte[] bytes = new byte[(int)length];

        try {
            InputStream is = new FileInputStream(file);

            // Read in the bytes
            int offset = 0;
            int numRead = 0;
            while (offset < bytes.length
                    && (numRead=is.read(bytes, offset, bytes.length-offset)) >= 0) {
                offset += numRead;
            }

            // Ensure all the bytes have been read in
            if (offset < bytes.length) {
                throw new IOException("Could not completely read file "+file.getName());
            }

            // Close the input stream and return bytes
            is.close();
        } catch(Exception e ) {
            throw new RuntimeException(e);
        }
        return bytes;
    }    
    
    public static String readFromUrl(URL url) {
        String ret = "";
    	if (url == null) return ret;
    	try {
	    	BufferedReader in = new BufferedReader(new InputStreamReader(url.openStream()));
	        String inputLine;
			while ((inputLine = in.readLine()) != null)
			  ret += inputLine;
	        in.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
        return ret;
    }
    
    public static String readFromUrl(String url) {
    	URL u =null;
    	try {
			u = new URL(url);
		} catch (MalformedURLException e) {
			e.printStackTrace();
		}
        return readFromUrl(u);
    }
    
	

}
