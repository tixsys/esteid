/*
 * PKCS11SignatureFactory.java
 * PROJECT: JDigiDoc
 * DESCRIPTION: Digi Doc functions for creating
 *	and reading signed documents. 
 * AUTHOR:  Veiko Sinivee, S|E|B IT Partner Estonia
 *==================================================
 * Copyright (C) AS Sertifitseerimiskeskus
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * GNU Lesser General Public Licence is available at
 * http://www.gnu.org/copyleft/lesser.html
 *==================================================
 */

package ee.sk.digidoc.factory;
import ee.sk.digidoc.DigiDocException;
//import ee.sk.digidoc.SignedDoc;
import java.security.*; 
import java.security.cert.*; 
import iaik.pkcs.pkcs11.*; 
import iaik.pkcs.pkcs11.objects.*; 
import ee.sk.utils.ConfigManager;
import java.io.IOException;
//import java.io.File;
import java.io.ByteArrayInputStream;
import java.util.Vector;
// Logging classes
import org.apache.log4j.Logger;

/**
 * PKCS#11 based signature implementation
 * @author  Veiko Sinivee
 * @version 1.0
 */
public class PKCS11SignatureFactory implements SignatureFactory 
{
    /** Object represent a current PKCS#11 module. */
    private Module m_pkcs11Module;
    /** An array of available tokens. */
    private Token[] m_availableTokens;
    /** An array of available authentication tokens. */
    private Token[] m_authTokens;
    /** A current session object are used to perform cryptographic operations on a token. */
    private Session m_currentSession;
    /** selected (current token) */
    private int m_selToken;
    /** selected authentication (current token) */
    private int m_selAuthToken;
    /** security provider */
    private Provider m_secProvider;
    /** SHA1 algortihm prefix */
    private static final byte[] sha1AlgPrefix = { 
        0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2b, 
        0x0e, 0x03, 0x02, 0x1a, 0x05, 0x00, 0x04, 0x14 };
	/** log4j logger */
    private Logger m_logger = null;
// A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1.1

    /** PKCS#11 module  is initialized */
    private static boolean isInitialized;
// L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1.1


    /** Creates new PKCS11SignatureFactory */
    public PKCS11SignatureFactory() {
        m_pkcs11Module = null;
        m_availableTokens = null;
		m_authTokens = null;
        m_currentSession = null;
        m_selToken = -1;
		m_selAuthToken = -1;
        m_secProvider = null;
        m_logger = Logger.getLogger(PKCS11SignatureFactory.class);
// A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1.1
        isInitialized = false;
// L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1.1

    }
    
    /** 
     * initializes the implementation class  
     */
    public void init()
        throws DigiDocException
    {
        if(m_pkcs11Module == null)
            initPKCS11();
        if(m_secProvider == null)
            initProvider();
    }   
    

