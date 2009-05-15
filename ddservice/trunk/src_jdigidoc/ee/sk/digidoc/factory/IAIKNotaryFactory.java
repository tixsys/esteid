/*
 * IAIKNotaryFactory.java
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
import ee.sk.digidoc.SignedDoc;
import ee.sk.utils.ConfigManager;
import ee.sk.digidoc.Base64Util;
import ee.sk.digidoc.Notary;
import ee.sk.digidoc.Signature;
//import ee.sk.digidoc.SignatureValue;

import java.security.cert.X509Certificate;
import java.security.PrivateKey;
//import java.security.PublicKey;
import java.security.Provider;
import java.security.Security;

import java.security.KeyStore;

import java.math.BigInteger;

import iaik.x509.ocsp.OCSPRequest;
import iaik.x509.ocsp.OCSPResponse;
import iaik.x509.ocsp.Request;
import iaik.x509.ocsp.CertID;
import iaik.x509.ocsp.ReqCert;
import iaik.x509.ocsp.BasicOCSPResponse;
import iaik.x509.ocsp.SingleResponse;
import iaik.x509.ocsp.CertStatus;


//import iaik.asn1.structures.Name;
import iaik.asn1.structures.AlgorithmID;
import iaik.asn1.structures.GeneralName;
import java.util.Date;
import java.util.Hashtable;
import java.text.SimpleDateFormat;

import java.net.*;
import java.io.*;

// Logging classes
import org.apache.log4j.Logger;

/**
 * Implements the NotaryFactory by using
 * IAIK JCE toolkit
 * @author  Veiko Sinivee
 * @version 1.0
 */
public class IAIKNotaryFactory implements NotaryFactory 
{
    /** cert used to add to all OCSP requests */
    private iaik.x509.X509Certificate m_signCert;
    /** key used to sign all OCSP requests */
    private PrivateKey m_signKey;
    private boolean m_bSignRequests;
    private Logger m_logger = null;
    private Hashtable m_ocspCerts;
    private Hashtable m_ocspCACerts;
    
    /** Creates new IAIKNotaryFactory */
    public IAIKNotaryFactory() {
        m_signCert = null;
        m_signKey = null;
        m_ocspCerts = new Hashtable();
        m_ocspCACerts = new Hashtable();
        m_bSignRequests = false;
        m_logger = Logger.getLogger(IAIKNotaryFactory.class);
    }
        
    /**
     * Returns the OCSP responders certificate
     * @param responderCN responder-id's CN
     * @param specificCertNr specific cert number that we search.
     * If this parameter is null then the newest cert is seleced (if many exist)
     * @returns OCSP responders certificate
     */
    public X509Certificate getNotaryCert(String responderCN, String specificCertNr)
    {
    	return (X509Certificate)m_ocspCerts.get(responderCN);
    }
    
    
    /**
     * Returns the OCSP responders CA certificate
     * @param responderCN responder-id's CN
     * @returns OCSP responders CA certificate
     */
    public X509Certificate getCACert(String responderCN)
    {
    	return (X509Certificate)m_ocspCACerts.get(responderCN);
    }

