//==================================================
// FILE:	digidoc.c
// PROJECT:     Digi Doc
// DESCRIPTION: Utility program to demonstrate the
//   functionality of DigiDocLib
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
//      12.01.2004      Veiko Sinivee
//                      Creation
//==================================================

#include <libdigidoc/DigiDocDefs.h>
#include <libdigidoc/DigiDocLib.h>
#include <libdigidoc/DigiDocConfig.h>
#include <libdigidoc/DigiDocPKCS11.h>
#include <libdigidoc/DigiDocSAXParser.h>
#include <libdigidoc/DigiDocParser.h>
#include <libdigidoc/DigiDocEncSAXParser.h>
#include <libdigidoc/DigiDocEnc.h>
#include <libdigidoc/DigiDocEncGen.h>
#include <libdigidoc/DigiDocConvert.h>
#include <libdigidoc/DigiDocDebug.h>
#include <libdigidoc/DigiDocCert.h>
#include <libdigidoc/DigiDocObj.h>
#include <libdigidoc/DigiDocGen.h>
#include <libdigidoc/DigiDocDfExtract.h>

#ifdef WITH_MSSP
  #include <libdigidoc/mssp/DigiDocMsspGw.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/objects.h>
#include <sys/stat.h>
#include <time.h>

//==========< global constants >====================

// programm arguments
char* p_szInFile = 0;
char* p_szInEncFile = 0;
char* p_szInDecFile = 0;
char* p_szOutFile = 0;
char* p_szConfigFile = 0;
int p_parseMode = 1;

int g_cgiMode = 0;   // 1=output in CGI mode, 0=normal e.g. human readable mode
char* g_szOutputSeparator = 0;
char *errorClass[] = {"NO_ERRORS", "TECHNICAL", "USER", "LIBRARY"};
char* g_szProgNameVer = "cdigidoc/"DIGIDOC_VERSION;

#ifdef WITH_MSSP
MSSP* g_pMssp = 0;
#endif

//==========< helper functions for argument handling >====================



//--------------------------------------------------
// Handles one argument
//--------------------------------------------------
int checkArguments(int argc, char** argv, int* pCounter, char** dest)
{
  int nLen = 0;

  if(argc > (*pCounter)+1 && argv[(*pCounter)+1][0] != '-') {
    nLen = strlen(argv[(*pCounter)+1]) * 2;
    ddocConvertInput((const char*)argv[(*pCounter)+1], dest);
    (*pCounter)++;
  }
  return nLen;
}

//--------------------------------------------------
// checks the existence of one command line argument
//--------------------------------------------------
int hasCmdLineArg(int argc, char** argv, const char* argName)
{
  int i;

  for(i = 1; i < argc; i++) {
    if(argv[i] != NULL &&  argv[i][0] == '-' &&
       !strcmp(argv[i], argName)) {
      return i;
    }
  }
  return 0;
}

//--------------------------------------------------
// Returns the value of one command line argument
//--------------------------------------------------
int checkCmdLineArg(int argc, char** argv, const char* argName, char** dest)
{
  int i;

  *dest = 0; // mark as not found
  for(i = 1; i < argc; i++) {
    if(argv[i] != NULL &&  argv[i][0] == '-' &&
       !strcmp(argv[i], argName)) {
       if((i + 1 < argc) &&  (argv[i+1][0] != '-')) {
         ddocConvertInput((const char*)argv[i+1], dest);
         return 0;
       }
       else
         return ERR_BAD_PARAM;
    }
  }
  return 0; // not found but no error
}

//--------------------------------------------------
// prints usage statement
//--------------------------------------------------
void printUsage()
{
  fprintf(stderr, "USAGE: cdigidoc <command(s)> [-in <input-file>] [-out <output-file>] [-config <config-file>]\n");
  fprintf(stderr, "COMMANDS:\n");
  fprintf(stderr, "\t[-new] [format] [version]\n");
  fprintf(stderr, "\t-check-cert <-cerificate-file-name (PEM format)>\n");
  fprintf(stderr, "\t-add <file-name> <mime-type> [<content-type>] [<charset>]\n");
  fprintf(stderr, "\t-verify \n");
  fprintf(stderr, "\t-list \n");
  fprintf(stderr, "\t-sign <pin> [<manifest>] [<city> <state> <zip> <country>]\n");
  fprintf(stderr, "\t-extract <doc-id> <file-name> [<charset>] [<file-name-charset>]\n");
  fprintf(stderr, "\t-encrypt <file-name>\n");
  fprintf(stderr, "\t-encrecv <cert-file> [<recipient>] [<keyname>] [<carried-key-name>]\n");
  fprintf(stderr, "\t-decrypt <file-name> <pin1>\n");
  fprintf(stderr, "\t-denc-list <file-name>\n");
  fprintf(stderr, "\t-encrypt-file <in-file-name> <out-file-name> [<in-file-mime-type>]\n");
  fprintf(stderr, "\t-decrypt-file <in-file-name> <out-file-name> <pin1>\n");

  fprintf(stderr, "\t-calc-sign <cert-file> [<manifest>] [<city> <state> <zip> <country>]\n");
  fprintf(stderr, "\t-add-sign-value <sign-value-file> <sign-id>\n");
  fprintf(stderr, "\t-del-sign <sign-id>\n");
  fprintf(stderr, "\t-get-confirmation <sign-id>\n");

#ifdef WITH_MSSP
  fprintf(stderr, "\t-mssp-sign <phone-no> <polling-freq-seconds> (0=none) [<manifest>] [<city> <state> <zip> <country>]\n");
  fprintf(stderr, "\t-mssp-status\n");
#endif

  fprintf(stderr, "OPTIONS:\n");
  fprintf(stderr, "\t-cgimode [<ouput-separator] - output in CGI mode\n");
  fprintf(stderr, "\t-SAX - use SAX parser\n");
  fprintf(stderr, "\t-XRDR - use XmlReader parser\n");
}

//--------------------------------------------------
// Checks program runtime arguments
//--------------------------------------------------
int checkProgArguments(int argc, char** argv)
{
  int err = ERR_OK;

  // -?, -help -> print usage
  if(!err && hasCmdLineArg(argc, argv, "-?") ||
     hasCmdLineArg(argc, argv, "-help"))
     printUsage();
  // -in <input-file>
  if(!err)
    if((err = checkCmdLineArg(argc, argv, "-in", &p_szInFile)) != ERR_OK)
      addError(err, __FILE__, __LINE__, "Missing or invalid input file name");
  // -SAX  -> use Sax parser (default)
  if(!err)
    if(hasCmdLineArg(argc, argv, "-SAX"))
      p_parseMode = 1;
  // -SAX  -> use Sax parser (default)
  if(!err)
    if(hasCmdLineArg(argc, argv, "-XRDR"))
      p_parseMode = 2;
  // -out <outut-file>
  if(!err)
    if((err = checkCmdLineArg(argc, argv, "-out", &p_szOutFile)) != ERR_OK)
      addError(err, __FILE__, __LINE__, "Missing or invalid output file name");
  // -encrypt <encryption-input-file>
  if(!err)
    if((err = checkCmdLineArg(argc, argv, "-encrypt", &p_szInEncFile)) != ERR_OK)
      addError(err, __FILE__, __LINE__, "Missing or invalid encrypt input file name");
  // -config <config-file>
  if(!err)
    if((err = checkCmdLineArg(argc, argv, "-config", &p_szConfigFile)) != ERR_OK)
      addError(err, __FILE__, __LINE__, "Missing or invalid configuration file name");
  // -CGI  -> use CGI output mode
  if(!err)
    if((err = checkCmdLineArg(argc, argv, "-cgimode", &g_szOutputSeparator)) != ERR_OK)
      addError(err, __FILE__, __LINE__, "Missing or invalid cgi output separator");
		g_cgiMode = 1;
  if(!g_szOutputSeparator || !g_szOutputSeparator[0] || err) {
    g_szOutputSeparator = strdup("|");
    err = ERR_OK;
  }
  return err;
}

//--------------------------------------------------
// reads various statusflags from config file
//--------------------------------------------------
void readConfigParams()
{
  int n = 0;
  const char* s = 0;

  // check if we are in CGI mode
  n = ConfigItem_lookup_bool("DIGIDOC_CGI_MODE", 0);
  if(!g_cgiMode && n)
    g_cgiMode = n;
  s = ConfigItem_lookup("DIGIDOC_CGI_SEPARATOR");
  if(!g_szOutputSeparator && s)
    g_szOutputSeparator = (char*)strdup(s);
}


//==========< command handlers >====================

