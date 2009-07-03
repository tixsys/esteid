/**
 * DigiDocServiceImpl.java
 *
 */

package ee.itp.dds.service;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;
import java.rmi.RemoteException;
import java.security.Security;
import java.security.cert.CertificateEncodingException;
import java.security.cert.CertificateException;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.util.ArrayList;
import java.util.List;

import javax.xml.rpc.holders.IntHolder;
import javax.xml.rpc.holders.StringHolder;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.jdom.JDOMException;
import org.springframework.context.ApplicationContext;
import org.springframework.context.support.ClassPathXmlApplicationContext;

import ee.itp.dds.core.Configuration;
import ee.itp.dds.core.ReceivedDataHolder;
import ee.itp.dds.core.SessionHolderComponent;
import ee.itp.dds.core.SignatureModuleDesc;
import ee.itp.dds.core.SignatureModuleDesc.Phase;
import ee.itp.dds.core.SignatureModuleDesc.Platform;
import ee.itp.dds.core.SignatureModuleDesc.Type;
import ee.itp.dds.hibernate.ActionEnum;
import ee.itp.dds.hibernate.Document;
import ee.itp.dds.hibernate.Session;
import ee.itp.dds.service.holders.DataFileDataHolder;
import ee.itp.dds.service.holders.SignedDocInfoHolder;
import ee.itp.dds.util.TransformUtil;
import ee.sk.digidoc.Base64Util;
import ee.sk.digidoc.CertID;
import ee.sk.digidoc.CertValue;
import ee.sk.digidoc.DataFile;
import ee.sk.digidoc.DigiDocException;
import ee.sk.digidoc.Signature;
import ee.sk.digidoc.SignatureProductionPlace;
import ee.sk.digidoc.SignedDoc;
import ee.sk.utils.ConfigManager;
import ee.sk.utils.ConvertUtils;

public class DigiDocServiceImpl implements DigiDocServicePortType, DigiDocServiceConstants {

    private static final Log log = LogFactory.getLog(DigiDocServiceImpl.class);
    private boolean debug = false;
    
    public static final String ERROR_CAUSE_CLIENT = "Client";
    public static final String ERROR_CAUSE_SENDER = "Sender";

    /** The ac. */
    private ApplicationContext ac;
    private SessionHolderComponent sessionHolder;

    /**
     * Instantiates a new service binding impl.
     */
    public DigiDocServiceImpl() {
        ac = new ClassPathXmlApplicationContext(ReceivedDataHolder.getContextConfigLocation());
        init();
    }

    /**
     * Instantiates a new service binding impl.
     */
    public DigiDocServiceImpl(ApplicationContext applicationContext) {
        ac = applicationContext;
        init();
    }

    /**
     * Inits the.
     */
    public void init() {
        sessionHolder = (SessionHolderComponent) ac.getBean("sessionHolderComponent");
        Security.addProvider(new org.bouncycastle.jce.provider.BouncyCastleProvider());
        Configuration cfg = (Configuration) ac.getBean("configuration");
        ConfigManager.init(cfg.getJDigiDocConfig());
        debug = cfg.isDebugMode(); 
    }

