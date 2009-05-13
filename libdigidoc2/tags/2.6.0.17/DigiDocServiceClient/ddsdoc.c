//==================================================
// FILE:	ddsdoc.c
// PROJECT:     DigiDocService
// DESCRIPTION: Utility program to demonstrate the
//   functionality of DDS webservice client lib
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
//      14.01.2006      Veiko Sinivee
//                      Creation
//==================================================


#include "DigiDocDds.h"
#include "ddsStub.h"

#include <libdigidoc/DigiDocDefs.h>
#include <libdigidoc/DigiDocLib.h>
#include <libdigidoc/DigiDocError.h>
#include <libdigidoc/DigiDocConfig.h>
#include <libdigidoc/DigiDocPKCS11.h>
#include <libdigidoc/DigiDocSAXParser.h>
#include <libdigidoc/DigiDocConvert.h>
#include <libdigidoc/DigiDocDebug.h>
#include <libdigidoc/DigiDocCert.h>
#include <libdigidoc/mssp/MsspErrors.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/objects.h>
#include <sys/stat.h>
#include <time.h>

//==========< global constants >====================

// programm arguments
char* p_szConfigFile = 0;
int g_cgiMode = 0;   // 1=output in CGI mode, 0=normal e.g. human readable mode
char* g_szOutputSeparator = 0;
char *errorClass[] = {"NO_ERRORS", "TECHNICAL", "USER", "LIBRARY"};
char* g_szProgNameVer = "ddsdoc/2.2.4";

DDS* g_pDds = 0;
unsigned long g_lSessCode = 0;

//==========< helper functiosn for argument handling >====================


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
  fprintf(stderr, "USAGE: ddsdoc <command(s)> [-config <config-file>]\n");
  fprintf(stderr, "COMMANDS:\n");
  fprintf(stderr, "\t[-sesscode <old-sescode>]\n");
  fprintf(stderr, "\t-start-digidoc <digidoc-file-name>\n");
  fprintf(stderr, "\t-start-file <file-name> <mime-type> <content-type>\n");
  fprintf(stderr, "\t-start\n");
  fprintf(stderr, "\t-close\n");
  fprintf(stderr, "\t-create-ddoc <format> <version>\n");
  fprintf(stderr, "\t-add-datafile <file-name> <mime-type> <content-type>\n");
  fprintf(stderr, "\t-del-datafile <id>\n");
  fprintf(stderr, "\t-get-datafile <id> <file-name>\n");
  fprintf(stderr, "\t-get-ddoc <file-name>\n");
  fprintf(stderr, "\t-get-info\n");
  fprintf(stderr, "\t-get-signer-cert <id> <file-name>\n");
  fprintf(stderr, "\t-get-notary-cert <id> <file-name>\n");
  fprintf(stderr, "\t-get-notary <id> <file-name>\n");
  fprintf(stderr, "\t-mssp-sign-cntr <id-code> <country-cd> <data-to-display> [<lang>] [<ddoc-file>] [<manifest>] [<city> <state> <zip> <country>]\n");
  fprintf(stderr, "\t-mssp-sign-phone <phone-no> <data-to-display> [<lang>] [<ddoc-file>] [<manifest>] [<city> <state> <zip> <country>]\n");
  fprintf(stderr, "\t-mssp-auth-cntr <id-code> <country-cd> <msg-to-display> [<service>] [<wait (0)>] [<lang>]\n");
  fprintf(stderr, "\t-mssp-auth-phone <phone-no> <msg-to-display> [<service>] [<wait (0)>] [<lang>]\n");
  fprintf(stderr, "\t-mssp-sign2-cntr <file-name> <mime-type> <digidoc> <id-code> <country-cd> <data-to-display> [<lang>] [<manifest>] [<city> <state> <zip> <country>]\n");
  fprintf(stderr, "\t-mssp-sign2-phone <file-name> <mime-type> <digidoc> <phone-no> <data-to-display> [<lang>] [<manifest>] [<city> <state> <zip> <country>]\n");
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
    g_szOutputSeparator = (char*)s;
}

//==========< display output >=====================

void displayStatus(const char* funcName)
{
  if(g_cgiMode)
    fprintf(stdout, "\n%s%s%ld%s%s%s%s%s%d%s%s%s%s%s%s", funcName, 
	    g_szOutputSeparator, g_pDds->lDdsSesscode, 
	    g_szOutputSeparator, (g_pDds->szStatus ? g_pDds->szStatus : ""), 
	    g_szOutputSeparator, (g_pDds->szStatusCode ? g_pDds->szStatusCode : ""), 
	    g_szOutputSeparator, g_pDds->err,
	    g_szOutputSeparator, (g_pDds->szSoapFaultCode ? g_pDds->szSoapFaultCode : ""),
	    g_szOutputSeparator, (g_pDds->szSoapFaultString ? g_pDds->szSoapFaultString : ""),
	    g_szOutputSeparator, (g_pDds->szSoapFaultDetail ? g_pDds->szSoapFaultDetail : ""));
  else
    fprintf(stdout, "\n%s session: %ld status: %s status-code: %s rc: %d fault-code: %s fault-string: fault-detail: %s", funcName,
	    g_pDds->lDdsSesscode, g_pDds->szStatus, g_pDds->szStatusCode, g_pDds->err,
	    (g_pDds->szSoapFaultCode ? g_pDds->szSoapFaultCode : ""), 
	    (g_pDds->szSoapFaultString ? g_pDds->szSoapFaultString : ""), 
	    (g_pDds->szSoapFaultDetail ? g_pDds->szSoapFaultDetail : ""));

}