//--------------------------------------------------
// Creates a new signed doc
//--------------------------------------------------
int cmdNew(SignedDoc** ppSigDoc, const char* pFormat, const char* pVersion)
{
  int err = ERR_OK;
  const char* format = pFormat, *version = pVersion;

  if(!p_szOutFile) {
    err = ERR_BAD_PARAM;
    addError(err, __FILE__, __LINE__, "No output file specified");
    return err;
  }
  if(!format)
    format = ConfigItem_lookup("DIGIDOC_FORMAT");
  if(!version)
    version = ConfigItem_lookup("DIGIDOC_VERSION");
  if(format && version) {
     err = SignedDoc_new(ppSigDoc, format, version);
     RETURN_IF_NOT(err == ERR_OK, err);
  } else {
    err = ERR_UNSUPPORTED_FORMAT;
    addError(err, __FILE__, __LINE__, "Error finding new document format or version");
  }
  return err;
}



//--------------------------------------------------
// Adds a DataFile to signed doc
//--------------------------------------------------
int cmdAddDataFile(SignedDoc** ppSigDoc, const char* file, 
		   const char* mime, const char* content, const char* charset)
{
  int err = ERR_OK, l1;
  DataFile  *pDataFile;
  char *p = 0, buf1[300];

  // if there was no new command then implicitly create a new document
  if(!(*ppSigDoc)) {
    err = cmdNew(ppSigDoc, NULL, NULL);
    RETURN_IF_NOT(err == ERR_OK, err); 
  }
  // convert to UTF-8
  err = ddocConvertInput(file, &p);
  l1 = sizeof(buf1);
  getFullFileName(p, buf1, l1);
  if(p)
    free(p);
  // add a file	
  err = DataFile_new(&pDataFile, *ppSigDoc, NULL, buf1, 
		     content, mime, 0, NULL, 0, NULL, charset);
  if(!err)
    err = calculateDataFileSizeAndDigest(*ppSigDoc, pDataFile->szId, buf1, DIGEST_SHA1);
  
  RETURN_IF_NOT(err == ERR_OK, err);
    return err;
}

//--------------------------------------------------
// Create digidoc and adds datafiles
//--------------------------------------------------
int runAddCmds(int argc, char** argv, SignedDoc** ppSigDoc)
{
  int err = ERR_OK, i;

  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      // create new digidoc
      if(!strcmp(argv[i], "-new")) {
	char* format = NULL;
	char* version = NULL;
	// optional content and charset
      checkArguments(argc, argv, &i, &format);
      checkArguments(argc, argv, &i, &version);
	//printf("format: %s version: %s\n", format, version);
	err = cmdNew(ppSigDoc, format, version);
	RETURN_IF_NOT(err == ERR_OK, err);
      }
      // add a DataFile
      if(!strcmp(argv[i], "-add")) {
	if(argc > i+2 && argv[i+1][0] != '-' && argv[i+2][0] != '-') {
	  char* file = argv[i+1];
	  char* mime = argv[i+2];
	  char* content = CONTENT_EMBEDDED_BASE64;
	  char* charset = CHARSET_UTF_8;
	  i += 2;
	  // optional content and charset
      checkArguments(argc, argv, &i, &content);
      checkArguments(argc, argv, &i, &charset);
	  err = cmdAddDataFile(ppSigDoc, (const char*)file, (const char*)mime,
			       (const char*)content, (const char*)charset);
	} else {
	  err = ERR_BAD_PARAM;
	  addError(err, __FILE__, __LINE__, "Missing <file> and <mime-type> arguments of -add command");
	}
      }
      
    }
  }
  return err;
}

//--------------------------------------------------
// Create digidoc and adds datafiles
//--------------------------------------------------
/*int runAddSignValue(int argc, char** argv, SignedDoc** ppSigDoc)
{
  int err = ERR_OK, i;

  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      // create new digidoc
      if(!strcmp(argv[i], "-addsignvalue")) {
		char* infile = NULL;
		char* sigid = NULL;
		char* sigvalf = NULL;
	  checkArguments(argc, argv, &i, &infile);
      checkArguments(argc, argv, &i, &sigid);
	  checkArguments(argc, argv, &i, &sigvalf);
		err = cmdReadDigiDoc(ppSigDoc, 0, 1);

      }
      // add a DataFile
      if(!strcmp(argv[i], "-addsignvalue")) {
	if(argc > i+2 && argv[i+1][0] != '-' && argv[i+2][0] != '-') {
	  char* file = argv[i+1];
	  char* mime = argv[i+2];
	  char* content = CONTENT_EMBEDDED_BASE64;
	  char* charset = CHARSET_UTF_8;
	  i += 2;
	  // optional content and charset
      checkArguments(argc, argv, &i, &content);
      checkArguments(argc, argv, &i, &charset);
	  err = cmdAddDataFile(ppSigDoc, (const char*)file, (const char*)mime,
			       (const char*)content, (const char*)charset);
	} else {
      err = ERR_BAD_PARAM;
      addError(err, __FILE__, __LINE__, "Missing <in-digidoc> and <sig-id> or <sign-value-file> arguments of -addsignvalue command");
      }
      
    }
  }
  return err;
}*/

//--------------------------------------------------
// Add ecryption recipients
//--------------------------------------------------
int runRecipientCmds(int argc, char** argv, DEncEncryptedData** ppEncData)
{
  int err = ERR_OK, i;
  DEncEncryptedKey* pEncKey = 0;
  X509* pCert = 0;

  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      // add a recipients key
      if(!strcmp(argv[i], "-encrecv")) {
	if(argc > i+1 && argv[i+1][0] != '-') {
	  char* certfile = argv[i+1];
	  char* recipient = NULL;
	  char* keyname = NULL;
	  char* carriedkeyname = NULL;
	  char* id = NULL;
	  i++;
	  if(!(*ppEncData))
	    err = dencEncryptedData_new(ppEncData, DENC_XMLNS_XMLENC, DENC_ENC_METHOD_AES128, 0, 0, 0);
	  err = dencMetaInfo_SetLibVersion(*ppEncData);
	  if(err) return err;
	  err = dencMetaInfo_SetFormatVersion(*ppEncData);
	  if(err) return err;

	  // optional arguments
	  checkArguments(argc, argv, &i, &recipient);
	  checkArguments(argc, argv, &i, &keyname);
	  checkArguments(argc, argv, &i, &carriedkeyname);
	  err = ReadCertificate(&pCert, certfile);
	  if(err) return err;
	  err = dencEncryptedKey_new(*ppEncData, &pEncKey, pCert,
				     DENC_ENC_METHOD_RSA1_5,
				     id, recipient, keyname, carriedkeyname);
	} else {
	  err = ERR_BAD_PARAM;
	  addError(err, __FILE__, __LINE__, "Missing <cert-file> argument of -encrecv command");
	}
      }
    }
  }
  return err;
}




//--------------------------------------------------
// reads in a signed doc
//--------------------------------------------------
int cmdReadDigiDoc(SignedDoc** ppSigDoc, DEncEncryptedData** ppEncData, int nMode)
{
  int err = ERR_OK;
  int nMaxDfLen;

  if(!p_szInFile) {
    err = ERR_BAD_PARAM;
    addError(err, __FILE__, __LINE__, "No input file specified");
    return err;
  }
  nMaxDfLen = ConfigItem_lookup_int("DATAFILE_MAX_CACHE", 0);
  
  if(strstr(p_szInFile, ".cdoc")) {
    err = dencSaxReadEncryptedData(ppEncData, p_szInFile);
  } else {
    switch(nMode) {
    case 2: // new XMLReader interface
      //err = ddocXRdrReadSignedDocFromFile(p_szInFile, CHARSET_ISO_8859_1, ppSigDoc, nMaxDfLen);
      break;
    case 1:
    default:
      err = ddocSaxReadSignedDocFromFile(ppSigDoc, p_szInFile, 0, nMaxDfLen);
    }
  }
  return err;
}

//--------------------------------------------------
// writes a digidoc in file
//--------------------------------------------------
int cmdWrite(SignedDoc* pSigDoc, DEncEncryptedData* pEncData)
{
  int err = ERR_OK;
  FILE* hFile;
  
  if(!p_szOutFile) {
    err = ERR_BAD_PARAM;
    addError(err, __FILE__, __LINE__, "No output file specified");
    return err;
  }
  if(pSigDoc)
    err = createSignedDoc(pSigDoc, p_szInFile, p_szOutFile);
  if(pEncData) {
    if(pEncData->nDataStatus == DENC_DATA_STATUS_ENCRYPTED_AND_NOT_COMPRESSED ||
       pEncData->nDataStatus == DENC_DATA_STATUS_ENCRYPTED_AND_COMPRESSED) {
      err = dencGenEncryptedData_writeToFile(pEncData, p_szOutFile);
    } else {
      if((hFile = fopen(p_szOutFile, "wb")) != NULL) {
	fwrite((pEncData)->mbufEncryptedData.pMem, 1, 
	       (pEncData)->mbufEncryptedData.nLen, hFile);
	fclose(hFile);
      }
      else
	err = ERR_FILE_WRITE;
    }
  }
  if (err)
    addError(err, __FILE__, __LINE__, "Error writing file\n");
  return err;
}


