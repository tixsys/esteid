/*
 * SimpleBDigiDocFactory.java
 * PROJECT: JDigiDoc
 * DESCRIPTION: BDigi Doc functions for creating
 *	and reading signed documents. 
 * AUTHOR:  Kalev Suik, Inga Suik,Veiko Sinivee, Business IT Partner
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
/**

 * 
 * @author  Kalev Suik
 * @version 0.7
 */

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.StringReader;
import java.io.UnsupportedEncodingException;
import java.math.BigInteger;
import java.security.cert.X509Certificate;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.StringTokenizer;
import java.util.logging.Level;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

import org.apache.log4j.Logger;
import org.apache.xerces.parsers.DOMParser;
import org.w3c.dom.DOMException;
import org.w3c.dom.Document;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

import ee.sk.digidoc.Base64Util;
import ee.sk.digidoc.CertID;
import ee.sk.digidoc.CertValue;
import ee.sk.digidoc.CompleteCertificateRefs;
import ee.sk.digidoc.CompleteRevocationRefs;
import ee.sk.digidoc.DataFile;
import ee.sk.digidoc.DigiDocException;
import ee.sk.digidoc.KeyInfo;
import ee.sk.digidoc.Notary;
import ee.sk.digidoc.QualifyingProperties;
import ee.sk.digidoc.Reference;
import ee.sk.digidoc.Signature;
import ee.sk.digidoc.SignatureFile;
import ee.sk.digidoc.SignatureProductionPlace;
import ee.sk.digidoc.SignatureValue;
import ee.sk.digidoc.SignedDoc;
import ee.sk.digidoc.SignedInfo;
import ee.sk.digidoc.SignedProperties;
import ee.sk.digidoc.UnsignedProperties;
import ee.sk.utils.ConfigManager;
import ee.sk.utils.ConvertUtils;

/** 
 * BDOC parser. Parses xml files in DOM model
 * @author  Kalev Suik 
 */
/**
 * @author B16005
 *
 */
public class SimpleBDigiDocFactory implements DigiDocFactory {
	Logger m_logger = Logger.getLogger(SimpleBDigiDocFactory.class);
        private static String Encoding="UTF8";
	private static String F_mimetypeFile="mimetype";
	private static String F_mainfestFile="META-INF/manifest.xml";
	private static String F_rootName="manifest:manifest";
	private static String F_containerRoot="/";
	private static String F_entry="manifest:file-entry";
	private static String F_entryType="manifest:media-type";
	private static String F_entryPath="manifest:full-path";
	private static String F_entrySignatureType="signature/";
	private static String F_signatureRootName="Signature";
	//IS: muudatus prefix
	private static String F_prefix1="ds:";
	private static String F_prefix2="dsig:";
	/** root/CA certificates */
	private Hashtable m_rootCerts;
	private SignedDoc sDoc;
	//IS: muudatus prefix
	private String prefix = "";
	/************************************************************************************************/
	/**
	 * This inner class represents manifest.xml file as DOM object.
	 * It parses the xml files and creates signature and data  file 
	 * representations.
	 */
	private class ManifestXMLHandler extends DOMParser {
		private Document doc = null;
		private List/*DataFile*/ dataFiles = new ArrayList();
		private List/*SignatureFile*/ signatureFiles = new ArrayList();
		/**
		 * Constructor reads manifest.xml file from given buffered reader and parses it.
		 * 
		 * @param fileContents
		 * @throws IOException 
		 * @throws SAXException 
		 * @throws DigiDocException 
		 */
		public ManifestXMLHandler(BufferedReader bufferedReader) throws SAXException, IOException, DigiDocException{
			String contents=readFileReader(bufferedReader);
			dataFiles.clear();
			signatureFiles.clear();
			parse(new InputSource(new StringReader( contents )));
			doc = getDocument();
			parse(doc.getDocumentElement());
		}
		/**
		 * Returns Datafiles described in this mainfest.xml file.
		 * 
		 * @return
		 * @throws DigiDocException
		 */
		public List /*<DataFile>*/ getDataFiles() throws DigiDocException  {
			if(dataFiles.size()>0){
				return dataFiles;
			}
			parse(doc.getDocumentElement());
			return dataFiles;
		}
		
		/**
		 * Parses manifest.xml file into DOM object and creates entries for signature and datafiles.
		 * @param rn
		 * @throws DigiDocException
		 */

