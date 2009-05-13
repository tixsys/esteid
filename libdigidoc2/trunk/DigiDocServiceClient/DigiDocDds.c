//==================================================
// FILE:	DigiDocDds.c
// PROJECT:     Digi Doc
// DESCRIPTION: Digi Doc functions for DigiDocService client
// AUTHOR:  Veiko Sinivee, S|E|B IT Partner Estonia
//==================================================
// Copyright (C) AS Sertifitseerimiskeskus
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// GNU Lesser General Public Licence is available at
// http://www.gnu.org/copyleft/lesser.html
//==========< HISTORY >=============================
//      07.01.2006      Veiko Sinivee
//                      Creation
//==================================================

#include "DigiDocDds.h"
#include <libdigidoc/DigiDocDefs.h>
#include <libdigidoc/DigiDocError.h>
#include <libdigidoc/DigiDocConfig.h>
#include <libdigidoc/DigiDocDebug.h>
#include <libdigidoc/DigiDocCert.h>
#include <libdigidoc/DigiDocConvert.h>
#include <libdigidoc/DigiDocGen.h>
#include <libdigidoc/DigiDocLib.h>
#include <libdigidoc/DigiDocSAXParser.h>
#include "DigiDocService.nsmap"
#include "ddsH.h"
#include "ddsStub.h"

// global flag to mark SSL init status
int g_nSslInited = 0;

//--------------------------------------------------
// Locates client PEM file containing client private
// key and cert for service access.
// szClientPem - buffer for found file name
// nLen - length of buffer
// returns ERR_OK on success
//--------------------------------------------------
int ddocLocateClientKeyAndCert(char* szClientPem, int nLen)
{
	int err = ERR_OK;
    X509* pCert = 0;
    EVP_PKEY *pkey = 0;
	char *pkcs12file = 0, *pkcs12passwd = 0, *p;

	// init return buffer
	memset(szClientPem, 0, nLen);
	pkcs12file = (char*)ConfigItem_lookup("DIGIDOC_PKCS_FILE");
	pkcs12passwd = (char*)ConfigItem_lookup("DIGIDOC_PKCS_PASSWD");
	ddocDebug(3, "ddocLocateClientKeyAndCert", "PKCS12: %s passwd: \'%s\'",
		(pkcs12file ? pkcs12file : "NULL"), (pkcs12passwd ? pkcs12passwd : "NONE"));
    if(pkcs12file) {
		// compose default pem file location
		strncpy(szClientPem, pkcs12file, nLen); 
		szClientPem[nLen] = 0;
		p = strrchr(szClientPem, '.');
		if(p) *p = 0;
		strncat(szClientPem, ".pem", nLen - strlen(szClientPem));
		szClientPem[nLen] = 0;
		// check if pem exists there
		err = checkFileExists(szClientPem);
		ddocDebug(3, "ddocLocateClientKeyAndCert", "Check PEM: %s exists: %d", szClientPem, err);
		// if it doesn't exist then attempt to create
		if(!err) {
			err = ReadCertificateByPKCS12(&pCert, pkcs12file, pkcs12passwd, &pkey);
			ddocDebug(3, "ddocLocateClientKeyAndCert", "Read p12: %s key: %s cert: %s",
				(pkcs12file ? pkcs12file : "NULL"), (pkey ? "OK" : "NO"), (pCert ? "OK" : "NO"));
			if(!err && pCert && pkey) {
				err = ddocWriteKeyAndCertPem(pkey, pCert, szClientPem, pkcs12passwd);
				ddocDebug(3, "ddocLocateClientKeyAndCert", "Write pem: %s - %d", szClientPem, err);
			}
			// if no permission to write then try temp file
			if(err) {
				szClientPem[0] = 0; // no luck
				if(pCert && pkey) {
					err = getTempFileName(szClientPem, nLen);
					if(!err) {
						err = ddocWriteKeyAndCertPem(pkey, pCert, szClientPem, pkcs12passwd);
						ddocDebug(3, "ddocLocateClientKeyAndCert", "Write pem: %s - %d", szClientPem, err);
					}
					if(err) szClientPem[0] = 0; // no luck
				}
			}
		} // else pem exists
		else
			err = 0;
	}
	if(pCert)
		X509_free(pCert);
	if(pkey)
		EVP_PKEY_free(pkey);

	return err;
}