void displayStringItem(const char* name, const char* value)
{
  if(value) {
  if(g_cgiMode)
    fprintf(stdout, "%s%s", g_szOutputSeparator, value);
  else
    fprintf(stdout, " %s: %s", name, value);
  }
}

void displayIntItem(const char* name, int value)
{
  if(g_cgiMode)
    fprintf(stdout, "%s%d", g_szOutputSeparator, value);
  else
    fprintf(stdout, " %s: %d", name, value);
}

void displayDataFileAttribute(struct d__DataFileAtribute* pAttr)
{
  if(pAttr) {
    fprintf(stdout, "\n\t\tATTRIBUTE");
    displayStringItem(pAttr->name, pAttr->value);
  }
}

void displayDataFile(struct d__DataFileInfo* pDf)
{
  int i;
  if(pDf) {
    fprintf(stdout, "\n\tDATAFILE");
    displayStringItem("Id", pDf->Id);
    displayStringItem("Filename", pDf->Filename);
    displayStringItem("MimeType", pDf->MimeType);
    displayStringItem("ContentType", pDf->ContentType);
    displayStringItem("DigestType", pDf->DigestType);
    displayStringItem("DigestValue", pDf->DigestValue);
    displayIntItem("Size", pDf->Size);
    for(i = 0; (pDf->__ptrAtributes) && (i < pDf->__sizeOfAtributes); i++)
      displayDataFileAttribute(pDf->__ptrAtributes + i);
  }
}

void displaySignatureError(struct d__Error* pErr)
{
  if(pErr) {
    fprintf(stdout, "\n\t\tERROR");
    displayIntItem("code", pErr->code);
    displayStringItem("category", pErr->category);
    displayStringItem("Filename", pErr->description);
  }
}

void displayRole(struct d__SignerRole* pRole)
{
  if(pRole) {
    fprintf(stdout, "\n\t\tROLE");
    displayIntItem("certified", pRole->certified);
    displayStringItem("Role", pRole->Role);
  }
}

void displaySignatureProductionPlace(struct d__SignatureProductionPlace* pAdr)
{
  if(pAdr) {
    fprintf(stdout, "\n\t\tADR");
    displayStringItem("City", pAdr->City);
    displayStringItem("StateOrProvince", pAdr->StateOrProvince);
    displayStringItem("PostalCode", pAdr->PostalCode);
    displayStringItem("CountryName", pAdr->CountryName);
  }
}

void displayCertPolicy(struct d__CertificatePolicy* pPol)
{
  if(pPol) {
    fprintf(stdout, "\n\t\t\t\tCERT-POLICY");
    displayStringItem("OID", pPol->OID);
    displayStringItem("URL", pPol->URL);
    displayStringItem("Description", pPol->Description);
  }
}

void displayCertificateInfo(struct d__CertificateInfo* pCert)
{
  int i;
  if(pCert) {
    fprintf(stdout, "\n\t\t\tCERTIFICATE");
    displayStringItem("Issuer", pCert->Issuer);
    displayStringItem("Subject", pCert->Subject);
    displayStringItem("ValidFrom", pCert->ValidFrom);
    displayStringItem("ValidTo", pCert->ValidTo);
    displayStringItem("IssuerSerial", pCert->IssuerSerial);
    for(i = 0; (pCert->__ptrPolicies) && (i < pCert->__sizePolicies); i++)
      displayCertPolicy(pCert->__ptrPolicies + i);
  }
}

void displaySignerInfo(struct d__SignerInfo* pSigner)
{
  if(pSigner) {
    fprintf(stdout, "\n\t\t\tSIGNER");
    displayStringItem("CommonName", pSigner->CommonName);
    displayStringItem("IDCode", pSigner->IDCode);
    displayCertificateInfo(pSigner->Certificate);
  }
}

void displayConfirmationInfo(struct d__ConfirmationInfo* pNot)
{
  if(pNot) {
    fprintf(stdout, "\n\t\t\tCONFIRMATION");
    displayStringItem("ResponderID", pNot->ResponderID);
    displayStringItem("ProducedAt", pNot->ProducedAt);
    displayCertificateInfo(pNot->ResponderCertificate);
  }
}

void displaySignatureInfo(struct d__SignatureInfo* pSig)
{
  int i;
  if(pSig) {
    fprintf(stdout, "\n\tSIGNATURE");
    displayStringItem("Id", pSig->Id);
    displayStringItem("Status", pSig->Status);
    displaySignatureError(pSig->Error);
    displayStringItem("SigningTime", pSig->SigningTime);
    for(i = 0; (pSig->__ptrSignerRole) && (i < pSig->__sizeRoles); i++)
      displayRole(pSig->__ptrSignerRole + i);
    displaySignatureProductionPlace(pSig->SignatureProductionPlace);
    displaySignerInfo(pSig->Signer);
    displayConfirmationInfo(pSig->Confirmation);    
  }
}