		private void /*<DataFile>*/ parse(Node rn) throws DigiDocException{
			List daList=new ArrayList();
			List siList=new ArrayList();
			
			boolean selfTypeVerified=false;
			
			if( ! F_rootName.equals(rn.getNodeName())){
				throw new IllegalArgumentException("Root element must be "+F_rootName+" not "+rn.getNodeName());
			}
			
			String versionInMime=null;
			String versionInSignature=null;
		
			if(rn.hasChildNodes()){
				NodeList nlist = rn.getChildNodes();
				int childrencount = nlist.getLength();
				for (int i=0; i<childrencount; i++)	{
					Node ch=nlist.item(i);
					String name=ch.getNodeName();
					if(F_entry.equals(name)){
						//3. entries; datafile, signature file, self
						NamedNodeMap nnm= ch.getAttributes();
						Node typeNode=nnm.getNamedItem(F_entryType);
						if( typeNode == null){
							throw new IllegalArgumentException(F_entryType+" missing");
						}
						String stype=typeNode.getNodeValue();
						if( stype == null){
							throw new IllegalArgumentException(F_entryType+" missing for "+typeNode);
						}
						Node pathNode=nnm.getNamedItem(F_entryPath);
						if( pathNode == null){
							throw new IllegalArgumentException(F_entryPath+" missing in "+typeNode);
						}
						String path=pathNode.getNodeValue();
						//A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1.2
						//path = ConvertUtils.data2str(path.getBytes(), "UTF-8");//Lauri commented this out, beacause this data is already UTF-8, by applying it twice it will breake the string.
						//L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1.2
						if(stype.startsWith(F_entrySignatureType)){
							//signature/BDOC-0.7/TM
							StringTokenizer st=new StringTokenizer(stype,"/");
							while (st.hasMoreElements()) {
								String token = (String) st.nextElement();
								if(token.startsWith(SignedDoc.FORMAT_BDOC)){
									String ver=token.substring(SignedDoc.FORMAT_BDOC.length()+1);
									if(versionInMime!=null){
										//we already know container's version
										if( ! versionInMime.equals(ver)){
											throw new DigiDocException(DigiDocException.ERR_DIGIDOC_VERSION, 
								                    "Signature version not same as container's", null);
										}
									}else{
										if(versionInSignature==null){
											versionInSignature=ver;
										}else{
											if( ! versionInSignature.equals(ver)){
												throw new DigiDocException(DigiDocException.ERR_DIGIDOC_VERSION, 
									                    "Signatures have different versions", null);
											}
										}
									}
								}
							}
							SignatureFile sfi = new SignatureFile();
							sfi.setFullPath(path);
							sfi.setType(stype);
							siList.add(sfi);
						}else if (stype.startsWith(SignedDoc.FORMAT_BDOC_MIME)){
							//if path is "/" then itself, otherwise a datafile
							if(F_containerRoot.equals(path)){
								//so it is BDOC's mimetype 
								sDoc.setFormat(SignedDoc.FORMAT_BDOC);
								//let's find version from "application/vnd.bdoc-0.7"
								versionInMime=stype.substring(SignedDoc.FORMAT_BDOC_MIME.length()+1);
								if(versionInSignature!=null){
									if( ! versionInSignature.equals(versionInMime)){
										throw new DigiDocException(DigiDocException.ERR_DIGIDOC_VERSION, 
							                    "Signature version not same as container's", null);
									}
								}
								sDoc.setVersion(versionInMime);
								if(selfTypeVerified){
									//there can be only one mime type in the root
									throw new IllegalArgumentException("only one mimetype entry is allowed");
								}
								selfTypeVerified=true;
								
							}else{
								//has to be datafile
								File f = new File(path);
								DataFile dfi = new DataFile(String.valueOf(i),DataFile.CONTENT_EMBEDDED,f.getName(),stype,sDoc,null,null,path);
								dfi.setFullName(path);
								daList.add(dfi);
							}
						}else{
							//has to be datafile
							File f = new File(path);
							DataFile dfi = new DataFile(String.valueOf(i),DataFile.CONTENT_EMBEDDED,f.getName(),stype,sDoc,null,null,path);
							dfi.setFullName(path);
							daList.add(dfi);
						}
					}
				}
			}
			if( !selfTypeVerified){
				throw new IllegalArgumentException("container type missing");
			}
			dataFiles=daList;//load datafiles into SignedDoc ?
			signatureFiles=siList;
		}//parse
		/**
		 * Returns signature files described in this mainfest.xml file.
		 * 
		 * @return
		 * @throws DigiDocException
		 */
		public List /*<SignatureFile>*/ getSignatureFiles() throws DigiDocException  {
			if(signatureFiles.size()>0){
				return signatureFiles;
			}
			parse(doc.getDocumentElement());
			return signatureFiles;
		}
		
	}//**********************************************************************
	/**
	 * This inner class parses signature xml file into DOM object.
	 * Then it parses the xml files and creates signature and data  file 
	 * representations.
	 */
	/**
	 * @author B16005
	 *
	 */
	/**
	 * @author B16005
	 *
	 */
	/**
	 * @author B16005
	 *
	 */
	private class SignatureXMLHandler extends DOMParser {
		private Signature  signature;
		/**
		 * Reads the contents of given signature xml file, parses it and puts into given Signature object
		 * 
		 * @param fileContents
		 * @throws IOException 
		 * @throws SAXException 
		 * @throws DigiDocException 
		 */
		public SignatureXMLHandler(Signature  sign,BufferedReader bufferedReader) throws SAXException, IOException, DigiDocException{
			String contents=readFileReader(bufferedReader);
			parse(new InputSource(new StringReader( contents )));
			signature=sign;
			parseRoot(getDocument().getDocumentElement());
		}
		/**
		 * Reads the root of BDigidoc signature file.
		 * Method expects to find ID attribute and four substructures: SignatureValue, KeyInfo, Object and SignedInfo.
		 * Corresponding parse...() method is invoked for each four members.
		 * @param rn
		 * @throws DigiDocException
		 */
		private void parseRoot(Node rn) throws DigiDocException{
			if( ! F_signatureRootName.equals(rn.getNodeName())){
				//IS: muudatus prefix
				if ((F_prefix1+F_signatureRootName).equals(rn.getNodeName())){
					prefix = F_prefix1;
				}
				else{
					throw new IllegalArgumentException("signatureRoot element must be "+F_rootName+" not "+rn.getNodeName());
				}
			}
			NamedNodeMap attrib= rn.getAttributes();
			Node ids =attrib.getNamedItem("Id");
			if( ids == null){
				throw new IllegalArgumentException("Signature is missing");
			}
			String idsv=ids.getNodeValue();
			if( idsv == null){
				throw new IllegalArgumentException("Signature is empty ");
			}
			signature.setId(idsv);
			HashMap hm=new HashMap();
			if(rn.hasChildNodes()){
				NodeList nlist = rn.getChildNodes();
				int childrencount = nlist.getLength();
				for (int i=0; i<childrencount; i++)	{
					Node ch=nlist.item(i);
					String name=ch.getNodeName();
					hm.put(name, ch);
				}
			}
			//IS: muudatus prefix
			Node signatureValue=(Node)hm.get(prefix+"SignatureValue");
			if(signatureValue==null){
				throw new IllegalArgumentException("<SignatureValue> section missing");
			}
			SignatureValue sv=new SignatureValue();
			parseSignatureValue(signatureValue,sv);
			signature.setSignatureValue(sv);
			//IS: muudatus prefix
			Node keyInfo=(Node)hm.get(prefix+"KeyInfo");
			if(keyInfo==null){
				throw new IllegalArgumentException("<KeyInfo> section missing");
			}
			KeyInfo kf = new KeyInfo();
			kf.setSignature(signature);
			parseKeyInfo(keyInfo,kf);
			signature.setKeyInfo(kf);
			//IS: muudatus prefix
			Node object=(Node)hm.get(prefix+"Object");
			if(object==null){
				throw new IllegalArgumentException("<Object> section missing");
			}
			parseObject(object,signature);
			//IS: muudatus prefix
			Node sigInf=(Node)hm.get(prefix+"SignedInfo");
			if(sigInf==null){
				throw new IllegalArgumentException("<SignedInfo> section missing");
			}
			SignedInfo si = new SignedInfo(signature);
			parseSignedInfo(sigInf, si);
			signature.setSignedInfo(si);
		}
		/**
		 * Parses SignedInfo in signature's root node.
		 * Needed attributes are CanonicalizationMethod,SignatureMethod and Reference.
		 * References will be parsed in parseReference() method.
		 * 
		 * @param Node
		 * @param SignedInfo
		 * @throws DigiDocException
		 */
		private void parseSignedInfo(Node inrn, SignedInfo signedInfo) throws DigiDocException{
			if(inrn.hasChildNodes()){
				NodeList nlist = inrn.getChildNodes();
				int childrencount = nlist.getLength();
				for (int i=0; i<childrencount; i++)	{
					Node rn=nlist.item(i);
					String name=rn.getNodeName();
					//IS: muudatus prefix
					if((prefix+"CanonicalizationMethod").equals(name)){
						NamedNodeMap attrib= rn.getAttributes();
						Node ids =attrib.getNamedItem("Algorithm");
						if( ids == null){
							throw new IllegalArgumentException("CanonicalizationMethod Algorithm is missing");
						}
						String idsv=ids.getNodeValue();
						if( idsv == null){
							throw new IllegalArgumentException("CanonicalizationMethod Algorithm is empty ");
						}
						signedInfo.setCanonicalizationMethod(idsv);
					//IS: muudatus prefix
					}else if((prefix+"SignatureMethod").equals(name)){
						NamedNodeMap attrib= rn.getAttributes();
						Node ids =attrib.getNamedItem("Algorithm");
						if( ids == null){
							throw new IllegalArgumentException("SignatureMethod Algorithm is missing");
						}
						String idsv=ids.getNodeValue();
						if( idsv == null){
							throw new IllegalArgumentException("SignatureMethod Algorithm is empty ");
						}
						signedInfo.setSignatureMethod(idsv);
						//IS: muudatus prefix
					}else if((prefix+"Reference").equals(name)){
						Reference ref =  new Reference(signedInfo);
						parseReference( rn,ref);
						signedInfo.addReference(ref);
					}
				}
				
			}
		}
		