//-----------------------------------------
// prints certificate info
//-----------------------------------------
void printCertificateInfo(SignedDoc* pSigDoc, X509* pCert)
{
  int err, p;
  char buf1[500];
  PolicyIdentifier* pPolicies;
  DigiDocMemBuf mbuf;
  int nPols;

  mbuf.pMem = 0;
  mbuf.nLen = 0;
  // serial number
  err = ReadCertSerialNumber(buf1, sizeof(buf1), pCert);
  if(g_cgiMode)
    fprintf(stdout, "\nX509Certificate%s%s",  g_szOutputSeparator, buf1);
  else
    fprintf(stdout, "\nX509Certificate nr: %s",  buf1);
  // issuer
  err = ddocCertGetIssuerDN(pCert, &mbuf);
  if(g_cgiMode)
    fprintf(stdout, "%s%s",  g_szOutputSeparator, (char*)mbuf.pMem);
  else
    fprintf(stdout, "\n\tIssuer: %s", (char*)mbuf.pMem);
  ddocMemBuf_free(&mbuf);
  // subject
  err = ddocCertGetSubjectDN(pCert, &mbuf);
  if(g_cgiMode)
    fprintf(stdout, "%s%s",  g_szOutputSeparator, (char*)mbuf.pMem);
  else
    fprintf(stdout, "\n\tSubject: %s", (char*)mbuf.pMem);
  ddocMemBuf_free(&mbuf);
  // ValidFrom
  memset(buf1, 0, sizeof(buf1));
  err = getCertNotBefore(pSigDoc, pCert, buf1, sizeof(buf1));
  if(g_cgiMode)
    fprintf(stdout, "%s%s",  g_szOutputSeparator, buf1);
  else
    fprintf(stdout, "\n\tNotBefore: %s", buf1, sizeof(buf1));
  // ValidTo
  memset(buf1, 0, sizeof(buf1));
  err = getCertNotAfter(pSigDoc, pCert, buf1, sizeof(buf1));
  if(g_cgiMode)
    fprintf(stdout, "%s%s",  g_szOutputSeparator, buf1);
  else
    fprintf(stdout, "\n\tNotAfter: %s",  buf1);
  // policy URL
  err = readCertPolicies(pCert, &pPolicies, &nPols);
  if(nPols) {
    for(p = 0; p < nPols; p++) {
      if(g_cgiMode)
	fprintf(stdout, "\nSignaturePolicy%s%s%s%s%s%s",  
		g_szOutputSeparator, pPolicies[p].szOID,
		g_szOutputSeparator, pPolicies[p].szCPS,
		g_szOutputSeparator, pPolicies[p].szUserNotice);
      else
	fprintf(stdout, "\n\tnSignaturePolicy oid: %s cps: %s desc: %s",  pPolicies[p].szOID,
		pPolicies[p].szCPS, pPolicies[p].szUserNotice);
    } // for p < nPols
  } 
  PolicyIdentifiers_free(pPolicies, nPols);
}



//--------------------------------------------------
// Verifys sigantures and notaries
//--------------------------------------------------
int cmdVerify(SignedDoc* pSigDoc)
{
  int err = ERR_OK, s, d, l, m, l1;
  SignatureInfo* pSigInfo = 0;
  NotaryInfo* pNot;
  DigiDocMemBuf mbuf;
  const DigiDocMemBuf* pMBuf = 0;
  char buf1[100];

  mbuf.pMem = 0;
  mbuf.nLen = 0;
  d = getCountOfSignatures(pSigDoc);
  for(s = 0; s < d; s++) {
    pSigInfo = getSignature(pSigDoc, s);
    err = ddocCertGetSubjectCN(ddocSigInfo_GetSignersCert(pSigInfo), &mbuf);
    err = verifySignatureAndNotary(pSigDoc, pSigInfo, p_szInFile);
    if(g_cgiMode) {
      fprintf(stdout, "\nSignature%s%s%s%s%s%d%s%s",
	      g_szOutputSeparator, 
	      pSigInfo->szId, 
	      g_szOutputSeparator,
	      (const char*)mbuf.pMem, 
	      g_szOutputSeparator,
	      err, 
	      g_szOutputSeparator,
	      getErrorString(err));
    } else {
      fprintf(stdout, "\nSignature: %s - %s - %s", pSigInfo->szId, 
	      (const char*)mbuf.pMem, ((!err) ? "OK" : "ERROR"));
    }
    ddocMemBuf_free(&mbuf);
    // print signers roles / manifests
    m = getCountOfSignerRoles(pSigInfo, 0);
    for(l = 0; l < m; l++) {
      if(g_cgiMode)
        fprintf(stdout, "\nClaimedRole%s%s", g_szOutputSeparator, getSignerRole(pSigInfo, 0, l));
      else
        fprintf(stdout, "\n\tClaimedRole: %s", getSignerRole(pSigInfo, 0, l));
    }
    if(pSigInfo->sigProdPlace.szCity || pSigInfo->sigProdPlace.szStateOrProvince ||
       pSigInfo->sigProdPlace.szPostalCode || pSigInfo->sigProdPlace.szCountryName)
      if(g_cgiMode)
        fprintf(stdout, "\nSignatureProductionPlace%s%s%s%s%s%s%s%s", g_szOutputSeparator,
		(pSigInfo->sigProdPlace.szCountryName ? pSigInfo->sigProdPlace.szCountryName : ""), 
		g_szOutputSeparator,
		(pSigInfo->sigProdPlace.szStateOrProvince ? pSigInfo->sigProdPlace.szStateOrProvince : ""), 
		g_szOutputSeparator,
		(pSigInfo->sigProdPlace.szCity ? pSigInfo->sigProdPlace.szCity : ""), 
		g_szOutputSeparator,
		(pSigInfo->sigProdPlace.szPostalCode ? pSigInfo->sigProdPlace.szPostalCode : ""));
      else
        fprintf(stdout, "\n\tnSignatureProductionPlace - Country: %s, State: %s, City: %s, Postal code: %s",
		(pSigInfo->sigProdPlace.szCountryName ? pSigInfo->sigProdPlace.szCountryName : ""),
		(pSigInfo->sigProdPlace.szStateOrProvince ? pSigInfo->sigProdPlace.szStateOrProvince : ""),
		(pSigInfo->sigProdPlace.szCity ? pSigInfo->sigProdPlace.szCity : ""),
		(pSigInfo->sigProdPlace.szPostalCode ? pSigInfo->sigProdPlace.szPostalCode : ""));
  //AM 24.09.08 RIK
	//}
  // signers certificate
  if(ddocSigInfo_GetSignersCert(pSigInfo))
    printCertificateInfo(pSigDoc, ddocSigInfo_GetSignersCert(pSigInfo));
  // confirmation
  if(pSigDoc && pSigInfo) {
    pNot = getNotaryWithSigId(pSigDoc, pSigInfo->szId);
    if(pNot) {
    pMBuf = ddocNotInfo_GetResponderId(pNot);
    buf1[0] = 0;
    l1 = sizeof(buf1);
    if(pNot->nRespIdType == RESPID_NAME_TYPE) {
      strncpy(buf1, (const char*)pMBuf->pMem, l1);
    }
    if(pNot->nRespIdType == RESPID_KEY_TYPE) {
      encode((const byte*)pMBuf->pMem, pMBuf->nLen, (byte*)buf1, &l1);
    }
    }
    if(pNot && pMBuf) {
      if(g_cgiMode)
	fprintf(stdout, "\nRevocationValues%s%s%s%s",  
		g_szOutputSeparator, 
		buf1,
		g_szOutputSeparator, 
		pNot->timeProduced);
      else
	fprintf(stdout, "\n\tRevocationValues responder: %s produced-at: %s",  
		buf1, pNot->timeProduced);
      // certificate
      printCertificateInfo(pSigDoc, ddocSigInfo_GetOCSPRespondersCert(pSigInfo));
    }
  }
	//AM 24.09.08 RIK
	}
  return err;
}


//--------------------------------------------------
// Signs the document and gets configrmation
//--------------------------------------------------
int cmdSign(SignedDoc* pSigDoc, const char* pin, const char* manifest,
			const char* city, const char* state, const char* zip, const char* country)
{
  int err = ERR_OK;
  SignatureInfo* pSigInfo = NULL;
	
  ddocDebug(3, "cmdSign", "Creating new digital signature");
  err = signDocument(pSigDoc, &pSigInfo, pin, manifest, city, state, zip, country);
  RETURN_IF_NOT(err == ERR_OK, err);
  return ERR_OK;
}