    /** 
     * initializes the PKCS#11 subsystem  
     */
    public void initPKCS11()
        throws DigiDocException
    {
       try {
       		if(m_logger.isInfoEnabled())
       			m_logger.info("Loading PKCS11 driver: " + ConfigManager.
                     instance().getProperty("DIGIDOC_SIGN_PKCS11_DRIVER"));
            // load PKCS11 module
            m_pkcs11Module = (Module)AccessController.doPrivileged(
                new PrivilegedExceptionAction()  {
                    public java.lang.Object run() throws IOException {
                        String moduleName = ConfigManager.
                            instance().getProperty("DIGIDOC_SIGN_PKCS11_DRIVER");
                        //System.out.println("Init module: " + moduleName);
                        Module m = Module.getInstance(moduleName);
                        return m;
                    }
                }
            );
            //System.out.println("PKCS11 module loaded");    
            // A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1.1

            if (!isInitialized){
            	m_pkcs11Module.initialize(null); // initializes the module
            	isInitialized = true;
            }
            // L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1.1

            Slot[] availSlots = m_pkcs11Module.getSlotList(Module.SlotRequirement.ALL_SLOTS); // get an array of slot objects
            if (availSlots.length > 0) {
                Vector tokensVector = new Vector(1,1);
				Vector authTokensVector = new Vector(1,1);
                for (int i = 0; i < availSlots.length; i++) {
                    SlotInfo slotInfo = availSlots[i].getSlotInfo(); // get information about this slot object
                    if(m_logger.isDebugEnabled())
       					m_logger.debug("Slot: " + slotInfo);
                    if (slotInfo.isTokenPresent()) { // indicates, if there is a token present in this slot
                        Token tok = availSlots[i].getToken();
                        if(m_logger.isDebugEnabled())
       						m_logger.debug("Token: " + tok);
                        if(isSignatureToken(tok))
                            tokensVector.add(tok); // get an object for handling the token that is currently present in this slot
                        else
							authTokensVector.add(tok);
                    }
                } // end for
                m_availableTokens = new Token[tokensVector.size()]; // create an specified size array of available tokens
                for (int i = 0; i < tokensVector.size(); i++)
                    m_availableTokens[i] = (Token)tokensVector.get(i); // get available token
				m_authTokens = new Token[authTokensVector.size()]; // create an specified size array of available tokens
				for (int i = 0; i < authTokensVector.size(); i++)
					m_authTokens[i] = (Token)authTokensVector.get(i); // get available token
            } // end if
            else { // no slot with present token found
                m_availableTokens = new Token[0]; // create an empty array of available token 
				m_authTokens = new Token[0];
            }
            if(m_logger.isDebugEnabled()) {
            	m_logger.debug("Signature tokens: " + m_availableTokens.length);
				m_logger.debug("Authentication tokens: " + m_authTokens.length);
            }
        } catch(Exception e) {
            m_pkcs11Module = null; // reset since we had an error
            DigiDocException.handleException(e, DigiDocException.ERR_CRYPTO_DRIVER);
        }
        if((m_availableTokens == null) || (m_availableTokens.length == 0)) 
            throw new DigiDocException(DigiDocException.ERR_PKCS11_INIT,
                "Error reading signature certificates from card!", null);
    }

    /**
     * Checks if this is a signature token
     * @param tok token object
     * @return true if this is a signature token
     */
    public boolean isSignatureToken(Token tok)
        throws DigiDocException 
    {
        boolean rc = false;
        try {
            Session sess = tok.openSession(Token.SessionType.SERIAL_SESSION,
                Token.SessionReadWriteBehavior.RO_SESSION,null,null); 
            X509PublicKeyCertificate tempCert = new X509PublicKeyCertificate();
            sess.findObjectsInit(tempCert);
            iaik.pkcs.pkcs11.objects.Object[] foundCerts = sess.findObjects(10); // find first
            if(m_logger.isDebugEnabled()) 
            	m_logger.debug("Certs: " + foundCerts.length);
            if (foundCerts.length > 0) {
            	if(m_logger.isDebugEnabled()) 
                	m_logger.debug("Examining cert data");
                X509PublicKeyCertificate pkcert = (X509PublicKeyCertificate)foundCerts[0];
                if(m_logger.isDebugEnabled()) 
                	m_logger.debug("Cert0: " + pkcert);
                CertificateFactory certFac = CertificateFactory.getInstance("X.509"); // generates a certificate factory object that implements the X.509 certificate type
                byte[] encCert = pkcert.getValue().getByteArrayValue(); // an array of bytes representing the DER encoded X.509 certificate
                //if(m_logger.isDebugEnabled()) m_logger.debug("Cert data: " + encCert.length + " bytes");
                X509Certificate cert = (X509Certificate)certFac.generateCertificate(new ByteArrayInputStream(encCert)); // generates a X.509 certificate object and initializes it
                //if(m_logger.isDebugEnabled()) 
                //	m_logger.debug("Cert: " + cert);
                boolean keyUsages[] = cert.getKeyUsage();
                StringBuffer sb = new StringBuffer("KeyUsages:");
                for(int k = 0; (keyUsages != null) && (k < keyUsages.length); k++) {
                	sb.append(" ");
                	sb.append(keyUsages[1]);
                }
                if(m_logger.isDebugEnabled())
                	m_logger.debug("KeyUsages: " + ((keyUsages != null) ? keyUsages.length : 0) + " - " + sb.toString());
                if(keyUsages != null && keyUsages.length > 2 && keyUsages[1] == true) {
                	if(m_logger.isDebugEnabled()) 
                    	m_logger.debug("Signature cert!");
                    rc = true;
                } else
                	if(m_logger.isDebugEnabled()) 
                    	m_logger.debug("Auth cert!");
            } // end if
            sess.findObjectsFinal();
            sess.closeSession();
            
        } catch(Exception e) {
            DigiDocException.handleException(e, DigiDocException.ERR_READ_TOKEN_INFO);
        } // end catch
        return rc;
    }