		/**
		 * @param  xml Reference node
		 * @param Reference object to be filled
		 * There are two types of references: datafiles and signed properties 
		 * @throws DigiDocException
		 */
		private void parseReference(Node rn, Reference ref) throws DigiDocException {
			//common
			NamedNodeMap attrib= rn.getAttributes();
			Node ids =attrib.getNamedItem("URI");
			if( ids == null){
				throw new IllegalArgumentException("Reference URI is missing");
			}
			String uRI=ids.getNodeValue();
			//A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1.2
			uRI = ConvertUtils.data2str(uRI.getBytes(), "UTF-8");
			//L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1.2
			if( uRI == null){
				throw new IllegalArgumentException("Reference URI is empty ");
			}
			if(uRI.startsWith("/") || uRI.startsWith("#")){
				uRI=uRI.substring(1);
			}
			String digest=null;
			String digestMethodAlgorithm=null;
			NodeList referenceNodeList=rn.getChildNodes();
			int referenceNodeListCount = referenceNodeList.getLength();
			for (int ref_i=0; ref_i<referenceNodeListCount; ref_i++)	{
				Node refn=referenceNodeList.item(ref_i);
				String refnname=refn.getNodeName();
				//IS: muudatus prefix
				if((prefix+"DigestMethod").equals(refnname)){
					NamedNodeMap  refNamedNodeMap =refn.getAttributes();
					Node dma=refNamedNodeMap.getNamedItem("Algorithm");
					if(dma==null){
						throw new IllegalArgumentException("Algorithm missing in Reference URI "+uRI);
					}
					digestMethodAlgorithm=dma.getNodeValue();
				//IS: muudatus prefix
				}else if((prefix+"DigestValue").equals(refnname)){
					digest=refn.getTextContent();
					if(digest==null){
						throw new IllegalArgumentException("DigestValue missing in Reference URI "+uRI);
					}
				}
			}
			//end of common
			DataFile dataFile=null;
			String typeName=null;
			Node type =attrib.getNamedItem("Type");
			if(type!=null){
				typeName=type.getNodeValue(); 
			}
			if(typeName!=null && typeName.endsWith("SignedProperties")){
				//signed properties
				SignedProperties sp= signature.getSignedProperties();
				String spURI=sp.getId();
				if( spURI.startsWith("#")){
					spURI=spURI.substring(1);
				}
				if( ! uRI.equals(spURI)){
					throw new IllegalArgumentException("Reference URI "+uRI+" does not point to existing SignedProperties ");
				}
			}else{
				//data file
				for (int j = 0; j < sDoc.countDataFiles(); j++) {
					DataFile d=sDoc.getDataFile(j);
                                        
                                        try {
                                            //signature.xml ds:Reference URI is URL encoded, so must decode this
                                            String ecUri = java.net.URLDecoder.decode(uRI, Encoding);
                                            if(ecUri.equals(d.getFileName())){
						dataFile=d;
						break;
                                            }
                                        } catch (UnsupportedEncodingException ex) {
                                            java.util.logging.Logger.getLogger(SimpleBDigiDocFactory.class.getName()).log(Level.SEVERE, null, ex);
                                        }
					
				}
				if(dataFile==null){
					throw new IllegalArgumentException("Reference URI "+uRI+" does not point to file");
				}
			}
			ref.setDigestAlgorithm(digestMethodAlgorithm);
			if (uRI.indexOf("SignedProperties") != -1){
				ref.setUri("#" + uRI);
			} else {
				ref.setUri(uRI);
			}
			ref.setDigestValue(Base64Util.decode(digest));
			ref.setDataFile(dataFile);
			ref.setType(typeName);
		}
		
	

		/**
		 * Parses SignatureValue
		 * 
		 * @param SignatureValue xml node
		 * @param SignatureValue's object
		 * @throws DigiDocException
		 */
		private void parseSignatureValue(Node rnSV, SignatureValue sv) throws DigiDocException{
			String value = rnSV.getTextContent();
			sv.setValue(Base64Util.decode(value));
			NamedNodeMap nnm= rnSV.getAttributes();
			Node sigIdNode=nnm.getNamedItem("Id");
			//IS: muudatus prefix
			if(sigIdNode==null){
				System.out.println(" "+rnSV.getLocalName()+" is missing id");
				sv.setId("");
			}
			else {
				sv.setId(sigIdNode.getNodeValue());
			}
		}
		