//--------------------------------------------------
// Runs some test. This is used simply to test
// new features of the library and the functionality
// may change.
//--------------------------------------------------
void cmdTest(const char* data)
{
#ifdef WITH_TS
  DigiDocMemBuf mbuf1, mbuf2;
  int err = ERR_OK, l1;
  TS_RESP *pResp = 0;
  FILE *hFile;

  mbuf1.pMem = 0;
  mbuf1.nLen = 0;
  mbuf2.pMem = 0;
  mbuf2.nLen = 0;
  ddocDebug(3, "cmdTest", "msg: %s", data);
  err = ddocMemAssignData(&mbuf1, data, -1);
  l1 =40;
  ddocMemSetLength(&mbuf2, l1);
  calculateDigest((const byte*)mbuf1.pMem, mbuf1.nLen, DIGEST_SHA1, (byte*)mbuf2.pMem, &l1);
  mbuf2.nLen = l1;
  if(!err) {
    //err = ddocTsCreateTsReqBin(&mbuf2, &mbuf1, 0, 1, 0);
    err = ddocSendTSRequest(&mbuf2, &pResp, 0, 1, 1, ConfigItem_lookup("TSA_URL"), 
			    ConfigItem_lookup("DIGIDOC_PROXY_HOST"), ConfigItem_lookup("DIGIDOC_PROXY_PORT"));
    ddocDebug(3, "cmdTest", "err1: %d", err);
    /*if((hFile = fopen("test1.tsq", "wb")) != NULL) {
      fwrite(mbuf2.pMem, 1, mbuf2.nLen, hFile);
      fclose(hFile);
      }*/
  }
  ddocMemBuf_free(&mbuf1);
  ddocMemBuf_free(&mbuf2);
#endif
}

//#ifdef WITH_DEPRECATED_FUNCTIONS
//--------------------------------------------------
// Runs some test. This is used simply to test
// new features of the library and the functionality
// may change.
//--------------------------------------------------
void cmdTest2(const char* infile)
{
  X509* pCert = 0;
  char buf1[X509_NAME_LEN + 10];
  int l1, err = ERR_OK, nPols = 0;
  PolicyIdentifier *pPolicies = 0;

  ddocDebug(3, "cmdTest2", "Reading cert: %s", infile);
  err = ReadCertificate(&pCert, infile);
  if(!err && pCert) {
    err = readCertPolicies(pCert, &pPolicies, &nPols);
	printf("Read pols rc: %d pols: %d\n", err, nPols);

	  /*
    l1 = sizeof(buf1);
    memset(buf1, 0, sizeof(buf1));
    err = getCertSubjectName(pCert, buf1, &l1);
    if(g_cgiMode)
      fprintf(stdout, "\nDN%s%s%s%d",  
	      g_szOutputSeparator, buf1,
	      g_szOutputSeparator, l1);
    else
      fprintf(stdout, "\nDN: %s len: %d", buf1, l1);
	  */
  }
  if(pCert)
    X509_free(pCert);
}
//#endif

//--------------------------------------------------
// Encrypts a file
//--------------------------------------------------
int cmdEncrypt(DEncEncryptedData** ppEncData, const char* szFileName) 
{
  FILE* hFile;
  SignedDoc *pSigDoc = 0;
  DEncEncryptionProperty* pEncProperty = 0;
  int len, err = ERR_OK;
  char buf[2048], *p;

  err = dencEncryptedData_new(ppEncData, DENC_XMLNS_XMLENC, DENC_ENC_METHOD_AES128, 0, 0, 0);
  if(err) return err;
  err = dencMetaInfo_SetLibVersion(*ppEncData);
  if(err) return err;
  err = dencMetaInfo_SetFormatVersion(*ppEncData);
  if(err) return err;
  p = 0;
  err = ddocConvertInput(szFileName, &p);
  if(err) return err;
  err = dencEncryptionProperty_new(*ppEncData, &pEncProperty, 0, 0, ENCPROP_FILENAME, getSimpleFileName(p));
  if(strstr(p, ".ddoc")) {
    err = dencEncryptionProperty_new(*ppEncData, &pEncProperty, 0, 0, ENCPROP_ORIG_MIME, DENC_ENCDATA_TYPE_DDOC);
  } 
  free(p);
  if(err) return err;
  if((hFile = fopen(szFileName, "rb")) != NULL) {
    do {
      memset(buf,0,sizeof(buf));
      len = fread(buf, 1, sizeof(buf), hFile);
      if(len) {
	err = dencEncryptedData_AppendData(*ppEncData, buf, len);
      }
    } while(len && !err);
    fclose(hFile);
  }
  // check for digidoc
  if(!err && strstr(szFileName, ".ddoc")) {
    err = ddocSaxReadSignedDocFromFile(&pSigDoc, szFileName, 0, 0);
    if(!err)
      err = dencOrigContent_registerDigiDoc(*ppEncData, pSigDoc);
    if(pSigDoc)
      SignedDoc_free(pSigDoc);
  }
  if(!err)
    //err = dencEncryptedData_encryptData(*ppEncData, DENC_COMPRESS_NEVER);
    err = dencEncryptedData_encryptData(*ppEncData, DENC_COMPRESS_BEST_EFFORT);
  return err;
}




//--------------------------------------------------
// Runs various tests. Just for trying out new features
//--------------------------------------------------
int runTestCmds(int argc, char** argv)
{
  int err = ERR_OK, i;
  char* infile = NULL;
	
  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      if(!strcmp(argv[i], "-test")) {
	// add a recipients key
	cmdTest(argv[i+1]);	
      }
//#ifdef WITH_DEPRECATED_FUNCTIONS
      if(!strcmp(argv[i], "-test2")) {
	ddocConvertInput((const char*)argv[i+1], &infile);
	cmdTest2(argv[i+1]);	
      }
//#endif
    }
  }
  return err;
}

//--------------------------------------------------
// Encrypts whole files
//--------------------------------------------------
int runEncryptFileCmds(int argc, char** argv, DEncEncryptedData* pEncData)
{
  int err = ERR_OK, i;

  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      // add a recipients key
      if(!strcmp(argv[i], "-encrypt-file")) {
	if(argc > i+2 && argv[i+1][0] != '-' && argv[i+2][0] != '-') {
	  char* infile = NULL;
	  char* outfile = NULL;
	  char* mime = NULL;
	  ddocConvertInput((const char*)argv[i+1], &infile);
	  ddocConvertInput((const char*)argv[i+2], &outfile);
	  i += 2;
	  // optional arguments
	  checkArguments(argc, argv, &i, &mime);
	  // encrypt the file
	  err = dencEncryptFile(pEncData, infile, outfile, mime);
	  if(err) return err;
	} else {
	  err = ERR_BAD_PARAM;
	  addError(err, __FILE__, __LINE__, "Missing <in-file> or <out-file> argument of -encrypt-file command");
	}
      }
    }
  }
  return err;
}


//--------------------------------------------------
// Decrypts whole files
//--------------------------------------------------
int runDecryptFileCmds(int argc, char** argv)
{
  int err = ERR_OK, i;

  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      // decrypt a file
      if(!strcmp(argv[i], "-decrypt-file")) {
	if(argc > i+2 && argv[i+1][0] != '-' && 
	   argv[i+2][0] != '-' && argv[i+3][0] != '-') {
	  char* infile = NULL;
	  char* outfile = NULL;
	  char* pin = NULL;
	  ddocConvertInput((const char*)argv[i+1], &infile);
	  ddocConvertInput((const char*)argv[i+2], &outfile);
	  ddocConvertInput((const char*)argv[i+3], &pin);
	  i += 3;
	  // decrypt the file
	  err = dencSaxReadDecryptFile(infile, outfile, pin);
	  if(err) return err;
	} else {
	  err = ERR_BAD_PARAM;
	  addError(err, __FILE__, __LINE__, "Missing <in-file>, <out-file> or <pin> argument of -decrypt-file command");
	}
      }
    }
  }
  return err;
}

//--------------------------------------------------
// Lists SignedDoc objects
//--------------------------------------------------
int cmdListSignedDoc(SignedDoc* pSigDoc)
{
  int err = ERR_OK, i, n;
  DataFile *pDf = 0;

  // print signed doc format and version
  if(g_cgiMode) {
    fprintf(stdout, "\nSignedDoc%s%s%s%s",
	    g_szOutputSeparator,
	    pSigDoc->szFormat, g_szOutputSeparator,
	    pSigDoc->szFormatVer);
  } else {
    fprintf(stdout, "\nSignedDoc format: %s version: %s\n", 
	    pSigDoc->szFormat, pSigDoc->szFormatVer);
  }
  n = getCountOfDataFiles(pSigDoc);
  for(i = 0; i < n; i++) {
    pDf = getDataFile(pSigDoc, i);
    if(g_cgiMode)
      fprintf(stdout, "\nDataFile%s%s%s%s%s%ld%s%s%s%s",
	      g_szOutputSeparator, pDf->szId, g_szOutputSeparator,
	      pDf->szFileName, g_szOutputSeparator,
	      pDf->nSize, g_szOutputSeparator,
	      pDf->szMimeType, g_szOutputSeparator,
	      pDf->szContentType);
    else
      fprintf(stdout, "\nDataFile: %s, file: %s, size: %ld, mime-type: %s content-type: %s",
	      pDf->szId, pDf->szFileName, pDf->nSize, pDf->szMimeType, pDf->szContentType);
  }
  // print signatures 
  err = cmdVerify(pSigDoc);
  return err;
}

