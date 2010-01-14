package ee.sk.utils;
import java.io.InputStream;
import java.io.FilterInputStream;
import java.io.IOException;

/**
 * Helper class to decode base64 on the fly
 * @author veiko
 *
 */
public class Base64InputStream extends FilterInputStream 
{
	/** buffer */
	private int[] buffer;
	/** A counter for values in the buffer. */
	private int bufferCounter = 0;
	/** End-of-stream flag. */
	private boolean eof = false;
	static String chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	static char pad = '=';
	
	/**
	 * Constructor for Base64InputStream
	 * @param is basic input stream to read from
	 */
	public Base64InputStream(InputStream is) {
		super(is);
	}

	/**
	 * @see java.io.FilterInputStream.read()
	 */
	public int read() 
		throws IOException
	{
		if (buffer == null || bufferCounter == buffer.length) {
			if (eof) {
				return -1;
			}
			acquire();
			if (buffer.length == 0) {
				buffer = null;
				return -1;
			}
			bufferCounter = 0;
		}
		return buffer[bufferCounter++];
	}
	
	
	/**
	 * Reads from the underlying stream, decodes the data and puts the decoded
	 * bytes into the buffer.
	 */
	private void acquire() throws IOException {
		char[] four = new char[4];
		int i = 0;
		do {
			int b = in.read();
			if (b == -1) {
				if (i != 0) {
					throw new IOException("Bad base64 stream");
				} else {
					buffer = new int[0];
					eof = true;
					return;
				}
			}
			char c = (char) b;
			if (chars.indexOf(c) != -1 || c == pad) {
				four[i++] = c;
			}
		} while (i < 4);
		if (four[0] == pad || four[1] == pad) {
			throw new IOException("Bad base64 stream");
		}
		if (four[2] == pad && four[3] != pad) {
			throw new IOException("Bad base64 stream");
		}
		int l;
		if (four[3] == pad) {
			/*if (in.read() != -1) {
				throw new IOException("Bad base64 stream");
			}*/
			eof = true;
			if (four[2] == pad) {
				l = 1;
			} else {
				l = 2;
			}
		} else {
			l = 3;
		}
		int aux = 0;
		for (i = 0; i < 4; i++) {
			if (four[i] != pad) {
				aux = aux | (chars.indexOf(four[i]) << (6 * (3 - i)));
			}
		}
		buffer = new int[l];
		for (i = 0; i < l; i++) {
			buffer[i] = (aux >>> (8 * (2 - i))) & 0xFF;
		}
	}
	
}