		/**
		 * Parses KeyInfo section i.e signers certificate
		 * 
		 * @param KeyInfo xml node
		 * @param KeyInfo object
		 * @throws DigiDocException
		 */
		private void parseKeyInfo(Node rnKeyInfo, KeyInfo kf) throws DigiDocException{
			if(rnKeyInfo.hasChildNodes()){
				NodeList nKeyInfoList = rnKeyInfo.getChildNodes();
				int childrencount = nKeyInfoList.getLength();
				for (int i=0; i<childrencount; i++)	{
					Node rnOb=nKeyInfoList.item(i);
					String name=rnOb.getNodeName();
					//IS: muudatus prefix
					if((prefix+"X509Data").equals(name)){
						Node rnX509DataList=rnOb.getFirstChild();
						do{
						//IS: muudatus prefix
							if((prefix+"X509Certificate").equals(rnX509DataList.getNodeName())){
								String value = rnX509DataList.getTextContent();
								X509Certificate cert= SignedDoc.readCertificate(Base64Util.decode(value));
								kf.setSignersCertificate(cert);
								break;
							}
							//IS: muudatus rnOb.getNextSibling() -> rnX509DataList.getNextSibling()
						}while ( (rnX509DataList=rnX509DataList.getNextSibling()) !=null);
					}/*else if("KeyValue".equals(name)){
					}*/
				}//
			}//if(rn.hasChildNodes())
		}
		/**
		 * Parses Object section, there are several Properties section's like QualifyingProperties,
		 * UnsignedProperties, UnsignedSignatureProperties and SignedProperties. Two last ones have parser methods for themselves
		 * 
		 * @param rn xml node
		 * @param signature Signature object
		 * @throws DigiDocException
		 */
		private void parseObject(Node rn, Signature signature) throws DigiDocException{
			/* QualifyingProperties Target="#S0" */
			if(rn.hasChildNodes()){
				NodeList nlist = rn.getChildNodes();
				int childrencount = nlist.getLength();
				for (int i=0; i<childrencount; i++)	{
					Node rnOb=nlist.item(i);
					String name=rnOb.getNodeName();
					if("QualifyingProperties".equals(name)){
						QualifyingProperties qp=new QualifyingProperties();
						NamedNodeMap nnm= rnOb.getAttributes();
						Node tar= nnm.getNamedItem("Target");
						qp.setTarget(tar.getNodeValue());
						signature.setQualifyingProperties(qp);
						
						if(rnOb.hasChildNodes()){
							NodeList rnOblist = rnOb.getChildNodes();
							int rnOchildrencount = rnOblist.getLength();
							for (int rnO_i=0; rnO_i<rnOchildrencount; rnO_i++)	{
								Node rnOb2=rnOblist.item(rnO_i);
								String name2=rnOb2.getNodeName();
								if("SignedProperties".equals(name2)){
									parseSignedProperties(rnOb2,signature);
								}else if("UnsignedProperties".equals(name2)){
									NodeList rnrnOb2list = rnOb2.getChildNodes();
									for (int  rnrnOb2list_i=0;  rnrnOb2list_i< rnrnOb2list.getLength();  rnrnOb2list_i++)	{
										Node rnObUn2=rnrnOb2list.item(rnrnOb2list_i);
										String namernObUn2=rnObUn2.getNodeName();
										if("UnsignedSignatureProperties".equals(namernObUn2)){
											parseUnSignedProperties(rnObUn2,signature);
										}
									}
								}
							}
						}
					}
				}
			}
		}
		/**
		 * Parses and canonicalizes SignedProperties
		 * 
		 * @param SignedProperties xml node
		 * @param signature object
		 * @throws DOMException
		 * @throws DigiDocException
		 */
		private void parseSignedProperties(Node rn, Signature signature) throws DOMException, DigiDocException{
			NamedNodeMap nnm= rn.getAttributes();
			Node sigIdNode=nnm.getNamedItem("Id");
			SignedProperties sPro= new SignedProperties(signature);
			sPro.setId(sigIdNode.getNodeValue());
			if(rn.hasChildNodes()){
				NodeList nlist = rn.getChildNodes();
				int childrencount = nlist.getLength();
				for (int i=0; i<childrencount; i++)	{
					Node rnOb=nlist.item(i);
					String name=rnOb.getNodeName();
					if("SignedSignatureProperties".equals(name)){
						NodeList rnOblist = rnOb.getChildNodes();
						for (int rnOblist_i=0; rnOblist_i<rnOblist.getLength(); rnOblist_i++)	{
							Node rnssp=rnOblist.item(rnOblist_i);
							String nameSSPn=rnssp.getNodeName();
							if("SigningTime".equals(nameSSPn)){
								String timeString=rnssp.getTextContent();
								sPro.setSigningTime(ConvertUtils.string2date(timeString, signature.getSignedDoc()));
							}else if("SignatureProductionPlace".equals(nameSSPn)){
								SignatureProductionPlace spp = new SignatureProductionPlace();
								parseSignatureProductionPlace(rnssp,spp);
								sPro.setSignatureProductionPlace(spp);
							}else if("SigningCertificate".equals(nameSSPn)){
								NodeList rnCertOblist = rnssp.getChildNodes();
								for (int rnCertOblist_i=0; rnCertOblist_i<rnCertOblist.getLength(); rnCertOblist_i++)	{
									Node rnCert=rnCertOblist.item(rnCertOblist_i);
									String nameCrt=rnCert.getNodeName();
									if("Cert".equals(nameCrt)){
										//signer cert
										NamedNodeMap scNM=rnCert.getAttributes();
										String cIDS=null;
										Node certIdNode=scNM.getNamedItem("Id");
										CertID certID=new CertID();
										if(certIdNode!=null){
											cIDS=certIdNode.getNodeValue();
											certID.setId(cIDS);
										}
										signature.addCertID(certID);
										certID.setSignature(signature);
										parseCertID(rnCert,certID);
										certID.setType(CertID.CERTID_TYPE_SIGNER);
										sPro.setCertDigestAlgorithm(certID.getDigestAlgorithm());
										sPro.setCertDigestValue(certID.getDigestValue());
										if(certID.getId()!=null){
											sPro.setCertId(certID.getId());
										}
										sPro.setCertSerial(certID.getSerial());
										// A Kalev <2008 aprill> BDOCiga seotud muudatused xml-is 1.1
										// sPro.setOrigDigest(
										// L Kalev <2008 aprill> BDOCiga seotud muudatused xml-is 1.1

									}
								}
							}else if("SignerRole".equals(nameSSPn)){
								NodeList rnSR = rnssp.getChildNodes();
								for (int rnSR_i=0; rnSR_i<rnSR.getLength(); rnSR_i++)	{
									Node rnCLS=rnOblist.item(rnSR_i);
									String nameCLS=rnssp.getNodeName();
									if("ClaimedRoles".equals(nameCLS)){
										NodeList rnRolls = rnCLS.getChildNodes();
										for (int rnRolls_i=0; rnRolls_i<rnRolls.getLength(); rnRolls_i++)	{
											Node rnRo=rnRolls.item(rnRolls_i);
											String nameRo=rnRo.getNodeName();
											if("ClaimedRole".equals(nameRo)){
												sPro.addClaimedRole(rnRo.getTextContent());
											}
										}
									}
								}
							}else if("DataObjectFormat".equals(nameSSPn)){
								sPro.setDataObjectFormat(rnssp.getTextContent());
							}
								
						}
					}else if("SignedDataObjectProperties".equals(name)){
						sPro.setSignedDataObjectProperties(rnOb.getTextContent()); 
					}
				}
			}
			// A Kalev <2008 aprill> BDOCiga seotud muudatused xml-is 1.1
			CanonicalizationFactory canFac = ConfigManager.instance().getCanonicalizationFactory();
			byte[] bCanProp = canFac.canonicalize(sPro.toString().getBytes(),
					SignedDoc.CANONICALIZATION_METHOD_20010315);
			sPro.setOrigDigest(SignedDoc.digest(bCanProp));
			signature.setSignedProperties(sPro);
			// L Kalev <2008 aprill> BDOCiga seotud muudatused xml-is 1.1

		}
		/**
		 * Parses SignatureProductionPlace
		 * 
		 * @param rnssp SignatureProductionPlace's xml node
		 * @param spp SignatureProductionPlace object
		 * @throws DOMException
		 * @throws DigiDocException
		 */
		private void parseSignatureProductionPlace(Node rnssp,SignatureProductionPlace spp) throws DOMException, DigiDocException {
			Node chNode=rnssp.getFirstChild();
			do {
				String ppName=chNode.getNodeName();
				if("City".equals(ppName)){
					spp.setCity(ConvertUtils.utf82str(chNode.getTextContent()));
				}else if("StateOrProvince".equals(ppName)){
					spp.setStateOrProvince(ConvertUtils.utf82str(chNode.getTextContent()));
				}else if("PostalCode".equals(ppName)){
					spp.setPostalCode(ConvertUtils.utf82str(chNode.getTextContent()));
				}else if("CountryName".equals(ppName)){
					spp.setCountryName(ConvertUtils.utf82str(chNode.getTextContent()));
				}
			} while ( (chNode=chNode.getNextSibling() )!=null );
		}
		
		

