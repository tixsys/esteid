package ee.itp.dds.core;

import ee.sk.digidoc.DigiDocException;

import java.security.Key;
import java.security.KeyStore;
import java.io.FileInputStream;
import java.util.Enumeration;

/**
 * KeyStoreHelper.
 * 
 */
public class KeyStoreHelper {
  
  /**
	 * Get's key from a keystore.
	 * 
	 * @param keyStoreFileName
	 *            file name of the key store
	 * @param keyStoreType
	 *            the key store type
	 * @param password
	 *            the password
	 * 
	 * @return key
	 * 
	 * @throws DigiDocException
	 *             if key could not be read from a given keystore
	 */
  public static Key getKey(String keyStoreFileName, String keyStoreType, String password) throws DigiDocException {
    String privateAlias = null;
    try {
      FileInputStream fis = new FileInputStream(keyStoreFileName);
      KeyStore ks = KeyStore.getInstance(keyStoreType);
      ks.load(fis, password.toCharArray());
      fis.close();

      Enumeration e = ks.aliases();

      while ((privateAlias == null) && e.hasMoreElements()) {
        String alias = (String) e.nextElement();
        // Does alias refer to a private key?
        if (ks.isKeyEntry(alias)) {
          privateAlias = alias;
        }
      }

      if (privateAlias != null) {
        return ks.getKey(privateAlias, password.toCharArray());
      } else {
        throw new DigiDocException(DigiDocException.ERR_SIGNERS_CERT, "Private key alias was not found.", null);
      }
    } catch (Exception e) {
      throw new DigiDocException(DigiDocException.ERR_SIGNERS_CERT, "Read keystore.", e);
    }
  }
}
