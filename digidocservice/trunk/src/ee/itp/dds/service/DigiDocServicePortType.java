/**
 * DigiDocServicePortType.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis 1.4 Apr 22, 2006 (06:55:48 PDT) WSDL2Java emitter.
 */

package ee.itp.dds.service;

import java.io.IOException;
import java.security.cert.CertificateException;

import javax.xml.rpc.holders.IntHolder;
import javax.xml.rpc.holders.StringHolder;

import org.jdom.JDOMException;

import ee.itp.dds.service.holders.DataFileDataHolder;
import ee.itp.dds.service.holders.SignatureModulesArrayHolder;
import ee.itp.dds.service.holders.SignedDocInfoHolder;
import ee.sk.digidoc.DigiDocException;

public interface DigiDocServicePortType extends java.rmi.Remote {

    /**
     * Service definition of function d__StartSession
     */
    public void startSession(String signingProfile, String sigDocXML, boolean bHoldSession, DataFileData datafile, StringHolder status, IntHolder sesscode, SignedDocInfoHolder signedDocInfo) throws IOException, ClassNotFoundException ,JDOMException;

    /**
     * Service definition of function d__CloseSession
     */
    public String closeSession(int sesscode) throws java.rmi.RemoteException;

    /**
     * Service definition of function d__CreateSignedDoc
     * @throws Exception 
     */
    public void createSignedDoc(int sesscode, String format, String version, StringHolder status, SignedDocInfoHolder signedDocInfo) throws Exception;

    /**
     * Service definition of function d__AddDataFile
     */
    public void addDataFile(int sesscode, String fileName, String mimeType, String contentType, int size, String digestType, String digestValue, String content, StringHolder status, SignedDocInfoHolder signedDocInfo) throws IOException, ClassNotFoundException;

    /**
     * Service definition of function d__RemoveDataFile
     */
    public void removeDataFile(int sesscode, String dataFileId, StringHolder status, SignedDocInfoHolder signedDocInfo) throws IOException, ClassNotFoundException;

    /**
     * Service definition of function d__GetSignedDoc
     */
    public void getSignedDoc(int sesscode, StringHolder status, StringHolder signedDocData) throws IOException, ClassNotFoundException , DigiDocException;

    /**
     * Service definition of function d__GetSignedDocInfo
     */
    public void getSignedDocInfo(int sesscode, StringHolder status, SignedDocInfoHolder signedDocInfo) throws IOException, ClassNotFoundException ,DigiDocException;

    /**
     * Service definition of function d__GetDataFile
     */
    public void getDataFile(int sesscode, String dataFileId, StringHolder status, DataFileDataHolder dataFileData) throws IOException, ClassNotFoundException;

    /**
     * Service definition of function d__GetSignersCertificate
     * @throws ClassNotFoundException 
     * @throws IOException 
     */
    public void getSignersCertificate(int sesscode, String signatureId, StringHolder status, StringHolder certificateData) throws java.rmi.RemoteException, IOException, ClassNotFoundException;

    /**
     * Service definition of function d__GetNotarysCertificate
     * @throws ClassNotFoundException 
     * @throws IOException 
     */
    public void getNotarysCertificate(int sesscode, String signatureId, StringHolder status, StringHolder certificateData) throws java.rmi.RemoteException, IOException, ClassNotFoundException;

    /**
     * Service definition of function d__GetNotary
     */
    public void getNotary(int sesscode, String signatureId, StringHolder status, StringHolder ocspData) throws IOException, ClassNotFoundException, DigiDocException;

    /**
     * Service definition of function d__GetTSACertificate
     */
    public void getTSACertificate(int sesscode, String timestampId, StringHolder status, StringHolder certificateData) throws java.rmi.RemoteException;

    /**
     * Service definition of function d__GetTimestamp
     */
    public void getTimestamp(int sesscode, String timestampId, StringHolder status, StringHolder timestampData) throws java.rmi.RemoteException;

    /**
     * Service definition of function d__GetCRL
     */
    public void getCRL(int sesscode, String signatureId, StringHolder status, StringHolder CRLData) throws java.rmi.RemoteException;

    /**
     * Service definition of function d__GetSignatureModules
     */
    public void getSignatureModules(int sesscode, String platform, String phase, String type, StringHolder status, SignatureModulesArrayHolder modules) throws java.rmi.RemoteException;

    /**
     * Service definition of function d__PrepareSignature
     */
    public void prepareSignature(int sesscode, String signersCertificate, String signersTokenId, String role, String city, String state, String postalCode, String country, String signingProfile, StringHolder status, StringHolder signatureId, StringHolder signedInfoDigest) throws CertificateException, IOException, ClassNotFoundException , DigiDocException;

    /**
     * Service definition of function d__FinalizeSignature
     */
    public void finalizeSignature(int sesscode, String signatureId, String signatureValue, StringHolder status, SignedDocInfoHolder signedDocInfo) throws IOException, ClassNotFoundException, DigiDocException;

    /**
     * Service definition of function d__RemoveSignature
     */
    public void removeSignature(int sesscode, String signatureId, StringHolder status, SignedDocInfoHolder signedDocInfo) throws IOException, ClassNotFoundException ,DigiDocException;

    /**
     * Service definition of function d__GetVersion
     */
    public void getVersion(StringHolder name, StringHolder version) throws java.rmi.RemoteException;

    /**
     * Service definition of function d__GetStatusInfo
     */
    public void getStatusInfo(int sesscode, boolean returnDocInfo, boolean waitSignature, StringHolder status, StringHolder statusCode, SignedDocInfoHolder signedDocInfo) throws IOException, ClassNotFoundException, DigiDocException;
}