		/**
		 * Parses certificate properties
		 * 
		 * @param rnCert
		 * @param certID
		 * @throws DigiDocException
		 */
		private void parseCertID(Node rnCert, CertID certID) throws DigiDocException{
			NodeList rnCertlist = rnCert.getChildNodes();
			for (int rnCertlist_i=0; rnCertlist_i<rnCertlist.getLength(); rnCertlist_i++)	{
				Node rnCertDet=rnCertlist.item(rnCertlist_i);
				String nameCertDet=rnCertDet.getNodeName();
				if("CertDigest".equals(nameCertDet)){
					NodeList rnCertDetlist = rnCertDet.getChildNodes();
					for (int rnCertDetlist_i=0; rnCertDetlist_i<rnCertDetlist.getLength(); rnCertDetlist_i++)	{
						Node rnCertDigest=rnCertDetlist.item(rnCertDetlist_i);
						String nameCrtDig=rnCertDigest.getNodeName();
						//IS: muudatus prefix
						if((prefix+"DigestMethod").equals(nameCrtDig)){
							NamedNodeMap cerDnnm= rnCertDigest.getAttributes();
							Node n=cerDnnm.getNamedItem("Algorithm");
							String value=n.getNodeValue();
							certID.setDigestAlgorithm(value);
							//IS: muudatus prefix
						}else if((prefix+"DigestValue").equals(nameCrtDig)){
							String value=rnCertDigest.getTextContent();
							certID.setDigestValue(Base64Util.decode(value));
						}
					}
				}else if("IssuerSerial".equals(nameCertDet)){
					NodeList rnCertDetlist = rnCertDet.getChildNodes();
					for (int rnCertDetlist_i=0; rnCertDetlist_i<rnCertDetlist.getLength(); rnCertDetlist_i++)	{
						Node rnIssuer=rnCertDetlist.item(rnCertDetlist_i);
						String nameCrtDig=rnIssuer.getNodeName();
						//IS: muudatus prefix
						if((prefix+"X509IssuerName").equals(nameCrtDig)){
							String value=rnIssuer.getTextContent();
							certID.setIssuer(value);
						//IS: muudatus prefix
						}else if((prefix+"X509SerialNumber").equals(nameCrtDig)){
							String value=rnIssuer.getTextContent();
							certID.setSerial(new BigInteger(value));
						}
					}
				}
			}
		}
		
		/**
		 * Parses unsigned properties like certificate refs
		 * 
		 * @param rn UnSignedProperties xml node
		 * @param siganture Signature object to be added
		 * @throws DigiDocException
		 */
		private void parseUnSignedProperties(Node rn, Signature siganture) throws DigiDocException{
			UnsignedProperties usp=new UnsignedProperties(siganture);
			siganture.setUnsignedProperties(usp);
			 if(rn.hasChildNodes()){
				NodeList nlist = rn.getChildNodes();
				int childrencount = nlist.getLength();
				for (int i=0; i<childrencount; i++)	{
					Node rnOb=nlist.item(i);
					String name=rnOb.getNodeName();
					if("CompleteCertificateRefs".equals(name)){
						CompleteCertificateRefs ccf=new CompleteCertificateRefs();
						ccf.setUnsignedProperties(usp);
						usp.setCompleteCertificateRefs(ccf);
						NodeList nCertlist = rnOb.getChildNodes();
						for (int nCertlist_i=0; nCertlist_i<nCertlist.getLength(); nCertlist_i++)	{
							Node certrfs=nCertlist.item(nCertlist_i);
							String nameCcertrfs=certrfs.getNodeName();
							if("CertRefs".equals(nameCcertrfs)){
								NodeList nCerts = certrfs.getChildNodes();
								for (int nCerts_i=0; nCerts_i<nCerts.getLength(); nCerts_i++)	{
									Node cert=nCerts.item(nCerts_i);
									String nameCert=cert.getNodeName();
									if("Cert".equals(nameCert)){
										//ref to issuer CA certificate
										CertID certID=new CertID();
										certID.setType(CertID.CERTID_TYPE_CA);
										parseCertID(cert, certID);
										ccf.addCertID(certID);
									}
								}
							}
						}
					}else if("CompleteRevocationRefs".equals(name)){
						CompleteRevocationRefs crr = new CompleteRevocationRefs();
						crr.setUnsignedProperties(usp);
						usp.setCompleteRevocationRefs(crr);
						NodeList nCertlist = rnOb.getChildNodes();
						for (int nCertlist_i=0; nCertlist_i<nCertlist.getLength(); nCertlist_i++)	{
							Node certrfs=nCertlist.item(nCertlist_i);
							String nameCcertrfs=certrfs.getNodeName();
							if("OCSPRefs".equals(nameCcertrfs)){
								NodeList nCerts = certrfs.getChildNodes();
								for (int nCerts_i=0; nCerts_i<nCerts.getLength(); nCerts_i++)	{
									Node cert=nCerts.item(nCerts_i);
									String nameCert=cert.getNodeName();
									if("OCSPRef".equals(nameCert)){
										parseCompleteRevocationRefs(cert,crr);
									}
								}
							}
						}
					}else if("CertificateValues".equals(name)){
						NodeList nCertValuelist = rnOb.getChildNodes();
						for (int nCertValuelist_i=0; nCertValuelist_i<nCertValuelist.getLength(); nCertValuelist_i++)	{
							Node certrfs=nCertValuelist.item(nCertValuelist_i);
							String nameCcertrfs=certrfs.getNodeName();
							if("EncapsulatedX509Certificate".equals(nameCcertrfs)){
								CertValue cv = new CertValue();
								//cv.setType();
								cv.setSignature(siganture);
								parseCertValue( certrfs,cv);
								siganture.addCertValue(cv);
								// A Kalev <2008 aprill> BDOCiga seotud muudatused xml-is 1.1
								findCertIDandCertValueTypes(siganture);
								// L Kalev <2008 aprill> BDOCiga seotud muudatused xml-is 1.1
							}
						}
					}else if("RevocationValues".equals(name)){
						NodeList nCertValuelist = rnOb.getChildNodes();
						for (int nCertValuelist_i=0; nCertValuelist_i<nCertValuelist.getLength(); nCertValuelist_i++)	{
							Node certrfs=nCertValuelist.item(nCertValuelist_i);
							String nameCcertrfs=certrfs.getNodeName();
							if("OCSPValues".equals(nameCcertrfs)){
								NodeList nOCSPValuelist = certrfs.getChildNodes();
								for (int nOCSPValuelist_i=0; nOCSPValuelist_i<nOCSPValuelist.getLength(); nOCSPValuelist_i++)	{
									Node nOCSPValue=nOCSPValuelist.item(nOCSPValuelist_i);
									String nameOCSPValue=nOCSPValue.getNodeName();
									if("EncapsulatedOCSPValue".equals(nameOCSPValue)){
										String value=nOCSPValue.getTextContent();
										Notary not = new Notary();
										NamedNodeMap nnm=nOCSPValue.getAttributes();
										Node idN=nnm.getNamedItem("Id");
										String id=idN.getNodeValue();
										not.setId(id);
										usp.setNotary(not);
										not.setOcspResponseData(Base64Util.decode(value));
										NotaryFactory notFac = ConfigManager.instance().getNotaryFactory();
										notFac.parseAndVerifyResponse(siganture, not);	
									}
								}	
							}
						}
					}
				}
			}
		}
		/**
		 * Reads certificate value and parses it into a certificate object
		 * 
		 * @param certrfs Certificate value node 
		 * @param cv CertValue object
		 * @throws DigiDocException
		 */
		private void parseCertValue(Node certrfs, CertValue cv) throws DigiDocException {
			//IS: muudatus prefix A
			NamedNodeMap nnm;
			if (!certrfs.hasAttributes()){
				Node certValue = certrfs.getParentNode();
				nnm=certValue.getAttributes();
			} else {
				nnm=certrfs.getAttributes();
			}
			//IS: muudatus prefix L
			Node idN=nnm.getNamedItem("Id");
			String id=idN.getNodeValue();
			cv.setId(id);
			if(id.indexOf("RESPONDER")>-1){
				cv.setType(CertValue.CERTVAL_TYPE_RESPONDER);
			}else if(id.indexOf("TSA")>-1){
				cv.setType(CertValue.CERTVAL_TYPE_TSA);
			}else if(id.indexOf("CA")>-1){
				cv.setType(CertValue.CERTVAL_TYPE_CA);
			}else {
				cv.setType(CertValue.CERTVAL_TYPE_SIGNER);
			}
			String value=certrfs.getTextContent();
			cv.setCert(SignedDoc.readCertificate(Base64Util.decode(value)));
		}
		