//--------------------------------------------------
// Lists EncryptedData objects
//--------------------------------------------------
int cmdListEncryptedData(DEncEncryptedData* pEncData)
{
  int err = ERR_OK, k;
  char buf1[50], buf2[50];
  
  if(g_cgiMode) {
    fprintf(stdout, "\nEncryptedData%s%s%s%s%s%s%s%s",
	    g_szOutputSeparator, (pEncData->szId ? pEncData->szId : ""),
	    g_szOutputSeparator, (pEncData->szType ? pEncData->szType : ""),
	    g_szOutputSeparator, (pEncData->szMimeType ? pEncData->szMimeType : ""),
	    g_szOutputSeparator, (pEncData->szEncryptionMethod ? pEncData->szEncryptionMethod : ""));
    err = dencMetaInfo_GetLibVersion(pEncData, buf1, buf2);
    fprintf(stdout, "\nLIBRARY%s%s%s%s", g_szOutputSeparator, buf1, g_szOutputSeparator, buf2);
    err = dencMetaInfo_GetFormatVersion(pEncData, buf1, buf2);
    fprintf(stdout, "\nFORMAT%s%s%s%s", g_szOutputSeparator, buf1, g_szOutputSeparator, buf2);
  } else {
    fprintf(stdout, "\nEncryptedData - Id=%s Type=%s MimeType=%s EncryptionMethod=%s",
	    (pEncData->szId ? pEncData->szId : ""),
	    (pEncData->szType ? pEncData->szType : ""),
	    (pEncData->szMimeType ? pEncData->szMimeType : ""),
	    (pEncData->szEncryptionMethod ? pEncData->szEncryptionMethod : ""));
    err = dencMetaInfo_GetLibVersion(pEncData, buf1, buf2);
    fprintf(stdout, "\n\tLIBRARY: %s VERSION: %s", buf1, buf2);
    err = dencMetaInfo_GetFormatVersion(pEncData, buf1, buf2);
    fprintf(stdout, "\n\tFORMAT: %s VERSION: %s", buf1, buf2);
  }
  for(k = 0; k < pEncData->nEncryptedKeys; k++) {
    DEncEncryptedKey *pEncKey = pEncData->arrEncryptedKeys[k];
    if(g_cgiMode)
      fprintf(stdout, "\nEncryptedKey%s%s%s%s%s%s%s%s%s%s%s%s",
         g_szOutputSeparator, (pEncKey->szId ? pEncKey->szId : ""),
         g_szOutputSeparator, (pEncKey->szRecipient ? pEncKey->szRecipient : ""),
         g_szOutputSeparator, (pEncKey->szKeyName ? pEncKey->szKeyName : ""),
         g_szOutputSeparator, (pEncKey->szCarriedKeyName ? pEncKey->szCarriedKeyName : ""),
         g_szOutputSeparator, (pEncKey->szEncryptionMethod ? pEncKey->szEncryptionMethod : ""),
	     g_szOutputSeparator, (pEncKey->pCert ? "OK" : "NULL"));
    else
      fprintf(stdout, "\nEncryptedKey: Id=%s Recipient=%s KeyName=%s CarriedKeyName=%s EncryptionMethod=%s Cert: %s",
         (pEncKey->szId ? pEncKey->szId : ""),
         (pEncKey->szRecipient ? pEncKey->szRecipient : ""),
         (pEncKey->szKeyName ? pEncKey->szKeyName : ""),
         (pEncKey->szCarriedKeyName ? pEncKey->szCarriedKeyName : ""),
         (pEncKey->szEncryptionMethod ? pEncKey->szEncryptionMethod : ""),
	     (pEncKey->pCert ? "OK" : "NULL"));
  }
  if(pEncData->encProperties.nEncryptionProperties > 0) {
    if(g_cgiMode)
      fprintf(stdout, "\nEncryptionProperties%s%s",
	   g_szOutputSeparator, (pEncData->encProperties.szId ? pEncData->encProperties.szId : ""));
    else
      fprintf(stdout, "\nEncryptionProperties: %s",
	   (pEncData->encProperties.szId ? pEncData->encProperties.szId : ""));
    for(k = 0; k < pEncData->encProperties.nEncryptionProperties; k++) {
      DEncEncryptionProperty *pEncProp = pEncData->encProperties.arrEncryptionProperties[k];
      if(g_cgiMode)
        fprintf(stdout, "\nEncryptionProperty%s%s%s%s%s%s%s%s\n",
          g_szOutputSeparator, (pEncProp->szId ? pEncProp->szId : ""),
          g_szOutputSeparator, (pEncProp->szTarget ? pEncProp->szTarget : ""),
          g_szOutputSeparator, (pEncProp->szName ? pEncProp->szName : ""),
          g_szOutputSeparator, (pEncProp->szContent ? pEncProp->szContent : ""));
      else
        fprintf(stdout, "\nEncryptionProperty Id=%s Target=%s  Name=%s Content=%s\n",
          (pEncProp->szId ? pEncProp->szId : ""),
          (pEncProp->szTarget ? pEncProp->szTarget : ""),
          (pEncProp->szName ? pEncProp->szName : ""),
          (pEncProp->szContent ? pEncProp->szContent : ""));
    }
  }
  return err;
}

//--------------------------------------------------
// Add ecryption recipients
//--------------------------------------------------
int runDEncListCmds(int argc, char** argv)
{
  int err = ERR_OK, i;
  DEncEncryptedData* pEncData = 0;

  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      // add a recipients key
      if(!strcmp(argv[i], "-denc-list")) {
	if(argc > i+1 && argv[i+1][0] != '-') {
	  char* file = argv[i+1];
	  err = dencSaxReadEncryptedData(&pEncData, file);
	  if(!err)
	    err = cmdListEncryptedData(pEncData);
	  if(pEncData && !err)
	    dencEncryptedData_free(pEncData);
	} else {
	  err = ERR_BAD_PARAM;
	  addError(err, __FILE__, __LINE__, "Missing <file> argument of -denc-list command");
	}
      }
    }
  }
  return err;
}

//--------------------------------------------------
// decryption file 
//--------------------------------------------------
int runDecryptCmds(int argc, char** argv, DEncEncryptedData** ppEncData)
{
  int err = ERR_OK, i;
  DEncEncryptedKey* pEncKey = 0;

  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      // decrypt file
      if(!strcmp(argv[i], "-decrypt")) {
	if(argc > i+3 && 
	   argv[i+1][0] != '-' && 
	   argv[i+2][0] != '-') {
	  char* infile = argv[i+1];
	  char* pin = argv[i+2];
	  i += 2;
	  err = dencSaxReadEncryptedData(ppEncData, infile);
	  if(err) return err;
	  err = dencEncryptedData_findEncryptedKeyByPKCS11(*ppEncData, &pEncKey);
	  if(pEncKey) {
	    err = dencEncryptedData_decrypt(*ppEncData, pEncKey, pin);
	    if(err) return err;
	  } else {
	    err = ERR_DENC_DECRYPT;
	    addError(err, __FILE__, __LINE__, "No transport key found for your smartcard");
	  }
	  // p_szOutFile
	} else {
	  err = ERR_BAD_PARAM;
	  addError(err, __FILE__, __LINE__, "Missing <file> or <pin> argument of -decrypt command");
	}
      }
    }
  }
  return err;
}

//--------------------------------------------------
// Create digidoc and adds datafiles
//--------------------------------------------------
int runExtractCmds(int argc, char** argv, SignedDoc** ppSigDoc)
{
  int err = ERR_OK, i;

  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      // add a DataFile
      if(!strcmp(argv[i], "-extract")) {
	if(argc > i+2 && argv[i+1][0] != '-' && argv[i+2][0] != '-') {
	  char* id = NULL;
	  char* file = NULL;
	  char* charset = CHARSET_UTF_8;
	  checkArguments(argc, argv, &i, &id);
	  checkArguments(argc, argv, &i, &file);
	  // optional charset & filenamecharset
	  //checkArguments(argc, argv, &i, &charset);
	  err = ddocExtractDataFile(*ppSigDoc, (const char*)p_szInFile, (const char*)file,
	    (const char*)id, (const char*)charset);
	  // report success
	  if(g_cgiMode)
		fprintf(stdout, "\nDataFile%s%s%s%s%s%d",  g_szOutputSeparator, id, 
			g_szOutputSeparator, file, g_szOutputSeparator, err);
	  else
		fprintf(stdout, "\nDataFile id: %s path: %s rc: %d",  id, file, err);

	  //if(id) free(id);
	  //if(file) free(file);
	  //if(charset) free(charset);

	} else {
	  err = ERR_BAD_PARAM;
	  addError(err, __FILE__, __LINE__, "Missing <file> argument of -extract command");
	}
      }
    }
  }
  return err;
}

//--------------------------------------------------
// Verfys sigantures and notaries
//--------------------------------------------------
int runVerifyCmds(int argc, char** argv, SignedDoc** ppSigDoc)
{
  int err = ERR_OK, i;
  
  for(i = 1; !err && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      // create new digidoc
      if(!strcmp(argv[i], "-verify")) {
	err = cmdVerify(*ppSigDoc);
      }
    }
  }
  return err;	
}