    /**
     * Get confirmation from AS Sertifitseerimiskeskus
     * by creating an OCSP request and parsing the returned
     * OCSP response
     * @param nonce signature nonce
     * @param signersCert signature owners cert
     * @param notId new id for Notary object
     * @returns Notary object
     */
    public Notary getConfirmation(byte[] nonce, 
        X509Certificate signersCert, String notId) 
        throws DigiDocException 
    {    	
        return getConfirmation(nonce, 
        	signersCert, getCACert(SignedDoc.getCommonName(signersCert.getIssuerDN().getName())), notId);
    }

    
    /**
     * Get confirmation from AS Sertifitseerimiskeskus
     * by creating an OCSP request and parsing the returned
     * OCSP response
     * @param nonce signature nonce
     * @param signersCert signature owners cert
     * @param caCert CA cert for this signer
     * @param notId new id for Notary object
     * @returns Notary object
     */
    public Notary getConfirmation(byte[] nonce, 
        X509Certificate signersCert, X509Certificate caCert, String notId) 
        throws DigiDocException 
    {
        Notary not = null;
        try {        
        	if(m_logger.isDebugEnabled())
                m_logger.debug("getConfirmation, nonce " + Base64Util.encode(nonce, 0) +
                " cert: " + ((signersCert != null) ? signersCert.getSerialNumber().toString() : "NULL") + 
                " CA: " + ((caCert != null) ? caCert.getSerialNumber().toString() : "NULL") +
                " notId: " + notId + " signRequest: " + m_bSignRequests);
            // create the request
            OCSPRequest req = createOCSPRequest(nonce, signersCert, caCert);
            // sign the request if necessary
            if(m_bSignRequests)
            	req = signRequest(req);
            if(m_logger.isDebugEnabled())
                m_logger.debug("REQUEST:\n" + Base64Util.encode(req.getEncoded(), 0));
            // send it
            OCSPResponse resp = sendRequest(req);
            if(m_logger.isDebugEnabled())
                m_logger.debug("RESPONSE:\n" + Base64Util.encode(resp.getEncoded(), 0));
            // check the result
            not = parseAndVerifyResponse(notId, signersCert, resp, nonce);
            if(m_logger.isDebugEnabled())
                m_logger.debug("Confirmation OK!");
        } catch(DigiDocException ex) {
            throw ex;
        } catch(Exception ex) {
            DigiDocException.handleException(ex, DigiDocException.ERR_OCSP_GET_CONF);        
        }
        return not;
    }


    /**
     * Get confirmation from AS Sertifitseerimiskeskus
     * by creating an OCSP request and parsing the returned
     * OCSP response
     * @param sig Signature object. 
     * @param signersCert signature owners cert
     * @param caCert CA cert for this signer
     * @returns Notary object
     */
    public Notary getConfirmation(Signature sig, 
        X509Certificate signersCert, X509Certificate caCert) 
        throws DigiDocException 
    {
    	
        Notary not = null;
        try {
        	String notId = sig.getId().replace('S', 'N');
            // calculate the nonce
            byte[] nonce = SignedDoc.digest(sig.getSignatureValue().getValue());
            // check the result
            not = getConfirmation(nonce, signersCert, caCert, notId);
        } catch(DigiDocException ex) {
            throw ex;
        } catch(Exception ex) {
            DigiDocException.handleException(ex, DigiDocException.ERR_OCSP_GET_CONF);        
        }
        return not;
    }

    /**
     * Get confirmation from AS Sertifitseerimiskeskus
     * by creating an OCSP request and parsing the returned
     * OCSP response. CA and reponders certs are read 
     * using paths in the config file or maybe from
     * a keystore etc.
     * @param sig Signature object
     * @param signersCert signature owners cert
     * @returns Notary object
     */
    public Notary getConfirmation(Signature sig, X509Certificate signersCert) 
        throws DigiDocException 
    {
        return getConfirmation(sig, signersCert, 
        	getCACert(SignedDoc.getCommonName(signersCert.getIssuerDN().getName())));
    }
    
    private String ocspFileName(X509Certificate cert)
    {
    	StringBuffer sb = new StringBuffer(cert.getSerialNumber().toString());
    	sb.append("_");
    	Date dtNow = new Date();
    	SimpleDateFormat df = new SimpleDateFormat("HHmmss");
    	sb.append(df.format(dtNow));    	
    	return sb.toString();	
    }
    