		/**
		 * Parses OCSP related objects
		 * 
		 * @throws DigiDocException
		 */

		private void parseCompleteRevocationRefs(Node rnOCSPref, CompleteRevocationRefs crr) throws DigiDocException{
			NodeList rnCertlist = rnOCSPref.getChildNodes();
			for (int rnCertlist_i=0; rnCertlist_i<rnCertlist.getLength(); rnCertlist_i++)	{
				Node rnCertDet=rnCertlist.item(rnCertlist_i);
				String nameCertDet=rnCertDet.getNodeName();
				if("DigestAlgAndValue".equals(nameCertDet)){
					NodeList rnCertDetlist = rnCertDet.getChildNodes();
					for (int rnCertDetlist_i=0; rnCertDetlist_i<rnCertDetlist.getLength(); rnCertDetlist_i++)	{
						Node rnCertDigest=rnCertDetlist.item(rnCertDetlist_i);
						String nameCrtDig=rnCertDigest.getNodeName();
						if((prefix+"DigestMethod").equals(nameCrtDig)){
							NamedNodeMap cerDnnm= rnCertDigest.getAttributes();
							Node n=cerDnnm.getNamedItem("Algorithm");
							String value=n.getNodeValue();
							crr.setDigestAlgorithm(value);
						}else if((prefix+"DigestValue").equals(nameCrtDig)){
							String value=rnCertDigest.getTextContent();
							crr.setDigestValue(Base64Util.decode(value));
						}
					}
				}else if("OCSPIdentifier".equals(nameCertDet)){
					NamedNodeMap ocspNM=rnCertDet.getAttributes();
					Node ocspURI=ocspNM.getNamedItem("URI");
					String uri=ocspURI.getNodeValue();
					crr.setUri(uri);
					NodeList rnCertDetlist = rnCertDet.getChildNodes();
					for (int rnCertDetlist_i=0; rnCertDetlist_i<rnCertDetlist.getLength(); rnCertDetlist_i++)	{
						Node rnResp=rnCertDetlist.item(rnCertDetlist_i);
						String nameCrtDig=rnResp.getNodeName();
						if("ResponderID".equals(nameCrtDig)){
							NodeList rnRespNames = rnResp.getChildNodes();
							for (int rnRespNames_i=0; rnRespNames_i<rnRespNames.getLength(); rnRespNames_i++)	{
								Node rnRespNa=rnRespNames.item(rnRespNames_i);
								String nname=rnRespNa.getNodeName();
								if("ByName".equals(nname)){
									String value=rnRespNa.getTextContent();
									crr.setResponderId(value);
								}
							}
						}else if("ProducedAt".equals(nameCrtDig)){
							String value=rnResp.getTextContent();
							crr.setProducedAt(ConvertUtils.string2date(value, crr.getUnsignedProperties().getSignature().getSignedDoc()));
							
						}
					}
					
				}
			}
		}
	}
	
	/**
	 * Reads and parses bdoc's mainfest file
	 */ 
	ManifestXMLHandler getManifestXMLHandler(BufferedReader fileReader) throws IOException, SAXException, DigiDocException{
		//String content=readFile(fileName);
		return this.new ManifestXMLHandler(fileReader);
	}
	
	/**
	 * Reads content of file into String
	 * The second line of xml'll be ignore, if it is containing "<!DOCTYPE manifest:manifest  ..."
	 * because we do not need any online checking. The document will be verified by source code.
	 *
	 * @throws IOException 
	 */
	public static String readFile( String fileName ) throws IOException {
		File f = new File(fileName);
		//FileReader r = new FileReader(f);
                InputStreamReader isr = new InputStreamReader(new java.io.FileInputStream(fileName), Encoding);
		return readFileReader(  new BufferedReader(isr) );

               // return readFileReader(isr);
	}//readFile()
	