//--------------------------------------------------
// Handles signature commands
//--------------------------------------------------
int runSignCmds(int argc, char** argv, SignedDoc** ppSigDoc)
{
  int err = ERR_OK, i, nManif;

  for(i = 1; !err && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      // create new digidoc
      if(!strcmp(argv[i], "-sign")) {
	  char* pin = NULL;
	  char* manifest = NULL;
	  char* city = NULL;
	  char* state = NULL;
	  char* zip = NULL;
	  char* country = NULL;
	  checkArguments(argc, argv, &i, &pin);
	  // if pin is NULL check for autosign pin
	  if(!pin)
	    pin = (char*)ConfigItem_lookup("AUTOSIGN_PIN");
	  if(!pin) {
	    err = ERR_BAD_PARAM;
	    addError(err, __FILE__, __LINE__, "Missing <pin> argument of -sign command");
	    return err;
	  }
	  // check config file
	  nManif = ConfigItem_lookup_int("MANIFEST_MODE", 0);
	  if(nManif == 2) {
	    manifest = (char*)ConfigItem_lookup("DIGIDOC_ROLE_MANIFEST");
	    country = (char*)ConfigItem_lookup("DIGIDOC_ADR_COUNTRY");
	    state = (char*)ConfigItem_lookup("DIGIDOC_ADR_STATE");
	    city = (char*)ConfigItem_lookup("DIGIDOC_ADR_CITY");
	    zip = (char*)ConfigItem_lookup("DIGIDOC_ADR_ZIP");
	  }
	  // optional mainfest argument
	  checkArguments(argc, argv, &i, &manifest);
	  // optional address
	  checkArguments(argc, argv, &i, &city);
	  checkArguments(argc, argv, &i, &state);
	  checkArguments(argc, argv, &i, &zip);
	  checkArguments(argc, argv, &i, &country);	  
	  err = cmdSign(*ppSigDoc, (const char*)pin, (const char*)manifest,
			(const char*)city, (const char*)state, (const char*)zip, 
			(const char*)country);
	  if(manifest) free(manifest);
	  if(city) free(city);
	  if(state) free(state);
	  if(zip) free(zip);
	  if(country) free(country);
      }
    }
  }
  return err;
}

//--------------------------------------------------
// Handles list commands
//--------------------------------------------------
int runListCmds(int argc, char** argv, SignedDoc* pSigDoc, DEncEncryptedData* pEncData)
{
  int err = ERR_OK, i, nFound = 0;

  for(i = 1; !err && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      // create new digidoc
      if(!strcmp(argv[i], "-list")) {
	nFound = 1;
      }
    }
  }
  if(nFound) {
    if(pSigDoc)
      err = cmdListSignedDoc(pSigDoc);
    if(pEncData)
      err = cmdListEncryptedData(pEncData);
  }
  return err;
}

//--------------------------------------------------
// Handles commands related to checking certificates
//--------------------------------------------------
int runCheckCertCmds(int argc, char** argv)
{
  int err = ERR_OK, i;
  char* szCertFile = 0;
  X509* pCert = 0;
  DigiDocMemBuf mbuf;

  mbuf.pMem = 0;
  mbuf.nLen = 0;
  for(i = 1; !err && (i < argc); i++) {
    if(!strcmp(argv[i], "-check-cert") &&
       argv[i+1][0] != '-') {
      szCertFile = argv[i+1];
      i++;
      if(szCertFile) {
	err = ReadCertificate(&pCert, szCertFile);
	if(!err && pCert) {
	  err = ddocVerifyCertByOCSP(pCert, NULL);
	  err = ddocCertGetSubjectCN(pCert, &mbuf);
	  printf("Verifying cert: %s --> RC :%d\n", (const char*)mbuf.pMem, err);
	  ddocMemBuf_free(&mbuf);
	}
	if(pCert) {
	  X509_free(pCert);
	  pCert = 0;
	}
      }
    }
  }
  return err;
}

//--------------------------------------------------
// Handles commands related to adding signature value
// from a file containging base64 encoded RSA-SHA1 signature value
//--------------------------------------------------
int runAddSignValueCmds(int argc, char** argv, SignedDoc* pSigDoc)
{
  int err = ERR_OK, i;
  char *szSignFile = 0, *szSignId = 0;
  SignatureInfo* pSignInfo = 0;

  for(i = 1; !err && (i < argc); i++) {
    if(!strcmp(argv[i], "-add-sign-value") && i < argc - 2 &&
       argv[i+1][0] != '-' && argv[i+2][0] != '-') {
      checkArguments(argc, argv, &i, &szSignFile);
      checkArguments(argc, argv, &i, &szSignId);
      if(!pSigDoc) {
	err = ERR_BAD_PARAM;
	addError(err, __FILE__, __LINE__, "No digidoc document read in yet!");
	return err;
      }
      if(szSignFile && szSignId) {
	pSignInfo = getSignatureWithId(pSigDoc, szSignId);
	if(pSignInfo) {
	  // read the file
	  err = setSignatureValueFromFile(pSignInfo, szSignFile);
	} else {
	  err = ERR_BAD_PARAM;
	  addError(err, __FILE__, __LINE__, "Wrong signature id");
	}
	// report on success
	if(g_cgiMode)
	  fprintf(stdout, "\nAddSignValue%s%s%s%s%s%d",  g_szOutputSeparator, szSignId, 
		  g_szOutputSeparator, szSignFile, g_szOutputSeparator, err);
	else
	  fprintf(stdout, "\nAddSignValue id: %s path: %s rc: %d",  szSignId, szSignFile, err);
	free(szSignFile);
	free(szSignId);
      } else {
	err = ERR_BAD_PARAM;
	addError(err, __FILE__, __LINE__, "Missing <sign-value-file> or <sign-id> argument of -add-sign-value command");
      }
    }
  }
  return err;
}


//--------------------------------------------------
// Handles commands related to deleting signatures
//--------------------------------------------------
int runDelSignCmds(int argc, char** argv, SignedDoc* pSigDoc)
{
  int err = ERR_OK, i;
  char *szSignId = 0;
  SignatureInfo* pSignInfo = 0;

  for(i = 1; !err && (i < argc); i++) {
    if(!strcmp(argv[i], "-del-sign") && i < argc - 1 &&
       argv[i+1][0] != '-') {
      checkArguments(argc, argv, &i, &szSignId);
      if(!pSigDoc) {
	err = ERR_BAD_PARAM;
	addError(err, __FILE__, __LINE__, "No digidoc document read in yet!");
	return err;
      }
      if(szSignId) {
	pSignInfo = getSignatureWithId(pSigDoc, szSignId);
	if(pSignInfo) {
	  err = SignatureInfo_delete(pSigDoc, szSignId);
	  // report on success
	  if(g_cgiMode)
	    fprintf(stdout, "\nDelSign%s%s%s%d",  g_szOutputSeparator, szSignId, 
		    g_szOutputSeparator, err);
	  else
	    fprintf(stdout, "\nDelSign id: %s rc: %d",  szSignId, err);

	} else {
	  err = ERR_BAD_PARAM;
	  addError(err, __FILE__, __LINE__, "wrong signature id!");
	}
	free(szSignId);
      } else {
	err = ERR_BAD_PARAM;
	addError(err, __FILE__, __LINE__, "Missing <sign-id> argument of -del-sign command");
      }
    }
  }
  return err;
}


//--------------------------------------------------
// Handles commands related to notarizing signatures
//--------------------------------------------------
int runGetConfirmationCmds(int argc, char** argv, SignedDoc* pSigDoc)
{
  int err = ERR_OK, i;
  char *szSignId = 0;
  SignatureInfo* pSignInfo = 0;

  for(i = 1; !err && (i < argc); i++) {
    if(!strcmp(argv[i], "-get-confirmation") && i < argc - 1 &&
       argv[i+1][0] != '-') {
      checkArguments(argc, argv, &i, &szSignId);
      if(!pSigDoc) {
	err = ERR_BAD_PARAM;
	addError(err, __FILE__, __LINE__, "No digidoc document read in yet!");
	return err;
      }
      if(szSignId) {
	pSignInfo = getSignatureWithId(pSigDoc, szSignId);
	if(pSignInfo) {
	  err = notarizeSignature(pSigDoc, pSignInfo);
	  // report on success
	  if(g_cgiMode)
	    fprintf(stdout, "\nNotarizeSignature%s%s%s%d",  g_szOutputSeparator, szSignId, 
		    g_szOutputSeparator, err);
	  else
	    fprintf(stdout, "\nNotarizeSignature id: %s rc: %d",  szSignId, err);

	} else {
	  err = ERR_BAD_PARAM;
	  addError(err, __FILE__, __LINE__, "wrong signature id!");
	}
	free(szSignId);
      } else {
	err = ERR_BAD_PARAM;
	addError(err, __FILE__, __LINE__, "Missing <sign-id> argument of -get-confirmation command");
      }
    }
  }
  return err;
}