void displaySignedDoc()
{
  int i;

  if(g_pDds && g_pDds->pSigDocInfo) {
    fprintf(stdout, "\nDIGIDOC");
    displayStringItem("format", g_pDds->pSigDocInfo->format);
    displayStringItem("version", g_pDds->pSigDocInfo->version);
    for(i = 0; (g_pDds->pSigDocInfo->__ptrDataFileInfo) && 
	  (i < g_pDds->pSigDocInfo->__sizeOfDataFileInfos); i++) {
      displayDataFile(g_pDds->pSigDocInfo->__ptrDataFileInfo + i);
    }
    for(i = 0; (g_pDds->pSigDocInfo->__ptrSignatureInfo) && 
	  (i < g_pDds->pSigDocInfo->__sizeOfSignatureInfos); i++) {
      displaySignatureInfo(g_pDds->pSigDocInfo->__ptrSignatureInfo + i);
    }
  }
}


//==========< command handlers >====================


//--------------------------------------------------
// start session by sending a digidoc file to server
//--------------------------------------------------
int runSessioncodeCmds(int argc, char** argv)
{
  int err = ERR_OK, i;

  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      // add a DataFile
      if(!strcmp(argv[i], "-sesscode")) {
	if(argc > i+1 && argv[i+1][0] != '-') {
	  g_lSessCode = atol(argv[i+1]);
	  g_pDds->lDdsSesscode = g_lSessCode;
	} else {
	  err = ERR_BAD_PARAM;
	  addError(err, __FILE__, __LINE__, "Missing <sesscode> argument of -sesscode command");
	}
      }
    }
  }
  return err;
}

//--------------------------------------------------
// start session by sending a digidoc file to server
//--------------------------------------------------
int runStartSessionWithDigidocCmds(int argc, char** argv)
{
  int err = ERR_OK, i, bHoldSession = 1;

  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      // add a DataFile
      if(!strcmp(argv[i], "-start-digidoc")) {
	if(argc > i+1 && argv[i+1][0] != '-') {
	  char* file = NULL;
	  checkArguments(argc, argv, &i, &file);
	  // send SOAP command
	  err = ddocDdsStartSession(g_pDds, bHoldSession, 0, file, 0, 0, 0, 0);
	  // report success
	  displayStatus("START-DIGIDC");
	  displaySignedDoc();
	  // cleanup
	  if(file) free(file);
	} else {
	  err = ERR_BAD_PARAM;
	  addError(err, __FILE__, __LINE__, "Missing <file> argument of -start-digidoc command");
	}
      }
    }
  }
  return err;
}

//--------------------------------------------------
// start session by sending a digidoc file to server
//--------------------------------------------------
int runStartSessionWithFileCmds(int argc, char** argv)
{
  int err = ERR_OK, i, bHoldSession = 1;

  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      // add a DataFile
      if(!strcmp(argv[i], "-start-file")) {
	if(argc > i+1 && argv[i+1][0] != '-') {
	  char* file = NULL;
	  char* mime = NULL;
	  char* content = NULL;
	  checkArguments(argc, argv, &i, &file);
	  checkArguments(argc, argv, &i, &mime);
	  checkArguments(argc, argv, &i, &content);
	  // send SOAP command
	  err = ddocDdsStartSession(g_pDds, bHoldSession, 0, 0, file, mime, content, 0);
	  // report success
	  displayStatus("START-FILE");
	  displaySignedDoc();
	  // cleanup
	  if(file) free(file);
	  if(mime) free(mime);
	  if(content) free(content);
	} else {
	  err = ERR_BAD_PARAM;
	  addError(err, __FILE__, __LINE__, "Missing <file> argument of -start-file command");
	}
      }
    }
  }
  return err;
}

//--------------------------------------------------
// start session
//--------------------------------------------------
int runStartSessionCmds(int argc, char** argv)
{
  int err = ERR_OK, i, bHoldSession = 1;

  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      // add a DataFile
      if(!strcmp(argv[i], "-start")) {
	// send SOAP command
	err = ddocDdsStartSession(g_pDds, bHoldSession, 0, 0, 0, 0, 0, 0);
	// report success
	displayStatus("START");
	displaySignedDoc();
      }
    }
  }
  return err;
}

//--------------------------------------------------
// close session
//--------------------------------------------------
int runCloseSessionCmds(int argc, char** argv)
{
  int err = ERR_OK, i, bHoldSession = 1;

  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      // add a DataFile
      if(!strcmp(argv[i], "-close")) {
	// send SOAP command
	err = ddocDdsCloseSession(g_pDds);
	// report success
	displayStatus("CLOSE");
      }
    }
  }
  return err;
}