    /**
     * Verifies the certificate by creating an OCSP request
     * and sending it to SK server.
     * @param cert certificate to verify
     * @throws DigiDocException if the certificate is not valid
     */
    public void checkCertificate(X509Certificate cert) 
        throws DigiDocException 
    {
        try {
        	String verifier = ConfigManager.instance().
                getStringProperty("DIGIDOC_CERT_VERIFIER", "OCSP");
            if(verifier != null && verifier.equals("OCSP")) {
            	// create a nonce for this request
            	String strTime = new java.util.Date().toString();
            	byte[] nonce1 = SignedDoc.digest(strTime.getBytes());
        	// create the request
        	X509Certificate caCert = getCACert(SignedDoc.getCommonName(cert.getIssuerDN().getName()));
            OCSPRequest req = createOCSPRequest(nonce1, cert, caCert);
            // sign the request if necessary
            req = signRequest(req);
            if(m_logger.isDebugEnabled()) {
            	m_logger.debug("Sending ocsp request: " + req.getEncoded().length + " bytes");
                m_logger.debug("REQUEST:\n" + Base64Util.encode(req.getEncoded(), 0));
            }    
            // send it
            OCSPResponse resp = sendRequest(req);
            if(m_logger.isDebugEnabled()) {
                m_logger.debug("Got ocsp response: " + resp.getEncoded().length + " bytes");
                m_logger.debug("RESPONSE:\n" + Base64Util.encode(resp.getEncoded(), 0));
            }
            // check the result
            //not = parseAndVerifyResponse(sig, resp, nonce, notaryCert);
            if(resp == null || resp.getResponseStatus() != OCSPResponse.successful)
                throw new DigiDocException(DigiDocException.ERR_OCSP_UNSUCCESSFULL,
                    "OCSP response unsuccessfull!", null);
            // now read the info from the response
            BasicOCSPResponse basResp = 
                (BasicOCSPResponse)resp.getResponse(); 
            // check the nonce
            byte[] nonce2 = basResp.getNonce();
            if(!SignedDoc.compareDigests(nonce1, nonce2)) 
            	throw new DigiDocException(DigiDocException.ERR_OCSP_UNSUCCESSFULL,
                    "Invalid nonce value! Possible replay attack!", null);
            
            // verify the response
            try {
            	X509Certificate notaryCert = getNotaryCert(SignedDoc.getCommonName(basResp.getResponderID().toString()), null);
                basResp.verify(notaryCert.getPublicKey()); 
            } catch (Exception ex) {
                m_logger.error("OCSP Signature verification error!!!", ex); 
                DigiDocException.handleException(ex, DigiDocException.ERR_OCSP_VERIFY);
            } 
            // check the response about this certificate
            checkCertStatus(cert, basResp);
            } else if(verifier != null && verifier.equals("CRL")) {
            	CRLFactory crlFac = ConfigManager.instance().getCRLFactory();
            	crlFac.checkCertificate(cert, new Date());
            }
        } catch(DigiDocException ex) {
            throw ex;
        } catch(Exception ex) {
            DigiDocException.handleException(ex, DigiDocException.ERR_OCSP_GET_CONF);        
        }
    }
    