//--------------------------------------------------
// Signs the document and gets configrmation
//--------------------------------------------------
int cmdCalcSign(SignedDoc* pSigDoc, const char* manifest,
		const char* city, const char* state, const char* zip, 
		const char* country, const char* certFile)
{
  int err = ERR_OK, l1;
  SignatureInfo* pSigInfo = NULL;
  X509* pCert = NULL;
  char buf1[50];

  ddocDebug(3, "cmdCalcSign", "Creating new digital signature");
  RETURN_IF_NULL_PARAM(pSigDoc);
  RETURN_IF_NULL_PARAM(certFile);
  // read certificate
  // this certificate will nto be released since new signature takes ownership!
  err = ReadCertificate(&pCert, certFile);
  RETURN_IF_NOT(err == ERR_OK, err);
  // prepare signature
  err = ddocPrepareSignature(pSigDoc, &pSigInfo,
	   manifest, city, state, zip, country, pCert, NULL);
  // base64 encode final hash
  l1 = sizeof(buf1);
  err = ddocGetSignedHash(pSigInfo, buf1, &l1, 2, 0);
  RETURN_IF_NOT(err == ERR_OK, err);
  // report on success
  if(g_cgiMode)
    fprintf(stdout, "\nSignatureHash%s%s%s%s%s%d",  g_szOutputSeparator, pSigInfo->szId, 
	    g_szOutputSeparator, buf1, g_szOutputSeparator, err);
  else
    fprintf(stdout, "\nSignatureHash id: %s hash: %s rc: %d", pSigInfo->szId, buf1, err);


  RETURN_IF_NOT(err == ERR_OK, err);
  return ERR_OK;
}

//--------------------------------------------------
// Handles signature commands
//--------------------------------------------------
int runCalcSignCmds(int argc, char** argv, SignedDoc* pSigDoc)
{
  int err = ERR_OK, i, nManif;

  for(i = 1; !err && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      // create new digidoc
      if(!strcmp(argv[i], "-calc-sign")) {
	  char* certFile = NULL;
	  char* manifest = NULL;
	  char* city = NULL;
	  char* state = NULL;
	  char* zip = NULL;
	  char* country = NULL;
	  checkArguments(argc, argv, &i, &certFile);
	  // check config file
	  nManif = ConfigItem_lookup_int("MANIFEST_MODE", 0);
	  if(nManif == 2) {
	    manifest = (char*)ConfigItem_lookup("DIGIDOC_ROLE_MANIFEST");
	    country = (char*)ConfigItem_lookup("DIGIDOC_ADR_COUNTRY");
	    state = (char*)ConfigItem_lookup("DIGIDOC_ADR_STATE");
	    city = (char*)ConfigItem_lookup("DIGIDOC_ADR_CITY");
	    zip = (char*)ConfigItem_lookup("DIGIDOC_ADR_ZIP");
	  }
	  // optional mainfest argument
	  checkArguments(argc, argv, &i, &manifest);
	  // optional address
	  checkArguments(argc, argv, &i, &city);
	  checkArguments(argc, argv, &i, &state);
	  checkArguments(argc, argv, &i, &zip);
	  checkArguments(argc, argv, &i, &country);
	  if(!certFile) {
	    err = ERR_BAD_PARAM;
	    addError(err, __FILE__, __LINE__, "Missing <cert-file> argument of -calc-sign command");
	  }
	  else
	    err = cmdCalcSign(pSigDoc, (const char*)manifest,
			(const char*)city, (const char*)state, (const char*)zip, 
			    (const char*)country, (const char*)certFile);
	  if(manifest) free(manifest);
	  if(city) free(city);
	  if(state) free(state);
	  if(zip) free(zip);
	  if(country) free(country);
	  if(certFile) free(certFile);
      }
    }
  }
  return err;
}

// MSSP functions

#ifdef WITH_MSSP

char* getMsspStatus()
{
	switch(g_pMssp->nStatusCode) {
	case INTERNAL_ERROR: return "INTERNAL_ERROR";
	case USER_CANCEL: return "USER_CANCEL";
	case EXPIRED_TRANSACTION: return "EXPIRED_TRANSACTION";
	case SIGNATURE: return "SIGNATURE";
	case OUTSTANDING_TRANSACTION: return "OUTSTANDING_TRANSACTION";
	}
	return "Unknown MSSP status";
}

//--------------------------------------------------
// Signs the document and gets configrmation
//--------------------------------------------------
int cmdMsspStatus(SignedDoc* pSigDoc, unsigned long lTxnNo)
{
  int err = ERR_OK, nStatus = 0;

  // connect if necessary
  err = ddocMsspConnect(&g_pMssp);
  RETURN_IF_NOT(err == ERR_OK, err);
  ddocDebug(3, "cmdMsspStatus", "Get MSSP status for: %ld",
	    (lTxnNo ? lTxnNo : g_pMssp->lMsspTxnId));

  nStatus = ddocConfMsspGetStatus(g_pMssp, pSigDoc, NULL);
  if(nStatus == INTERNAL_ERROR || nStatus == USER_CANCEL ||
     nStatus == EXPIRED_TRANSACTION || nStatus == SIGNATURE ||
     nStatus == OUTSTANDING_TRANSACTION) {
    err = ERR_OK;
    g_pMssp->nStatusCode = nStatus;
  } else
    err = nStatus;
  ddocDebug(3, "cmdMsspStatus", "Txn1: %ld status: %d err: %d", g_pMssp->lMsspTxnId, g_pMssp->nStatusCode, err);
  // report on success
  if(g_cgiMode)
    fprintf(stdout, "\nMSSPStatusReq%s%ld%s%d%s%d%s%s%s%s%s%s",  
	    g_szOutputSeparator, (lTxnNo ? lTxnNo : g_pMssp->lMsspTxnId),
	    g_szOutputSeparator, err, 
		g_szOutputSeparator, g_pMssp->nStatusCode, g_szOutputSeparator, getMsspStatus(),
	    g_szOutputSeparator, (err ? ddocGetSoapFaultString(g_pMssp) : ""),
	    g_szOutputSeparator, (err ? ddocGetSoapFaultDetail(g_pMssp) : ""));
  else
    fprintf(stdout, "\nMSSPStatusReq txn: %ld rc: %d soap: %d - %s fault-string: %s fault-detail: %s", 
	    (lTxnNo ? lTxnNo : g_pMssp->lMsspTxnId), err,
		g_pMssp->nStatusCode, getMsspStatus(), 
	    (err ? ddocGetSoapFaultString(g_pMssp) : ""),
	    (err ? ddocGetSoapFaultDetail(g_pMssp) : ""));
  ddocDebug(3, "cmdMsspStatus", "Txn2: %ld status: %d err: %d", g_pMssp->lMsspTxnId, g_pMssp->nStatusCode, err);
  return err;
}


//--------------------------------------------------
// Handles MSSP status commands
//--------------------------------------------------
int runMsspStatusCmds(int argc, char** argv, SignedDoc* pSigDoc)
{
  int err = ERR_OK, i;
  unsigned long lTxnNo = 0;

  for(i = 1; !err && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      // create new digidoc
      if(!strcmp(argv[i], "-mssp-status")) {
	if(i < argc - 1 && isdigit(argv[i+1][0]))
	  lTxnNo = atol(argv[i+1]);
	err = cmdMsspStatus(pSigDoc, lTxnNo);
      }
    }
  }
  return err;
}


//--------------------------------------------------
// Signs the document and gets configrmation
//--------------------------------------------------
int cmdMsspSign(SignedDoc* pSigDoc, const char* szPhoneNo,
		const char* manifest, const char* city, 
		const char* state, const char* zip, 
		const char* country)
{
  int err = ERR_OK, nPollFreq = 0;
#ifdef WIN32
  time_t tNew, tOld;
#endif

  nPollFreq = ConfigItem_lookup_int("DIGIDOC_MSSP_STATUS_POLL_FREQ", 0);
  // connect if necessary
  err = ddocMsspConnect(&g_pMssp);
  RETURN_IF_NOT(err == ERR_OK, err);
  ddocDebug(3, "cmdMsspSign", "Creating WPKI signature using: %s poll freq: %d", szPhoneNo, nPollFreq);
  // send signature req
  err = ddocConfMsspSign(pSigDoc, g_pMssp, szPhoneNo,
			 manifest, city, state, zip, country,
			 p_szOutFile, 0, 0);
  // report on success
  if(g_cgiMode)
    fprintf(stdout, "\nMSSPSinatureReq%s%ld%s%d%s%s%s%s",  
	    g_szOutputSeparator, g_pMssp->lMsspTxnId, 
	    g_szOutputSeparator, err,
	    g_szOutputSeparator, (err ? ddocGetSoapFaultString(g_pMssp) : ""),
	    g_szOutputSeparator, (err ? ddocGetSoapFaultDetail(g_pMssp) : ""));
  else
    fprintf(stdout, "\nMSSPSignatureReq MSSPTransId: %ld rc: %d fault-string: %s fault-detail: %s", 
	    g_pMssp->lMsspTxnId, err,
	    (err ? ddocGetSoapFaultString(g_pMssp) : ""),
	    (err ? ddocGetSoapFaultDetail(g_pMssp) : ""));
  // if we should keep on polling
  if(!err && nPollFreq) {
    // now check status
    do {
#ifdef WIN32
	time(&tOld);
	do {
		time(&tNew);
	} while(tNew - tOld < nPollFreq);
#else
      sleep(nPollFreq);
#endif
      err = cmdMsspStatus(pSigDoc, g_pMssp->lMsspTxnId);
      ddocDebug(3, "cmdMsspSign", "Txn: %ld status: %d err: %d", g_pMssp->lMsspTxnId, g_pMssp->nStatusCode, err);
    } while(!err && g_pMssp->nStatusCode == OUTSTANDING_TRANSACTION);
  }

  RETURN_IF_NOT(err == ERR_OK, err);
  return ERR_OK;
}


