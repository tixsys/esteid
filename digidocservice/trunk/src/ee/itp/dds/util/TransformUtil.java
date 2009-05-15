package ee.itp.dds.util;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collection;
import java.util.GregorianCalendar;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.jdom.Document;
import org.jdom.Element;
import org.jdom.JDOMException;
import org.jdom.input.SAXBuilder;

import ee.itp.dds.core.SignatureModuleDesc;
import ee.itp.dds.service.CertificateInfo;
import ee.itp.dds.service.CertificatePolicy;
import ee.itp.dds.service.ConfirmationInfo;
import ee.itp.dds.service.DataFileAttribute;
import ee.itp.dds.service.DataFileData;
import ee.itp.dds.service.DataFileInfo;
import ee.itp.dds.service.Error;
import ee.itp.dds.service.SignatureInfo;
import ee.itp.dds.service.SignatureModule;
import ee.itp.dds.service.SignatureProductionPlace;
import ee.itp.dds.service.SignerInfo;
import ee.itp.dds.service.SignerRole;
import ee.itp.dds.service.TstInfo;
import ee.sk.digidoc.Base64Util;
import ee.sk.digidoc.CertID;
import ee.sk.digidoc.CertValue;
import ee.sk.digidoc.DataFile;
import ee.sk.digidoc.DigiDocException;
import ee.sk.digidoc.Signature;
import ee.sk.digidoc.TimestampInfo;


public class TransformUtil implements Constants{
  
    private static final Log log = LogFactory.getLog(TransformUtil.class);
  
    public static DataFileInfo createFileInfo(DataFile file) throws DigiDocException{

        DataFileInfo ret = new DataFileInfo();
        ret.setId(file.getId());
        ret.setFilename(resolveFileName(file.getFileName()));
        ret.setMimeType(file.getMimeType());
        ret.setContentType(file.getContentType());
        
        if(file.getBody() != null ){
          
          ret.setDigestType(ee.sk.digidoc.DataFile.DIGEST_TYPE_SHA1);
          ret.setDigestValue(Base64Util.encode(SecureUtils.hashCode(file.getBody())));
          ret.setSize((int)file.getBody().length);
          
        }else if(file.getDigestValue() != null){
          
          ret.setDigestType(file.getDigestType());
          ret.setDigestValue(Base64Util.encode(file.getDigestValue()));
          ret.setSize((int)file.getSize());//TODO cast long to int !!!!
          
         }//else if

        ArrayList<DataFileAttribute> retAttrList = new ArrayList<DataFileAttribute>();
        
        for (int i =0; file.countAttributes() > i ; i++) {
          
          ee.sk.digidoc.DataFileAttribute fileArt = file.getAttribute(i);
          DataFileAttribute retAttr = createAttribute(fileArt);
          retAttrList.add(retAttr);
          
        }//for
        
        ret.setAttributes(retAttrList.toArray(new DataFileAttribute[retAttrList.size()]));
        return ret;
      }//createFileInfo
    
    
      
      
      