    /**
     * Initializes Java cryptography provider
     */
    private void initProvider()
        throws DigiDocException
    {
        try {
            m_secProvider = (Provider)Class.forName(ConfigManager.
            instance().getProperty("DIGIDOC_SECURITY_PROVIDER")).newInstance();
            Security.addProvider(m_secProvider);
            //System.out.println("Provider OK");
        } catch(Exception ex) {
            m_secProvider = null;
            DigiDocException.handleException(ex, DigiDocException.ERR_CRYPTO_PROVIDER);
        }
    }

    /**
     * Method returns an array of strings representing the 
     * list of available token names.
     * @return an array of available token names.
     * @throws DigiDocException if reading the token information fails.
     */
    public String[] getAvailableTokenNames()
        throws DigiDocException 
    { 
        if(m_pkcs11Module == null)
            initPKCS11();
        String[] availableTokenNames = null;
        try {
            availableTokenNames = new String[m_availableTokens.length];
            for (int i = 0; i < m_availableTokens.length; i++) {
                TokenInfo tokenInfo = m_availableTokens[i].getTokenInfo(); // get information about this token
                availableTokenNames[i] = tokenInfo.getLabel(); // get the label of this token
            } 
        } catch(TokenException e) {
            DigiDocException.handleException(e, DigiDocException.ERR_READ_TOKEN_INFO);
        } 
        return availableTokenNames;
    } 
    
    /**
     * Method opens a new session to perfom operations on 
     * specified token and logs in the user
     * or the security officer to the session.
     * @param bSignSession true if we want to open a session with signature token
     * @param token the specified token index.
     * @param pin the PIN.
     * @throws DigiDocException if the session could not be opened or if login fails.
     */
    public void openSession(boolean bSignSession, int token, String pin)
        throws DigiDocException
    { 
        if(m_pkcs11Module == null)
            initPKCS11();
        try {
            // don't login if the session exists
            if(m_currentSession == null || 
            	(bSignSession && m_selToken != token) ||
            	(!bSignSession && m_selAuthToken != token)) {
                // close the old session if necessary
                if(m_currentSession != null)
                     closeSession();
                if(m_logger.isDebugEnabled())
       				m_logger.debug("Open session for token: " + token);
       			if(bSignSession) {
       				TokenInfo tokenInfo = m_availableTokens[token].getTokenInfo();
       				if(m_logger.isDebugEnabled())
           				m_logger.debug("Open session for sign token: " + tokenInfo.getLabel());
                	// open a new session to perfom operations on this token
                	m_currentSession = m_availableTokens[token].
                    	openSession(Token.SessionType.SERIAL_SESSION,
                        	Token.SessionReadWriteBehavior.RO_SESSION,null,null);
					m_selToken = token;
					m_selAuthToken = -1; 
       			} else {
					m_currentSession = m_authTokens[token].
						openSession(Token.SessionType.SERIAL_SESSION,
												Token.SessionReadWriteBehavior.RO_SESSION,null,null);
					m_selToken = -1;
					m_selAuthToken = token; 
       			}
                // logs in the user or the security officer to the session
                m_currentSession.login(Session.UserType.USER, pin.toCharArray());                 
            }
        } catch(TokenException e) {
            m_selToken = -1;
			m_selAuthToken = -1; 
            m_currentSession = null;
            DigiDocException.handleException(e, DigiDocException.ERR_TOKEN_LOGIN);
        } 
    } 