//--------------------------------------------------
// Handles MSSP signature commands
//--------------------------------------------------
int runMsspSignCmds(int argc, char** argv, SignedDoc* pSigDoc)
{
  int err = ERR_OK, i, nManif = 0;

  for(i = 1; !err && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      // create new digidoc
      if(!strcmp(argv[i], "-mssp-sign")) {
	  char* phoneNo = NULL;
	  char* manifest = NULL;
	  char* city = NULL;
	  char* state = NULL;
	  char* zip = NULL;
	  char* country = NULL;
	  checkArguments(argc, argv, &i, &phoneNo);
	  // check config file
	  nManif = ConfigItem_lookup_int("MANIFEST_MODE", 0);
	  if(nManif == 2) {
	    manifest = (char*)ConfigItem_lookup("DIGIDOC_ROLE_MANIFEST");
	    country = (char*)ConfigItem_lookup("DIGIDOC_ADR_COUNTRY");
	    state = (char*)ConfigItem_lookup("DIGIDOC_ADR_STATE");
	    city = (char*)ConfigItem_lookup("DIGIDOC_ADR_CITY");
	    zip = (char*)ConfigItem_lookup("DIGIDOC_ADR_ZIP");
	  }
	  // optional mainfest argument
	  checkArguments(argc, argv, &i, &manifest);
	  // optional address
	  checkArguments(argc, argv, &i, &city);
	  checkArguments(argc, argv, &i, &state);
	  checkArguments(argc, argv, &i, &zip);
	  checkArguments(argc, argv, &i, &country);
	  if(!phoneNo) {
	    err = ERR_BAD_PARAM;
	    addError(err, __FILE__, __LINE__, "Missing <phone-no> argument of -mssp-sign command");
	  }
	  else
	    err = cmdMsspSign(pSigDoc, (const char*)phoneNo, (const char*)manifest,
			      (const char*)city, (const char*)state, (const char*)zip, 
			      (const char*)country);
	  if(manifest) free(manifest);
	  if(city) free(city);
	  if(state) free(state);
	  if(zip) free(zip);
	  if(country) free(country);
	  if(phoneNo) free(phoneNo);
      }
    }
  }
  return err;
}

#endif // WITH_MSSP




//--------------------------------------------------
// Program main function
//--------------------------------------------------
int main(int argc, char** argv)
{
  int err = ERR_OK;
  SignedDoc* pSigDoc = 0;
  DEncEncryptedData* pEncData = 0;
  time_t t1, t2;
  char buf1[250];

  if(argc <= 1) {
    printUsage();
    exit(0);
  }
  // init DigiDoc library
  initDigiDocLib();
#ifdef WIN32
  // find out program home if invoked with a long command line
  memset(buf1, 0, sizeof(buf1));
  getFileNamePath(argv[0], buf1, sizeof(buf1));
  if(strlen(buf1)) {
    strncat(buf1, "digidoc.ini", sizeof(buf1));
    err = initConfigStore(buf1);
  } else
    err = initConfigStore(NULL);
#else
  // read in config file
  err = initConfigStore(NULL);
#endif
  // clear all errors that were encountered by not finding config files
  err = checkProgArguments(argc, argv);
  time(&t1);
  // register program name and version
  setGUIVersion(g_szProgNameVer);
  ddocDebugTruncateLog();
  // use command line argument if required
  if(p_szConfigFile)
    err = readConfigFile(p_szConfigFile, ITEM_TYPE_PRIVATE);
  // read flags from config file
  readConfigParams();
  

  // display programm name and version
  if(g_cgiMode) {
    if(ConfigItem_lookup_bool("DIGIDOC_CGI_PRINT_HEADER", 1))
      fprintf(stdout, "%s%s%s", getLibName(), g_szOutputSeparator, getLibVersion());
  } else {
    fprintf(stdout, "%s - %s", getLibName(), getLibVersion());
  }

  // execute the commands now 
  // check certificate status throught OCSP
  if(!err)
    err = runCheckCertCmds(argc, argv);
  // read input file if necessary
  if(p_szInFile && !err)
    err = cmdReadDigiDoc(&pSigDoc, &pEncData, p_parseMode);
  // various tests
  if(!err)
    err = runTestCmds(argc, argv);
  if(p_szInEncFile && !err)
    err = cmdEncrypt(&pEncData, p_szInEncFile);
  // list encrypted files
  if(!err)
    err = runDEncListCmds(argc, argv);
  // add data files
  if(!err)
    err = runAddCmds(argc, argv, &pSigDoc);
  // add recipients
  if(!err)
    err = runRecipientCmds(argc, argv, &pEncData);
  // encrypt whole files
  if(!err)
    err = runEncryptFileCmds(argc, argv, pEncData);
  // decrypt whole files
  if(!err)
    err = runDecryptFileCmds(argc, argv);
  // run signature commands
  if(!err)
    err = runSignCmds(argc, argv, &pSigDoc);
  // calculate signature hash commands
  if(!err)
    err = runCalcSignCmds(argc, argv, pSigDoc);
#ifdef WITH_MSSP
  // mssp signature commands
  if(!err)
    err = runMsspSignCmds(argc, argv, pSigDoc);
  // mssp status commands
  if(!err)
    err = runMsspStatusCmds(argc, argv, pSigDoc);
#endif // WITH_MSSP
  // verify signatures
  if(!err)
    err = runVerifyCmds(argc, argv, &pSigDoc);
  // extract datafiles
  if(!err)
    err = runExtractCmds(argc, argv, &pSigDoc);
  // decrypt files
  if(!err)
    err = runDecryptCmds(argc, argv, &pEncData);

  // add signature value from a file
  if(!err)
    err = runAddSignValueCmds(argc, argv, pSigDoc);
  // delete signatures
  if(!err)
    err = runDelSignCmds(argc, argv, pSigDoc);
  // notarize signatures
  if(!err)
    err = runGetConfirmationCmds(argc, argv, pSigDoc);


  // write output file
  if(p_szOutFile && !err)
    err = cmdWrite(pSigDoc, pEncData);
  // list digidoc or encrypted documents
  if(!err)
    err = runListCmds(argc, argv, pSigDoc, pEncData);
  time(&t2);

  // display programm error code and elapsed time
  if(g_cgiMode) {
    if(ConfigItem_lookup_bool("DIGIDOC_CGI_PRINT_TRAILER", 1))
      fprintf(stdout, "\n%s%s%d%s%ld", getLibName(), g_szOutputSeparator, err, g_szOutputSeparator, (long)(t2-t1));
       
       
  } else {
    fprintf(stdout, "\n%s - RC: %d - time: %ld [sek]", getLibName(), err, (long)(t2-t1));
  }
  // list all errors
  while(hasUnreadErrors()) {
    ErrorInfo* pErr = getErrorInfo();
    char* pErrStr = getErrorString(pErr->code);
    if(g_cgiMode) {
      fprintf(stdout, "\nERROR%s%d%s%s%s%s%s%d%s%s%s%s",
	      g_szOutputSeparator, pErr->code,
	      g_szOutputSeparator, pErrStr,
	      g_szOutputSeparator, pErr->fileName,
	      g_szOutputSeparator, pErr->line,
	      g_szOutputSeparator, pErr->assertion,
	      g_szOutputSeparator, errorClass[getErrorClass(pErr->code)]);
    } else {
      fprintf(stdout, "\nERROR: %d - %s - %s",
	      pErr->code, pErrStr, pErr->assertion);
    }
  }
  fprintf(stdout, "\n");

  // cleanup
  if(pSigDoc)
    SignedDoc_free(pSigDoc);
  if(pEncData)
    dencEncryptedData_free(pEncData);
  if(g_szOutputSeparator)
	  free(g_szOutputSeparator);
  // cleanup
  cleanupConfigStore(NULL);
  finalizeDigiDocLib();

#ifdef WITH_MSSP
  if(g_pMssp)
    ddocMsspDisconnect(g_pMssp);
#endif // WITH_MSSP

  return err;
}