	/**
	 * Reads manifest file into one string.
	 * It ignores the second line if it is containing "<!DOCTYPE manifest:manifest  ..."
	 * because we do not need any online checking. The document will be verified by source code.
	 * 
	 * @param br
	 * @return
	 * @throws IOException
	 */
	public static String readFileReader(BufferedReader br ) throws IOException {
		StringBuffer ret = new StringBuffer();
		String line;
		
		while ( (line = br.readLine()) != null ) {
		  if (line.startsWith("#")) {
			  continue;
		  }
		  //<!DOCTYPE manifest:manifest PUBLIC "-//OpenOffice.org//DTD Manifest 1.0//EN" "Manifest.dtd">
		  if(line.startsWith("<!DOCTYPE") && line.indexOf("Manifest.dtd")>-1 ){
			  continue;
		  }
		  ret.append( line );
		}
		return ret.toString(); 
	}//readFileReader()

	/**
	 * Finds the CA for this certificate
	 * if the root-certs table is not empty
	 * @param cert certificate to search CA for
	 * @return CA certificate
	 */
	public X509Certificate findCAforCertificate(X509Certificate cert) {
		X509Certificate caCert = null;
		if (cert != null && m_rootCerts != null && !m_rootCerts.isEmpty()) {
			//String cn = SignedDoc.getCommonName(cert.getIssuerX500Principal().getName("RFC1779"));
			String dn = cert.getIssuerX500Principal().getName("RFC1779");
			if (m_logger.isDebugEnabled())
				m_logger.debug("Find CA cert for issuer: " + dn);
			caCert = (X509Certificate) m_rootCerts.get(dn);
			if (m_logger.isDebugEnabled())
				m_logger.debug("CA: " + ((caCert == null) ? "NULL" : "OK"));
		}
		return caCert;
	}

	/** 
	 * Initilaizes i.e loads root certificates
	 * 
	 * @see ee.sk.digidoc.factory.DigiDocFactory#init()
	 */
	public void init() throws DigiDocException {
		if (m_rootCerts == null){
			initCACerts();
		}
	}
	/** 
	 * initializes the CA certificates  
	 */
	private void initCACerts() throws DigiDocException {
		try {
			m_rootCerts = new Hashtable();
			if (m_logger.isDebugEnabled())
				m_logger.debug("Loading CA certs");
                        ConfigManager config = ConfigManager.instance();
			int nCerts = config.getIntProperty("DIGIDOC_CA_CERTS", 0);
			for (int i = 0; i < nCerts; i++) {
				String certFile =config.getFilePathProperty("DIGIDOC_CA_CERT" + (i + 1));
				if (m_logger.isDebugEnabled())
					m_logger.debug("CA: " + ("DIGIDOC_CA_CERT" + (i + 1)) + " file: " + certFile);
				X509Certificate cert =
					SignedDoc.readCertificate(certFile);
				if (cert != null) {
					if (m_logger.isDebugEnabled())
						m_logger.debug("CA subject: " + cert.getSubjectDN() + " issuer: " + cert.getIssuerX500Principal().getName("RFC1779"));
					m_rootCerts.put(cert.getSubjectX500Principal().getName("RFC1779"), cert);
				}
			}
		} catch (Exception e) {
			DigiDocException.handleException(
				e,
				DigiDocException.ERR_CA_CERT_READ);
		}
	}

	/** 
	 * Reads parses and adds signature to SignedDoc object
	 * 
	 * @see ee.sk.digidoc.factory.DigiDocFactory#readSignature(ee.sk.digidoc.SignedDoc, java.io.InputStream)
	 */
	public Signature readSignature(SignedDoc sdoc, InputStream sigStream)throws DigiDocException {
		Signature ssi  = new Signature(sDoc);
		try {
                        BufferedReader br=new BufferedReader(new InputStreamReader( sigStream, Encoding ));
			this.new SignatureXMLHandler(ssi,br);
			br.close();
			sDoc.addSignature(ssi);
			return ssi;
		} catch (SAXDigiDocException ex) {
			throw ex.getDigiDocException();
		} catch (Exception ex) {
			DigiDocException.handleException(ex,
				DigiDocException.ERR_PARSE_XML);
		}
		if (sDoc.getLastSignature() == null)
			throw new DigiDocException(DigiDocException.ERR_DIGIDOC_FORMAT,
				"This document is not in Signature format", null);
		return sDoc.getLastSignature();
	}
	
	/**
     * Reads in a DigiDoc file
     * @param fileName file name
     * @return signed document object if successfully parsed
     */
	public SignedDoc readSignedDoc(String fileName) throws DigiDocException {
		
		try {
			if(fileName==null){
				throw new IllegalArgumentException("ContainerName=null");
			}
			File f =new File(fileName); 
			if( ! (f).exists() ){
				throw new FileNotFoundException(fileName);
			}
			
			ZipFile zp = new ZipFile(f);
			ZipEntry ze_mime=zp.getEntry(F_mimetypeFile);
			if(ze_mime==null){
				String fullPathName=F_mimetypeFile.replace('/', '\\');
				ze_mime=zp.getEntry(fullPathName);
				if(ze_mime==null){
					throw new FileNotFoundException(F_mimetypeFile);
				}
			}
			ZipEntry ze_manifest=zp.getEntry(F_mainfestFile);
			if(ze_manifest==null){
				String fullPathName=F_mainfestFile.replace('/', '\\');
				ze_manifest=zp.getEntry(fullPathName);
				if(ze_manifest==null){
					throw new FileNotFoundException(F_mainfestFile);
				}
			}
			sDoc= new SignedDoc();
			BufferedReader br=new BufferedReader(new InputStreamReader( zp.getInputStream(ze_manifest), Encoding));
			ManifestXMLHandler xmH = this.getManifestXMLHandler(br);
			br.close();
			String mimeFileC=readFileReader(br=new BufferedReader(new InputStreamReader( zp.getInputStream(ze_mime), Encoding)));
			br.close();
			if (mimeFileC.startsWith(SignedDoc.FORMAT_BDOC_MIME)){
				//let's find version from "application/vnd.bdoc-0.7"
				if(mimeFileC.length()<SignedDoc.FORMAT_BDOC_MIME.length()+1){
					throw new IllegalArgumentException("mime type does not have version");
				}
				String ver=mimeFileC.substring(SignedDoc.FORMAT_BDOC_MIME.length()+1);
				//version has to be the same as in manifestfile
				if( ! sDoc.getVersion().equals(ver)){
					throw new IllegalArgumentException("Versions in mime and manifest are different");
				}
			}else{
				throw new IllegalArgumentException("Application is not "+SignedDoc.FORMAT_BDOC_MIME);
			}
			//Do we have all datafiles
			List dFiles=xmH.getDataFiles();
 			for (Iterator iterator = dFiles.iterator(); iterator.hasNext();) {
				DataFile dfile = (DataFile) iterator.next();
				String fullPathName=dfile.getFileName();
                                
				ZipEntry ze=zp.getEntry(fullPathName);
				if(ze==null){
					//perhaps different separator
					fullPathName=fullPathName.replace('/', '\\');
					ze=zp.getEntry(fullPathName);
					if(ze==null){
						throw new IllegalArgumentException("Datafile "+dfile.getFileName()+" not found in container");
					}
				}
				sDoc.addDataFile(dfile);
				dfile.setZipEntry(ze);
				dfile.setBDOCcontainer(zp);
				
			}
			List sFiles=xmH.getSignatureFiles();
			for (Iterator iterator = sFiles.iterator(); iterator.hasNext();) {
				SignatureFile sfile = (SignatureFile) iterator.next();
				String fullPathName=sfile.getFullPath();
				ZipEntry ze=zp.getEntry(fullPathName);
				if(ze==null){
					//perhaps different separator
					fullPathName=fullPathName.replace('/', '\\');
					ze=zp.getEntry(fullPathName);
					if(ze==null){
						throw new IllegalArgumentException("Signaturefile "+sfile.getFullPath()+" not found in container");
					}
				}
				readSignature(sDoc, zp.getInputStream(ze));
			}
			
    		return sDoc;
    	} catch(DigiDocException ex) {
            throw ex; // already handled
        } catch(Exception ex) {
            DigiDocException.handleException(ex, DigiDocException.ERR_READ_FILE);
        }
		return null;
	}
	