//--------------------------------------------------
// create a new signed doc on server
//--------------------------------------------------
int runCreateSignedDocCmds(int argc, char** argv)
{
  int err = ERR_OK, i;

  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      // add a DataFile
      if(!strcmp(argv[i], "-create-ddoc")) {
	if(argc > i+2 && argv[i+1][0] != '-' && argv[i+2][0] != '-') {
	  char* format = NULL;
	  char* version = NULL;
	  checkArguments(argc, argv, &i, &format);
	  checkArguments(argc, argv, &i, &version);
	  // send SOAP command
	  err = ddocDdsCreateSignedDoc(g_pDds, format, version);
	  // report success
	  displayStatus("CREATE-DDOC");
	  displaySignedDoc();
	  // cleanup
	  if(format) free(format);
	  if(version) free(version);
	} else {
	  err = ERR_BAD_PARAM;
	  addError(err, __FILE__, __LINE__, "Missing <format> or <version> argument of -create-ddoc command");
	}
      }
    }
  }
  return err;
}

//--------------------------------------------------
// add a new datafile
//--------------------------------------------------
int runAddDataFileCmds(int argc, char** argv)
{
  int err = ERR_OK, i;

  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      // add a DataFile
      if(!strcmp(argv[i], "-add-datafile")) {
	if(argc > i+2 && argv[i+1][0] != '-' && argv[i+2][0] != '-') {
	  char* file = NULL;
	  char* mime = NULL;
	  char* content = NULL;
	  checkArguments(argc, argv, &i, &file);
	  checkArguments(argc, argv, &i, &mime);
	  checkArguments(argc, argv, &i, &content);
	  // send SOAP command
	  err = ddocDdsAddDataFile(g_pDds, file, mime, content);
	  // report success
	  displayStatus("ADD-DF");
	  displaySignedDoc();
	  // cleanup
	  if(file) free(file);
	  if(mime) free(mime);
	  if(content) free(content);
	} else {
	  err = ERR_BAD_PARAM;
	  addError(err, __FILE__, __LINE__, "Missing <file>, <mime-type> or <content-type> argument of -add-datafile command");
	}
      }
    }
  }
  return err;
}

//--------------------------------------------------
// remove a datafile
//--------------------------------------------------
int runRemoveDataFileCmds(int argc, char** argv)
{
  int err = ERR_OK, i;

  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      // add a DataFile
      if(!strcmp(argv[i], "-del-datafile")) {
	if(argc > i+1 && argv[i+1][0] != '-') {
	  char* id = NULL;
	  checkArguments(argc, argv, &i, &id);
	  // send SOAP command
	  err = ddocDdsRemoveDataFile(g_pDds, id);
	  // report success
	  displayStatus("DEL-DF");
	  displaySignedDoc();
	  // cleanup
	  if(id) free(id);
	} else {
	  err = ERR_BAD_PARAM;
	  addError(err, __FILE__, __LINE__, "Missing <id> argument of -del-datafile command");
	}
      }
    }
  }
  return err;
}

//--------------------------------------------------
// retrieve a digidoc from server
//--------------------------------------------------
int runGetDdocCmds(int argc, char** argv)
{
  int err = ERR_OK, i;

  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      if(!strcmp(argv[i], "-get-ddoc")) {
	if(argc > i+1 && argv[i+1][0] != '-') {
	  char* file = NULL;
	  checkArguments(argc, argv, &i, &file);
	  // send SOAP command
	  err = ddocDdsGetSignedDoc(g_pDds, file);
	  // report success
	  displayStatus("GET-DDOC");
	  displaySignedDoc();
	  // cleanup
	  if(file) free(file);
	} else {
	  err = ERR_BAD_PARAM;
	  addError(err, __FILE__, __LINE__, "Missing <file> argument of -get-ddoc command");
	}
      }
    }
  }
  return err;
}


//--------------------------------------------------
// display current session digidoc info
//--------------------------------------------------
int runInfoCmds(int argc, char** argv)
{
  int err = ERR_OK, i;

  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      if(!strcmp(argv[i], "-info")) {
	// send SOAP command
	err = ddocDdsGetSignedDocInfo(g_pDds);
	// report success
	displayStatus("GET-DDOC");
	displaySignedDoc();
      }
    }
  }
  return err;
}

//--------------------------------------------------
// get datafile
//--------------------------------------------------
int runGetDataFileCmds(int argc, char** argv)
{
  int err = ERR_OK, i;

  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      if(!strcmp(argv[i], "-get-datafile")) {
	if(argc > i+2 && argv[i+1][0] != '-' && argv[i+2][0] != '-') {
	  char* file = NULL;
	  char* id = NULL;
	  checkArguments(argc, argv, &i, &id);
	  checkArguments(argc, argv, &i, &file);
	  // send SOAP command
	  err = ddocDdsGetDataFile(g_pDds, id, file);
	  // report success
	  displayStatus("GET-DF");
	  displaySignedDoc();
	  // cleanup
	  if(file) free(file);
	  if(id) free(id);
	} else {
	  err = ERR_BAD_PARAM;
	  addError(err, __FILE__, __LINE__, "Missing <id> or <file> argument of -get-datafile command");
	}
      }
    }
  }
  return err;
}