//--------------------------------------------------
// Initializes DDS connection
// pDds - pointer to DDS context structure
// returns ERR_OK on success
//--------------------------------------------------
EXP_OPTION int ddocDdsConnect(DDS** ppDds)
{
  int err = ERR_OK, e2;
  char buf1[250], buf2[250];

  RETURN_IF_NULL_PARAM(ppDds);
  // alloc new object only if there is none
  if(!*ppDds) {
    // allocate new DDS context object
    *ppDds = (DDS*)malloc(sizeof(DDS));
    RETURN_IF_BAD_ALLOC(*ppDds);
    memset(*ppDds, 0, sizeof(DDS));
  }
  ddocDebug(3, "ddocDdsConnect", "Connecting to: %s", (char*)ConfigItem_lookup("DIGIDOC_DDS_ENDPOINT"));
  (*ppDds)->endpoint = (char*)ConfigItem_lookup("DIGIDOC_DDS_ENDPOINT");
  if((char*)ConfigItem_lookup("DIGIDOC_PROXY_HOST"))
	  ddocDebug(3, "ddocDdsConnect", "Using proxy: %s:%s", 
		(char*)ConfigItem_lookup("DIGIDOC_PROXY_HOST"),
		(char*)ConfigItem_lookup("DIGIDOC_PROXY_PORT"));
  (*ppDds)->soap.proxy_host = (char*)ConfigItem_lookup("DIGIDOC_PROXY_HOST");
  if(ConfigItem_lookup("DIGIDOC_PROXY_PORT"))
    (*ppDds)->soap.proxy_port = atoi(ConfigItem_lookup("DIGIDOC_PROXY_PORT"));
  //soap_init(&((*ppDds)->soap));
  soap_init2(&((*ppDds)->soap), SOAP_IO_KEEPALIVE | SOAP_IO_CHUNK | SOAP_C_UTFSTRING, 
	     SOAP_IO_KEEPALIVE | SOAP_C_UTFSTRING);
  (*ppDds)->soap.namespaces = namespaces;
  // if using HTTPS connection
#ifdef WIN32
   if((*ppDds)->endpoint && !_strnicmp((*ppDds)->endpoint, "https", 5)) {
#else
   if((*ppDds)->endpoint && !strncasecmp((*ppDds)->endpoint, "https", 5)) {
#endif
	// set client authentication
    err = ddocLocateClientKeyAndCert(buf2, sizeof(buf2));
    // compose DDS GW server cert path
	buf1[0] = 0;
	if(ConfigItem_lookup("DIGIDOC_DDS_CERT")) {
		strncpy(buf1, (char*)ConfigItem_lookup("CA_CERT_PATH"), sizeof(buf1));
		if(buf1[0]) {
#ifdef WIN32
			strcat(buf1, "\\");
#else
		strcat(buf1, "/");
#endif
		}
      strncat(buf1, (char*)ConfigItem_lookup("DIGIDOC_DDS_CERT"), sizeof(buf1)-strlen(buf1)-1); 
	}
    ddocDebug(3, "ddocDdsConnect", "Server cert: %s client-pem: %s", buf1, buf2);
	// init SSL if necessary
	if(!g_nSslInited) {
	  soap_ssl_init();
	  g_nSslInited = 1;
	}
    if((e2 = soap_ssl_client_context(&((*ppDds)->soap), 
	//	SOAP_SSL_NO_AUTHENTICATION, NULL, NULL, NULL,
			(ConfigItem_lookup("DIGIDOC_DDS_CERT") ? SOAP_SSL_DEFAULT : SOAP_SSL_NO_AUTHENTICATION),
	           (strlen(buf2) ? buf2 : NULL), // client auth pem
	           (strlen(buf2) ? (char*)ConfigItem_lookup("DIGIDOC_PKCS_PASSWD") : NULL), // client key passwd
		       (ConfigItem_lookup("DIGIDOC_DDS_CERT") ? buf1 : NULL), // server ca
			     NULL, // optional capath
			     NULL // no randfile
			     ))) {
		if(e2 == SOAP_SSL_ERROR)
			err = ERR_WPKI_UNTRUSTED_SRVICE;
		else
			err = ERR_GENERIC_SOAP_ERR;
      soap_print_fault(&((*ppDds)->soap), stderr);
      return err; // ???
    }
  }
  
  return err;
}

//--------------------------------------------------
// Cleanup Certificate info
// pInfo - cert info pointer
//--------------------------------------------------
void ddocDdsCleanupCertificateInfo(struct d__CertificateInfo *pInfo)
{
  int i;
  if(pInfo) {
    if(pInfo->Issuer)
      free(pInfo->Issuer);
    if(pInfo->Subject)
      free(pInfo->Subject);
    if(pInfo->ValidFrom)
      free(pInfo->ValidFrom);
    if(pInfo->ValidTo)
      free(pInfo->ValidTo);
    if(pInfo->IssuerSerial)
      free(pInfo->IssuerSerial);
    if(pInfo->__ptrPolicies && pInfo->__sizePolicies > 0) {
      for(i = 0; i < pInfo->__sizePolicies; i++) {
	if(pInfo->__ptrPolicies[i].OID)
	  free(pInfo->__ptrPolicies[i].OID);
	if(pInfo->__ptrPolicies[i].URL)
	  free(pInfo->__ptrPolicies[i].URL);
	if(pInfo->__ptrPolicies[i].Description)
	  free(pInfo->__ptrPolicies[i].Description);	
      }
      free(pInfo->__ptrPolicies);
    }
    free(pInfo);
  }
}

//--------------------------------------------------
// Cleanup DDS connection error & fault info
// pDds - pointer to DDS context structure
//--------------------------------------------------
int ddocDdsCleanupStatusAndFault(DDS* pDds)
{
  RETURN_IF_NULL_PARAM(pDds);
  pDds->err = ERR_OK;
  if(pDds->szStatus) {
    free(pDds->szStatus);
    pDds->szStatus = 0;
  }
  if(pDds->szStatusCode) {
    free(pDds->szStatusCode);
    pDds->szStatusCode = 0;
  }
  if(pDds->szSoapFaultCode) {
    free(pDds->szSoapFaultCode);
    pDds->szSoapFaultCode = 0;
  }
  if(pDds->szSoapFaultString) {
    free(pDds->szSoapFaultString);
    pDds->szSoapFaultString = 0;
  }
  if(pDds->szSoapFaultDetail) {
    free(pDds->szSoapFaultDetail);
    pDds->szSoapFaultDetail = 0;
  }
  return ERR_OK;
}

//--------------------------------------------------
// Cleanup DDS connection status & signed doc info
// pDds - pointer to DDS context structure
//--------------------------------------------------
EXP_OPTION int ddocDdsCleanupInfo(DDS* pDds)
{
  int err = ERR_OK, i, j;

  RETURN_IF_NULL_PARAM(pDds);
  // cleanup signed doc info
  if(pDds->pSigDocInfo) {
    // format
    if(pDds->pSigDocInfo->Format)
      free(pDds->pSigDocInfo->Format);
    // version
    if(pDds->pSigDocInfo->Version)
      free(pDds->pSigDocInfo->Version);
    // data files
    if(pDds->pSigDocInfo->__ptrDataFileInfo && 
       pDds->pSigDocInfo->__sizeOfDataFileInfos > 0) {
      for(i = 0; i < pDds->pSigDocInfo->__sizeOfDataFileInfos; i++) {
	if(pDds->pSigDocInfo->__ptrDataFileInfo[i].Id)
	  free(pDds->pSigDocInfo->__ptrDataFileInfo[i].Id);
	if(pDds->pSigDocInfo->__ptrDataFileInfo[i].Filename)
	  free(pDds->pSigDocInfo->__ptrDataFileInfo[i].Filename);
	if(pDds->pSigDocInfo->__ptrDataFileInfo[i].MimeType)
	  free(pDds->pSigDocInfo->__ptrDataFileInfo[i].MimeType);
	if(pDds->pSigDocInfo->__ptrDataFileInfo[i].ContentType)
	  free(pDds->pSigDocInfo->__ptrDataFileInfo[i].ContentType);
	if(pDds->pSigDocInfo->__ptrDataFileInfo[i].DigestType)
	  free(pDds->pSigDocInfo->__ptrDataFileInfo[i].DigestType);
	if(pDds->pSigDocInfo->__ptrDataFileInfo[i].DigestValue)
	  free(pDds->pSigDocInfo->__ptrDataFileInfo[i].DigestValue);
	// data file atributes
	if(pDds->pSigDocInfo->__ptrDataFileInfo[i].__ptrAttributes &&
	   pDds->pSigDocInfo->__ptrDataFileInfo[i].__sizeOfAttributes > 0) {
	  for(j = 0; j < pDds->pSigDocInfo->__ptrDataFileInfo[i].__sizeOfAttributes; j++) {
	    if(pDds->pSigDocInfo->__ptrDataFileInfo[i].__ptrAttributes[j].Name)
	      free(pDds->pSigDocInfo->__ptrDataFileInfo[i].__ptrAttributes[j].Name);
	    if(pDds->pSigDocInfo->__ptrDataFileInfo[i].__ptrAttributes[j].Value)
	      free(pDds->pSigDocInfo->__ptrDataFileInfo[i].__ptrAttributes[j].Value);
	  }
	  free(pDds->pSigDocInfo->__ptrDataFileInfo[i].__ptrAttributes);
	}
      }
      free(pDds->pSigDocInfo->__ptrDataFileInfo);
    } // data files
    // signatures
    if(pDds->pSigDocInfo->__ptrSignatureInfo && 
       pDds->pSigDocInfo->__sizeOfSignatureInfos > 0) {
      for(i = 0; i < pDds->pSigDocInfo->__sizeOfSignatureInfos; i++) {
	if(pDds->pSigDocInfo->__ptrSignatureInfo[i].Id)
	  free(pDds->pSigDocInfo->__ptrSignatureInfo[i].Id);
	if(pDds->pSigDocInfo->__ptrSignatureInfo[i].Status)
	  free(pDds->pSigDocInfo->__ptrSignatureInfo[i].Status);
	if(pDds->pSigDocInfo->__ptrSignatureInfo[i].SigningTime)
	  free(pDds->pSigDocInfo->__ptrSignatureInfo[i].SigningTime);
	// signature error
	if(pDds->pSigDocInfo->__ptrSignatureInfo[i].Error) {
	  if(pDds->pSigDocInfo->__ptrSignatureInfo[i].Error->Category)
	    free(pDds->pSigDocInfo->__ptrSignatureInfo[i].Error->Category);
	  if(pDds->pSigDocInfo->__ptrSignatureInfo[i].Error->Description)
	    free(pDds->pSigDocInfo->__ptrSignatureInfo[i].Error->Description);
	  free(pDds->pSigDocInfo->__ptrSignatureInfo[i].Error);
	}
	// roles
	if(pDds->pSigDocInfo->__ptrSignatureInfo[i].__ptrSignerRole &&
	   pDds->pSigDocInfo->__ptrSignatureInfo[i].__sizeRoles) {
	  for(j = 0; j < pDds->pSigDocInfo->__ptrSignatureInfo[i].__sizeRoles; j++) {
	    if(pDds->pSigDocInfo->__ptrSignatureInfo[i].__ptrSignerRole[j].Role)
	      free(pDds->pSigDocInfo->__ptrSignatureInfo[i].__ptrSignerRole[j].Role);
	  }
	  free(pDds->pSigDocInfo->__ptrSignatureInfo[i].__ptrSignerRole);
	}
	// signature production place
	if(pDds->pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace) {
	  if(pDds->pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace->City)
	    free(pDds->pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace->City);
	  if(pDds->pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace->StateOrProvince)
	    free(pDds->pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace->StateOrProvince);
	  if(pDds->pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace->PostalCode)
	    free(pDds->pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace->PostalCode);
	  if(pDds->pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace->CountryName)
	    free(pDds->pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace->CountryName);
	  free(pDds->pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace);
	}
	// Signer
	if(pDds->pSigDocInfo->__ptrSignatureInfo[i].Signer) {
	  if(pDds->pSigDocInfo->__ptrSignatureInfo[i].Signer->CommonName)
	    free(pDds->pSigDocInfo->__ptrSignatureInfo[i].Signer->CommonName);
	  if(pDds->pSigDocInfo->__ptrSignatureInfo[i].Signer->IDCode)
	    free(pDds->pSigDocInfo->__ptrSignatureInfo[i].Signer->IDCode);
	  // Signers cert
	  ddocDdsCleanupCertificateInfo(pDds->pSigDocInfo->__ptrSignatureInfo[i].Signer->Certificate);
	  free(pDds->pSigDocInfo->__ptrSignatureInfo[i].Signer);
	} // Signer
	// Confirmation
	if(pDds->pSigDocInfo->__ptrSignatureInfo[i].Confirmation) {
	  if(pDds->pSigDocInfo->__ptrSignatureInfo[i].Confirmation->ResponderID)
	    free(pDds->pSigDocInfo->__ptrSignatureInfo[i].Confirmation->ResponderID);
	  if(pDds->pSigDocInfo->__ptrSignatureInfo[i].Confirmation->ProducedAt)
	    free(pDds->pSigDocInfo->__ptrSignatureInfo[i].Confirmation->ProducedAt);
	  // Notarys cert
	  ddocDdsCleanupCertificateInfo(pDds->pSigDocInfo->__ptrSignatureInfo[i].Confirmation->ResponderCertificate);
	  free(pDds->pSigDocInfo->__ptrSignatureInfo[i].Confirmation);
	} // Confirmation
      } // for signatures
      free(pDds->pSigDocInfo->__ptrSignatureInfo);
    } // signatures
    // set pointer to 0
    pDds->pSigDocInfo = 0;
  }
  return err;
}

//--------------------------------------------------
// Cleanup DDS connection but don't disconnect
// pDds - pointer to DDS context structure
// returns ERR_OK on success
//--------------------------------------------------
EXP_OPTION int ddocDdsCleanup(DDS* pDds)
{
  int err = ERR_OK;
  ddocDebug(3, "ddocDdsCleanup", "cleanup");
  if(pDds) {
  ddocDebug(3, "ddocDdsCleanup", "cleanup: %s", (pDds->endpoint ? pDds->endpoint : ""));
  soap_destroy(&(pDds->soap));
  soap_end(&(pDds->soap));
  soap_done(&(pDds->soap));
  }
  //pDds->endpoint = 0; // mark disconnected
  return err;
}

//--------------------------------------------------
// Cleanup DDS connection
// pDds - pointer to DDS context structure
// returns ERR_OK on success
//--------------------------------------------------
EXP_OPTION int ddocDdsDisconnect(DDS* pDds)
{
  int err = ERR_OK;
  // cleanup cached info
  if(pDds)
    ddocDdsCleanupInfo(pDds);
  ddocDebug(3, "ddocDdsDisconnect", "Disconnecting");
  if(pDds && pDds->endpoint) {
    ddocDebug(3, "ddocDdsDisconnect", "Disconnecting from: %s", pDds->endpoint);
    soap_destroy(&(pDds->soap));
    soap_end(&(pDds->soap));
    soap_done(&(pDds->soap));
    pDds->endpoint = 0; // mark disconnected
  }
  if(pDds)
    free(pDds);
  return err;
}


//--------------------------------------------------
// Copies Certificate info from soap response
// ppDest - address of place for new pointer
// pSrc - cert info in soap response
// return error code or ERR_OK;
//--------------------------------------------------
int ddocDdsCopyCertificateInfo(struct d__CertificateInfo **ppDest, struct d__CertificateInfo* pSrc)
{
  int err = ERR_OK, i;
  RETURN_IF_NULL_PARAM(ppDest);
  if(pSrc) {
    *ppDest = (struct d__CertificateInfo*)malloc(sizeof(struct d__CertificateInfo));
    RETURN_IF_BAD_ALLOC(*ppDest);
    memset(*ppDest, 0, sizeof(struct d__CertificateInfo));
    if(pSrc->Issuer)
      (*ppDest)->Issuer = strdup(pSrc->Issuer);
    if(pSrc->Subject)
      (*ppDest)->Subject = strdup(pSrc->Subject);
    if(pSrc->ValidFrom)
      (*ppDest)->ValidFrom = strdup(pSrc->ValidFrom);
    if(pSrc->ValidTo)
      (*ppDest)->ValidTo = strdup(pSrc->ValidTo);
    if(pSrc->IssuerSerial)
      (*ppDest)->IssuerSerial = strdup(pSrc->IssuerSerial);
    if(pSrc->__ptrPolicies && pSrc->__sizePolicies > 0) {
      (*ppDest)->__ptrPolicies = (struct d__CertificatePolicy*)
	malloc(sizeof(struct d__CertificatePolicy) * pSrc->__sizePolicies);
      RETURN_IF_BAD_ALLOC((*ppDest)->__ptrPolicies);
      memset((*ppDest)->__ptrPolicies, 0, sizeof(struct d__CertificatePolicy) * pSrc->__sizePolicies);
      (*ppDest)->__sizePolicies = pSrc->__sizePolicies;
      for(i = 0; i < pSrc->__sizePolicies; i++) {
	if(pSrc->__ptrPolicies[i].OID)
	  (*ppDest)->__ptrPolicies[i].OID = strdup(pSrc->__ptrPolicies[i].OID);
	if(pSrc->__ptrPolicies[i].URL)
	  (*ppDest)->__ptrPolicies[i].URL = strdup(pSrc->__ptrPolicies[i].URL);
	if(pSrc->__ptrPolicies[i].Description)
	  (*ppDest)->__ptrPolicies[i].Description = strdup(pSrc->__ptrPolicies[i].Description);	
      }
    }
  }
  return err;
}

//--------------------------------------------------
// Retrieves SOAP fault string
// pDds - pointer to DDS context structure
// returns SOAP fault string or NULL
//--------------------------------------------------
char* ddocDdsGetSoapFaultString(DDS* pDds) 
{
  char *s = NULL, **p = NULL;
  if(pDds && pDds->soap.error) {
    ddocDebug(4, "ddocDdsGetSoapFaultString", "Get fault for err: %d", pDds->soap);
    p = (char**)soap_faultstring(&(pDds->soap));
    ddocDebug(4, "ddocDdsGetSoapFaultString", "p: %s, s: %s", 
	      (p ? "OK" : "NULL"), ((p && *p) ? *p : "NULL"));
    if(p && *p)
      s = (char*)*p;
  }
  return s;
}

//--------------------------------------------------
// Retrieves SOAP fault code
// pDds - pointer to DDS context structure
// returns SOAP fault code or NULL
//--------------------------------------------------
char* ddocDdsGetSoapFaultCode(DDS* pDds) 
{
  char *s = NULL, **d = NULL;
  if(pDds && pDds->soap.error) {
    d = (char**)soap_faultcode(&(pDds->soap));
    if(d && *d)
      s = *d;
  }
  return s;
}

//--------------------------------------------------
// Retrieves SOAP fault detail
// pDds - pointer to DDS context structure
// returns SOAP fault detail or NULL
//--------------------------------------------------
char* ddocDdsGetSoapFaultDetail(DDS* pDds) 
{
  char *s = NULL, **d = NULL;
  if(pDds && pDds->soap.error) {
    d = (char**)soap_faultdetail(&(pDds->soap));
    if(d && *d)
      s = *d;
  }
  return s;
}

//--------------------------------------------------
// Copy response status, error code and soap fault data
// pDds - pointer to DDS context structure
// szStatus - response status as returned by service
// err - error code returned by service
//--------------------------------------------------
int ddocDdsCopyStatusAndFault(DDS* pDds, char* szStatus, char* szStatusCode, int err)
{
  char *s;
  RETURN_IF_NULL_PARAM(pDds);
  // cleanup old info
  ddocDdsCleanupStatusAndFault(pDds);
  // copy status
  if(szStatus)
    pDds->szStatus = strdup(szStatus);
  if(szStatusCode)
    pDds->szStatusCode = strdup(szStatusCode);
  // error
  pDds->err = err;
  // soap fault info
  if(err) {
    s = ddocDdsGetSoapFaultCode(pDds);
    if(s)
      pDds->szSoapFaultCode = strdup(s);
    s = ddocDdsGetSoapFaultString(pDds);
    if(s)
      pDds->szSoapFaultString = strdup(s);
    s = ddocDdsGetSoapFaultDetail(pDds);
    if(s)
      pDds->szSoapFaultDetail = strdup(s);
  }
  return ERR_OK;
}


//--------------------------------------------------
// Copies SignedDoc Info from soap response to
// DDS structure so it can be further examined
// in client application. Internal function.
// pDds - pointer to DDS context structure
// pSigDocInfo - signed doc info from soap
// return error code or ERR_OK;
//--------------------------------------------------
int ddocDdsCopySoapRespInfo(DDS* pDds, struct d__SignedDocInfo* pSigDocInfo)
{
  int err = ERR_OK, i, j;

  RETURN_IF_NULL_PARAM(pDds);
  // cleanup old info
  ddocDdsCleanupInfo(pDds);
  // copy signed doc info
  if(pSigDocInfo) {
    pDds->pSigDocInfo = (struct d__SignedDocInfo*)malloc(sizeof(struct d__SignedDocInfo));
    RETURN_IF_BAD_ALLOC(pDds->pSigDocInfo);
    memset(pDds->pSigDocInfo, 0, sizeof(struct d__SignedDocInfo));
    // format
    if(pSigDocInfo->Format)
      pDds->pSigDocInfo->Format = strdup(pSigDocInfo->Format);
    // version
    if(pSigDocInfo->Version)
      pDds->pSigDocInfo->Version = strdup(pSigDocInfo->Version);
    // data files
    if(pSigDocInfo->__ptrDataFileInfo && pSigDocInfo->__sizeOfDataFileInfos > 0) {
      pDds->pSigDocInfo->__ptrDataFileInfo = (struct d__DataFileInfo*)
	malloc(sizeof(struct d__DataFileInfo) * pSigDocInfo->__sizeOfDataFileInfos);
      RETURN_IF_BAD_ALLOC(pDds->pSigDocInfo->__ptrDataFileInfo);
      memset(pDds->pSigDocInfo->__ptrDataFileInfo, 0,
	     sizeof(struct d__DataFileInfo) * pSigDocInfo->__sizeOfDataFileInfos);
      pDds->pSigDocInfo->__sizeOfDataFileInfos = pSigDocInfo->__sizeOfDataFileInfos;
      for(i = 0; i < pSigDocInfo->__sizeOfDataFileInfos; i++) {
	if(pSigDocInfo->__ptrDataFileInfo[i].Id)
	  pDds->pSigDocInfo->__ptrDataFileInfo[i].Id = strdup(pSigDocInfo->__ptrDataFileInfo[i].Id);
	if(pSigDocInfo->__ptrDataFileInfo[i].Filename)
	  pDds->pSigDocInfo->__ptrDataFileInfo[i].Filename = strdup(pSigDocInfo->__ptrDataFileInfo[i].Filename);
	if(pSigDocInfo->__ptrDataFileInfo[i].MimeType)
	  pDds->pSigDocInfo->__ptrDataFileInfo[i].MimeType = strdup(pSigDocInfo->__ptrDataFileInfo[i].MimeType);
	if(pSigDocInfo->__ptrDataFileInfo[i].ContentType)
	  pDds->pSigDocInfo->__ptrDataFileInfo[i].ContentType = strdup(pSigDocInfo->__ptrDataFileInfo[i].ContentType);
	if(pSigDocInfo->__ptrDataFileInfo[i].DigestType)
	  pDds->pSigDocInfo->__ptrDataFileInfo[i].DigestType = strdup(pSigDocInfo->__ptrDataFileInfo[i].DigestType);
	if(pSigDocInfo->__ptrDataFileInfo[i].DigestValue)
	  pDds->pSigDocInfo->__ptrDataFileInfo[i].DigestValue = strdup(pSigDocInfo->__ptrDataFileInfo[i].DigestValue);
	pDds->pSigDocInfo->__ptrDataFileInfo[i].Size = pSigDocInfo->__ptrDataFileInfo[i].Size;
	// data file atributes
	if(pSigDocInfo->__ptrDataFileInfo[i].__ptrAttributes &&
	   pSigDocInfo->__ptrDataFileInfo[i].__sizeOfAttributes > 0) {
	  pDds->pSigDocInfo->__ptrDataFileInfo[i].__ptrAttributes = (struct d__DataFileAttribute*)
	    malloc(sizeof(struct d__DataFileAttribute) * pSigDocInfo->__ptrDataFileInfo[i].__sizeOfAttributes);
	  RETURN_IF_BAD_ALLOC(pDds->pSigDocInfo->__ptrDataFileInfo[i].__ptrAttributes);
	  memset(pDds->pSigDocInfo->__ptrDataFileInfo[i].__ptrAttributes, 0, 
		 sizeof(struct d__DataFileAttribute) * pSigDocInfo->__ptrDataFileInfo[i].__sizeOfAttributes);
	  pDds->pSigDocInfo->__ptrDataFileInfo[i].__sizeOfAttributes = 
	    pSigDocInfo->__ptrDataFileInfo[i].__sizeOfAttributes;
	  for(j = 0; j < pSigDocInfo->__ptrDataFileInfo[i].__sizeOfAttributes; j++) {
	    if(pSigDocInfo->__ptrDataFileInfo[i].__ptrAttributes[j].Name)
	      pDds->pSigDocInfo->__ptrDataFileInfo[i].__ptrAttributes[j].Name = 
		strdup(pSigDocInfo->__ptrDataFileInfo[i].__ptrAttributes[j].Name);
	    if(pSigDocInfo->__ptrDataFileInfo[i].__ptrAttributes[j].Value)
	      pDds->pSigDocInfo->__ptrDataFileInfo[i].__ptrAttributes[j].Value = 
		strdup(pSigDocInfo->__ptrDataFileInfo[i].__ptrAttributes[j].Value);
	  }
	}
      }
    } // data files
    // signatures
    if(pSigDocInfo->__ptrSignatureInfo && pSigDocInfo->__sizeOfSignatureInfos > 0) {
      pDds->pSigDocInfo->__ptrSignatureInfo = (struct d__SignatureInfo*)
	malloc(sizeof(struct d__SignatureInfo) * pSigDocInfo->__sizeOfSignatureInfos);
      RETURN_IF_BAD_ALLOC(pDds->pSigDocInfo->__ptrSignatureInfo);
      memset(pDds->pSigDocInfo->__ptrSignatureInfo, 0, 
	     sizeof(struct d__SignatureInfo) * pSigDocInfo->__sizeOfSignatureInfos);
      pDds->pSigDocInfo->__sizeOfSignatureInfos = pSigDocInfo->__sizeOfSignatureInfos;
      // signatures
      for(i = 0; i < pSigDocInfo->__sizeOfSignatureInfos; i++) {
	if(pSigDocInfo->__ptrSignatureInfo[i].Id)
	  pDds->pSigDocInfo->__ptrSignatureInfo[i].Id = strdup(pSigDocInfo->__ptrSignatureInfo[i].Id);
	if(pSigDocInfo->__ptrSignatureInfo[i].Status)
	  pDds->pSigDocInfo->__ptrSignatureInfo[i].Status = strdup(pSigDocInfo->__ptrSignatureInfo[i].Status);
	if(pSigDocInfo->__ptrSignatureInfo[i].SigningTime)
	  pDds->pSigDocInfo->__ptrSignatureInfo[i].SigningTime = strdup(pSigDocInfo->__ptrSignatureInfo[i].SigningTime);
	// signature error
	if(pSigDocInfo->__ptrSignatureInfo[i].Error) {
	  pDds->pSigDocInfo->__ptrSignatureInfo[i].Error = (struct d__Error*)malloc(sizeof(struct d__Error));
	  RETURN_IF_BAD_ALLOC(pDds->pSigDocInfo->__ptrSignatureInfo[i].Error);
	  memset(pDds->pSigDocInfo->__ptrSignatureInfo[i].Error, 0, sizeof(struct d__Error));
	  if(pSigDocInfo->__ptrSignatureInfo[i].Error->Category)
	    pDds->pSigDocInfo->__ptrSignatureInfo[i].Error->Category = 
	      strdup(pSigDocInfo->__ptrSignatureInfo[i].Error->Category);
	  if(pSigDocInfo->__ptrSignatureInfo[i].Error->Description)
	    pDds->pSigDocInfo->__ptrSignatureInfo[i].Error->Description = 
	      strdup(pSigDocInfo->__ptrSignatureInfo[i].Error->Description);
	  pDds->pSigDocInfo->__ptrSignatureInfo[i].Error->Code = 
	    pSigDocInfo->__ptrSignatureInfo[i].Error->Code;
	}
	// roles
	if(pSigDocInfo->__ptrSignatureInfo[i].__ptrSignerRole &&
	   pSigDocInfo->__ptrSignatureInfo[i].__sizeRoles) {
	  pDds->pSigDocInfo->__ptrSignatureInfo[i].__ptrSignerRole = (struct d__SignerRole*)
	    malloc(sizeof(struct d__SignerRole) * pSigDocInfo->__ptrSignatureInfo[i].__sizeRoles);
	  RETURN_IF_BAD_ALLOC(pDds->pSigDocInfo->__ptrSignatureInfo[i].__ptrSignerRole);
	  memset(pDds->pSigDocInfo->__ptrSignatureInfo[i].__ptrSignerRole, 0,
		 sizeof(struct d__SignerRole) * pSigDocInfo->__ptrSignatureInfo[i].__sizeRoles);
	  pDds->pSigDocInfo->__ptrSignatureInfo[i].__sizeRoles = 
	    pSigDocInfo->__ptrSignatureInfo[i].__sizeRoles;
	  for(j = 0; j < pSigDocInfo->__ptrSignatureInfo[i].__sizeRoles; j++) {
	    pDds->pSigDocInfo->__ptrSignatureInfo[i].__ptrSignerRole[j].Certified =
	      pSigDocInfo->__ptrSignatureInfo[i].__ptrSignerRole[j].Certified;
	    if(pSigDocInfo->__ptrSignatureInfo[i].__ptrSignerRole[j].Role)
	      pDds->pSigDocInfo->__ptrSignatureInfo[i].__ptrSignerRole[j].Role = 
		strdup(pSigDocInfo->__ptrSignatureInfo[i].__ptrSignerRole[j].Role);
	  }
	}
	// signature production place
	if(pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace) {
	  pDds->pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace = (struct d__SignatureProductionPlace*)
	    malloc(sizeof(struct d__SignatureProductionPlace));
	  RETURN_IF_BAD_ALLOC(pDds->pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace);
	  memset(pDds->pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace, 0, sizeof(struct d__SignatureProductionPlace));
	  if(pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace->City)
	    pDds->pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace->City =
	      strdup(pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace->City);
	  if(pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace->StateOrProvince)
	    pDds->pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace->StateOrProvince =
	      strdup(pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace->StateOrProvince);
	  if(pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace->PostalCode)
	    pDds->pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace->PostalCode =
	      strdup(pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace->PostalCode);
	  if(pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace->CountryName)
	    pDds->pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace->CountryName =
	      strdup(pSigDocInfo->__ptrSignatureInfo[i].SignatureProductionPlace->CountryName);
	}
	// Signer
	if(pSigDocInfo->__ptrSignatureInfo[i].Signer) {
	  pDds->pSigDocInfo->__ptrSignatureInfo[i].Signer = (struct d__SignerInfo*)
	    malloc(sizeof(struct d__SignerInfo));
	  RETURN_IF_BAD_ALLOC(pDds->pSigDocInfo->__ptrSignatureInfo[i].Signer);
	  memset(pDds->pSigDocInfo->__ptrSignatureInfo[i].Signer, 0, sizeof(struct d__SignerInfo));
	  if(pSigDocInfo->__ptrSignatureInfo[i].Signer->CommonName)
	    pDds->pSigDocInfo->__ptrSignatureInfo[i].Signer->CommonName =
	      strdup(pSigDocInfo->__ptrSignatureInfo[i].Signer->CommonName);
	  if(pSigDocInfo->__ptrSignatureInfo[i].Signer->IDCode)
	    pDds->pSigDocInfo->__ptrSignatureInfo[i].Signer->IDCode =
	      strdup(pSigDocInfo->__ptrSignatureInfo[i].Signer->IDCode);
	  // Signers cert
	  ddocDdsCopyCertificateInfo(&(pDds->pSigDocInfo->__ptrSignatureInfo[i].Signer->Certificate),
				     pSigDocInfo->__ptrSignatureInfo[i].Signer->Certificate);
	} // Signer
	// Confirmation
	if(pSigDocInfo->__ptrSignatureInfo[i].Confirmation) {
	  pDds->pSigDocInfo->__ptrSignatureInfo[i].Confirmation = (struct d__ConfirmationInfo*)
	    malloc(sizeof(struct d__ConfirmationInfo));
	  RETURN_IF_BAD_ALLOC(pDds->pSigDocInfo->__ptrSignatureInfo[i].Confirmation);
	  memset(pDds->pSigDocInfo->__ptrSignatureInfo[i].Confirmation, 0, sizeof(struct d__ConfirmationInfo));
	  if(pSigDocInfo->__ptrSignatureInfo[i].Confirmation->ResponderID)
	    pDds->pSigDocInfo->__ptrSignatureInfo[i].Confirmation->ResponderID =
	      strdup(pSigDocInfo->__ptrSignatureInfo[i].Confirmation->ResponderID);
	  if(pSigDocInfo->__ptrSignatureInfo[i].Confirmation->ProducedAt)
	    pDds->pSigDocInfo->__ptrSignatureInfo[i].Confirmation->ProducedAt =
	      strdup(pSigDocInfo->__ptrSignatureInfo[i].Confirmation->ProducedAt);
	  // Notarys cert
	  ddocDdsCopyCertificateInfo(&(pDds->pSigDocInfo->__ptrSignatureInfo[i].Confirmation->ResponderCertificate),
				     pSigDocInfo->__ptrSignatureInfo[i].Confirmation->ResponderCertificate);
	} // Confirmation
      } // for signatures
    } // signatures
  }
  //  err = ddocDdsCopySigDocInfo(pDds, pSigDocInfo);
  return err;
}


//--------------------------------------------------
// Internal function. Initializes a DataFileData structure
// with new data file data
// sFileName - full path and filename of any new data file
// szMimeType - mime type of new data file
// szContentType - content type of new data file
// returns error code or SOAP_OK
//--------------------------------------------------
int initDataFileWithFile(struct d__DataFileData* pDf, const char* szFileName, 
			 const char* szMimeType, const char* szContentType)
{
  int err = SOAP_OK, l1, l2;
  long lFileSize;
  DigiDocMemBuf mbuf1;
  char *p1 = 0, buf1[30];

  mbuf1.pMem = 0;
  mbuf1.nLen = 0;
  ddocDebug(3, "initDataFileWithFile", 
	    "df-file: %s df-mime: %s df-content: %s", 
	    (szFileName ? szFileName : "NULL"), (szMimeType ? szMimeType : "NULL"),
	    (szContentType ? szContentType : "NULL"));
  RETURN_IF_NULL_PARAM(pDf);
  RETURN_IF_NULL_PARAM(szFileName);
  RETURN_IF_NULL_PARAM(szMimeType);
  RETURN_IF_NULL_PARAM(szContentType);
  // init output struct
  memset(pDf, 0, sizeof(struct d__DataFileData));
  // check for existence of file and calculate it's size
  err = calculateFileSize(szFileName, &lFileSize);
  pDf->Size = lFileSize;
  pDf->MimeType = (char*)szMimeType;
  pDf->ContentType = (char*)szContentType;
  pDf->Filename = (char*)getSimpleFileName(szFileName);
  // for embedded cases read in file contents
  if(!strcmp(szContentType, CONTENT_EMBEDDED) ||
     !strcmp(szContentType, CONTENT_EMBEDDED_BASE64)) {
    err = ddocReadFile(szFileName, &mbuf1);
    if(!strcmp(szContentType, CONTENT_EMBEDDED)) {
      pDf->DfData = (char*)mbuf1.pMem; // caller must free this!
      mbuf1.pMem = 0;
      mbuf1.nLen = 0; // release ownership of this mem-block
    }
    if(!strcmp(szContentType, CONTENT_EMBEDDED_BASE64)) {
      l1 = mbuf1.nLen * 2;
      p1 = (char*)malloc(l1);
      if(p1) {
	memset(p1, 0, l1);
	encode((const byte*)mbuf1.pMem, mbuf1.nLen, (byte*)p1, &l1);
	pDf->DfData = (char*)p1; // caller must free this 
      }
    }
  }
  if(!strcmp(szContentType, CONTENT_DETATCHED)) {
    // this is a static string, don't free
    pDf->DigestType = DIGEST_SHA1_NAME;
    l1 = sizeof(buf1);
    err = calculateFileDigest(szFileName, DIGEST_SHA1,
			      (byte*)buf1, &l1, &lFileSize);
    l2 = l1 * 2;
    pDf->DigestValue = (char*)malloc(l2);
    if(pDf->DigestValue) {
      memset(pDf->DigestValue, 0, l2);
      encode((const byte*)buf1, l1, (byte*)pDf->DigestValue, &l2);
    }
  }
  // cleanup
  ddocMemBuf_free(&mbuf1);
  return err;
}

//--------------------------------------------------
// Cleanup DataFileData struct
// pDf - pointer to struct
// return error code or ERR_OK
//--------------------------------------------------
int cleanupDataFileData(struct d__DataFileData* pDf)
{
  int err = ERR_OK;

  RETURN_IF_NULL_PARAM(pDf);
  if(pDf->DigestValue)
    free(pDf->DigestValue);
  if(pDf->DfData)
    free(pDf->DfData);
  // the rest is static strings
  return err;
}


//--------------------------------------------------
// Sends an DDS request to start a new session
// Optionally loads an existing digidoc file or
// just any other file and creates digidoc based on it.
// pDds - pointer to DDS context structure
// bHoldSession - 1=hold session, 0=terminate after this call
// szSigningProfile - signing profile URL
// szDigiDocFile - full path and filename of digidoc document
// szDataFile - full path and filename of any new data file
// szDfMimeType - mime type of new data file
// szDfContentType - content type of new data file
// pMBufDdoc - memory buffer with digidoc xml data to send (optional)
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsStartSession(DDS* pDds, int bHoldSession,
				   const char* szSigningProfile,
				   const char* szDigiDocFile, const char* szDataFile, 
				   const char* szDfMimeType, const char* szDfContentType,
				   DigiDocMemBuf* pMBufDdoc)
{
  int err = SOAP_OK;
  struct d__DataFileData df;
  struct d__StartSessionResponse resp;
  DigiDocMemBuf mbuf1;

  mbuf1.pMem = 0;
  mbuf1.nLen = 0;
  RETURN_IF_NULL_PARAM(pDds);
  memset(&df, 0, sizeof(struct d__DataFileData));
  memset(&resp, 0, sizeof(struct d__StartSessionResponse));
  ddocDebug(3, "ddocDdsStartSession", 
	    "Hold sess: %d digidoc: %s, mbuf: %s, df: %s df-mime: %s df-content: %s", 
	    bHoldSession, (szDigiDocFile ? szDigiDocFile : "NULL"), (pMBufDdoc ? "OK" : "NULL"),
	    (szDataFile ? szDataFile : "NULL"), (szDfMimeType ? szDfMimeType : "NULL"),
	    (szDfContentType ? szDfContentType : "NULL"));
  // cleanup old info if applicable
  //ddocDdsCleanupStatusAndFault(pDds);
  //ddocDdsCleanupInfo(pDds);
  // init parameters
  if(szDigiDocFile && !pMBufDdoc) {
    err = ddocReadFile(szDigiDocFile, &mbuf1);
  } else if(szDataFile) {
    err = initDataFileWithFile(&df, szDataFile, szDfMimeType, szDfContentType);
  }
  // send DDS request
  ddocDebug(3, "ddocDdsStartSession", "Sending request, err: %d!", err);
  if(!err)
  err = soap_call_d__StartSession(&(pDds->soap), 
				  (const char*)pDds->endpoint, "",
				  (char*)szSigningProfile,
				  (pMBufDdoc ? (char*)pMBufDdoc->pMem : ((szDigiDocFile ? (char*)mbuf1.pMem : 0))), 
				  bHoldSession, 
				  ((!szDigiDocFile && szDataFile) ? &df : 0), &resp);
  if(err == SOAP_OK) {
	  err = ERR_OK;
  } else if(err == SOAP_TCP_ERROR) {
	  err = ERR_CONNECTION_FAILURE;
	  SET_LAST_ERROR(ERR_CONNECTION_FAILURE);
  } else
	  err = ERR_GENERIC_SOAP_ERR;
  // report error or success
  ddocDebug(3, "ddocDdsStartSession", "status: %s", resp.Status);
  // copy response data for client to use it before releasing soap call data
  pDds->lDdsSesscode = (unsigned long)resp.Sesscode;
  // copy response status & err & faults
  ddocDdsCopyStatusAndFault(pDds, resp.Status, 0, err);
  // copy response signed doc info
  ddocDdsCopySoapRespInfo(pDds, resp.SignedDocInfo);
  // cleanup
  cleanupDataFileData(&df);
  ddocMemBuf_free(&mbuf1);
  ddocDdsCleanup(pDds);
  return err;
}

//--------------------------------------------------
// Sends an DDS request to close a session
// pDds - pointer to DDS context structure
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsCloseSession(DDS* pDds)
{
  int err = SOAP_OK;
  struct d__CloseSessionResponse resp;

  RETURN_IF_NULL_PARAM(pDds);
  memset(&resp, 0, sizeof(struct d__CloseSessionResponse));
  ddocDebug(3, "ddocDdsCloseSession", "sesscode: %ld", pDds->lDdsSesscode);
  // cleanup old info if applicable
  ddocDdsCleanupStatusAndFault(pDds);
  ddocDdsCleanupInfo(pDds);
  // send DDS request
  ddocDebug(3, "ddocDdsCloseSession", "Sending request!");
  err = soap_call_d__CloseSession(&(pDds->soap), 
				  (const char*)pDds->endpoint, "",
				  pDds->lDdsSesscode, &resp);
  if(err == SOAP_OK) {
	  err = ERR_OK;
  } else if(err == SOAP_TCP_ERROR) {
	  err = ERR_CONNECTION_FAILURE;
	  SET_LAST_ERROR(ERR_CONNECTION_FAILURE);
  } else
	  err = ERR_GENERIC_SOAP_ERR;
  // report error or success
  ddocDebug(3, "ddocDdsCloseSession", "status: %s", resp.Status);
  // reset sesscode for next session
  pDds->lDdsSesscode = 0;
  // copy response status & err & faults
  ddocDdsCopyStatusAndFault(pDds, resp.Status, 0, err);
  // cleanup
  ddocDdsCleanup(pDds);
  return err;
}


//--------------------------------------------------
// Sends an DDS request to create a new empty digidoc
// pDds - pointer to DDS context structure
// szFormat - digidoc format
// szVersion - digidoc format version
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsCreateSignedDoc(DDS* pDds, const char* szFormat, const char* szVersion)
{
  int err = SOAP_OK;
  struct d__CreateSignedDocResponse resp;

  RETURN_IF_NULL_PARAM(pDds);
  memset(&resp, 0, sizeof(struct d__CreateSignedDocResponse));
  ddocDebug(3, "ddocDdsCreateSignedDoc", "sesscode: %ld format: %s version: %s", 
	    pDds->lDdsSesscode, (szFormat ? szFormat : "NULL"), 
	    (szVersion ? szVersion : "NULL"));
  // cleanup old info if applicable
  ddocDdsCleanupStatusAndFault(pDds);
  ddocDdsCleanupInfo(pDds);
  // send DDS request
  ddocDebug(3, "ddocDdsCreateSignedDoc", "Sending request!");
  err = soap_call_d__CreateSignedDoc(&(pDds->soap), 
				  (const char*)pDds->endpoint, "",
				     pDds->lDdsSesscode, (char*)szFormat, (char*)szVersion, &resp);
  // report error or success
  ddocDebug(3, "ddocDdsCreateSignedDoc", "status: %s", resp.Status);
  // copy response status & err & faults
  ddocDdsCopyStatusAndFault(pDds, resp.Status, 0, err);
  // copy response signed doc info
  ddocDdsCopySoapRespInfo(pDds, resp.SignedDocInfo);
  // cleanup soap data
  ddocDdsCleanup(pDds);
  if(err != SOAP_OK)
    err = ERR_GENERIC_SOAP_ERR;
  return err;
}


//--------------------------------------------------
// Sends an DDS request to add a new data file
// pDds - pointer to DDS context structure
// szFileName - filename and path
// szMimeType - mime type
// szContentType - content type
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsAddDataFile(DDS* pDds, const char* szFileName, 
	      const char* szMimeType, const char* szContentType)
{
  int err = SOAP_OK;
  struct d__DataFileData df;
  struct d__AddDataFileResponse resp;

  RETURN_IF_NULL_PARAM(pDds);
  RETURN_IF_NULL_PARAM(szFileName);
  RETURN_IF_NULL_PARAM(szMimeType);
  RETURN_IF_NULL_PARAM(szContentType);
  memset(&df, 0, sizeof(struct d__DataFileData));
  memset(&resp, 0, sizeof(struct d__AddDataFileResponse));
  ddocDebug(3, "ddocDdsAddDataFile", "sesscode: %ld file: %s mime: %s, content: %s", 
      pDds->lDdsSesscode, (szFileName ? szFileName : "NULL"), 
      (szMimeType ? szMimeType : "NULL"), (szContentType ? szContentType : "NULL"));
  // cleanup old info if applicable
  ddocDdsCleanupStatusAndFault(pDds);
  ddocDdsCleanupInfo(pDds);
  // init parameters
  err = initDataFileWithFile(&df, szFileName, szMimeType, szContentType);
   // send DDS request
  ddocDebug(3, "ddocDdsAddDataFile", "Sending request, err: %d", err);
  if(!err)
    err = soap_call_d__AddDataFile(&(pDds->soap), 
				   (const char*)pDds->endpoint, "", pDds->lDdsSesscode, 
				   df.Filename, df.MimeType, df.ContentType,
				   df.Size, df.DigestType, df.DigestValue,
				   df.DfData, &resp);
  // report error or success
  ddocDebug(3, "ddocDdsAddDataFile", "status: %s", resp.Status);
  // copy response status & err & faults
  ddocDdsCopyStatusAndFault(pDds, resp.Status, 0, err);
  // copy response signed doc info
  ddocDdsCopySoapRespInfo(pDds, resp.SignedDocInfo);
  // cleanup soap data
  cleanupDataFileData(&df);
  ddocDdsCleanup(pDds);
  if(err != SOAP_OK)
    err = ERR_GENERIC_SOAP_ERR;
  return err;
}


//--------------------------------------------------
// Sends an DDS request to remove a data file
// pDds - pointer to DDS context structure
// szId - data file id
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsRemoveDataFile(DDS* pDds, const char* szId)
{
  int err = SOAP_OK;
  struct d__RemoveDataFileResponse resp;

  RETURN_IF_NULL_PARAM(pDds);
  RETURN_IF_NULL_PARAM(szId);
  memset(&resp, 0, sizeof(struct d__RemoveDataFileResponse));
  ddocDebug(3, "ddocDdsRemoveDataFile", "sesscode: %ld id: %s", 
      pDds->lDdsSesscode, (szId ? szId : "NULL"));
  // cleanup old info if applicable
  ddocDdsCleanupStatusAndFault(pDds);
  ddocDdsCleanupInfo(pDds);
  // send DDS request
  ddocDebug(3, "ddocDdsRemoveDataFile", "Sending request!");
  err = soap_call_d__RemoveDataFile(&(pDds->soap), 
	      (const char*)pDds->endpoint, "", pDds->lDdsSesscode, 
				    (char*)szId, &resp);
  // report error or success
  ddocDebug(3, "ddocDdsRemoveDataFile", "status: %s", resp.Status);
  // copy response status & err & faults
  ddocDdsCopyStatusAndFault(pDds, resp.Status, 0, err);
  // copy response signed doc info
  ddocDdsCopySoapRespInfo(pDds, resp.SignedDocInfo);
  // cleanup soap data
  ddocDdsCleanup(pDds);
  if(err != SOAP_OK)
    err = ERR_GENERIC_SOAP_ERR;
  return err;
}

//--------------------------------------------------
// Sends an DDS request to retrieve current 
// digidoc data and stores in in the given file
// pDds - pointer to DDS context structure
// szFileName - filename to store the digidoc
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsGetSignedDoc(DDS* pDds, const char* szFileName)
{
  int err = SOAP_OK;
  struct d__GetSignedDocResponse resp;
  FILE* hFile;

  RETURN_IF_NULL_PARAM(pDds);
  RETURN_IF_NULL_PARAM(szFileName);
  memset(&resp, 0, sizeof(struct d__GetSignedDocResponse));
  ddocDebug(3, "ddocDdsGetSignedDoc", "sesscode: %ld file: %s", 
      pDds->lDdsSesscode, (szFileName ? szFileName : "NULL"));
  // cleanup old info if applicable
  ddocDdsCleanupStatusAndFault(pDds);
  //ddocDdsCleanupInfo(pDds);
  // send DDS request
  ddocDebug(3, "ddocDdsGetSignedDoc", "Sending request!");
  err = soap_call_d__GetSignedDoc(&(pDds->soap), 
	      (const char*)pDds->endpoint, "", pDds->lDdsSesscode, &resp);
  // report error or success
  ddocDebug(3, "ddocDdsGetSignedDoc", "status: %s, data: %d", 
	    resp.Status, strlen(resp.SignedDocData));
  // copy response status & err & faults
  ddocDdsCopyStatusAndFault(pDds, resp.Status, 0, err);
  if(!err && resp.SignedDocData) {
    if((hFile = fopen(szFileName, "wb")) != NULL) {
      fputs(resp.SignedDocData, hFile);
      fclose(hFile);
      ddocDebug(3, "ddocDdsGetSignedDoc", "stored: %d bytes in: %s", 
		strlen(resp.SignedDocData), szFileName);
    }
  }
  // copy response signed doc info
  //ddocDdsCopySoapRespInfo(pDds, resp.SignedDocInfo);
  // cleanup soap data
  ddocDdsCleanup(pDds);
  if(err != SOAP_OK)
    err = ERR_GENERIC_SOAP_ERR;
  return err;
}


//--------------------------------------------------
// Sends an DDS request to retrieve current 
// digidoc info.
// pDds - pointer to DDS context structure
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsGetSignedDocInfo(DDS* pDds)
{
  int err = SOAP_OK;
  struct d__GetSignedDocInfoResponse resp;

  RETURN_IF_NULL_PARAM(pDds);
  memset(&resp, 0, sizeof(struct d__GetSignedDocInfoResponse));
  ddocDebug(3, "ddocDdsGetSignedDocInfo", "sesscode: %ld", pDds->lDdsSesscode);
  // cleanup old info if applicable
  ddocDdsCleanupStatusAndFault(pDds);
  ddocDdsCleanupInfo(pDds);
  // send DDS request
  ddocDebug(3, "ddocDdsGetSignedDocInfo", "Sending request!");
  err = soap_call_d__GetSignedDocInfo(&(pDds->soap), 
	      (const char*)pDds->endpoint, "", pDds->lDdsSesscode, &resp);
  // report error or success
  ddocDebug(3, "ddocDdsGetSignedDocInfo", "status: %s", resp.Status);
  // copy response status & err & faults
  ddocDdsCopyStatusAndFault(pDds, resp.Status, 0, err);
  // copy response signed doc info
  ddocDdsCopySoapRespInfo(pDds, resp.SignedDocInfo);
  // cleanup soap data
  ddocDdsCleanup(pDds);
  if(err != SOAP_OK)
    err = ERR_GENERIC_SOAP_ERR;
  return err;
}

//--------------------------------------------------
// Sends an DDS request to retrieve the given 
// data file info and store it in a file.
// pDds - pointer to DDS context structure
// szId - data file id [mandatory]
// szFileName - file to store data in. Use NULL to use
// original data file name (but current directory!)
// Use directory name (ending with / or \ ) to
// use original data file name but in a directory
// of your choice.
// returns error code or SOAP_OK
//--------------------------------------------------
 EXP_OPTION int ddocDdsGetDataFile(DDS* pDds, const char* szId, const char* szFileName)
{
  int err = SOAP_OK, l1, l2;
  struct d__GetDataFileResponse resp;
  FILE* hFile;
  char *pFileName = 0, *pContent = 0;

  RETURN_IF_NULL_PARAM(pDds);
  RETURN_IF_NULL_PARAM(szId);
  memset(&resp, 0, sizeof(struct d__GetDataFileResponse));
  ddocDebug(3, "ddocDdsGetDataFile", "sesscode: %ld", pDds->lDdsSesscode);
  // cleanup old info if applicable
  ddocDdsCleanupStatusAndFault(pDds);
  //ddocDdsCleanupInfo(pDds);
  // send DDS request
  ddocDebug(3, "ddocDdsGetDataFile", "Sending request!");
  err = soap_call_d__GetDataFile(&(pDds->soap), 
	 (const char*)pDds->endpoint, "", pDds->lDdsSesscode, (char*)szId, &resp);
  // report error or success
  ddocDebug(3, "ddocDdsGetDataFile", "err: %d status: %s, data-len: %d content: %s", 
	    err, resp.Status, (resp.DataFileData ?  resp.DataFileData->Size : 0),
	    (resp.DataFileData ?  resp.DataFileData->ContentType : "?"));
  // store data in a file
  if(!err && resp.DataFileData) {
    // chek filename param
    if(szFileName) {
      l1 = strlen(szFileName);
      if(szFileName[l1-1] == '\\' || szFileName[l1-1] == '/') {
	pFileName = (char*)malloc(l1 + strlen(resp.DataFileData->Filename) + 1);
	if(pFileName) {
	  strcpy(pFileName, szFileName);
	  strcat(pFileName, resp.DataFileData->Filename);
	}
      } else
	pFileName = (char*)szFileName;
    } else {
      pFileName = resp.DataFileData->Filename;
    }
    // default content is data file content
    if(!strcmp(resp.DataFileData->ContentType, CONTENT_EMBEDDED))
      pContent = resp.DataFileData->DfData;
    if(!strcmp(resp.DataFileData->ContentType, CONTENT_EMBEDDED_BASE64)) {
      l1 = l2 = strlen(resp.DataFileData->DfData);
      pContent = (char*)malloc(l1);
      if(pContent)
	decode((const byte*)resp.DataFileData->DfData, l1, (byte*)pContent, &l2);
    }
    // store in file
    ddocDebug(3, "ddocDdsGetDataFile", "content: %s file: %s", 
	      (pContent ? "OK" : "NULL"), pFileName);
    if(pContent && pFileName) {
      if((hFile = fopen(pFileName, "wb")) != NULL) {
	l1 = fwrite(pContent, 1, l2, hFile);
	ddocDebug(3, "ddocDdsGetDataFile", "wrote: %d bytes in: %s", l2, pFileName);
	fclose(hFile);
      }
    }
  }
  // copy response status & err & faults
  ddocDdsCopyStatusAndFault(pDds, resp.Status, 0, err);
  // copy response signed doc info
  //ddocDdsCopySoapRespInfo(pDds, resp.SignedDocInfo);
  // cleanup
  if(pFileName && pFileName != szFileName && 
     resp.DataFileData && pFileName != resp.DataFileData->Filename)
    free(pFileName);
  if(pContent && resp.DataFileData && pContent != resp.DataFileData->DfData)
    free(pContent);
  // cleanup soap data
  ddocDdsCleanup(pDds);
  if(err != SOAP_OK)
    err = ERR_GENERIC_SOAP_ERR;
  return err;
}

char* g_szCertPemHeader = "-----BEGIN CERTIFICATE-----\r\n";
char* g_szCertPemTrailer = "\n-----END CERTIFICATE-----";

//--------------------------------------------------
// Sends an DDS request to retrieve the given 
// data signatures signers certificate
// pDds - pointer to DDS context structure
// szId - signature id [mandatory]
// szFileName - file to store data in. [mandatory]
// Data will be in PEM format!
// returns error code or SOAP_OK
//--------------------------------------------------
 EXP_OPTION int ddocDdsGetSignersCertificate(DDS* pDds, const char* szId, const char* szFileName)
{
  int err = SOAP_OK;
  struct d__GetSignersCertificateResponse resp;
  FILE* hFile;

  RETURN_IF_NULL_PARAM(pDds);
  RETURN_IF_NULL_PARAM(szId);
  RETURN_IF_NULL_PARAM(szFileName);
  memset(&resp, 0, sizeof(struct d__GetSignersCertificateResponse));
  ddocDebug(3, "ddocDdsGetSignersCertificate", "sesscode: %ld, id: %s file: %s", 
	    pDds->lDdsSesscode, szId, szFileName);
  // cleanup old info if applicable
  ddocDdsCleanupStatusAndFault(pDds);
  //ddocDdsCleanupInfo(pDds);
  // send DDS request
  ddocDebug(3, "ddocDdsGetSignersCertificate", "Sending request!");
  err = soap_call_d__GetSignersCertificate(&(pDds->soap), 
	 (const char*)pDds->endpoint, "", pDds->lDdsSesscode, (char*)szId, &resp);
  // report error or success
  ddocDebug(3, "ddocDdsGetSignersCertificate", "status: %s", resp.Status);
  // store data in a file
  if(!err && resp.CertificateData) {
    if((hFile = fopen(szFileName, "wb")) != NULL) {
      fputs(g_szCertPemHeader, hFile);
      fputs(resp.CertificateData, hFile);
      fputs(g_szCertPemTrailer, hFile);
      ddocDebug(3, "ddocDdsGetSignersCertificate", "wrote cert in file: %s", szFileName);
      fclose(hFile);
    }
  }
  // copy response status & err & faults
  ddocDdsCopyStatusAndFault(pDds, resp.Status, 0, err);
  // copy response signed doc info
  //ddocDdsCopySoapRespInfo(pDds, resp.SignedDocInfo);
  // cleanup soap data
  ddocDdsCleanup(pDds);
  if(err != SOAP_OK)
    err = ERR_GENERIC_SOAP_ERR;
  return err;
}

//--------------------------------------------------
// Sends an DDS request to retrieve the given 
// data signatures notarys (responders) certificate
// pDds - pointer to DDS context structure
// szId - signature id [mandatory]
// szFileName - file to store data in. [mandatory]
// Data will be in PEM format!
// returns error code or SOAP_OK
//--------------------------------------------------
 EXP_OPTION int ddocDdsGetNotarysCertificate(DDS* pDds, const char* szId, const char* szFileName)
{
  int err = SOAP_OK;
  struct d__GetNotarysCertificateResponse resp;
  FILE* hFile;

  RETURN_IF_NULL_PARAM(pDds);
  RETURN_IF_NULL_PARAM(szId);
  RETURN_IF_NULL_PARAM(szFileName);
  memset(&resp, 0, sizeof(struct d__GetNotarysCertificateResponse));
  ddocDebug(3, "ddocDdsGetNotarysCertificate", "sesscode: %ld, id: %s file: %s", 
	    pDds->lDdsSesscode, szId, szFileName);
  // cleanup old info if applicable
  ddocDdsCleanupStatusAndFault(pDds);
  //ddocDdsCleanupInfo(pDds);
  // send DDS request
  ddocDebug(3, "ddocDdsGetNotarysCertificate", "Sending request!");
  err = soap_call_d__GetNotarysCertificate(&(pDds->soap), 
	 (const char*)pDds->endpoint, "", pDds->lDdsSesscode, (char*)szId, &resp);
  // report error or success
  ddocDebug(3, "ddocDdsGetNotarysCertificate", "status: %s", resp.Status);
  // store data in a file
  if(!err && resp.CertificateData) {
    if((hFile = fopen(szFileName, "wb")) != NULL) {
      fputs(g_szCertPemHeader, hFile);
      fputs(resp.CertificateData, hFile);
      fputs(g_szCertPemTrailer, hFile);
      ddocDebug(3, "ddocDdsGetNotarysCertificate", "wrote cert in file: %s", szFileName);
      fclose(hFile);
    }
  }
  // copy response status & err & faults
  ddocDdsCopyStatusAndFault(pDds, resp.Status, 0, err);
  // copy response signed doc info
  //ddocDdsCopySoapRespInfo(pDds, resp.SignedDocInfo);
  // cleanup soap data
  ddocDdsCleanup(pDds);
  if(err != SOAP_OK)
    err = ERR_GENERIC_SOAP_ERR;
  return err;
}

char* g_szOcspPemHeader = "-----BEGIN OCSP RESPONSE-----\n";
char* g_szOcspPemTrailer = "\n-----END OCSP RESPONSE-----";

//--------------------------------------------------
// Sends an DDS request to retrieve the given 
// data signatures OCSP response data
// pDds - pointer to DDS context structure
// szId - signature id [mandatory]
// szFileName - file to store data in. [mandatory]
// Data will be in PEM format!
// returns error code or SOAP_OK
//--------------------------------------------------
 EXP_OPTION int ddocDdsGetNotary(DDS* pDds, const char* szId, const char* szFileName)
{
  int err = SOAP_OK;
  struct d__GetNotaryResponse resp;
  FILE* hFile;

  RETURN_IF_NULL_PARAM(pDds);
  RETURN_IF_NULL_PARAM(szId);
  RETURN_IF_NULL_PARAM(szFileName);
  memset(&resp, 0, sizeof(struct d__GetNotaryResponse));
  ddocDebug(3, "ddocDdsGetNotary", "sesscode: %ld, id: %s file: %s", 
	    pDds->lDdsSesscode, szId, szFileName);
  // cleanup old info if applicable
  ddocDdsCleanupStatusAndFault(pDds);
  //ddocDdsCleanupInfo(pDds);
  // send DDS request
  ddocDebug(3, "ddocDdsGetNotary", "Sending request!");
  err = soap_call_d__GetNotary(&(pDds->soap), 
	 (const char*)pDds->endpoint, "", pDds->lDdsSesscode, (char*)szId, &resp);
  // report error or success
  ddocDebug(3, "ddocDdsGetNotary", "status: %s", resp.Status);
  // store data in a file
  if(!err && resp.OcspData) {
    if((hFile = fopen(szFileName, "wb")) != NULL) {
      fputs(g_szOcspPemHeader, hFile);
      fputs(resp.OcspData, hFile);
      fputs(g_szOcspPemTrailer, hFile);
      ddocDebug(3, "ddocDdsGetNotary", "wrote OCSP in file: %s", szFileName);
      fclose(hFile);
    }
  }
  // copy response status & err & faults
  ddocDdsCopyStatusAndFault(pDds, resp.Status, 0, err);
  // copy response signed doc info
  //ddocDdsCopySoapRespInfo(pDds, resp.SignedDocInfo);
  // cleanup soap data
  ddocDdsCleanup(pDds);
  if(err != SOAP_OK)
    err = ERR_GENERIC_SOAP_ERR;
  return err;
}


//--------------------------------------------------
// Sends an DDS request to retrieve fixed PKI
// signature modules for the given platform, phase
// and type
// pDds - pointer to DDS context structure
// szPlatform - LINUX-MOZILLA, WIN32-MOZILLA, WIN32-IE
// szPhase - PREPARE or FINALIZE
// szType - DYNAMIC or STATIC or ALL
// Data will be in PEM format!
// returns error code or SOAP_OK
//--------------------------------------------------
 EXP_OPTION int ddocDdsGetSignatureModules(DDS* pDds, const char* szPlatform, 
					   const char* szPhase, const char* szType)
{
  int err = SOAP_OK;
  struct d__GetSignatureModulesResponse resp;

  RETURN_IF_NULL_PARAM(pDds);
  RETURN_IF_NULL_PARAM(szPlatform);
  RETURN_IF_NULL_PARAM(szPhase);
  RETURN_IF_NULL_PARAM(szType);
  memset(&resp, 0, sizeof(struct d__GetSignatureModulesResponse));
  ddocDebug(3, "ddocDdsGetSignatureModules", "sesscode: %ld, platform: %s phase: %s type: %s", 
	    pDds->lDdsSesscode, szPlatform, szPhase, szType);
  // cleanup old info if applicable
  ddocDdsCleanupStatusAndFault(pDds);
  //ddocDdsCleanupInfo(pDds);
  // send DDS request
  ddocDebug(3, "ddocDdsGetSignatureModules", "Sending request!");
  err = soap_call_d__GetSignatureModules(&(pDds->soap), 
	 (const char*)pDds->endpoint, "", pDds->lDdsSesscode, 
	 (char*)szPlatform, (char*)szPhase, (char*)szType, &resp);
  // report error or success
  ddocDebug(3, "ddocDdsGetSignatureModules", "status: %s", resp.Status);
  // store data in a file
  if(!err && resp.Modules ) {
    // what should I do here? - not usefull for C apps!

  }
  // copy response status & err & faults
  ddocDdsCopyStatusAndFault(pDds, resp.Status, 0, err);
  // copy response signed doc info
  //ddocDdsCopySoapRespInfo(pDds, resp.SignedDocInfo);
  // cleanup soap data
  ddocDdsCleanup(pDds);
  if(err != SOAP_OK)
    err = ERR_GENERIC_SOAP_ERR;
  return err;
}

//--------------------------------------------------
// Sends an DDS request to prepare a new signature
// pDds - pointer to DDS context structure
// pCert - direct X509 cert [must rpovide cert or cert filename]
// szCertFile - cert PEM filename 
// szTokenId -
// szRole - signers role [optional]
// szCity - signers address, city [optional]
// szState - signers address, state [optional]
// szPostalCode - signers address, postal code [optional]
// szCountry - signers address, country [optional]
// szSigId - buffer for new signature id [mandatory]
// szHash - buffer for new hash code to be signed [mandatory]
// pHashLen - pointer to length (int) of hash code [mandatory]
// szSigningProfile - signing profile URL
// returns error code or SOAP_OK
//--------------------------------------------------
 EXP_OPTION int ddocDdsPrepareSignature(DDS* pDds, const char* szSigningProfile,
                const X509* pCert, const char* szCertFile,
		const char* szTokenId, const char* szRole, const char* szCity,
		const char* szState, const char* szPostalCode, const char* szCountry,
		char* szSigId, char* szHash, int *pHashLen)
{
  int err = SOAP_OK, l1, l2;
  struct d__PrepareSignatureResponse resp;
  X509* pCert2 = 0;
  char *p1 = 0, *p2 = 0;

  RETURN_IF_NULL_PARAM(pDds);
  RETURN_IF_NULL_PARAM(pCert || szCertFile);
  RETURN_IF_NULL_PARAM(szTokenId);
  RETURN_IF_NULL_PARAM(szSigId);
  RETURN_IF_NULL_PARAM(szHash);
  RETURN_IF_NULL_PARAM(pHashLen);
  memset(&resp, 0, sizeof(struct d__PrepareSignatureResponse));
  ddocDebug(3, "ddocDdsPrepareSignature", "sesscode: %ld, cert: %s token: %s role: %s city: %s state: %s zip: %s country: %s", 
	    pDds->lDdsSesscode, (pCert ? "CERT" : szCertFile), szTokenId, 
	    (szRole ? szRole : ""), (szCity ? szCity : ""), 
	    (szState ? szState : ""), (szPostalCode ? szPostalCode : ""),
	    (szCountry ? szCountry : ""));
  // cleanup old info if applicable
  ddocDdsCleanupStatusAndFault(pDds);
  //ddocDdsCleanupInfo(pDds);
  // prepare cert
  pCert2 = (X509*)pCert;
  if(!pCert)
    err = ReadCertificate(&pCert2, szCertFile);
  if(err || !pCert)
    return err;
  l1 = i2d_X509(pCert2, NULL);
  p1 = (char*)malloc(l1+10);
  RETURN_IF_BAD_ALLOC(p1);
  p2 = p1;
  i2d_X509(pCert2, (unsigned char**)&p2);
  l2 = l1 * 2 + 10;
  p2 = (char*)malloc(l2);
  if(!p2) {
    free(p1);
    RETURN_IF_BAD_ALLOC(p2);
  }
  bin2hex((const byte*)p1, l1, (char*)p2, &l2);
  // send DDS request
  ddocDebug(3, "ddocDdsPrepareSignature", "Sending request!");
  err = soap_call_d__PrepareSignature(&(pDds->soap), 
	 (const char*)pDds->endpoint, "", pDds->lDdsSesscode, 
          p2, (char*)szTokenId, (char*)szRole, (char*)szCity,
         (char*)szState, (char*)szPostalCode, (char*)szCountry,
				      (char*)szSigningProfile, &resp);
  // report error or success
  ddocDebug(3, "ddocDdsPrepareSignature", "status: %s signatur: %s, hash: %s", 
	    resp.Status, (resp.SignatureId ? resp.SignatureId : "NULL"),
	    (resp.SignedInfoDigest ? resp.SignedInfoDigest : "NULL"));
  // return data
  if(!err && resp.SignatureId && resp.SignedInfoDigest) {
    strcpy(szSigId, resp.SignatureId);
    hex2bin((const char*)resp.SignedInfoDigest, (byte*)szHash, pHashLen);
  }
  // copy response status & err & faults
  ddocDdsCopyStatusAndFault(pDds, resp.Status, 0, err);
  // copy response signed doc info
  //ddocDdsCopySoapRespInfo(pDds, resp.SignedDocInfo);
  // cleanup
  if(p1)
    free(p1);
  if(p2)
    free(p2);
  // cleanup soap data
  ddocDdsCleanup(pDds);
  if(err != SOAP_OK)
    err = ERR_GENERIC_SOAP_ERR;
  return err;
}

//--------------------------------------------------
// Sends an DDS request to finalize a new signature
// pDds - pointer to DDS context structure
// szId - siganture id [mandatory]
// szSigValHEX - signature value in hex [mandatory]
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsFinalizeSignature(DDS* pDds, const char* szId, const char* szSigValHEX)
{
  int err = SOAP_OK;
  struct d__FinalizeSignatureResponse resp;

  RETURN_IF_NULL_PARAM(pDds);
  RETURN_IF_NULL_PARAM(szId);
  RETURN_IF_NULL_PARAM(szSigValHEX);
  memset(&resp, 0, sizeof(struct d__FinalizeSignatureResponse));
  ddocDebug(3, "ddocDdsFinalizeSignature", "sesscode: %ld, sig-id: %s signature: %s", 
	    pDds->lDdsSesscode, szId, szSigValHEX);
  // cleanup old info if applicable
  ddocDdsCleanupStatusAndFault(pDds);
  ddocDdsCleanupInfo(pDds);
  // send DDS request
  ddocDebug(3, "ddocDdsFinalizeSignature", "Sending request!");
  err = soap_call_d__FinalizeSignature(&(pDds->soap), 
	 (const char*)pDds->endpoint, "", pDds->lDdsSesscode, 
         (char*)szId, (char*)szSigValHEX, &resp);
  // report error or success
  ddocDebug(3, "ddocDdsFinalizeSignature", "status: %s", resp.Status);
  // copy response status & err & faults
  ddocDdsCopyStatusAndFault(pDds, resp.Status, 0, err);
  // copy response signed doc info
  ddocDdsCopySoapRespInfo(pDds, resp.SignedDocInfo);
  // cleanup soap data
  ddocDdsCleanup(pDds);
  if(err != SOAP_OK)
    err = ERR_GENERIC_SOAP_ERR;
  return err;
}


//--------------------------------------------------
// Sends an DDS request to remove a signature
// pDds - pointer to DDS context structure
// szId - siganture id [mandatory]
// szSigValHEX - signature value in hex [mandatory]
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsRemoveSignature(DDS* pDds, const char* szId)
{
  int err = SOAP_OK;
  struct d__RemoveSignatureResponse resp;

  RETURN_IF_NULL_PARAM(pDds);
  RETURN_IF_NULL_PARAM(szId);
  memset(&resp, 0, sizeof(struct d__RemoveSignatureResponse));
  ddocDebug(3, "ddocDdsRemoveSignature", "sesscode: %ld, sig-id: %s", 
	    pDds->lDdsSesscode, szId);
  // cleanup old info if applicable
  ddocDdsCleanupStatusAndFault(pDds);
  ddocDdsCleanupInfo(pDds);
  // send DDS request
  ddocDebug(3, "ddocDdsRemoveSignature", "Sending request!");
  err = soap_call_d__RemoveSignature(&(pDds->soap), 
	 (const char*)pDds->endpoint, "", pDds->lDdsSesscode, 
         (char*)szId, &resp);
  // report error or success
  ddocDebug(3, "ddocDdsRemoveSignature", "status: %s", resp.Status);
  // copy response status & err & faults
  ddocDdsCopyStatusAndFault(pDds, resp.Status, 0, err);
  // copy response signed doc info
  ddocDdsCopySoapRespInfo(pDds, resp.SignedDocInfo);
  // cleanup soap data
  ddocDdsCleanup(pDds);
  if(err != SOAP_OK)
    err = ERR_GENERIC_SOAP_ERR;
  return err;
}

//--------------------------------------------------
// Sends an DDS request to remove a signature
// pDds - pointer to DDS context structure
// szName - buffer for service name [mandatory]
// szVersion - buffer for service version [mandatory]
// returns error code or SOAP_OK
//--------------------------------------------------
 EXP_OPTION int ddocDdsGetVersion(DDS* pDds, char* szName, char* szVersion)
{
  int err = SOAP_OK;
  struct d__GetVersionResponse resp;

  RETURN_IF_NULL_PARAM(pDds);
  RETURN_IF_NULL_PARAM(szName);
  RETURN_IF_NULL_PARAM(szVersion);
  memset(&resp, 0, sizeof(struct d__GetVersionResponse));
  ddocDebug(3, "ddocDdsGetVersion", "sesscode: %ld", pDds->lDdsSesscode);
  // cleanup old info if applicable
  ddocDdsCleanupStatusAndFault(pDds);
  //ddocDdsCleanupInfo(pDds);
  // send DDS request
  ddocDebug(3, "ddocDdsGetVersion", "Sending request!");
  err = soap_call_d__GetVersion(&(pDds->soap), 
	 (const char*)pDds->endpoint, "", /*0,*/ &resp);
  // report error or success
  ddocDebug(3, "ddocDdsGetVersion", "name: %s version: %s", resp.Name, resp.Version);
  // copy response status & err & faults
  //ddocDdsCopyStatusAndFault(pDds, resp.status, 0, err);
  // return data
  if(!err && resp.Name && resp.Version) {
    strcpy(szName, resp.Name);
    strcpy(szVersion, resp.Version);
  }
  // copy response signed doc info
  //ddocDdsCopySoapRespInfo(pDds, resp.SignedDocInfo);
  // cleanup soap data
  ddocDdsCleanup(pDds);
  if(err != SOAP_OK)
    err = ERR_GENERIC_SOAP_ERR;
  return err;
}

//--------------------------------------------------
// Sends an DDS request to create a new signature via
// MSSP gateway and mobile signing
// pDds - pointer to DDS context structure
// szSigningProfile - signing profile URL
// szCountryCode - signers country code (2 chars)
// szSignerIdCode - signers personal id code [mnadatory]
// szSignerPhoneNo - signers phone number [mandatory]
// szDataToBeDisplayed - data to be displayed [optional]
// szLang - laguage
// szRole - signers role/manifest [optional]
// szCity - signers address, city [optional]
// szState - signers address, state [optional]
// szPostalCode - signers address, postal code [optional]
// szCountry - signers address, country [optional]
// szMsgMode - "asynchClientServer"
// szNotifURL - notification URL. Not used yet. Send NULL
// szCertURL - certificate url. [optional]
// bReturnDocInfo - 1/0 send digidoc info or not
// bReturnDocData - 1/0 send digidic data or not
// pMbChallengeId - buffer for challenge id [OUT]
// pMbChallenge - buffer for challenge [OUT]
// szSigningProfile - signing profile URL
// szFileName - filename to store the digidoc
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsMobileSign(DDS* pDds, const char* szSigningProfile, 
				 const char* szSignerIdCode, const char* szCountryCode,
				 const char* szSignerPhoneNo, const char* szDataToBeDisplayed,
				 const char* szLang, const char* szRole, const char* szCity,
				 const char* szState, const char* szPostalCode,
				 const char* szCountry, const char* szMsgMode,
				 //const char* szNotifURL, const char* szCertURL,
				 //int bReturnDocInfo, int bReturnDocData, 
				 DigiDocMemBuf *pMbChallengeId, DigiDocMemBuf *pMbChallenge,
				 const char* szDigiDocFileName)
{
  int err = SOAP_OK;
  struct d__MobileSignResponse resp;
  //FILE* hFile;

  RETURN_IF_NULL_PARAM(pDds);
  RETURN_IF_NULL_PARAM(((szSignerIdCode && szCountryCode) || szSignerPhoneNo));
  RETURN_IF_NULL_PARAM(szDataToBeDisplayed || szRole);
  //RETURN_IF_NULL_PARAM(szDigiDocFileName);
  //RETURN_IF_NULL_PARAM(szMsgMode);
  memset(&resp, 0, sizeof(struct d__MobileSignResponse));
  ddocDebug(3, "ddocDdsMobileSign", "sesscode: %ld signer-id: %s cntr-cd: %s phone-no: %s  data-to-be-displayed: \'%s\' lang: %s, role: %s city: %s state: %s zip: %s country: %s msg-mode: %s ddoc-name: %s",
	    pDds->lDdsSesscode, (szSignerIdCode ? szSignerIdCode : ""),  
	    (szCountryCode ? szCountryCode : ""), (szSignerPhoneNo ? szSignerPhoneNo : ""),
	    (szDataToBeDisplayed ? szDataToBeDisplayed : ""),
	    (szLang ? szLang : ""), (szRole ? szRole : ""), (szCity ? szCity : ""),
	    (szState ? szState : ""), (szPostalCode ? szPostalCode : ""),
		(szCountry ? szCountry : ""), (szMsgMode ? szMsgMode : ""), 
		//(szNotifURL ? szNotifURL : ""), (szCertURL ? szCertURL : ""), 
		//bReturnDocInfo, bReturnDocData,
		(szDigiDocFileName ? szDigiDocFileName : ""));
  // cleanup old info if applicable
  ddocDdsCleanupStatusAndFault(pDds);
  //if(bReturnDocInfo)
  //  ddocDdsCleanupInfo(pDds);
  // send DDS request
  ddocDebug(3, "ddocDdsMobileSign", "Sending request!");
  err = soap_call_d__MobileSign(&(pDds->soap), 
				(const char*)pDds->endpoint, "", pDds->lDdsSesscode, 
				(char*)szSignerIdCode, (char*)szCountryCode,
				(char*) szSignerPhoneNo, "DigiDoc Client", (char*)szDataToBeDisplayed,
				(szLang ? (char*)szLang : "ENG"), (char*)szRole, (char*)szCity,
				(char*)szState, (char*)szPostalCode, (char*)szCountry, 
				(char*)szSigningProfile,
				(szMsgMode ? (char*)szMsgMode : "asynchClientServer"),
				0 /*AsyncConfiguration*/,
				//(char*)szNotifURL, (char*)szCertURL,
				0, 0, /*bReturnDocInfo, bReturnDocData,*/ &resp);
  // report error or success
  ddocDebug(3, "ddocDdsMobileSign", "status: %s, status-code: %s challenge-id: %s", 
	    resp.Status, resp.StatusCode, resp.ChallengeID /*(resp.SignedDocData ? strlen(resp.SignedDocData) : 0)*/);
    // copy response status & err & faults
  ddocDdsCopyStatusAndFault(pDds, resp.Status, resp.StatusCode, err);
  // get real error code
  if(pDds && pDds->szSoapFaultString && isdigit(pDds->szSoapFaultString[0]))
	  err = atoi(pDds->szSoapFaultString);
  else
	  if(err != SOAP_OK)
		err = ERR_GENERIC_SOAP_ERR;
  /*if(!err && resp.SignedDocData && szDigiDocFileName) {
    if((hFile = fopen(szDigiDocFileName, "wb")) != NULL) {
      fputs(resp.SignedDocData, hFile);
      fclose(hFile);
      ddocDebug(3, "ddocDdsMobileSign", "stored: %d bytes in: %s", 
		strlen(resp.SignedDocData), szDigiDocFileName);
    }
  }*/
  // copy response signed doc info
  //if(bReturnDocInfo)
  //  ddocDdsCopySoapRespInfo(pDds, resp.SignedDocInfo);
  if(pMbChallengeId && resp.ChallengeID)
      ddocMemAssignData(pMbChallengeId, resp.ChallengeID, -1);
  //if(pMbChallenge && resp.Challenge)
  //    ddocMemAssignData(pMbChallenge, resp.Challenge, -1);
  // cleanup soap data
  ddocDdsCleanup(pDds);
  
  return err;
}


//--------------------------------------------------
// Sends an DDS request to create a new signature via
// MSSP gateway and mobile signing
// lSesscode - DDS sessioncode
// bReturnDocInfo - 1/0 send digidoc info or not
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsGetStatusInfo(long lSesscode, int bReturnDocInfo, char* pszStartusStr, int nStatusLen)
{
  int err = SOAP_OK;
  struct d__GetStatusInfoResponse resp;
  DDS* pDds = 0;

  memset(&resp, 0, sizeof(struct d__GetStatusInfoResponse));
  ddocDebug(3, "ddocDdsGetStatusInfo", "sesscode: %ld return-ddoc-info: %d",
	    lSesscode, bReturnDocInfo);
  // connect
  err = ddocDdsConnect(&pDds);
  pDds->lDdsSesscode = lSesscode;
  if(err) return err;
  // cleanup old info if applicable
  //ddocDdsCleanupStatusAndFault(pDds);
  //if(bReturnDocInfo)
  //  ddocDdsCleanupInfo(pDds);
  // send DDS request
  ddocDebug(3, "ddocDdsGetStatusInfo", "Sending request!");
  err = soap_call_d__GetStatusInfo(&(pDds->soap), 
	      (const char*)pDds->endpoint, "", pDds->lDdsSesscode, 
	       bReturnDocInfo, 0, &resp);
  // report error or success
  ddocDebug(3, "ddocDdsGetStatusInfo", "status: %s, status-code: %s", 
	    resp.Status, resp.StatusCode);
  // copy response status & err & faults
  ddocDdsCopyStatusAndFault(pDds, resp.Status, resp.StatusCode, err);
  // copy response signed doc info
  if(pszStartusStr) {
	  strncpy(pszStartusStr, pDds->szStatusCode, nStatusLen);
	  pszStartusStr[nStatusLen-1] = 0;
  }
  if(bReturnDocInfo)
    ddocDdsCopySoapRespInfo(pDds, resp.SignedDocInfo);
  // cleanup soap data
  //ddocDdsCleanup(pDds);
  ddocDdsDisconnect(pDds);
  if(err != SOAP_OK)
    err = ERR_GENERIC_SOAP_ERR;
  return err;
}

//--------------------------------------------------
// Sends an DDS request to authenticate a person through
// MSSP gateway and mobile signing
// pDds - pointer to DDS context structure
// szIdCode - authenticators personal id code 
// szCountryCode - signers country code (2 chars) 
// szPhoneNo - authenticators phone number 
// [mandatory] - either szIdCode and szCountryCode or szPhoneNo
// szLang - laguage [optional] default is ENG
// szMessageToDisplay - message to be displayed [optional]
// szSpcChallenge - AP challenge [optional]
// szCertUrl - certificate URL [optional]
// szMsgMode - "asynchClientServer"
// szNotifURL - notification URL. Not used yet. Send NULL
// bReturnCertData - 1/0 send cert data or not
// bReturnRevocationData - 1/0 send revocation data or not
// pMbUserIDCode - buffer for user id code [OUT]
// pMbUserGivenname - buffer for user givenname [OUT]
// pMbUserSurname - buffer for user surname [OUT]
// pMbUserCountry - buffer for user country [OUT]
// pMbUserCN - buffer for user CN [OUT]
// pMbCertData - buffer for user certificate data [OUT]
// pMbChallengeId - buffer for challenge id [OUT]
// pMbChallenge - buffer for challenge [OUT]
// pMbRevocationData - buffer for revocation data [OUT]
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsMobileAuthenticate(DDS* pDds, const char* szIdCode,
					 const char* szCountryCode,
				 const char* szPhoneNo, const char* szLang, 
				 const char* szMessageToDisplay,
				 const char* szSpcChallenge, const char* szCertUrl,
				 const char* szMsgMode, const char* szNotifURL, 
				 int bReturnCertData, int bReturnRevocationData,
				 const char* szServiceName,
				 DigiDocMemBuf *pMbUserIDCode, DigiDocMemBuf *pMbUserGivenname,
				 DigiDocMemBuf *pMbUserSurname, DigiDocMemBuf *pMbUserCountry,
				 DigiDocMemBuf *pMbUserCN, DigiDocMemBuf *pMbCertData,
				 DigiDocMemBuf *pMbChallengeId, DigiDocMemBuf *pMbChallenge, 
				 DigiDocMemBuf *pMbRevocationData)
{
  int err = SOAP_OK;
  struct d__MobileAuthenticateResponse resp;

  RETURN_IF_NULL_PARAM(pDds);
  RETURN_IF_NULL_PARAM(((szIdCode && szCountryCode) || szPhoneNo));
  RETURN_IF_NULL_PARAM(szMessageToDisplay);
  RETURN_IF_NULL_PARAM(szMsgMode);
  memset(&resp, 0, sizeof(struct d__MobileSignResponse));
  ddocDebug(3, "ddocDdsMobileAuthenticate", "id: %s cntr-cd: %s, phone-no: %s msg-to-display: \'%s\' lang: %s, spc-challenge: %s cert-url: %s msg-mode: %s notif-url: %s return-cert-data: %d return-revoc-data: %d service: %s",
	    (szIdCode ? szIdCode : ""), (szCountryCode ? szCountryCode : ""), 
	    (szPhoneNo ? szPhoneNo : ""), szMessageToDisplay,
	    (szLang ? szLang : ""), (szSpcChallenge ? szSpcChallenge : ""), 
	    (szCertUrl ? szCertUrl : ""), szMsgMode, (szNotifURL ? szNotifURL : ""),
	    bReturnCertData, bReturnRevocationData, (szServiceName ? szServiceName : ""));
  // cleanup old info if applicable
  ddocDdsCleanupStatusAndFault(pDds);
  // send DDS request
  ddocDebug(3, "ddocDdsMobileAuthenticate", "Sending request!");
  err = soap_call_d__MobileAuthenticate(&(pDds->soap), 
	      (const char*)pDds->endpoint, "", (char*)szIdCode, 
		  (char*)szCountryCode, (char*)szPhoneNo, 
	      (szLang ? (char*)szLang : "ENG"), 
		  (char*)szServiceName, (char*)szMessageToDisplay,
	      (char*)szSpcChallenge, //(char*)szCertUrl,
	      (szMsgMode ? (char*)szMsgMode : "asynchClientServer"), 0, 0, 0,
	      //(char*)szNotifURL, bReturnCertData, bReturnRevocationData, 
          &resp);
  // report error or success
  ddocDebug(3, "ddocDdsMobileAuthenticate", "status: %s, sesscode: %ld", 
	    resp.Status, resp.Sesscode);
  // copy response status & err & faults
  ddocDdsCopyStatusAndFault(pDds, resp.Status, NULL, err);
  // get real error code
  if(pDds && pDds->szSoapFaultString && isdigit(pDds->szSoapFaultString[0]))
	  err = atoi(pDds->szSoapFaultString);
  else
	  if(err != SOAP_OK)
		err = ERR_GENERIC_SOAP_ERR;
  // copy response data
  if(resp.Status && !strcmp(resp.Status, "OK")) {
    pDds->lDdsSesscode = resp.Sesscode;
    if(pMbUserIDCode && resp.UserIDCode)
      ddocMemAssignData(pMbUserIDCode, resp.UserIDCode, -1);
    if(pMbUserGivenname && resp.UserGivenname)
      ddocMemAssignData(pMbUserGivenname, resp.UserGivenname, -1);
    if(pMbUserSurname && resp.UserSurname)
      ddocMemAssignData(pMbUserSurname, resp.UserSurname, -1);
    if(pMbUserCountry && resp.UserCountry)
      ddocMemAssignData(pMbUserCountry, resp.UserCountry, -1);
    if(pMbUserCN && resp.UserCN)
      ddocMemAssignData(pMbUserCN, resp.UserCN, -1);
    if(pMbCertData && resp.CertificateData)
      ddocMemAssignData(pMbCertData, resp.CertificateData, -1);
    if(pMbChallengeId && resp.ChallengeID)
      ddocMemAssignData(pMbChallengeId, resp.ChallengeID, -1);
    if(pMbChallenge && resp.Challenge)
      ddocMemAssignData(pMbChallenge, resp.Challenge, -1);
    if(pMbRevocationData && resp.RevocationData)
      ddocMemAssignData(pMbRevocationData, resp.RevocationData, -1);
  }
  // cleanup soap data
  ddocDdsCleanup(pDds);
  return err;
}

//--------------------------------------------------
// Sends an DDS request to get authentication status
// pDds - pointer to DDS context structure
// pMbSignature - buffer for signature value [OUT]
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsGetMobileAuthenticateStatus(DDS* pDds, int bWaitSignature, 
						  DigiDocMemBuf *pMbSignature)
{
  int err = SOAP_OK, l1;
  struct d__GetMobileAuthenticateStatusResponse resp;

  RETURN_IF_NULL_PARAM(pDds);
  memset(&resp, 0, sizeof(struct d__GetStatusInfoResponse));
  ddocDebug(3, "ddocDdsGetMobileAuthenticateStatus", "sesscode: %ld wait: %d", 
	    pDds->lDdsSesscode, bWaitSignature);
  // cleanup old info if applicable
  ddocDdsCleanupStatusAndFault(pDds);
  // send DDS request
  ddocDebug(3, "ddocDdsGetMobileAuthenticateStatus", "Sending request!");
  err = soap_call_d__GetMobileAuthenticateStatus(&(pDds->soap), 
						 (const char*)pDds->endpoint, "", 
						 pDds->lDdsSesscode, bWaitSignature, &resp);
  // report error or success
  ddocDebug(3, "ddocDdsGetMobileAuthenticateStatus", "status: %s, signature: %s", 
	    resp.Status, (resp.Signature ? "OK" : "NULL"));
  // copy response status & err & faults
  ddocDdsCopyStatusAndFault(pDds, resp.Status, NULL, err);
  // copy response signed doc info
  if(pMbSignature && resp.Signature) {
    l1 = strlen(resp.Signature);
    ddocMemSetLength(pMbSignature, l1);
    hex2bin((const char*)resp.Signature, (byte*)pMbSignature->pMem, &l1);
    pMbSignature->nLen = l1;
  }
  // cleanup soap data
  ddocDdsCleanup(pDds);
  if(err != SOAP_OK)
    err = ERR_GENERIC_SOAP_ERR;
  return err;
}

//--------------------------------------------------
// Translates error codes and strings returned by service
// to libdigidoc error codes
// pDds - pointer to DDS context structure
// nErr - direct numeric error code returned by soap
// libdigidoc error code
//--------------------------------------------------
int findServiceErrCode(DDS* pDds, int nErr)
{
	int err = ERR_OK;
  // get real error code
  // ddocDebug(3, "findServiceErrCode err: %d - %s", nErr, (pDds ? pDds->szSoapFaultString : 0)); 
  if(pDds && pDds->szSoapFaultString && isdigit(pDds->szSoapFaultString[0]))
	  err = atoi(pDds->szSoapFaultString);
  else {
	  if(nErr == SOAP_TCP_ERROR) { 
	    err = ERR_CONNECTION_FAILURE;
	  } else if(nErr != SOAP_OK)
		err = ERR_GENERIC_SOAP_ERR;
  }
  // errors in string form
  if(pDds && pDds->szStatusCode && !strcmp(pDds->szStatusCode, "EXPIRED_TRANSACTION"))
	  err = ERR_WPKI_TIMEOUT;
  if(pDds && pDds->szStatusCode && !strcmp(pDds->szStatusCode, "USER_CANCEL"))
	  err = ERR_WPKI_CANCELLED;
  if(pDds && pDds->szStatusCode && !strcmp(pDds->szStatusCode, "MID_NOT_READY"))
	  err = ERR_WPKI_MID_NOT_READY;
  if(pDds && pDds->szStatusCode && !strcmp(pDds->szStatusCode, "PHONE_ABSENT"))
	  err = ERR_WPKI_PHONE_NOT_REACHABLE;
  if(pDds && pDds->szStatusCode && !strcmp(pDds->szStatusCode, "SENDING_ERROR"))
	  err = ERR_WPKI_SENDING_ERROR;
  if(pDds && pDds->szStatusCode && !strcmp(pDds->szStatusCode, "SIM_ERROR"))
	  err = ERR_WPKI_SIM_ERROR;
  if(pDds && pDds->szStatusCode && !strcmp(pDds->szStatusCode, "INTERNAL_ERROR"))
	  err = ERR_WPKI_SERVICE_ERR;
  // numeric code switch
  switch(err) {
  case 100:  err = ERR_GENERIC_SOAP_ERR; break;
  //case 101:  err = ERR_BAD_PARAM; break;
  case 102:  err = ERR_NULL_PARAM; break;
  case 103:  err = ERR_WPKI_UNTRUSTED_USER; break;
  //case 101:
  case 104: // id-code doesn't match phone number
	  err = ERR_WPKI_INVALID_PHONE_NO;
	  break;
  case 200:  err = ERR_WPKI_SERVICE_ERR; break;
  case 300:  err = ERR_WPKI_UNUSABLE_PHONE; break;
  case 301:  err = ERR_WPKI_UNKNOWN_USER; break;
  case 302:  err = ERR_OCSP_CERT_REVOKED; break;
  case 101:  err = ERR_OCSP_CERT_REVOKED; break;
  case 303:  err = ERR_OCSP_CERT_UNKNOWN; break;
  }
  return err;
}


//--------------------------------------------------
// Signs the document using DigiDocService.
// Digidoc file must have been saved before
// to calculate correct hashes.
// plSesscode - pointer to buffer for session code
// pSigDoc - signed document object
// szSigningProfile - signing profile
// szServiceName - service name
// szIdCode - signers id-code
// szCountryCode - signers country code
// szPhoneNo - users phone number
// szMessageToDisplay - message to display
// manifest - manifest or role
// city - signers address , city
// state - signers address , state or province
// zip - signers address , postal code
// country - signers address , country name
// szLang - language code
// pMbChallengeId - buffer for challenge id [OUT]
// return returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsCreateMobileSignature(long* plSesscode, const SignedDoc *pSigDoc,
					const char* szSigningProfile, const char* szServiceName,
				    const char* szIdCode, const char* szCountryCode,
				    const char* szPhoneNo, const char* szMessageToDisplay,
				    const char* manifest, const char* city, 
				    const char* state, const char* zip, 
				    const char* country, const char* szLang, 
					DigiDocMemBuf *pMbChallengeId)
{
  int err = ERR_OK, i, n, l;
  struct d__MobileCreateSignatureResponse resp;
  struct d__DataFileDigestList dflst;
  SignatureInfo* pSigInfo = 0;
  DataFile* pDf = 0;
  DigiDocMemBuf *pMbuf = 0;
  DDS* pDds = 0;
  char id[10];

  ddocDebug(3, "ddocDdsMobileCreateSignature", 
	  "Creating DDS signature for id: %s country: %s phone: %s msg: %s role: %s city: %s state: %s zip: %s country-name: %s lang: %s",
  		(szIdCode ? szIdCode : "NULL"), (szCountryCode ? szCountryCode : "NULL"), 
  		(szPhoneNo ? szPhoneNo : "NULL"), (szMessageToDisplay ? szMessageToDisplay : "NULL"),
		(manifest ? manifest : "NULL"), (city ? city : "NULL"),
		(state ? state : "NULL"), (zip ? zip : "NULL"),
		(country ? country : "NULL"), (szLang ? szLang : "NULL"));
  RETURN_IF_NULL_PARAM(plSesscode);
  RETURN_IF_NULL_PARAM(pSigDoc);
  RETURN_IF_NULL_PARAM(((szIdCode && szCountryCode) || szPhoneNo));
  //RETURN_IF_NULL_PARAM(szMessageToDisplay || manifest);
  // connect
  *plSesscode = 0;
  err = ddocDdsConnect(&pDds);
  if(err) return err;
  // set old DF & SIG info
  n = getCountOfDataFiles(pSigDoc);
  memset(&resp, 0, sizeof(struct d__MobileCreateSignatureResponse));
  memset(&dflst, 0, sizeof(struct d__DataFileDigestList));
  if(n > 0) {
    dflst.__ptrDataFileDigest = (struct d__DataFileDigest*)malloc(sizeof(struct d__DataFileDigest) * n);
	RETURN_IF_BAD_ALLOC(dflst.__ptrDataFileDigest);
	dflst.__sizeOfDataFileDigests = n;
    for(i = 0; i < n; i++) {
		pDf = getDataFile(pSigDoc, i);
		pMbuf = ddocDataFile_GetDigestValue(pDf);
		dflst.__ptrDataFileDigest[i].Id = pDf->szId;
		dflst.__ptrDataFileDigest[i].DigestType = DIGEST_SHA1_NAME;
		l = DIGEST_LEN * 2;
		dflst.__ptrDataFileDigest[i].DigestValue = (char*)malloc(l);
		RETURN_IF_BAD_ALLOC(dflst.__ptrDataFileDigest[i].DigestValue);
		memset(dflst.__ptrDataFileDigest[i].DigestValue, 0, l);
		encode((const byte*)pMbuf->pMem, pMbuf->nLen, 
			(byte*)dflst.__ptrDataFileDigest[i].DigestValue, &l);
		ddocDebug(3, "ddocDdsMobileCreateSignature", 
	       "Sending DF: %s hash: %s", pDf->szId, (char*)dflst.__ptrDataFileDigest[i].DigestValue);
	}
  }
  pSigInfo = ddocGetLastSignature(pSigDoc);
  snprintf(id, sizeof(id), "S%d", getNextSignatureId(pSigDoc));

  // send DDS request
  ddocDebug(3, "ddocDdsMobileCreateSignature", "Sending request!");
  err = soap_call_d__MobileCreateSignature(&(pDds->soap), (const char*)pDds->endpoint, "", 
							 (char*)szIdCode, (szCountryCode ? (char*)szCountryCode : " "), 
							 (char*)szPhoneNo, (char*)(szLang ? szLang : "ENG"), 
							 (char*)szServiceName, (char*)szMessageToDisplay,
							 (char*)manifest, (char*)city, (char*)state, (char*)zip, 
							 (char*)country, (char*)szSigningProfile, &dflst,
							 (char*)pSigDoc->szFormat, (char*)pSigDoc->szFormatVer, 
							 (char*)id, (char*)"asynchClientServer", 0, &resp);

  ddocDebug(3, "ddocDdsMobileCreateSignature", "RC: %d session: %ld status: %s challenge-id: %s", 
	  err, resp.Sesscode, (resp.Status ? (char*)resp.Status : "NULL"),
	  (resp.ChallengeID ? (char*)resp.ChallengeID : "NULL"));
  // copy response status & err & faults
  pDds->lDdsSesscode = *plSesscode = resp.Sesscode;
  ddocDdsCopyStatusAndFault(pDds, resp.Status, NULL, err);
  // try to find real error number
  err = findServiceErrCode(pDds, err);
  // copy response signed doc info
  if(pMbChallengeId && resp.ChallengeID)
      ddocMemAssignData(pMbChallengeId, resp.ChallengeID, -1);
  // cleanup soap data
  ddocDdsCleanup(pDds);
  // report on success
  ddocDebug(3, "ddocDdsMobileCreateSignature", "SignatureReq: %d sess: %ld status: %s code: %s", 
	    err, pDds->lDdsSesscode, (pDds->szStatus ? pDds->szStatus : ""),
	    (pDds->szStatusCode ? pDds->szStatusCode : ""),
	    (pDds->szSoapFaultCode ? pDds->szSoapFaultCode : ""),
	    (pDds->szSoapFaultString ? pDds->szSoapFaultString : ""),
	    (pDds->szSoapFaultDetail ? pDds->szSoapFaultDetail : ""));
  ddocDdsDisconnect(pDds);
  return err;
}


