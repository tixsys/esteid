#ifndef __DIGIDOC_DDS_H__
#define __DIGIDOC_DDS_H__
//==================================================
// FILE:	DigiDocDds.h
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

#include <libdigidoc/DigiDocDefs.h>
#include <libdigidoc/DigiDocMem.h>
#include <libdigidoc/DigiDocObj.h>
#include <openssl/x509.h>
#include <stdsoap2.h>

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct DDS_st
{
  struct soap soap;
  const char *endpoint;
  unsigned long lDdsSesscode;
  char* szStatus;
  char* szStatusCode;
  int err;
  char* szSoapFaultCode;
  char* szSoapFaultString;
  char* szSoapFaultDetail;
  struct d__SignedDocInfo* pSigDocInfo;
} DDS;

//--------------------------------------------------
// Initializes DDS connection
// pDds - pointer to DDS context structure
// returns ERR_OK on success
//--------------------------------------------------
EXP_OPTION int ddocDdsConnect(DDS** pDds);

//--------------------------------------------------
// Cleanup DDS connection. Free memory
// pDds - pointer to DDS context structure
// returns ERR_OK on success
//--------------------------------------------------
EXP_OPTION int ddocDdsDisconnect(DDS* pDds);

//--------------------------------------------------
// Cleanup DDS connection but don't disconnect
// pDds - pointer to DDS context structure
// returns ERR_OK on success
//--------------------------------------------------
EXP_OPTION int ddocDdsCleanup(DDS* pDds);

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
				   DigiDocMemBuf* pMBufDdoc);

//--------------------------------------------------
// Sends an DDS request to close a session
// pDds - pointer to DDS context structure
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsCloseSession(DDS* pDds);

//--------------------------------------------------
// Sends an DDS request to create a new empty digidoc
// pDds - pointer to DDS context structure
// szFormat - digidoc format
// szVersion - digidoc format version
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsCreateSignedDoc(DDS* pDds, const char* szFormat, const char* szVersion);

//--------------------------------------------------
// Sends an DDS request to add a new data file
// pDds - pointer to DDS context structure
// szFileName - filename and path
// szMimeType - mime type
// szContentType - content type
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsAddDataFile(DDS* pDds, const char* szFileName, 
				  const char* szMimeType, const char* szContentType);

//--------------------------------------------------
// Sends an DDS request to remove a data file
// pDds - pointer to DDS context structure
// szId - data file id
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsRemoveDataFile(DDS* pDds, const char* szId);

//--------------------------------------------------
// Sends an DDS request to retrieve current 
// digidoc data and stores in in the given file
// pDds - pointer to DDS context structure
// szFileName - filename to store the digidoc
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsGetSignedDoc(DDS* pDds, const char* szFileName);

//--------------------------------------------------
// Sends an DDS request to retrieve current 
// digidoc info.
// pDds - pointer to DDS context structure
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsGetSignedDocInfo(DDS* pDds);

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
EXP_OPTION int ddocDdsGetDataFile(DDS* pDds, const char* szId, const char* szFileName);


//--------------------------------------------------
// Sends an DDS request to retrieve the given 
// data signatures signers certificate
// pDds - pointer to DDS context structure
// szId - signature id [mandatory]
// szFileName - file to store data in. [mandatory]
// Data will be in PEM format!
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsGetSignersCertificate(DDS* pDds, const char* szId, const char* szFileName);

//--------------------------------------------------
// Sends an DDS request to retrieve the given 
// data signatures notarys (responders) certificate
// pDds - pointer to DDS context structure
// szId - signature id [mandatory]
// szFileName - file to store data in. [mandatory]
// Data will be in PEM format!
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsGetNotarysCertificate(DDS* pDds, const char* szId, const char* szFileName);

//--------------------------------------------------
// Sends an DDS request to retrieve the given 
// data signatures OCSP response data
// pDds - pointer to DDS context structure
// szId - signature id [mandatory]
// szFileName - file to store data in. [mandatory]
// Data will be in PEM format!
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsGetNotary(DDS* pDds, const char* szId, const char* szFileName);

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
// szSigningProfile - signing profile URL [optional]
// returns error code or SOAP_OK
//--------------------------------------------------
 EXP_OPTION int ddocDdsPrepareSignature(DDS* pDds, 
		const char* szSigningProfile,
                const X509* pCert, const char* szCertFile,
		const char* szTokenId, const char* szRole, const char* szCity,
		const char* szState, const char* szPostalCode, const char* szCountry,
		char* szSigId, char* szHash, int *pHashLen);

//--------------------------------------------------
// Sends an DDS request to finalize a new signature
// pDds - pointer to DDS context structure
// szId - siganture id [mandatory]
// szSigValHEX - signature value in hex [mandatory]
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsFinalizeSignature(DDS* pDds, const char* szId, const char* szSigValHEX);

//--------------------------------------------------
// Sends an DDS request to remove a signature
// pDds - pointer to DDS context structure
// szId - siganture id [mandatory]
// szSigValHEX - signature value in hex [mandatory]
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsRemoveSignature(DDS* pDds, const char* szId);

//--------------------------------------------------
// Sends an DDS request to remove a signature
// pDds - pointer to DDS context structure
// szName - buffer for service name [mandatory]
// szVersion - buffer for service version [mandatory]
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsGetVersion(DDS* pDds, char* szName, char* szVersion);

//--------------------------------------------------
// Sends an DDS request to create a new signature via
// DigiDocService and mobile signing
// pDds - pointer to DDS context structure
// szSigningProfile - signing profile URL
// szCountryCode - signers country code (2 chars)
// szSignerIdCode - signers personal id code 
// szSignerPhoneNo - signers phone number 
// [mandatory] - either szIdCode and szCountryCode or szPhoneNo
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
				 const char* szDigiDocFileName);

//--------------------------------------------------
// Sends an DDS request to create a new signature via
// MSSP gateway and mobile signing
// lSesscode - DDS sessioncode
// bReturnDocInfo - 1/0 send digidoc info or not
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsGetStatusInfo(long lSesscode, int bReturnDocInfo, 
									char* pszStartusStr, int nStatusLen);

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
				 const char* ServiceName,
				 DigiDocMemBuf *pMbUserIDCode, DigiDocMemBuf *pMbUserGivenname,
				 DigiDocMemBuf *pMbUserSurname, DigiDocMemBuf *pMbUserCountry,
				 DigiDocMemBuf *pMbUserCN, DigiDocMemBuf *pMbCertData,
				 DigiDocMemBuf *pMbChallengeId, DigiDocMemBuf *pMbChallenge, 
				 DigiDocMemBuf *pMbRevocationData);

//--------------------------------------------------
// Sends an DDS request to get authentication status
// pDds - pointer to DDS context structure
// bWaitSignature - 1=wait until signature or err, 0=return immediately
// pMbSignature - buffer for signature value [OUT]
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocDdsGetMobileAuthenticateStatus(DDS* pDds, int bWaitSignature, 
						  DigiDocMemBuf *pMbSignature);

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
					DigiDocMemBuf *pMbChallengeId);

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
												   SignedDoc** ppSigDoc);


#ifdef  __cplusplus
}
#endif

#endif // __DIGIDOC_DDS_H__