    /**
     * Verifies the certificate.
     * @param cert certificate to verify
     * @param bUseOcsp flag: use OCSP to verify cert. If false then use CRL instead
     * @throws DigiDocException if the certificate is not valid
     */   
    public void checkCertificateOcspOrCrl(X509Certificate cert, boolean bUseOcsp) 
        throws DigiDocException
    {
    	try {
        	if(bUseOcsp) {
            	// create a nonce for this request
            	String strTime = new java.util.Date().toString();
            	byte[] nonce1 = SignedDoc.digest(strTime.getBytes());
        	// create the request
        	X509Certificate caCert = getCACert(SignedDoc.getCommonName(cert.getIssuerDN().getName()));
            OCSPRequest req = createOCSPRequest(nonce1, cert, caCert);
            // sign the request if necessary
            req = signRequest(req);
            if(m_logger.isDebugEnabled()) {
            	m_logger.debug("Sending ocsp request: " + req.getEncoded().length + " bytes");
                m_logger.debug("REQUEST:\n" + Base64Util.encode(req.getEncoded(), 0));
            }    
            // send it
            OCSPResponse resp = sendRequest(req);
            if(m_logger.isDebugEnabled()) {
                m_logger.debug("Got ocsp response: " + resp.getEncoded().length + " bytes");
                m_logger.debug("RESPONSE:\n" + Base64Util.encode(resp.getEncoded(), 0));
            }
            // check the result
            //not = parseAndVerifyResponse(sig, resp, nonce, notaryCert);
            if(resp == null || resp.getResponseStatus() != OCSPResponse.successful)
                throw new DigiDocException(DigiDocException.ERR_OCSP_UNSUCCESSFULL,
                    "OCSP response unsuccessfull!", null);
            // now read the info from the response
            BasicOCSPResponse basResp = 
                (BasicOCSPResponse)resp.getResponse(); 
            // check the nonce
            byte[] nonce2 = basResp.getNonce();
            if(!SignedDoc.compareDigests(nonce1, nonce2)) 
            	throw new DigiDocException(DigiDocException.ERR_OCSP_UNSUCCESSFULL,
                    "Invalid nonce value! Possible replay attack!", null);
            
            // verify the response
            try {
            	X509Certificate notaryCert = getNotaryCert(SignedDoc.getCommonName(basResp.getResponderID().toString()), null);
                basResp.verify(notaryCert.getPublicKey()); 
            } catch (Exception ex) {
                m_logger.error("OCSP Signature verification error!!!", ex); 
                DigiDocException.handleException(ex, DigiDocException.ERR_OCSP_VERIFY);
            } 
            // check the response about this certificate
            checkCertStatus(cert, basResp);
            } else  {
            	CRLFactory crlFac = ConfigManager.instance().getCRLFactory();
            	crlFac.checkCertificate(cert, new Date());
            }
        } catch(DigiDocException ex) {
            throw ex;
        } catch(Exception ex) {
            DigiDocException.handleException(ex, DigiDocException.ERR_OCSP_GET_CONF);        
        }
    }
    
    /**
     * Check the response and parse it's data
     * @param sig Signature object
     * @param resp OCSP response
     * @param nonce1 nonve value used for request
     * @param notaryCert notarys own cert
     * @returns Notary object
     */
    private Notary parseAndVerifyResponse(Signature sig, OCSPResponse resp, 
        byte[] nonce1)
        throws DigiDocException
    {
    	String notId = sig.getId().replace('S', 'N');
    	X509Certificate sigCert = sig.getKeyInfo().getSignersCertificate();
    	return parseAndVerifyResponse(notId, sigCert, resp, nonce1);
    }

    /**
     * Check the response and parse it's data
     * @param notId new id for Notary object
     * @param signersCert signature owners certificate
     * @param resp OCSP response
     * @param nonce1 nonve value used for request
     * @returns Notary object
     */
    private Notary parseAndVerifyResponse(String notId, 
    	X509Certificate signersCert, OCSPResponse resp, byte[] nonce1)
        throws DigiDocException
    {
        Notary not = null;
        // check the result
        if(resp == null || resp.getResponseStatus() != OCSPResponse.successful)
            throw new DigiDocException(DigiDocException.ERR_OCSP_UNSUCCESSFULL,
                "OCSP response unsuccessfull!", null);
        try {            
            // now read the info from the response
            BasicOCSPResponse basResp = 
                (BasicOCSPResponse)resp.getResponse(); 
            // verify the response
            try {
            	X509Certificate notaryCert = getNotaryCert(SignedDoc.getCommonName(basResp.getResponderID().toString()), null);
                basResp.verify(notaryCert.getPublicKey()); 
            } catch (Exception ex) {
                m_logger.error("Signature verification error!!!", ex); 
                DigiDocException.handleException(ex, DigiDocException.ERR_OCSP_VERIFY);
            } 
            // done't care about SingleResponses because we have
            // only one response and the whole response was successfull
            // but we should verify that the nonce hasn't changed
            byte[] nonce2 = basResp.getNonce();
            boolean ok = true;
            if(nonce1.length != nonce2.length)
                ok = false;
            for(int i = 0; i < nonce1.length; i++)
                if(nonce1[i] != nonce2[i])
                    ok = false;
            if(!ok)
                throw new DigiDocException(DigiDocException.ERR_OCSP_NONCE,
                    "OCSP response's nonce doesn't match the requests nonce!", null);
            // check the response on our cert
            checkCertStatus(signersCert, basResp);
            // create notary            
            not = new Notary(notId, resp.getEncoded(), 
                basResp.getResponderID().toString(),
                    basResp.getProducedAt());
        } catch(DigiDocException ex) {
            throw ex;
        } catch(Exception ex) {
            DigiDocException.handleException(ex, DigiDocException.ERR_OCSP_PARSE);
        }
        return not;
    }

