package ee.itp.dds.core;

import ee.sk.digidoc.DigiDocException;
import ee.sk.digidoc.factory.SignatureFactory;
import ee.sk.utils.ConfigManager;
import org.apache.log4j.Logger;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import java.io.FileInputStream;
import java.security.Key;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;

/**
 * KeyStoreSignatureFactory
 * 
 */
public class KeyStoreSignatureFactory implements SignatureFactory {
  
  /** The LOG. */
  private static Logger LOG = Logger.getLogger(KeyStoreSignatureFactory.class);

  /** The Constant sha1AlgPrefix. */
  private static final byte[] sha1AlgPrefix = {
      0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2b,
      0x0e, 0x03, 0x02, 0x1a, 0x05, 0x00, 0x04, 0x14};

  /** The sign sertificate file name. */
  private String signSertificateFileName;
  
  /** The key store file name. */
  private String keyStoreFileName;

  /**
	 * initializes the implementation class.
	 * 
	 * @throws DigiDocException
	 *             the digi doc exception
	 */
  public void init() throws DigiDocException {
    signSertificateFileName = ConfigManager.instance().getProperty("DDS_CERT_FILE");
    keyStoreFileName = ConfigManager.instance().getProperty("DDS_KEY_FILE"); 
  }

  /**
	 * Method returns an array of strings representing the list of available
	 * token names.
	 * 
	 * @return an array of available token names.
	 * 
	 * @throws DigiDocException
	 *             if reading the token information fails.
	 */
  public String[] getAvailableTokenNames() throws DigiDocException {
    return new String[]{};
  }


  /* (non-Javadoc)
   * @see ee.sk.digidoc.factory.SignatureFactory#sign(byte[], int, java.lang.String)
   */
  public byte[] sign(byte[] digest, int token, String pin) throws DigiDocException {
    return signBC(digest, pin);
  }

  /**
	 * Sign bc.
	 * 
	 * @param digest
	 *            the digest
	 * @param pin
	 *            the pin
	 * 
	 * @return the byte[]
	 * 
	 * @throws DigiDocException
	 *             the digi doc exception
	 */
  private byte[] signBC(byte[] digest, String pin) throws DigiDocException {

    try {
      Key key = KeyStoreHelper.getKey(keyStoreFileName, "PKCS12", pin);

      Cipher cryptoEngine;
      cryptoEngine = Cipher.getInstance("RSA/NONE/PKCS1Padding", "BC");
      cryptoEngine.init(Cipher.ENCRYPT_MODE, key);

      byte[] ddata = new byte[sha1AlgPrefix.length + digest.length];
      System.arraycopy(sha1AlgPrefix, 0, ddata, 0, sha1AlgPrefix.length);
      System.arraycopy(digest, 0, ddata, sha1AlgPrefix.length, digest.length);

      return cryptoEngine.doFinal(ddata);

    } catch (NoSuchPaddingException ex) {
      LOG.error(ex.getMessage(), ex);
      throw new DigiDocException(DigiDocException.ERR_SIGNATURE_VALUE_VALUE, "Sign BC.", ex);
    } catch (NoSuchProviderException ex) {
      LOG.error(ex.getMessage(), ex);
      throw new DigiDocException(DigiDocException.ERR_CRYPTO_PROVIDER, "Sign BC.", ex);
    } catch (NoSuchAlgorithmException ex) {
      LOG.error(ex.getMessage(), ex);
      throw new DigiDocException(DigiDocException.ERR_DIGEST_ALGORITHM, "Sign BC.", ex);
    } catch (BadPaddingException ex) {
      LOG.error(ex.getMessage(), ex);
      throw new DigiDocException(DigiDocException.ERR_SIGNATURE_VALUE_VALUE, "Sign BC.", ex);
    } catch (IllegalBlockSizeException ex) {
      LOG.error(ex.getMessage(), ex);
      throw new DigiDocException(DigiDocException.ERR_SIGNATURE_VALUE_VALUE, "Sign BC.", ex);
    } catch (IllegalStateException ex) {
      LOG.error(ex.getMessage(), ex);
      throw new DigiDocException(DigiDocException.ERR_SIGNATURE_VALUE_VALUE, "Sign BC.", ex);
    } catch (java.security.InvalidKeyException ex) {
      LOG.error(ex.getMessage(), ex);
      throw new DigiDocException(DigiDocException.ERR_SIGNATURE_VALUE_VALUE, "Sign BC.", ex);
    }
  }

  /**
	 * Get's certificate.
	 * 
	 * @param token
	 *            does not matter beacuse certificate is taken from a configured
	 *            file in this implementation
	 * @param pin
	 *            does not matter also, because certificate is taken from a
	 *            configured file
	 * 
	 * @return certificate
	 * 
	 * @throws DigiDocException
	 *             the digi doc exception
	 */
  public X509Certificate getCertificate(int token, String pin) throws DigiDocException {
    try {
      return (X509Certificate) CertificateFactory.getInstance("X.509").generateCertificate(
          new FileInputStream(signSertificateFileName));
    } catch (Exception e) {
      LOG.error(e.getMessage(), e);
      throw new DigiDocException(DigiDocException.ERR_SIGPROP_CERT_ID, "Sign cert could not be generated", e);
    }
  }

  /**
	 * Resets the previous session and other selected values.
	 * 
	 * @throws DigiDocException
	 *             the digi doc exception
	 */
  public void reset() throws DigiDocException {

  }

  /* (non-Javadoc)
  * @see ee.sk.digidoc.factory.SignatureFactory#decrypt(byte[], int, java.lang.String)
  */
  public byte[] decrypt(byte[] arg0, int arg1, String arg2) throws DigiDocException {
    throw new UnsupportedOperationException("Operation not supported by this Signature factory implementation");
  }

  public void setSignSertificateFileName(String signSertificateFileName) {
    this.signSertificateFileName = signSertificateFileName;
  }

  public void setKeyStoreFileName(String keyStoreFileName) {
    this.keyStoreFileName = keyStoreFileName;
  }
}