//--------------------------------------------------
// get signers certificate
//--------------------------------------------------
int runGetSignerCertCmds(int argc, char** argv)
{
  int err = ERR_OK, i;

  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      if(!strcmp(argv[i], "-get-signer-cert")) {
	if(argc > i+2 && argv[i+1][0] != '-' && argv[i+2][0] != '-') {
	  char* file = NULL;
	  char* id = NULL;
	  checkArguments(argc, argv, &i, &id);
	  checkArguments(argc, argv, &i, &file);
	  // send SOAP command
	  err = ddocDdsGetSignersCertificate(g_pDds, id, file);
	  // report success
	  displayStatus("GET-SIGNER-CERT");
	  displaySignedDoc();
	  // cleanup
	  if(file) free(file);
	  if(id) free(id);
	} else {
	  err = ERR_BAD_PARAM;
	  addError(err, __FILE__, __LINE__, "Missing <id> or <file> argument of -get-signer-cert command");
	}
      }
    }
  }
  return err;
}

//--------------------------------------------------
// get notarys certificate
//--------------------------------------------------
int runGetNotaryCertCmds(int argc, char** argv)
{
  int err = ERR_OK, i;

  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      if(!strcmp(argv[i], "-get-notary-cert")) {
	if(argc > i+2 && argv[i+1][0] != '-' && argv[i+2][0] != '-') {
	  char* file = NULL;
	  char* id = NULL;
	  checkArguments(argc, argv, &i, &id);
	  checkArguments(argc, argv, &i, &file);
	  // send SOAP command
	  err = ddocDdsGetNotarysCertificate(g_pDds, id, file);
	  // report success
	  displayStatus("GET-NOTARY-CERT");
	  displaySignedDoc();
	  // cleanup
	  if(file) free(file);
	  if(id) free(id);
	} else {
	  err = ERR_BAD_PARAM;
	  addError(err, __FILE__, __LINE__, "Missing <id> or <file> argument of -get-notary-cert command");
	}
      }
    }
  }
  return err;
}

//--------------------------------------------------
// get notarys OCSP confirmation
//--------------------------------------------------
int runGetNotaryCmds(int argc, char** argv)
{
  int err = ERR_OK, i;

  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      if(!strcmp(argv[i], "-get-notary")) {
	if(argc > i+2 && argv[i+1][0] != '-' && argv[i+2][0] != '-') {
	  char* file = NULL;
	  char* id = NULL;
	  checkArguments(argc, argv, &i, &id);
	  checkArguments(argc, argv, &i, &file);
	  // send SOAP command
	  err = ddocDdsGetNotary(g_pDds, id, file);
	  // report success
	  displayStatus("GET-NOTARY");
	  displaySignedDoc();
	  // cleanup
	  if(file) free(file);
	  if(id) free(id);
	} else {
	  err = ERR_BAD_PARAM;
	  addError(err, __FILE__, __LINE__, "Missing <id> or <file> argument of -get-notary command");
	}
      }
    }
  }
  return err;
}


// TODO: prepare & finalize


//--------------------------------------------------
// removes a signature
//--------------------------------------------------
int runRemoveSignatureCmds(int argc, char** argv)
{
  int err = ERR_OK, i;

  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      if(!strcmp(argv[i], "-del-sig")) {
	if(argc > i+1 && argv[i+1][0] != '-') {
	  char* id = NULL;
	  checkArguments(argc, argv, &i, &id);
	  // send SOAP command
	  err = ddocDdsRemoveSignature(g_pDds, id);
	  // report success
	  displayStatus("DEL-SIG");
	  displaySignedDoc();
	  // cleanup
	  if(id) free(id);
	} else {
	  err = ERR_BAD_PARAM;
	  addError(err, __FILE__, __LINE__, "Missing <id> argument of -del-sig command");
	}
      }
    }
  }
  return err;
}