    /**
     * Verifies that the OCSP response is about our signers
     * cert and the response status is successfull
     * @param sig Signature object
     * @param basResp OCSP Basic response
     * @throws DigiDocException if the response is not successfull
     */
    private void checkCertStatus(Signature sig, BasicOCSPResponse basResp)
        throws DigiDocException
    {
        checkCertStatus(sig.getKeyInfo().getSignersCertificate(), basResp);
    }
    
    /**
     * Verifies that the OCSP response is about our signers
     * cert and the response status is successfull
     * @param sig Signature object
     * @param basResp OCSP Basic response
     * @throws DigiDocException if the response is not successfull
     */
    private void checkCertStatus(X509Certificate cert, BasicOCSPResponse basResp)
        throws DigiDocException
    {
        try {
        	if(m_logger.isDebugEnabled())
            	m_logger.debug("Checking response status, CERT: " + cert.getSubjectDN().getName() + 
            		" SEARCH: " + SignedDoc.getCommonName(cert.getIssuerDN().getName()));
            // check the response on our cert
            X509Certificate caCert = (X509Certificate)m_ocspCACerts.
            	get(SignedDoc.getCommonName(cert.getIssuerDN().getName()));
            if(m_logger.isDebugEnabled())
            	m_logger.debug("CA cert: " + ((caCert == null) ? "NULL" : "OK"));
            // vale kuna valib responderi mitte serdi jargi
            if(m_logger.isDebugEnabled())
            	m_logger.debug("RESP: " + basResp);
            if(m_logger.isDebugEnabled()) {
            	m_logger.debug("CERT: " + cert.getSubjectDN().getName() + 
            				" ISSUER: " + cert.getIssuerDN().getName());
            	m_logger.debug("CA CERT: " + caCert.getSubjectDN().getName());
            }
            ReqCert rc = createReqCert(cert, caCert);            
            SingleResponse sresp = basResp.getSingleResponse(rc);
            int nStatus = 0;
            if(sresp != null) {
            	nStatus = sresp.getCertStatus().getCertStatus();
            	switch(nStatus) {
            		case CertStatus.GOOD: // cert is OK!
            			break;
            		case CertStatus.REVOKED:
            		    if(m_logger.isDebugEnabled())
            		      m_logger.debug("Certificate " + cert.getSerialNumber().toString() + " has been revoked!");
                 		throw new DigiDocException(DigiDocException.ERR_CERT_REVOKED,
                    		"Certificate has been revoked!", null);
            		case CertStatus.UNKNOWN:
            		    if(m_logger.isDebugEnabled())
            		      m_logger.debug("Certificate " + cert.getSerialNumber().toString() + " status is unknown!");
                 		throw new DigiDocException(DigiDocException.ERR_CERT_UNKNOWN,
                    		"Certificate status is unknown!", null);
                 }
            } else
            	throw new DigiDocException(DigiDocException.ERR_OCSP_RESP_STATUS,
                    "Error finding this certificates status!", null);
            //System.out.println("Response status OK!");
        } catch(DigiDocException ex) {
        	throw ex;
        } catch(Exception ex) {
            throw new DigiDocException(DigiDocException.ERR_OCSP_RESP_STATUS,
                    "Error checking OCSP response status!", null);
        }
    }