      public static DataFileAttribute createAttribute(ee.sk.digidoc.DataFileAttribute fileArt){
        
        DataFileAttribute ret = new DataFileAttribute();
        ret.setName(fileArt.getName());
        ret.setValue(fileArt.getValue());
        return ret;
      }//createAttribute
      
      
      public static DataFileData createDataFile(DataFile file) throws DigiDocException{
        
        DataFileData ret = new DataFileData();
        
        ret.setId(file.getId());
        ret.setFilename(resolveFileName(file.getFileName()));
        ret.setMimeType(file.getMimeType());
        ret.setContentType(file.getContentType());

        if(file.getBody() != null){
          
          ret.setDfData(new String(file.getBody()));
          ret.setDigestType(ee.sk.digidoc.DataFile.DIGEST_TYPE_SHA1);
          ret.setSize((int)file.getBody().length);
          ret.setDigestValue(Base64Util.encode(SecureUtils.hashCode(file.getBody())));

        }else if(file.getDigestValue() != null ){
          
          ret.setDigestType(file.getDigestType());
          ret.setDigestValue(Base64Util.encode(file.getDigestValue()));
          ret.setSize((int)file.getSize());//TODO cast long to int !!!!
        }
        
        return ret; 
      }//createDataFile
      
      
      
      
      public static SignatureInfo createSignatureInfo(Signature sig, ArrayList<DigiDocException> errorList){
        SignatureInfo ret = new SignatureInfo();
        ret.setId(sig.getId());
        
        if(errorList != null && errorList.size() > 0){
          
          ret.setStatus(SignatureInfoStatus.ERROR.name());
          
          DigiDocException exception = errorList.get(0);
          Error err = new Error();
          err.setCode(exception.getCode());
          err.setCategory(SignatureInfoErrorCategory.TEHNICAL.name());//TODO
          err.setDescription(exception.getMessage());
          ret.setError(err);

        }else{
          
          ret.setStatus(SignatureInfoStatus.OK.name());
          
        }//if

        {
          if(sig.getLastTimestampInfo() != null && sig.getLastTimestampInfo().getTime() != null){
            Calendar time = new GregorianCalendar();
            time.setTime(sig.getLastTimestampInfo().getTime());
            ret.setSigningTime(time); //TODO hope it is right time
          }//if
        }
        if(sig.getSignedProperties() != null){
          ArrayList<SignerRole> sRoleList = new ArrayList<SignerRole>();
          for (int i = 0; sig.getSignedProperties().countClaimedRoles() > i; i++) {
            String inputRole = sig.getSignedProperties().getClaimedRole(i);
            SignerRole outputRole = new SignerRole();
            outputRole.setCertified(0);//TODO
            outputRole.setRole(inputRole);
            sRoleList.add(outputRole);
          }// for
          ret.setSignerRole(sRoleList.toArray(new SignerRole[sRoleList.size()]));
        }
        
        if(sig.getSignedProperties() != null && sig.getSignedProperties().getSignatureProductionPlace() != null){
          ret.setSignatureProductionPlace(convertToSignatureProductionPlace(sig.getSignedProperties().getSignatureProductionPlace()));
        }//if
        {
          SignerInfo sInfo = new SignerInfo();
          
          CertValue certval = sig.getCertValueOfType(CertValue.CERTVAL_TYPE_SIGNER);
          CertID cert = null;
          
          for (int i = 0; sig.countCertIDs() > i; i++) {
            cert = sig.getCertID(i);
            if(cert.getType() == CertID.CERTID_TYPE_SIGNER){
              break;
            }//if
            else{
              cert = null;
            }//else
          }// for 
          
          //sInfo.setCommonName(commonName);TODO
          //sInfo.setIDCode(IDCode);TODO
          sInfo.setCertificate(convertToCertificateInfo(certval, cert));
          ret.setSigner(sInfo);
        }
        {
          ConfirmationInfo confirm = new ConfirmationInfo();
          
          CertValue certval = sig.getCertValueOfType(CertValue.CERTVAL_TYPE_RESPONDER);
          CertID cert = null;
          
          for (int i = 0; sig.countCertIDs() > i; i++) {
            cert = sig.getCertID(i);
            if(cert.getType() == CertID.CERTID_TYPE_RESPONDER){
              break;
            }//if
            else{
              cert = null;
            }
          }// for
          
          //confirm.setResponderID(responderID);TODO
          //confirm.setProducedAt(producedAt);TODO
          confirm.setResponderCertificate(convertToCertificateInfo(certval, cert));
          ret.setConfirmation(confirm);
        }

        {//In current version not supported
          ArrayList<TstInfo> tsList = new ArrayList<TstInfo>();
          for (int i = 0; sig.countTimestampInfos() > i; i++) {
            TimestampInfo tsInfo = sig.getTimestampInfo(i);
            TstInfo retTSInfo = convertToTstInfo(tsInfo);
            tsList.add(retTSInfo);
          }// for
          ret.setTimestamps(tsList.toArray(new TstInfo[tsList.size()]));
        }
        //ret.setCRLInfo(CRLInfo) CRL = Certificate revocation list, In current version not supported
        return ret;
      }//createSignatureInfo
      
      
      
      
      public static SignatureProductionPlace convertToSignatureProductionPlace(ee.sk.digidoc.SignatureProductionPlace input){
        SignatureProductionPlace ret = new SignatureProductionPlace();
        ret.setCity(input.getCity());
        ret.setStateOrProvince(input.getStateOrProvince());
        ret.setPostalCode(input.getPostalCode());
        ret.setCountryName(input.getCountryName());
        return ret;
      }//convertToSignatureProductionPlace
      
      
      
      
      public static TstInfo convertToTstInfo(TimestampInfo input){
        TstInfo ret = new TstInfo();
        
        ret.setId(input.getId());
        ret.setType("" + input.getType());
        ret.setSerialNumber(""+input.getSerialNumber());
        if(input.getTime() != null){
          Calendar time = new GregorianCalendar();
          time.setTime(input.getTime());
          ret.setCreationTime(time); //TODO ?? do not know is it correct date 
        }//if
        ret.setPolicy(input.getPolicy());
        //ret.setErrorBound(input.) //TODO get correct field
        ret.setOrdered(input.isOrdered());
        //ret.setTSA(input.)//TODO get correct field
        //ret.setCertificate(convertToCertificateInfo());//TODO
        
        return ret;
      }
      
      
      
      public static CertificateInfo convertToCertificateInfo(CertValue cerVal , CertID cert){
        CertificateInfo ret = new CertificateInfo();
        
        if(cerVal != null && cerVal.getCert() != null){
          
          ret.setIssuer(cerVal.getCert().getIssuerDN().getName());
          ret.setSubject(cerVal.getCert().getSubjectDN().getName());
          {
            Calendar time = new GregorianCalendar();
            time.setTime(cerVal.getCert().getNotBefore());
            ret.setValidFrom(time);
          }
          {
            Calendar time = new GregorianCalendar();
            time.setTime(cerVal.getCert().getNotAfter());
            ret.setValidFrom(time);
          }
          
          ArrayList<CertificatePolicy> policyList = new ArrayList<CertificatePolicy>();

          for (String oid : cerVal.getCert().getCriticalExtensionOIDs()) { //cerVal.getCert().getNonCriticalExtensionOIDs()
            
            CertificatePolicy pol = new CertificatePolicy();
            pol.setDescription(oid);
            //pol.setURL(URL)//TODO
            //pol.setDescription(description)//TODO
            policyList.add(pol);
            
          }//for
          
          ret.setPolicies(policyList.toArray(new CertificatePolicy[policyList.size()]));
          
        }//if
        
        if(cert != null){
          ret.setIssuerSerial(""+cert.getSerial());
        }//if
        
        return ret;
      }//convertToCertificateInfo
      
      
      