//--------------------------------------------------
// signs via MSSP
//--------------------------------------------------
int runMobileSignatureCmds(int argc, char** argv)
{
  int err = ERR_OK, i, nPollFreq;
#ifdef WIN32
  time_t tNew, tOld;
#endif

  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      if(!strcmp(argv[i], "-mssp-sign-cntr") ||
	 !strcmp(argv[i], "-mssp-sign-phone")) {
	char* idcode = NULL;
	char* cntrcd = NULL;
	char* phone = NULL;
	char* datatodisp = NULL;
	char* lang = NULL;
	char* role = NULL;
	char* ddocfile = NULL;
	char* city = NULL;
	char* state = NULL;
	char* zip = NULL;
	char* country = NULL;
	if(!strcmp(argv[i], "-mssp-sign-cntr")) {
	  checkArguments(argc, argv, &i, &idcode);
	  checkArguments(argc, argv, &i, &cntrcd);
	}
	if(!strcmp(argv[i], "-mssp-sign-phone")) {
	  checkArguments(argc, argv, &i, &phone);
	}
	checkArguments(argc, argv, &i, &datatodisp);
	checkArguments(argc, argv, &i, &lang);
	checkArguments(argc, argv, &i, &ddocfile);
	checkArguments(argc, argv, &i, &role);
	checkArguments(argc, argv, &i, &city);
	checkArguments(argc, argv, &i, &state);
	checkArguments(argc, argv, &i, &zip);
	checkArguments(argc, argv, &i, &country);
	if(((idcode && cntrcd) || phone) && datatodisp) {
	  // send SOAP command
	  err = ddocDdsMobileSign(g_pDds, idcode, cntrcd, phone, datatodisp,
				  lang, role, city, state, zip, country,
				  0, "asynchClientServer", NULL, NULL,
				  0, 0, ddocfile);
	  // report success
	  displayStatus("MSSP-SIG");
	  //displaySignedDoc();
	  
	} else {
	  err = ERR_BAD_PARAM;
	  addError(err, __FILE__, __LINE__, "Missing <id-code>, <phone> or <data-to-display> argument of -mssp-sign command");
	}
	// cleanup
	if(idcode) free(idcode);
	if(phone) free(phone);
	if(datatodisp) free(datatodisp);
	if(lang) free(lang);
	if(role) free(role);
	if(ddocfile) free(ddocfile);
	if(city) free(city);
	if(state) free(state);
	if(zip) free(zip);
	if(country) free(country);
	if(cntrcd) free(cntrcd);
	if(err) return err;
	// poll for signature complete, cancel or error status
	nPollFreq = ConfigItem_lookup_int("DIGIDOC_MSSP_STATUS_POLL_FREQ", 10);
	do {
#ifdef WIN32
	  time(&tOld);
	  do {
	    time(&tNew);
	  } while(tNew - tOld < nPollFreq);
#else
	  sleep(nPollFreq);
#endif
	  err = ddocDdsGetStatusInfo(g_pDds, 0);
	  // report success
	  displayStatus("MSSP-SIG");
	} while(!err && 
		(!g_pDds->szStatusCode ||
		 !strcmp(g_pDds->szStatusCode, REQUEST_OK_CODE) ||
		 !strcmp(g_pDds->szStatusCode, OUTSTANDING_TRANSACTION_CODE)));
	printf("MSSP signature finished rc: %d status: %s", err, g_pDds->szStatus);
      }
    }
  }
  return err;
}


//--------------------------------------------------
// signs via MSSP
//--------------------------------------------------
int runMobileSignature2Cmds(int argc, char** argv)
{
  int err = ERR_OK, i, nPollFreq, l1;
  SignedDoc* pSigDoc = 0;
  DataFile  *pDf = 0;
#ifdef WIN32
  time_t tNew, tOld;
#endif
  char buf1[300];

  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      if(!strcmp(argv[i], "-mssp-sign2-cntr") ||
	 !strcmp(argv[i], "-mssp-sign2-phone")) {
	char* file = NULL;
	char* mime = NULL;
	char* digidoc = NULL;
	char* idcode = NULL;
	char* cntrcd = NULL;
	char* phone = NULL;
	char* datatodisp = NULL;
	char* lang = NULL;
	char* role = NULL;
	char* city = NULL;
	char* state = NULL;
	char* zip = NULL;
	char* country = NULL;
	char* p = NULL;
	// create digidoc file in bas64 format
	checkArguments(argc, argv, &i, &file);
	checkArguments(argc, argv, &i, &mime);
	checkArguments(argc, argv, &i, &digidoc);
	err = SignedDoc_new(&pSigDoc, NULL, NULL); //DIGIDOC_XML_1_1_NAME, DIGIDOC_XML_1_3_VER);
	err = ddocConvertInput(file, &p);
	l1 = sizeof(buf1);
	getFullFileName(p, buf1, l1);
	if(p)
	  free(p);
	err = DataFile_new(&pDf, pSigDoc, NULL, buf1, 
			   CONTENT_EMBEDDED_BASE64, mime, 
			   0, NULL, 0, NULL, CHARSET_UTF_8);
	if(!err)
	  err = calculateDataFileSizeAndDigest(pSigDoc, pDf->szId, buf1, DIGEST_SHA1);
	err = createSignedDoc(pSigDoc, NULL, digidoc);
	print("Created digidoc: %s from input: %s mime: %s", digidoc, file, mime);
	// start signing
	if(!strcmp(argv[i], "-mssp-sign2-cntr")) {
	  checkArguments(argc, argv, &i, &idcode);
	  checkArguments(argc, argv, &i, &cntrcd);
	}
	if(!strcmp(argv[i], "-mssp-sign2-phone")) {
	  checkArguments(argc, argv, &i, &phone);
	}
	checkArguments(argc, argv, &i, &datatodisp);
	checkArguments(argc, argv, &i, &lang);
	checkArguments(argc, argv, &i, &role);
	checkArguments(argc, argv, &i, &city);
	checkArguments(argc, argv, &i, &state);
	checkArguments(argc, argv, &i, &zip);
	checkArguments(argc, argv, &i, &country);
	if(((idcode && cntrcd) || phone) && datatodisp) {
	  // send SOAP command
	  err = ddocDdsSignDocWpkiStart(pSigDoc, &g_pDds, NULL,
					idcode, cntrcd, phone, digidoc,
					datatodisp, city, 
					state, zip, country, lang, NULL);
	  // report success
	  displayStatus("MSSP-SIG");
	  //displaySignedDoc();
	  
	} else {
	  err = ERR_BAD_PARAM;
	  addError(err, __FILE__, __LINE__, "Missing <id-code>, <phone> or <data-to-display> argument of -mssp-sign command");
	}
	// cleanup
	if(p) free(p);
	if(file) free(file);
	if(mime) free(mime);
	if(idcode) free(idcode);
	if(phone) free(phone);
	if(datatodisp) free(datatodisp);
	if(lang) free(lang);
	if(role) free(role);
	if(city) free(city);
	if(state) free(state);
	if(zip) free(zip);
	if(country) free(country);
	if(cntrcd) free(cntrcd);
	if(err) return err;
	// poll for signature complete, cancel or error status
	nPollFreq = ConfigItem_lookup_int("DIGIDOC_MSSP_STATUS_POLL_FREQ", 10);
	do {
#ifdef WIN32
	  time(&tOld);
	  do {
	    time(&tNew);
	  } while(tNew - tOld < nPollFreq);
#else
	  sleep(nPollFreq);
#endif
	  err = ddocDdsGetStatusInfo(g_pDds, 0);
	  // report success
	  displayStatus("MSSP-SIG");
	} while(!err && 
		(!g_pDds->szStatusCode ||
		 !strcmp(g_pDds->szStatusCode, REQUEST_OK_CODE) ||
		 !strcmp(g_pDds->szStatusCode, OUTSTANDING_TRANSACTION_CODE)));
	printf("MSSP signature finished rc: %d status: %s", err, g_pDds->szStatus);
      } // if
      if(!strcmp(g_pDds->szStatusCode, REQUEST_OK_CODE)) {
	err = ddocDdsSignDocWpkiStop(pSigDoc, g_pDds, digidoc, digidoc);
	printf("Completed digidoc: %s", digidoc);
      }
      if(digidoc) free(digidoc);
      if(pSigDoc) SignedDoc_free(pSigDoc);
    } // if(argv[0]...
  } // for
  return err;
}