    /**
     * Check the response and parse it's data
     * @param not initial Notary object that contains only the
     * raw bytes of an OCSP response
     * @returns Notary object with data parsed from OCSP response
     */
    public Notary parseAndVerifyResponse(Signature sig, Notary not)
        throws DigiDocException
    {
        try {            
            OCSPResponse resp = new OCSPResponse(not.getOcspResponseData());
            // now read the info from the response
            BasicOCSPResponse basResp = 
                (BasicOCSPResponse)resp.getResponse(); 
            // verify the response
            try {
            	X509Certificate notaryCert = getNotaryCert(SignedDoc.getCommonName(basResp.getResponderID().toString()), null);
            	
                basResp.verify(notaryCert.getPublicKey()); 
            } catch (Exception ex) {
                //System.out.println("Signature verification error!!!"); 
                DigiDocException.handleException(ex, DigiDocException.ERR_OCSP_VERIFY);
            } 
            // done't care about SingleResponses because we have
            // only one response and the whole response was successfull
            // but we should verify that the nonce hasn't changed
            // calculate the nonce
            byte[] nonce1 = SignedDoc.digest(sig.getSignatureValue().getValue());
            byte[] nonce2 = basResp.getNonce();
            boolean ok = true;
            if(nonce1.length != nonce2.length)
                ok = false;
            for(int i = 0; i < nonce1.length; i++)
                if(nonce1[i] != nonce2[i])
                    ok = false;
            if(!ok)
                throw new DigiDocException(DigiDocException.ERR_OCSP_NONCE,
                    "OCSP response's nonce doesn't match the requests nonce!", null);
            // check the response on our cert
            checkCertStatus(sig, basResp);
            not.setProducedAt(basResp.getProducedAt());
            not.setResponderId(basResp.getResponderID().toString());
        } catch(DigiDocException ex) {
            throw ex;
        } catch(Exception ex) {
            DigiDocException.handleException(ex, DigiDocException.ERR_OCSP_PARSE);
        }
        return not;
    }
    
    /**
     * Creates an ReqCert Object for an OCSP request
     * @param signersCert signature owners cert
     * @param caCert CA cert for this signer
     * @returns ReqCert object
     */
    private ReqCert createReqCert(X509Certificate signersCert, X509Certificate caCert)
        throws DigiDocException 
    {
        ReqCert rc = null;
        try {
            AlgorithmID hashAlgorithm = AlgorithmID.sha1;
            iaik.x509.X509Certificate icaCert = 
                new iaik.x509.X509Certificate(caCert.getEncoded());
            iaik.x509.X509Certificate iSigCert = 
                new iaik.x509.X509Certificate(signersCert.getEncoded());
            CertID certID = new CertID(hashAlgorithm, icaCert, iSigCert);
            rc = new ReqCert(ReqCert.certID, certID);
        } catch(Exception ex) {
            DigiDocException.handleException(ex, DigiDocException.ERR_OCSP_REQ_CREATE);
        }
        return rc;
    }
    