    /**
     * Method returns a digital signature. It finds the RSA private 
     * key object from the active token and
     * then signs the given data with this key and RSA mechanism.
     * @param digest digest of the data to be signed.
     * @param token token index
     * @param pin users pin code
     * @return an array of bytes containing digital signature.
     * @throws DigiDocException if signing the data fails.
     */
    public byte[] sign(byte[] digest, int token, String pin) 
        throws DigiDocException 
    {
        byte[] sigVal = null;
        if(m_currentSession == null)
            openSession(true, token, pin);
        try {
        	if(m_logger.isDebugEnabled())
       			m_logger.debug("Sign with token: " + token);
            // the RSA private key object that serves as a template for searching
            RSAPrivateKey tempKey = new RSAPrivateKey(); 
            // initializes a find operations to find RSA private key objects
            m_currentSession.findObjectsInit(tempKey); 
            // find first
            iaik.pkcs.pkcs11.objects.Object[] foundKeys = 
                m_currentSession.findObjects(1); 
            RSAPrivateKey sigKey = null;
            //System.out.println("Keys: " + foundKeys.length);
            if (foundKeys.length > 0) {
                sigKey = (RSAPrivateKey)foundKeys[0];
                Mechanism sigMech = Mechanism.RSA_PKCS;
                // initializes a new signing operation
                m_currentSession.signInit(sigMech, sigKey); 
                byte[] ddata = new byte[sha1AlgPrefix.length + digest.length];
                System.arraycopy(sha1AlgPrefix, 0, ddata, 0, sha1AlgPrefix.length);
                System.arraycopy(digest, 0, ddata, 
                     sha1AlgPrefix.length, digest.length);
                sigVal = m_currentSession.sign(ddata); // signs the given data with the key and mechansim given to the signInit method
            } 
            m_currentSession.findObjectsFinal(); // finalizes a find operation
        } catch(TokenException e) {
            DigiDocException.handleException(e, DigiDocException.ERR_SIGN);
        } 
        return sigVal;
    } 

    /**
     * Method returns a X.509 certificate object readed 
     * from the active token and representing an
     * user public key certificate value.
     * @return X.509 certificate object.
     * @throws DigiDocException if getting X.509 public key certificate 
     * fails or the requested certificate type X.509 is not available in 
     * the default provider package
     */
    public X509Certificate getCertificate(int token, String pin)
        throws DigiDocException 
    {
        //System.out.println("getCertificate()");
        if(m_currentSession == null)
            openSession(true, token, pin);
        X509Certificate cert = null;
        try {
            if(m_logger.isDebugEnabled())
       			m_logger.debug("Get cert for token: " + token);
            X509PublicKeyCertificate tempCert = new X509PublicKeyCertificate();
            m_currentSession.findObjectsInit(tempCert);
            iaik.pkcs.pkcs11.objects.Object[] foundCerts = 
                m_currentSession.findObjects(1); // find first
            //System.out.println("Certs: " + foundCerts.length);
            if (foundCerts.length > 0) {
               X509PublicKeyCertificate pkcert = (X509PublicKeyCertificate)foundCerts[0];
               // generates a certificate factory object that implements the X.509 certificate type
               CertificateFactory certFac = CertificateFactory.getInstance("X.509"); 
               // an array of bytes representing the DER encoded X.509 certificate
               byte[] encCert = pkcert.getValue().getByteArrayValue(); 
               // generates a X.509 certificate object and initializes it
               cert = (X509Certificate)certFac.generateCertificate(new ByteArrayInputStream(encCert)); 
            } 
            m_currentSession.findObjectsFinal(); 
        } catch(CertificateException e) {
            DigiDocException.handleException(e, DigiDocException.ERR_READ_CERT);
        } catch(TokenException e) {
            DigiDocException.handleException(e, DigiDocException.ERR_READ_CERT);
        } 
        return cert;
    } 
    