//--------------------------------------------------
// Sends an DDS request to create a new signature via
// MSSP gateway and mobile signing
// lSesscode - DDS sessioncode
// bWaitSig - 1=wait synchronously for signature
// pMbStatus - buffer for status
// szTempDdocFile - temporary digidoc file name
// ppSigDoc - address of SignedDoc object
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsGetCreateSignatureStatusInfo(long lSesscode, int bWaitSig,
												   DigiDocMemBuf *pMbStatus,
												   const char* szTempDdocFile,
												   SignedDoc** ppSigDoc)
{
  int err = SOAP_OK, l1;
  struct d__GetMobileCreateSignatureStatusResponse resp;
  DDS* pDds = 0;
  FILE* hFile = 0;
  char* buf1[110], *p1;
  long lSize, lPos;
  
  RETURN_IF_NULL_PARAM(pMbStatus);
  RETURN_IF_NULL_PARAM(szTempDdocFile);
  RETURN_IF_NULL_PARAM(ppSigDoc);
  memset(&resp, 0, sizeof(struct d__GetMobileCreateSignatureStatusResponse));
  ddocDebug(3, "ddocDdsGetCreateSignatureStatusInfo", "sesscode: %ld wait-sig: %d temp-file: %s",
	  lSesscode, bWaitSig, (szTempDdocFile ? szTempDdocFile : "NULL"));
  // connect
  err = ddocDdsConnect(&pDds);
  if(err) return err;
  pDds->lDdsSesscode = lSesscode;
  // send DDS request
  ddocDebug(3, "ddocDdsGetCreateSignatureStatusInfo", "Sending request!");
  err = soap_call_d__GetMobileCreateSignatureStatus(&(pDds->soap), 
	      (const char*)pDds->endpoint, "", pDds->lDdsSesscode, bWaitSig, &resp);
  // report error or success
  ddocDebug(3, "ddocDdsGetCreateSignatureStatusInfo", "status: %s, signature: %s", 
	    resp.Status, resp.Signature);
  // copy response status & err & faults
  ddocDdsCopyStatusAndFault(pDds, resp.Status, NULL, err);
  // try to find real error number
  err = findServiceErrCode(pDds, err);
  // copy response signed doc info
  if(pMbStatus && resp.Status)
      ddocMemAssignData(pMbStatus, resp.Status, -1);
  // if we got a new signature then add it to temp file ans reread it in
  if(resp.Status && !strcmp(resp.Status, "SIGNATURE") &&
	  resp.Signature) {
	  ddocDebug(3, "ddocDdsGetCreateSignatureStatusInfo", "appending signature to: %s", szTempDdocFile);
	  //calculateFileSize(szTempDdocFile, &lSize);
	  if((hFile = fopen(szTempDdocFile, "r+b")) != NULL) {
		memset(buf1, 0, sizeof(buf1));
		fseek(hFile, 0, SEEK_END);
		lSize = ftell(hFile);
		// seek back max 300 bytes
		fseek(hFile, ((lSize > 100) ? (lSize - 100) : 0), SEEK_SET);
		lPos = ftell(hFile);
		l1 = fread(buf1, 1, ((lSize > 100) ? 100 : lSize), hFile);
		p1 = strstr((const char*)buf1, "</SignedDoc>");
		ddocDebug(3, "ddocDdsGetCreateSignatureStatusInfo", 
			"pos: %ld go: %ld", lPos, (long)((long)p1 - (long)buf1 - l1));
		fseek(hFile, (long)((long)p1 - (long)buf1 - l1) , SEEK_CUR);
		lPos = ftell(hFile);
		// now write new signature
		fputs((const char*)resp.Signature, hFile);
		fputs("</SignedDoc>", hFile);
		fclose(hFile);
		ddocDebug(3, "ddocDdsGetCreateSignatureStatusInfo", "signature appended");
		// release old digidoc if necessary
		if(*ppSigDoc) {
			SignedDoc_free(*ppSigDoc);
			*ppSigDoc = 0;
		}
		err = ddocSaxReadSignedDocFromFile(ppSigDoc, szTempDdocFile, 0, 0);
		ddocDebug(3, "ddocDdsGetCreateSignatureStatusInfo", "read file: %d", err);
		
	  }
	  else
		 err = ERR_FILE_WRITE;
  }
  // cleanup soap data
  //ddocDdsCleanup(pDds);
  ddocDdsDisconnect(pDds);
  return err;
}