    /**
     * Creates a new OCSP request
     * @param nonce 128 byte RSA+SHA1 signatures digest
     * Use null if you want to verify only the certificate
     * and this is not related to any signature
     * @param signersCert signature owners cert
     * @param caCert CA cert for this signer
     */
    private OCSPRequest createOCSPRequest(byte[] nonce, 
        X509Certificate signersCert, X509Certificate caCert)
        throws DigiDocException 
    {
        OCSPRequest ocspRequest = new OCSPRequest();
        try {
            ReqCert reqCert = createReqCert(signersCert, caCert);
            Request request1 = new Request(reqCert);
            Request[] requestList = { request1 };
            ocspRequest.setRequestList(requestList);
            if(nonce != null)
                ocspRequest.setNonce(nonce);
        } catch(Exception ex) {
            DigiDocException.handleException(ex, DigiDocException.ERR_OCSP_REQ_CREATE);
        }
        return ocspRequest;
    }
    /*
    private void debugWriteFile(String name, byte[] data)
    {
        try {
            String str = "C:\\veiko\\work\\sk\\JDigiDoc\\" + name;
            System.out.println("Writing debug file: " + str);
            FileOutputStream fos = new FileOutputStream(str);
            fos.write(data);
            fos.close();
        } catch(Exception ex) {
            System.out.println("Error: " + ex);
            ex.printStackTrace(System.out);
        }
    }
    */
    /**
     * Sends the OCSP request to Notary and
     * retrieves the response
     * @param req OCSP request
     * @returns OCSP response
     */
    private OCSPResponse sendRequest(OCSPRequest req)
        throws DigiDocException 
    {
        OCSPResponse resp = null;
        try {
            
            byte[] breq = req.getEncoded();
            //debugWriteFile("request-iaik.req", breq);
            String responderUrl = ConfigManager.instance().
                getProperty("DIGIDOC_OCSP_RESPONDER_URL");
            URL url = new URL(responderUrl);
            URLConnection con = url.openConnection();            
            con.setAllowUserInteraction(false);
            con.setUseCaches(false);
            con.setDoOutput(true);
            con.setDoInput(true);
            // send the OCSP request
            con.setRequestProperty("Content-Type", "application/ocsp-request");
            OutputStream os = con.getOutputStream();
            os.write(breq);
            os.close();
            // read the response
            InputStream is = con.getInputStream();
            int cl = con.getContentLength();
            byte[] bresp = null;
            //System.out.println("Content: " + cl + " bytes");
            if(cl > 0) {
                int avail = 0;
                do {
                    avail = is.available();
                    byte[] data = new byte[avail];
                    int rc = is.read(data);
                    if(bresp == null) {
                        bresp = new byte[rc];
                        System.arraycopy(data, 0, bresp, 0, rc);
                    } else {
                        byte[] tmp = new byte[bresp.length + rc];
                        System.arraycopy(bresp, 0, tmp, 0, bresp.length);
                        System.arraycopy(data, 0, tmp, bresp.length, rc);
                        bresp = tmp;
                    }
                    //System.out.println("Got: " + avail + "/" + rc + " bytes!");
                    cl -= rc;
                } while(cl > 0);
            }
            is.close();
            if(bresp != null) {
                resp = new OCSPResponse(bresp);     
                //System.out.println("Response: " + resp.toString());
            }
        } catch(Exception ex) {
            DigiDocException.handleException(ex, DigiDocException.ERR_OCSP_REQ_SEND);        
        }
        return resp;
    }
    
    /**
     * Signs the OCSP request if necessary
     * @param req not signed request
     * @returns signed request
     */
    private OCSPRequest signRequest(OCSPRequest req)
        throws DigiDocException
    {
        try {
            if(m_signCert != null && m_signKey != null) {
                iaik.x509.X509Certificate[] reqCerts = 
                   new iaik.x509.X509Certificate[1];
                reqCerts[0] = m_signCert;
                // the name of the requestor:
                GeneralName requestorName = 
                     new GeneralName(GeneralName.directoryName, 
                         reqCerts[0].getSubjectDN());
                req.setRequestorName(requestorName);
                req.setCertificates(reqCerts);
                // sign the request:
                //System.out.println("Signing OCSP request!");
                req.sign(AlgorithmID.sha1WithRSAEncryption, m_signKey);
            }   
        } catch(Exception ex) {
            DigiDocException.handleException(ex, DigiDocException.ERR_OCSP_SIGN);
        }
        return req;
    }
    