	/**
		 * Method decrypts the data with the RSA private key
		 * corresponding to this certificate (which was used
		 * to encrypt it). Decryption will be done on the card.
		 * This operation closes the possibly opened previous
		 * session with signature token and opens a new one with
		 * authentication tokne if necessary
		 * @param data data to be decrypted.
		 * @param token index of authentication token
		 * @param pin PIN code
		 * @return decrypted data.
		 * @throws DigiDocException for all decryption errors
		 */
		public byte[] decrypt(byte[] data, int token, String pin) 
			throws DigiDocException 
		{
			byte[] value = null;
			if (m_pkcs11Module == null)
				initPKCS11();
			if(m_currentSession == null || m_selAuthToken != token) {
				openSession(false, token, pin);
			}
			try {
				RSAPrivateKey templateAuthKey = new RSAPrivateKey(); // the RSA private key object that serves as a template for searching
				m_currentSession.findObjectsInit(templateAuthKey); // initializes a find operations to find RSA private key objects
				iaik.pkcs.pkcs11.objects.Object[] foundAuthKeyObjects = m_currentSession.findObjects(100); // find first
				if(m_logger.isDebugEnabled()) {
					m_logger.debug("Decrypting " + data.length + " bytes");
					m_logger.debug("Decrypting with token: " + m_selToken);
					m_logger.debug("Signature length: " + foundAuthKeyObjects.length);
					m_logger.debug("session: " + m_currentSession);
				}
				if (foundAuthKeyObjects.length > 0) {
					RSAPrivateKey key = (RSAPrivateKey) foundAuthKeyObjects[m_selAuthToken];
					Mechanism m = Mechanism.RSA_PKCS;
					if(m_logger.isDebugEnabled()) {
						m_logger.debug("RSAPrivateKey = " + key);
						m_logger.debug("Mechanism = " + m);
					}
					m_currentSession.decryptInit(m, key); // initializes a new signing operation
					if(m_logger.isDebugEnabled()) {
						m_logger.debug("decryptInit OK");
					}
					value = m_currentSession.decrypt(data);
					if(m_logger.isDebugEnabled()) {
						m_logger.debug("value = " + value);
					}
				} // end if
				if(m_logger.isInfoEnabled()) 
					m_logger.info("Decrypted " + data.length + " bytes, got: " + value.length);
				m_currentSession.findObjectsFinal(); // finalizes a find operation
			} catch (TokenException e) {
				DigiDocException.handleException(e, DigiDocException.ERR_XMLENC_DECRYPT);
			} // end catch
			return value;
		}


    /**
     * Method closes the current session.
     * @throws DigiDocException if closing the session fails.
     */
    public void closeSession() 
        throws DigiDocException 
    { 
        try {
        	if(m_logger.isDebugEnabled())
       			m_logger.debug("Closing card session");
            // closes this session
            m_currentSession.closeSession(); 
            m_currentSession = null; // ???
        } catch(TokenException e) {
            DigiDocException.handleException(e, DigiDocException.ERR_TOKEN_LOGOUT);
        } 
    } 
    
    /**
     * This finalize method tries to finalize the module 
     * by calling finalize() of the PKCS#11 module.
     * @throws DigiDocException if PKCS#11 module finalization fails.
     */
    public void finalize()
        throws DigiDocException 
    {
        try {
            if(m_pkcs11Module != null)
                m_pkcs11Module.finalize(null); // finalizes this module
            m_pkcs11Module = null;
        } catch(TokenException e) {
            DigiDocException.handleException(e, DigiDocException.ERR_CRYPTO_FINALIZE);
        } 
    }
    
    /**
     * Resets the previous session
     * and other selected values
     */
    public void reset() 
        throws DigiDocException 
    {
    	if(m_logger.isDebugEnabled())
       			m_logger.debug("Resetting PKCS11SignatureFactory");
        m_selToken = -1;
        m_selAuthToken = -1;
        closeSession();
        m_pkcs11Module = null; //???
        m_secProvider = null; //???
        finalize();
    }


}
