package ee.itp.dds.util;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

public class SecureUtils {

    public static byte[] hashCode(byte[] data) {
        MessageDigest md = null;
        try {
            md = MessageDigest.getInstance("SHA-1");
            md.update(data);
            return md.digest();
        } catch (NoSuchAlgorithmException e) {
            return new byte[0];
        }
    }
    
    
}