    /**
     * initializes the implementation class
     */
    public void init()
        throws DigiDocException 
    {
        try {
        	if(m_logger.isDebugEnabled())
            	m_logger.debug("INITING ");
            String proxyHost = ConfigManager.instance().
                getProperty("DIGIDOC_PROXY_HOST");
            String proxyPort = ConfigManager.instance().
                getProperty("DIGIDOC_PROXY_PORT");
            if(proxyHost != null && proxyPort != null) {
                System.setProperty("http.proxyHost", proxyHost);
                System.setProperty("http.proxyPort", proxyPort);
            }
            String sigFlag = ConfigManager.
                instance().getProperty("SIGN_OCSP_REQUESTS");
        	m_bSignRequests = (sigFlag != null && sigFlag.equals("true"));
        	// only need this if we must sign the requests
            Provider prv = (Provider)Class.forName(ConfigManager.
                instance().getProperty("DIGIDOC_SECURITY_PROVIDER")).newInstance();
            Security.addProvider(prv);
        	if(m_bSignRequests) {
            // we need iaik provider for PKCS#12 signing
            prv = (Provider)Class.forName(ConfigManager.
                instance().getProperty("IAIK_SECURITY_PROVIDER")).newInstance();
            Security.addProvider(prv);
            // load the cert & private key for OCSP signing
            String p12file = ConfigManager.instance().
                getProperty("DIGIDOC_PKCS12_CONTAINER");
            String p12paswd = ConfigManager.instance().
                getProperty("DIGIDOC_PKCS12_PASSWD");            
            // PKCS#12 container has 2 certs
            // so use this serial to find the necessary one
            String p12serial = ConfigManager.instance().
                getProperty("DIGIDOC_OCSP_SIGN_CERT_SERIAL");
            if(m_logger.isDebugEnabled())
            	m_logger.debug("Looking for cert: " + p12serial);
            
            if(m_logger.isDebugEnabled())
            	m_logger.debug("Reading PKCS12 file: " + p12file);
            if(p12file != null && p12paswd != null) {
                FileInputStream fi = new FileInputStream(p12file);
                KeyStore store = KeyStore.getInstance("PKCS12", "BC");
                store.load(fi, p12paswd.toCharArray());
                java.util.Enumeration en = store.aliases();
                // find the key alias
            	String      pName = null;
            	while(en.hasMoreElements()) {
                	String  n = (String)en.nextElement();
                	if (store.isKeyEntry(n)) {
                    	pName = n;
                	}
            	}
				m_signKey = (PrivateKey)store.getKey(pName, null);
				java.security.cert.Certificate[] certs = store.getCertificateChain(pName);
				for(int i = 0; (certs != null) && (i < certs.length); i++) {
					java.security.cert.X509Certificate cert = (java.security.cert.X509Certificate)certs[i];
					if(m_logger.isDebugEnabled()) {
                    	m_logger.debug("Cert " + i + " subject: " + cert.getSubjectDN());
                    	m_logger.debug("Cert " + i + " issuer: " + cert.getIssuerDN());                    
                    	m_logger.debug("Cert " + i + " serial: " + cert.getSerialNumber());
					}
                    if(cert.getSerialNumber().equals(new BigInteger(p12serial)))
                        m_signCert = new iaik.x509.X509Certificate(certs[i].getEncoded());
                }
                   
            }
        	}
            // OCSP certs
            int nCerts = ConfigManager.instance().
            	getIntProperty("DIGIDOC_OCSP_COUNT", 0);
            for(int i = 1; i <= nCerts; i++) {
            	String ocspCN = ConfigManager.instance().getProperty("DIGIDOC_OCSP" + i + "_CN");
            	String ocspCertFile = ConfigManager.instance().getProperty("DIGIDOC_OCSP" + i + "_CERT");
            	String ocspCAFile = ConfigManager.instance().getProperty("DIGIDOC_OCSP" + i + "_CA_CERT");
            	String ocspCACN = ConfigManager.instance().getProperty("DIGIDOC_OCSP" + i + "_CA_CN");
            	if(m_logger.isDebugEnabled())
                    	m_logger.debug("Responder: " + ocspCN + " cert: " + 
                    	ocspCertFile + " ca-cert: " + ocspCAFile);
            	m_ocspCerts.put(ocspCN, SignedDoc.readCertificate(new File(ocspCertFile)));
            	m_ocspCACerts.put(ocspCACN, SignedDoc.readCertificate(new File(ocspCAFile)));
            }
                     
        } catch(Exception ex) {
            DigiDocException.handleException(ex, DigiDocException.ERR_NOT_FAC_INIT);
        }
    }
 
        
}