	/**
	 * Not supported in BDOC
	 */
	public SignedDoc readSignedDoc(InputStream digiDocStream)
			throws DigiDocException {
		return null;
	}
	
	/**
	 * Verifies that the signers cert
	 * has been signed by at least one
	 * of the known root certs
	 * @param cert certificate to check
	 */
	public boolean verifyCertificate(X509Certificate cert)throws DigiDocException {
		boolean rc = false;
		if (m_rootCerts != null && !m_rootCerts.isEmpty()) {
			try {
				X509Certificate rCert =
					(X509Certificate) m_rootCerts.get(
							cert.getIssuerX500Principal().getName("RFC1779"));
				if (rCert != null) {
					cert.verify(rCert.getPublicKey());
					rc = true;
				}
			} catch (Exception ex) {
				DigiDocException.handleException(
					ex,
					DigiDocException.ERR_UNKNOWN_CA_CERT);
			}
		}
		return rc;
	}
	
	// A Kalev <2008 aprill> BDOCiga seotud muudatused xml-is 1.1
	
	/**
	 * Find types for used certificates
	 */
	private void findCertIDandCertValueTypes(Signature sig){
		if(m_logger.isDebugEnabled()){
			m_logger.debug("Sig: " + sig.getId() + " certids: " + sig.countCertIDs());
		}
		for(int i = 0; (sig != null) && (i < sig.countCertIDs()); i++) {
			CertID cid = sig.getCertID(i);
			if(m_logger.isDebugEnabled()) {
				m_logger.debug("CertId: " + cid.getId() + " type: " + cid.getType() + " nr: " + cid.getSerial());
			}
			if(cid.getType() == CertID.CERTID_TYPE_UNKNOWN) {
				CertValue cval = sig.findCertValueWithSerial(cid.getSerial());
				if(cval != null) {
					String cn = null;
					try {
						cn = SignedDoc.
						getCommonName(cval.getCert().getSubjectDN().getName());
						if(m_logger.isDebugEnabled())
							m_logger.debug("CertId type: " + cid.getType() + " nr: " + cid.getSerial() + " cval: " + cval.getId() + " CN: " + cn);
						if(ConvertUtils.isKnownOCSPCert(cn)) {
							if(m_logger.isInfoEnabled())
								m_logger.debug("Cert: " + cn + " is OCSP responders cert");
							cid.setType(CertID.CERTID_TYPE_RESPONDER);
							cval.setType(CertValue.CERTVAL_TYPE_RESPONDER);
						}
						if(ConvertUtils.isKnownTSACert(cn)) {
							if(m_logger.isDebugEnabled())
								m_logger.debug("Cert: " + cn + " is TSA cert");
							cid.setType(CertID.CERTID_TYPE_TSA);
							cval.setType(CertValue.CERTVAL_TYPE_TSA);
							if(m_logger.isDebugEnabled())
								m_logger.debug("CertId: " + cid.getId() + " type: " + cid.getType() + " nr: " + cid.getSerial());
						}
					} catch(DigiDocException ex) {
						m_logger.error("Error setting type on certid or certval: " + cn);
					}
				}
			}
			
		} // for i < sig.countCertIDs()
	    if(m_logger.isDebugEnabled()){
	    	m_logger.debug("Sig: " + sig.getId() + " certvals: " + sig.countCertValues());
	    }
		for(int i = 0; (sig != null) && (i < sig.countCertValues()); i++) {
			CertValue cval = sig.getCertValue(i);
			if(m_logger.isDebugEnabled()){
				m_logger.debug("CertValue: " + cval.getId() + " type: " + cval.getType());
			}
			if(cval.getType() == CertValue.CERTVAL_TYPE_UNKNOWN) {
				String cn = null;
				try {
					cn = SignedDoc.
					getCommonName(cval.getCert().getSubjectDN().getName());
					if(ConvertUtils.isKnownOCSPCert(cn)) {
						if(m_logger.isDebugEnabled()){
							m_logger.debug("Cert: " + cn + " is OCSP responders cert");
						}
						cval.setType(CertValue.CERTVAL_TYPE_RESPONDER);
					}
					if(ConvertUtils.isKnownTSACert(cn)) {
						if(m_logger.isDebugEnabled()){
							m_logger.debug("Cert: " + cn + " is TSA cert");
						}
						cval.setType(CertValue.CERTVAL_TYPE_TSA);
					}
				} catch(DigiDocException ex) {
					m_logger.error("Error setting type on certid or certval: " + cn);
				}					
			}
		}	
	}
	// L Kalev <2008 aprill> BDOCiga seotud muudatused xml-is 1.1
	
	public static void main(String [] args) throws IOException, DigiDocException, SAXException{
		//ConfigManager.init("\\work\\src\\jbdigiWspace\\JBDigiDocProject\\JDigiDoc-sample.cfg");
		ConfigManager.init("\\work\\src\\digidocWorkspace\\JBDigiDocProject\\JDigiDoc-sample.cfg");
		ConfigManager.instance().getBDigiDocFactory();
		//String bdocFile="C:\\work\\src\\digidocWorkspace\\naited\\test1.bdoc";
		String bdocFile="C:\\src\\kkDigidocWorkspace\\Testfailid_ITP\\Antsu bdoc\\ccid.bdoc";
		SimpleBDigiDocFactory sbdocf=new SimpleBDigiDocFactory();
		SignedDoc sd= sbdocf.readSignedDoc(bdocFile);
		System.out.println();
		System.out.println("ccid.bdoc");
		System.out.println("DataFile(s) "+sd.countDataFiles());
		System.out.println("Version "+sd.getVersion());
		System.out.println("Format "+sd.getFormat());
	}
}