    public void startSession(String signingProfile, String sigDocXML, boolean bHoldSession, DataFileData datafile, StringHolder status, IntHolder sesscode, SignedDocInfoHolder signedDocInfo)
            throws IOException, ClassNotFoundException, JDOMException {

        status.value = ResponseStatus.OK.name();
        Session s = sessionHolder.createNewSession(ReceivedDataHolder.getHost());

        if (bHoldSession) {

            if (s == null || s.getCodeInt() == 0) {
                status.value = ResponseStatus.CANT_CREATE_NEW_SESSION.name();
                return;
            }
            log.info("new session created, code : " + s.getCode());

            sesscode.value = s.getCodeInt();

        }// if

        try {

            DataFileData actualDataFile = null;

            if (sigDocXML != null && !sigDocXML.equals("") && datafile != null) {
                status.value = ResponseStatus.INPUT_DATA_ERROR.name();
            } else if (sigDocXML != null && !sigDocXML.equals("")) {
                actualDataFile = TransformUtil.convertToDataFile(sigDocXML);
            } else if (datafile != null) {
                actualDataFile = datafile;
            }// if

            if (actualDataFile != null) {

                createSignedDoc(s.getCodeInt(), SignedDoc.FORMAT_DIGIDOC_XML, SignedDoc.VERSION_1_4, status, signedDocInfo);

                if (status.value != null && status.value.trim().equalsIgnoreCase(ResponseStatus.OK.name())) {

                    String inputfile = actualDataFile.getFilename();
                    String mime = actualDataFile.getMimeType();
                    String contentType = actualDataFile.getContentType();
                    int size = actualDataFile.getSize();
                    String digestType = actualDataFile.getDigestType();
                    String digestValue = actualDataFile.getDigestValue();
                    String content = actualDataFile.getDfData();

                    addDataFile(s.getCodeInt(), inputfile, mime, contentType, size, digestType, digestValue, content, status, signedDocInfo);

                }// if

            }// if

            if (!bHoldSession) {

                this.closeSession(s.getCodeInt());

            }// if

        } catch (RemoteException e1) {

            status.value = ResponseStatus.ERROR.name();
            log.error(this.getClass().getName() + "#startSession", e1);

        }// catch

    }// startSession

    public String closeSession(int sesscode) throws java.rmi.RemoteException {

        if (sesscode == 0) {
            return ResponseStatus.WRONG_SESSION_CODE.name();
        }
        boolean ret = sessionHolder.closeSession("" + sesscode);
        log.info((ret ? "session closed, code : " : "Session not found for closing :") + sesscode);
        if (!ret)
          throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Wrong session code" , debug);
        return ResponseStatus.OK.name();
    }

    /**
     * sesscode not null
     * 
     * @throws IOException
     */
    public void createSignedDoc(int sesscode, String format, String version, StringHolder status, SignedDocInfoHolder signedDocInfo) throws RemoteException , IOException {

        Session session = sessionHolder.loadSession("" + sesscode);
        if (session == null) 
          throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Wrong session code" , debug);

        status.value = ResponseStatus.OK.name();

        if (format != null && !format.trim().equals("") && version != null && !version.trim().equals("")) {

            try {

                Document SDBean = new Document();
                SignedDoc digiDoc = new SignedDoc(format, version);
                SDBean.setSignedDoc(digiDoc);
                session.setDocument(SDBean);
                
                sessionHolder.saveSessionWithAction(session, ActionEnum.CREATE_SIGNED_DOC);

            } catch (DigiDocException e) {

                log.error(this.getClass().getName() + "#createSignedDoc", e);
                throw new RemoteException("Invalid format & version combination!"); //fix to compotable with old version
                
            }// catch

        }// if
        else {
            log.error(this.getClass().getName() + "#createSignedDoc " + ResponseStatus.INPUT_DATA_ERROR.name());
            throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Invalid format & version combination!" , debug);//fix to compotable with old DDS version
        }

        signedDocInfo.value = new ee.itp.dds.service.SignedDocInfo();
        signedDocInfo.value.setFormat(format);
        signedDocInfo.value.setVersion(version);

    }// createSignedDoc