      public static DataFileData convertToDataFile(String dataFileString) throws JDOMException, IOException{

        DataFileData ret = new DataFileData();
        
        if(dataFileString != null){

          SAXBuilder builder = new SAXBuilder();
          InputStream input = new ByteArrayInputStream(unEscapeXml(dataFileString).getBytes());
          
          Document doc =  builder.build(input);
          Element root = doc.getRootElement();
          
          if(root.getName() != null && root.getName().equals(DataFile)){
          
            ret.setContentType(root.getAttributeValue(ContentType));
            ret.setFilename(root.getAttributeValue(Filename));
            ret.setId(root.getAttributeValue(Id));
            ret.setMimeType(root.getAttributeValue(MimeType));
            ret.setSize(Integer.parseInt(root.getAttributeValue(Size)));
            ret.setDigestType(root.getAttributeValue(DigestType));
            ret.setDigestValue(root.getAttributeValue(DigestValue));
            
            ret.setDfData(root.getText());
          
          }//if
          
        }//if

        return ret;
      }//convertToDataFile

      
      
      public static String unEscapeXml(String str) {
        str = replaceString(str, "&amp;" , "&");
        str = replaceString(str, "&lt;" , "<");
        str = replaceString(str, "&gt;" , ">");
        str = replaceString(str, "&quot;" , "\"");
        str = replaceString(str, "&apos;" , "'");
        return str;

      }
      
      
      
      public static String escapeXml(String str) {
        str = replaceString(str, "&", "&amp;");
        str = replaceString(str, "<", "&lt;");
        str = replaceString(str, ">", "&gt;");
        str = replaceString(str, "\"", "&quot;");
        str = replaceString(str, "'", "&apos;");
        return str;

      }
      
      
      
      public static String replaceString(String text, String repl, String with) {
        if (text == null) { return null; }

        StringBuffer buffer = new StringBuffer(text.length());
        int start = 0;
        int end = 0;
        while ((end = text.indexOf(repl, start)) != -1) {
          buffer.append(text.substring(start, end)).append(with);
          start = end + repl.length();
        }
        buffer.append(text.substring(start));

        return buffer.toString();
      }
      
      
      
      public static String resolveFileName(String filename){
        
        String ret = "";
        String delim1 = "/";
        String delim2 = "\\";
        
        if(filename != null && !filename.equals("")){
          
          if(filename.indexOf(delim1) != -1){
            
            ArrayList<String> splitedFileName  = new ArrayList<String> (splitString(filename , delim1));
            ret = splitedFileName.get(splitedFileName.size()-1);
            
          }else if(filename.indexOf(delim2) != -1){
            
            ArrayList<String> splitedFileName  = new ArrayList<String> (splitString(filename , delim2));
            ret = splitedFileName.get(splitedFileName.size()-1);
            
          }else{
            ret = filename;
          }
          
        }//if
        
        return ret;
      }//resolveFileName
      
      
      
      /**
       * Split given string to Collection of text, delimiters are also returned.
       * 
       * @param text string to be splited
       * @param delimiter delimiter for string
       * 
       * @return
       */
      public static Collection<String> splitString(String text, String delimiter){
        
        ArrayList<String> ret = new ArrayList<String>();
        
        int index = text.indexOf(delimiter);
        if(index != -1){
        
          String elementBefore = text.substring(0,index);
          String delimElement = text.substring(index, index + delimiter.length());
          String elementAfter = text.substring(index + (delimiter.length()) ,text.length());
          
          if(elementBefore != null && !elementBefore.equals(""))
            ret.add(elementBefore);
          if(delimElement != null && !delimElement.equals(""))
            ret.add(delimElement);
          if(elementAfter != null && !elementAfter.equals(""))
            ret.addAll(splitString(elementAfter,delimiter));

        }else{
          ret.add(text);
        }//else
        
        return ret;
        
      }//splitString
      
      public static SignatureModule toSignatureModule(SignatureModuleDesc smd, String modulesPath) {
          if (smd == null)
              return null;
          SignatureModule ret = new SignatureModule();
          ret.setContentType(smd.getContentType().name());
          ret.setLocation(smd.getLocation().name());
          ret.setName(smd.getName());
          ret.setType(smd.getType().name());
          String fn = modulesPath + smd.getFile();
          byte[] byte_content = new byte[0];
          try {
              byte_content = FileManager.loadFile(fn);
          } catch (IOException e) {
              log.error("Can't find SignatureModule file '" + fn + "'");
          }
          ret.setContent(Base64Util.encode(byte_content));    
          return ret;
      }
      
}