//--------------------------------------------------
// authenticates via MSSP
//--------------------------------------------------
int runMobileAuthenticateCmds(int argc, char** argv)
{
  int err = ERR_OK, i, nPollFreq;
  DigiDocMemBuf mbuf1, mbuf2, mbuf3, mbuf4, mbuf5, mbuf6, mbuf7, mbuf8, mbuf9;
#ifdef WIN32
  time_t tNew, tOld;
#endif

  mbuf1.pMem = 0;
  mbuf1.nLen = 0;
  mbuf2.pMem = 0;
  mbuf2.nLen = 0;
  mbuf3.pMem = 0;
  mbuf3.nLen = 0;
  mbuf4.pMem = 0;
  mbuf4.nLen = 0;
  mbuf5.pMem = 0;
  mbuf5.nLen = 0;
  mbuf6.pMem = 0;
  mbuf6.nLen = 0;
  mbuf7.pMem = 0;
  mbuf7.nLen = 0;
  mbuf8.pMem = 0;
  mbuf8.nLen = 0;
  mbuf9.pMem = 0;
  mbuf9.nLen = 0;
  for(i = 1; (err == ERR_OK) && (i < argc); i++) {
    if(argv[i][0] == '-') { // all commands and options must start with -
      if(!strcmp(argv[i], "-mssp-auth-cntr") ||
	 !strcmp(argv[i], "-mssp-auth-phone")) {
	char* idcode = NULL;
	char* cntrcd = NULL;
	char* phone = NULL;
	char* datatodisp = NULL;
	char* lang = NULL;
	char* service = NULL;
	int bWait = 0;
	char* s = NULL;
	if(!strcmp(argv[i], "-mssp-auth-cntr")) {
	  checkArguments(argc, argv, &i, &idcode);
	  checkArguments(argc, argv, &i, &cntrcd);
	}
	if(!strcmp(argv[i], "-mssp-auth-phone")) {
	  checkArguments(argc, argv, &i, &phone);
	}
	checkArguments(argc, argv, &i, &datatodisp);
	checkArguments(argc, argv, &i, &service);
	checkArguments(argc, argv, &i, &s);
	if(s)
	  bWait = atoi(s);
	checkArguments(argc, argv, &i, &lang);
	if(((idcode && cntrcd) || phone) && datatodisp) {
	  // send SOAP command
	  err = ddocDdsMobileAuthenticate(g_pDds, idcode, cntrcd, phone, lang, datatodisp,
					  0, 0, "asynchClientServer", 0, 0, 0, service,
					  &mbuf1, &mbuf2, &mbuf3, &mbuf4,
					  &mbuf5, &mbuf6, &mbuf7, &mbuf8, &mbuf9);
	  // report success
	  displayStatus("MSSP-AUTH");
	  displayStringItem("ID-Code", (mbuf1.pMem ? (char*)mbuf1.pMem : "-"));
	  displayStringItem("First Name", (mbuf2.pMem ? (char*)mbuf2.pMem : "-"));
	  displayStringItem("Last Name", (mbuf3.pMem ? (char*)mbuf3.pMem : "-"));
	  displayStringItem("Country", (mbuf4.pMem ? (char*)mbuf4.pMem : "-"));
	  displayStringItem("CN", (mbuf5.pMem ? (char*)mbuf5.pMem : "-"));
	  displayStringItem("Cert", (mbuf6.pMem ? (char*)mbuf6.pMem : "-"));
	  displayStringItem("ChallengeID", (mbuf7.pMem ? (char*)mbuf7.pMem : "-"));
	  displayStringItem("Challenge", (mbuf8.pMem ? (char*)mbuf8.pMem : "-"));
	  displayStringItem("OCSP", (mbuf9.pMem ? (char*)mbuf9.pMem : "-"));
	} else {
	  err = ERR_BAD_PARAM;
	  addError(err, __FILE__, __LINE__, "Missing <id-code>, <phone> or <data-to-display> argument of -mssp-auth command");
	}
	// cleanup
	if(idcode) free(idcode);
	if(phone) free(phone);
	if(cntrcd) free(cntrcd);
	if(s) free(s);
	if(datatodisp) free(datatodisp);
	if(lang) free(lang);
	ddocMemBuf_free(&mbuf1);
	ddocMemBuf_free(&mbuf2);
	ddocMemBuf_free(&mbuf3);
	ddocMemBuf_free(&mbuf4);
	ddocMemBuf_free(&mbuf5);
	ddocMemBuf_free(&mbuf6);
	ddocMemBuf_free(&mbuf7);
	ddocMemBuf_free(&mbuf8);
	ddocMemBuf_free(&mbuf9);
	if(err) return err;

	// poll for signature complete, cancel or error status
	nPollFreq = ConfigItem_lookup_int("DIGIDOC_MSSP_STATUS_POLL_FREQ", 10);
	do {
#ifdef WIN32
	  time(&tOld);
	  do {
	    time(&tNew);
	  } while(tNew - tOld < nPollFreq);
#else
	  sleep(nPollFreq);
#endif
	  err = ddocDdsGetMobileAuthenticateStatus(g_pDds, bWait, &mbuf1);
	  // report success
	  displayStatus("MSSP-AUTH");
	  //displayStringItem("Signature", (mbuf1.pMem ? (char*)mbuf1.pMem : "-"));
	  ddocMemBuf_free(&mbuf1);
	} while(!err && 
                (!g_pDds->szStatus || strcmp(g_pDds->szStatus, "USER_AUTHENTICATED")) &&
		(!g_pDds->szStatusCode ||
		 !strcmp(g_pDds->szStatusCode, REQUEST_OK_CODE) ||
		 !strcmp(g_pDds->szStatusCode, OUTSTANDING_TRANSACTION_CODE)));
	printf("MSSP authentication finished rc: %d status: %s", err, g_pDds->szStatus);
      }
    }
  }
  return err;
}