    /**
     * Adding DataFile to SignedDoc
     * 
     * @throws ClassNotFoundException
     * @throws IOException
     */
    public void addDataFile(int sesscode, String fileName, String mimeType, String contentType, int size, String digestType, String digestValue, String content, StringHolder status,
            SignedDocInfoHolder signedDocInfo) throws IOException, ClassNotFoundException {

        status.value = ResponseStatus.OK.name();

        Session session = sessionHolder.loadSession("" + sesscode);
        if (session == null) {
          throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Wrong session code" , debug);
        }

        if (fileName != null) {

            Document sdocBean = session.getDocument();

            if (sdocBean != null && sdocBean.getSignedDoc() != null) {

                try {

                    SignedDoc sdoc = sdocBean.getSignedDoc();

                    if (contentType != null) {

                        if (contentType.equals(DataFile.CONTENT_DETATCHED) || contentType.equals("HASHCODE")) {

                            DataFile df = sdoc.addDataFile(new File(fileName), mimeType, DataFile.CONTENT_DETATCHED);

                            if (digestType != null && digestType.equals(DataFile.DIGEST_TYPE_SHA1)) {

                                df.setDigestType(DataFile.DIGEST_TYPE_SHA1);
                                df.setDigestValue(Base64Util.decode(digestValue));
                                df.setSize(size);

                            } else {

                                log.error(this.getClass().getName() + "#addDataFile " + ResponseStatus.INPUT_DATA_ERROR.name() + " Unsupportrd HASCHCODE format :" + digestType);
                                throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , " Unsupportrd HASCHCODE format :" + digestType , debug);

                            }// else

                        } else if (contentType.equals(DataFile.CONTENT_EMBEDDED_BASE64)) {

                            DataFile df = sdoc.addDataFile(new File(fileName), mimeType, DataFile.CONTENT_EMBEDDED_BASE64);
                            byte[] u8b = ConvertUtils.str2data(content);
                            df.setBody(u8b, "UTF8");

                        } else if (contentType.equals(DataFile.CONTENT_EMBEDDED)) {

                            DataFile df = new DataFile(sdoc.getNewDataFileId(), contentType, fileName, mimeType, sdoc);
                            df.setContentType(DataFile.CONTENT_EMBEDDED);
                            sdoc.addDataFile(df);
                            byte[] u8b = ConvertUtils.str2data(TransformUtil.unEscapeXml(content));
                            df.setBody(u8b, "UTF8");

                        } else {

                            log.error(this.getClass().getName() + "#addDataFile " + ResponseStatus.INPUT_DATA_ERROR.name() + " Unsupportrd content type :" + contentType);
                            throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Unsupportrd content type :" + contentType , debug);

                        }// else

                    } else {

                        log.error(this.getClass().getName() + "#addDataFile " + ResponseStatus.INPUT_DATA_ERROR.name() + " Content type is NULL");
                        throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Content type is NULL" , debug);

                    }// else

                    sdocBean.setSignedDoc(sdoc);
                    session.setDocument(sdocBean);
                    sessionHolder.saveSessionWithAction(session, ActionEnum.ADD_DATA_FILE);
                    getSignedDocInfo(sesscode, status, signedDocInfo);

                } catch (DigiDocException e) {
                  log.error(this.getClass().getName() + "#addDataFile " + ResponseStatus.ERROR.name() + " message : " + e.getMessage());
                  throw new DigiDocServiceException(ServiceErrorCodes.GENERAL_ERROR, ERROR_CAUSE_CLIENT , "General error" , debug);

                }// catch

            }// if sdocBean
            else {
              log.error(this.getClass().getName() + "#addDataFile " + ResponseStatus.ERROR.name());
              throw new DigiDocServiceException(ServiceErrorCodes.GENERAL_ERROR, ERROR_CAUSE_CLIENT , "General error" , debug);
            }// else

        } else {

            status.value = ResponseStatus.INPUT_DATA_ERROR.name();
            log.error(this.getClass().getName() + "#addDataFile " + ResponseStatus.INPUT_DATA_ERROR.name());
            throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Input filae name" , debug);

        }// else

    }// addDataFile

    public void removeDataFile(int sesscode, String dataFileId, StringHolder status, SignedDocInfoHolder signedDocInfo) throws IOException, ClassNotFoundException {
        Session session = sessionHolder.loadSession("" + sesscode);
        if (session == null) {
            status.value = ResponseStatus.WRONG_SESSION_CODE.name();
            throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Wrong session code" , debug);
        }

        status.value = ResponseStatus.OK.name();

        if (signedDocInfo != null && dataFileId != null) {

            signedDocInfo.value = new SignedDocInfo();
            Document sdocBean = session.getDocument();

            if (sdocBean != null && sdocBean.getSignedDoc() != null) {

                try {

                    SignedDoc sdoc = sdocBean.getSignedDoc();

                    for (int i = 0; sdoc.countDataFiles() >= i; i++) {

                        DataFile theDFile = sdoc.getDataFile(i);
                        if (theDFile.getId().trim().equalsIgnoreCase(dataFileId.trim())) {

                            sdoc.removeDataFile(i);
                            break;

                        }// if

                    }// for

                    sdocBean.setSignedDoc(sdoc);
                    session.setDocument(sdocBean);
                    sessionHolder.saveSessionWithAction(session, ActionEnum.REMOVE_DATA_FILE);
                    // fills signedDocInfo
                    getSignedDocInfo(sesscode, status, signedDocInfo);

                } catch (DigiDocException e) {
                  
                  log.error(this.getClass().getName() + "#removeDataFile " + ResponseStatus.ERROR.name() + " message : " + e.getMessage());
                  status.value = ResponseStatus.ERROR.name();
                  throw new DigiDocServiceException(ServiceErrorCodes.GENERAL_ERROR, ERROR_CAUSE_CLIENT , "General error" , debug);
                    
                }// catch

            }// if sdocBean
            else {

                status.value = ResponseStatus.ERROR.name();
                log.error(this.getClass().getName() + "#removeDataFile " + ResponseStatus.ERROR.name());
                throw new DigiDocServiceException(ServiceErrorCodes.GENERAL_ERROR, ERROR_CAUSE_CLIENT , "General error" , debug);

            }// else

        } else {

            status.value = ResponseStatus.INPUT_DATA_ERROR.name();
            log.error(this.getClass().getName() + "#removeDataFile " + ResponseStatus.INPUT_DATA_ERROR.name());
            throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Input file id" , debug);          

        }// else

    }// removeDataFile

    
    
    public void getSignedDoc(int sesscode, StringHolder status, StringHolder signedDocData) throws RemoteException, IOException, ClassNotFoundException, DigiDocException {

        Session session = sessionHolder.loadSession("" + sesscode);
        if (session == null) {
            status.value = ResponseStatus.WRONG_SESSION_CODE.name();
            throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Wrong session code" , debug);
        }

        status.value = ResponseStatus.OK.name();

        Document sdocBean = session.getDocument();
        SignedDoc sdoc = sdocBean.getSignedDoc();
        sessionHolder.saveSessionWithAction(session, ActionEnum.GET_SIGNED_DOC);
        
        signedDocData.value = sdoc.toXML();

    }// getSignedDoc

    public void getSignedDocInfo(int sesscode, StringHolder status, SignedDocInfoHolder signedDocInfo) throws IOException, ClassNotFoundException, DigiDocException {

        Session session = sessionHolder.loadSession("" + sesscode);
        if (session == null) {
            status.value = ResponseStatus.WRONG_SESSION_CODE.name();
            throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Wrong session code" , debug);
        }

        status.value = ResponseStatus.OK.name();
        signedDocInfo.value = new SignedDocInfo();

        Document sdocBean = session.getDocument();
        SignedDoc sdoc = sdocBean.getSignedDoc();

        ArrayList<DataFileInfo> dataFileInfoList = new ArrayList<DataFileInfo>();

        for (int i = 0; sdoc.countDataFiles() > i; i++) {

            DataFile theDFile = sdoc.getDataFile(i);
            DataFileInfo dtaFileInfo = TransformUtil.createFileInfo(theDFile);
            dataFileInfoList.add(dtaFileInfo);

        }// for

        ArrayList<SignatureInfo> signatureInfoList = new ArrayList<SignatureInfo>();

        for (int j = 0; sdoc.countSignatures() > j; j++) {

            Signature theSig = sdoc.getSignature(j);
            ArrayList<DigiDocException> errorList = sdoc.validate(false);
            SignatureInfo theSigInfo = TransformUtil.createSignatureInfo(theSig , errorList);
            signatureInfoList.add(theSigInfo);

        }// for

        sessionHolder.saveSessionWithAction(session, ActionEnum.GET_SIGNED_DOC_INFO);
        
        signedDocInfo.value = new SignedDocInfo(sdoc.getFormat(), sdoc.getVersion(), dataFileInfoList.toArray(new DataFileInfo[dataFileInfoList.size()]), signatureInfoList
                .toArray(new SignatureInfo[signatureInfoList.size()]));

    }// getSignedDocInfo

    public void getDataFile(int sesscode, String dataFileId, StringHolder status, DataFileDataHolder dataFileData) throws IOException, ClassNotFoundException {

        Session session = sessionHolder.loadSession("" + sesscode);
        if (session == null) {
            status.value = ResponseStatus.WRONG_SESSION_CODE.name();
            throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Wrong session code" , debug);
        }

        status.value = ResponseStatus.OK.name();
        dataFileData.value = null;

        if (dataFileId != null) {

            Document sdocBean = session.getDocument();

            if (sdocBean != null && sdocBean.getSignedDoc() != null) {

                try {

                    SignedDoc sdoc = sdocBean.getSignedDoc();

                    boolean found = false;
                    
                    for (int i = 0; sdoc.countDataFiles() > i; i++) {

                        DataFile theDFile = sdoc.getDataFile(i);
                        if (theDFile.getId().trim().equalsIgnoreCase(dataFileId.trim())) {

                            dataFileData.value = new DataFileData();
                            dataFileData.value = TransformUtil.createDataFile(theDFile);
                            found = true;
                            break;

                        }// if

                    }// for
                    
                    if(!found){
                      throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_CLIENT , "No such DataFile" , debug);
                    }//if

                } catch (DigiDocException e) {
                    status.value = ResponseStatus.ERROR.name();
                    log.error(this.getClass().getName() + "#getDataFile " + ResponseStatus.ERROR.name() + " message : " + e.getMessage());
                    throw new DigiDocServiceException(ServiceErrorCodes.GENERAL_ERROR, ERROR_CAUSE_CLIENT , "General error" , debug);

                }// catch

            }// if sdocBean
            else {

                status.value = ResponseStatus.ERROR.name();
                log.error(this.getClass().getName() + "#getDataFile " + ResponseStatus.ERROR.name());
                throw new DigiDocServiceException(ServiceErrorCodes.GENERAL_ERROR, ERROR_CAUSE_CLIENT , "General error" , debug);

            }// else

        } else {

            status.value = ResponseStatus.INPUT_DATA_ERROR.name();
            log.error(this.getClass().getName() + "#getDataFile " + ResponseStatus.INPUT_DATA_ERROR.name());
            throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Input file id" , debug);
            
        }// else
        
        sessionHolder.saveSessionWithAction(session, ActionEnum.GET_DATA_FILE);
        
    }

    public void getSignersCertificate(int sesscode, String signatureId, StringHolder status, StringHolder certificateData) throws IOException, ClassNotFoundException {

        Session session = sessionHolder.loadSession("" + sesscode);
        if (session == null) {
            status.value = ResponseStatus.WRONG_SESSION_CODE.name();
            throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Wrong session code" , debug);
        }
        sessionHolder.saveSessionWithAction(session, ActionEnum.GET_SIGNERS_CERTIFICATE);
        
        status.value = ResponseStatus.OK.name();

        if (signatureId != null) {

            Document sdocBean = session.getDocument();
            SignedDoc sdoc = sdocBean.getSignedDoc();

            for (int j = 0; sdoc.countSignatures() > j; j++) {
                Signature theSig = sdoc.getSignature(j);
                if (signatureId.trim().equals(theSig.getId())) {
                    CertValue cv = theSig.getCertValueOfType(CertID.CERTID_TYPE_SIGNER);
                    try {
                        certificateData.value = Base64Util.encode(cv.getCert().getEncoded());
                    } catch (CertificateEncodingException e) {
                        status.value = ResponseStatus.CANT_READ_SIGNER_CERT.name();
                        throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Must supply Signature id!" , debug);
                    };
                    break;
                }// if
            }// for
            
            
        } else {
            status.value = ResponseStatus.NO_SIGNATURE_ID.name();
            throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Input signature id" , debug);
        }

    }

    public void getNotarysCertificate(int sesscode, String signatureId, StringHolder status, StringHolder certificateData) throws IOException, ClassNotFoundException {
        Session session = sessionHolder.loadSession("" + sesscode);
        if (session == null) {
            status.value = ResponseStatus.WRONG_SESSION_CODE.name();
            throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Wrong session code" , debug);
        }
        sessionHolder.saveSessionWithAction(session, ActionEnum.GET_NOTARYS_CERTIFICATE);
        
        status.value = ResponseStatus.OK.name();

        if (signatureId != null) {

            Document sdocBean = session.getDocument();
            SignedDoc sdoc = sdocBean.getSignedDoc();

            for (int j = 0; sdoc.countSignatures() > j; j++) {
                Signature theSig = sdoc.getSignature(j);
                if (signatureId.trim().equals(theSig.getId())) {
                    CertValue cv = theSig.getCertValueOfType(CertID.CERTID_TYPE_RESPONDER);
                    try {
                        certificateData.value = Base64Util.encode(cv.getCert().getEncoded());
                    } catch (CertificateEncodingException e) {
                        status.value = ResponseStatus.CANT_READ_NOTARYS_CERT.name();
                        throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "No notary for this Signature!" , debug);
                    };
                    break;
                }// if
            }// for
            
        } else {
            status.value = ResponseStatus.NO_SIGNATURE_ID.name();
            throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "No such Signature!" , debug);
        }
    }

    public void getNotary(int sesscode, String signatureId, StringHolder status, StringHolder ocspData) throws IOException, ClassNotFoundException, DigiDocException {
        Session session = sessionHolder.loadSession("" + sesscode);
        if (session == null) {
            status.value = ResponseStatus.WRONG_SESSION_CODE.name();
            throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Wrong session code" , debug);
        }

        status.value = ResponseStatus.OK.name();

        if (signatureId != null) {

            Document sdocBean = session.getDocument();
            SignedDoc sdoc = sdocBean.getSignedDoc();
            
            for (int j = 0; sdoc.countSignatures() > j; j++) {

                Signature theSig = sdoc.getSignature(j);

                if (signatureId.trim().equals(theSig.getId())) {

                    // theSig.getConfirmation(); ???
                    byte[] validConfirmation = theSig.getUnsignedProperties().getNotary().getOcspResponseData();
                    ocspData.value = Base64Util.encode(validConfirmation);
                    sessionHolder.saveSessionWithAction(session, ActionEnum.GET_NOTARY);
                    return;
                }// if

            }// for
            status.value = ResponseStatus.WRONG_SIGNATURE_ID.name();
            throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "No notary for this Signature!" , debug);
        } else  { 
            status.value = ResponseStatus.NO_SIGNATURE_ID.name();
            throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "No such Signature!" , debug);
        }

    }// getNotary

    public void getTSACertificate(int sesscode, String timestampId, StringHolder status, StringHolder certificateData) throws java.rmi.RemoteException {
        Session session = sessionHolder.loadSession("" + sesscode);
        if (session == null) {
            status.value = ResponseStatus.WRONG_SESSION_CODE.name();
            throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Wrong session code" , debug);
        }

        status.value = ResponseStatus.OK.name();
        certificateData.value = new String();
        // see documentation 7.19 Not Implemented
    }

    public void getTimestamp(int sesscode, String timestampId, StringHolder status, StringHolder timestampData) throws java.rmi.RemoteException {
        Session session = sessionHolder.loadSession("" + sesscode);
        if (session == null) {
            status.value = ResponseStatus.WRONG_SESSION_CODE.name();
            throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Wrong session code" , debug);
        }

        status.value = ResponseStatus.OK.name();
        timestampData.value = new String();
        // see documentation 7.20 Not Implemented
    }

    public void getCRL(int sesscode, String signatureId, StringHolder status, StringHolder CRLData) throws java.rmi.RemoteException {
        Session session = sessionHolder.loadSession("" + sesscode);
        if (session == null) {
            status.value = ResponseStatus.WRONG_SESSION_CODE.name();
            throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Wrong session code" , debug);
        }

        status.value = ResponseStatus.OK.name();
        CRLData.value = new String();
        // see documentation 7.21 Not Implemented
    }

    public void getSignatureModules(int sesscode, String platform, String phase, String type, StringHolder status, ee.itp.dds.service.holders.SignatureModulesArrayHolder modules)
            throws java.rmi.RemoteException {
        Session session = sessionHolder.loadSession("" + sesscode);
        if (session == null) {
            status.value = ResponseStatus.WRONG_SESSION_CODE.name();
            throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Wrong session code" , debug);
        }
        Platform _platform = Platform.valueOf(platform);
        if (_platform == null) {
            status.value = ResponseStatus.SIGNATURE_MODULE_WRONG_PLANFORM.name();
            return;
        }

        Phase _phase = Phase.valueOf(phase);
        if (_phase == null) {
            status.value = ResponseStatus.SIGNATURE_MODULE_WRONG_PHASE.name();
            return;
        }

        Type _type = Type.valueOf(type);
        if (_type == null) {
            status.value = ResponseStatus.SIGNATURE_MODULE_WRONG_TYPE.name();
            return;
        }

        List<SignatureModule> ret = new ArrayList<SignatureModule>();
        Configuration config = (Configuration) ac.getBean("configuration");
        List<SignatureModuleDesc> smdList = config.findSignatureModules(_platform, _phase, _type);
        for (SignatureModuleDesc smd : smdList) {
            SignatureModule sm = TransformUtil.toSignatureModule(smd, config.getModulesPath());
            ret.add(sm);
        }

        status.value = ResponseStatus.OK.name();
        modules.value = new  SignatureModule[ret.size()];
        ret.toArray(modules.value);
    }

    public void prepareSignature(int sesscode, String signersCertificate, String signersTokenId, String role, String city, String state, String postalCode, String country, String signingProfile,
            StringHolder status, StringHolder signatureId, StringHolder signedInfoDigest) throws CertificateException, IOException, ClassNotFoundException, DigiDocException {

        Session session = sessionHolder.loadSession("" + sesscode);
        if (session == null) {
            status.value = ResponseStatus.WRONG_SESSION_CODE.name();
            throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Wrong session code" , debug);
        }

        Document sdocBean = session.getDocument();
        SignedDoc sdoc = sdocBean.getSignedDoc();

        CertificateFactory certFac = CertificateFactory.getInstance("X.509");
        X509Certificate cert = (X509Certificate) certFac.generateCertificate(new ByteArrayInputStream(SignedDoc.hex2bin(signersCertificate)));

        SignatureProductionPlace address = new SignatureProductionPlace(city, state, country, postalCode);

        Signature sig = sdoc.prepareSignature(cert, new String[] { role }, address);

        signatureId.value = sig.getId();

        byte[] sidigest = sig.calculateSignedInfoDigest();

        signedInfoDigest.value = SignedDoc.bin2hex(sidigest);
        status.value = ResponseStatus.OK.name();

        sdocBean.setSignedDoc(sdoc);
        session.setDocument(sdocBean);
        sessionHolder.saveSession(session);

    }// prepareSignature

    public void finalizeSignature(int sesscode, String signatureId, String signatureValue, StringHolder status, SignedDocInfoHolder signedDocInfo) throws IOException, ClassNotFoundException,
            DigiDocException {
        Session session = sessionHolder.loadSession("" + sesscode);
        if (session == null) {
            status.value = ResponseStatus.WRONG_SESSION_CODE.name();
            throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Wrong session code" , debug);
        }

        status.value = ResponseStatus.OK.name();

        if (signatureId != null) {

            Document sdocBean = session.getDocument();
            SignedDoc sdoc = sdocBean.getSignedDoc();

            for (int j = 0; sdoc.countSignatures() > j; j++) {

                Signature theSig = sdoc.getSignature(j);

                if (signatureId.trim().equals(theSig.getId())) {

                    theSig.setSignatureValue(SignedDoc.hex2bin(signatureValue));

                    theSig.getConfirmation();
                    sdocBean.setSignedDoc(sdoc);
                    session.setDocument(sdocBean);
                    sessionHolder.saveSession(session);
                    break;

                }// if

            }// for

            // fills signedDocInfo
            getSignedDocInfo(sesscode, status, signedDocInfo);

        }// if

    }// finalizeSignature

    public void removeSignature(int sesscode, String signatureId, StringHolder status, SignedDocInfoHolder signedDocInfo) throws IOException {
        Session session = sessionHolder.loadSession("" + sesscode);
        if (session == null) {
            status.value = ResponseStatus.WRONG_SESSION_CODE.name();
            throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Wrong session code" , debug);
        }

        status.value = ResponseStatus.OK.name();
        signedDocInfo.value = new ee.itp.dds.service.SignedDocInfo();

        if (signatureId != null) {

            Document sdocBean = session.getDocument();
            SignedDoc sdoc = null;
            try {
              sdoc = sdocBean.getSignedDoc();
            } catch (ClassNotFoundException e) {
              throw new RemoteException(e.getMessage());
            }

            for (int j = 0; sdoc.countSignatures() > j; j++) {

                Signature theSig = sdoc.getSignature(j);

                if (signatureId.trim().equals(theSig.getId())) {

                    sdoc.removeSignature(j);
                    try {
                      sdocBean.setSignedDoc(sdoc);
                    } catch (DigiDocException e) {
                      throw new RemoteException(e.getMessage());
                    }
                    session.setDocument(sdocBean);
                    sessionHolder.saveSession(session);
                    // sessionHolder.saveDocument(sesscode, sdocBean);
                    break;

                }// if

            }// for

            // fills signedDocInfo
            try {
              getSignedDocInfo(sesscode, status, signedDocInfo);
            } catch (ClassNotFoundException e) {
              throw new RemoteException(e.getMessage());            
            } catch (DigiDocException e) {
              throw new RemoteException(e.getMessage());            
            }

        }// if

    }// removeSignature

    public void getVersion(StringHolder name, StringHolder version) throws RemoteException {
        name.value = SERVICE_NAME;
        version.value = SERVICE_VERSION;
    }

    public void getStatusInfo(int sesscode, boolean returnDocInfo, boolean waitSignature, StringHolder status, StringHolder statusCode, SignedDocInfoHolder signedDocInfo) throws IOException,
            ClassNotFoundException, DigiDocException {
        Session session = sessionHolder.loadSession("" + sesscode);
        if (session == null) {
            status.value = ResponseStatus.WRONG_SESSION_CODE.name();
            throw new DigiDocServiceException(ServiceErrorCodes.INCORRECT_INPUT_PARAMETERS, ERROR_CAUSE_SENDER , "Wrong session code" , debug);
        }

        // NOT USED status.value = StatusInfo.PHONE_ABSENT.name();// TODO ??? 7.6 in documentation
        statusCode.value = new String();// TODO ??? 7.6 in documentation

        Document sdocBean = session.getDocument();
        SignedDoc sdoc = sdocBean.getSignedDoc();

        // TODO IMPLEMENT !!!

        if (returnDocInfo) {

            // fills signedDocInfo
            getSignedDocInfo(sesscode, status, signedDocInfo);

        }// if returnDocInfo

    }// getStatusInfo

}