//--------------------------------------------------
// Program main function
//--------------------------------------------------
int main(int argc, char** argv)
{
  int err = ERR_OK;
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
  getFileNamePath(argv[0], buf1);
  if(strlen(buf1)) {
	strcat(buf1, "digidoc.ini");
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
  // connect to DDS service
  err = ddocDdsConnect(&g_pDds);

  // display programm name and version
  if(g_cgiMode) {
    if(ConfigItem_lookup_bool("DIGIDOC_CGI_PRINT_HEADER", 1))
      fprintf(stdout, "%s%s%s", getLibName(), g_szOutputSeparator, getLibVersion());
  } else {
    fprintf(stdout, "%s - %s", getLibName(), getLibVersion());
  }

  // execute command now
  if(!err) // copy old session code if available
    err = runSessioncodeCmds(argc, argv);
  // start session with existing digidoc file
  if(!err) // copy old session code if available
    err = runStartSessionWithDigidocCmds(argc, argv);
  // start session with new data file
  if(!err)
    err = runStartSessionWithFileCmds(argc, argv);
  // start session 
  if(!err)
    err = runStartSessionCmds(argc, argv);
  // create signed doc
  if(!err)
    err = runCreateSignedDocCmds(argc, argv);
  // add data files
  if(!err)
    err = runAddDataFileCmds(argc, argv);
  // remove data files
  if(!err)
    err = runRemoveDataFileCmds(argc, argv);
  // remove signature
  if(!err)
    err = runRemoveSignatureCmds(argc, argv);
  // mobile signature
  if(!err)
    err = runMobileSignatureCmds(argc, argv);
  // mobile authenticate
  if(!err)
    err = runMobileAuthenticateCmds(argc, argv);
  // mobile sign 2
  if(!err)
    err = runMobileSignature2Cmds(argc, argv);

  // get signed doc
  if(!err)
    err = runGetDdocCmds(argc, argv);
  // get datafile
  if(!err)
    err = runGetDataFileCmds(argc, argv);
  // get signers certificate
  if(!err)
    err = runGetSignerCertCmds(argc, argv);
  // get notarys certificate
  if(!err)
    err = runGetNotaryCertCmds(argc, argv);
  // get notarys OCSP confirmation
  if(!err)
    err = runGetNotaryCmds(argc, argv);

  // display info
  if(!err)
    err = runInfoCmds(argc, argv);
  // close session
  if(!err)
    err = runCloseSessionCmds(argc, argv);

  time(&t2);
  // disconnect from DDS service
  if(g_pDds)
    err = ddocDdsDisconnect(g_pDds);

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
  if(g_szOutputSeparator)
    free(g_szOutputSeparator);
  // cleanup
  cleanupConfigStore(NULL);
  finalizeDigiDocLib();

  return err;
}
