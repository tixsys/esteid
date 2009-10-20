////////////////////////////////////////////////////////////////////////////////////
// Sign.cpp : Implementation of CSign (main object)
// Signing utilities
// Author: Tarmo@sk.ee
// Last change: 23.04.2003
///////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <stdio.h>
#include <windows.h>
#include "EIDCard.h"
#include "Sign.h"
#include <comdef.h>
#include <WinCrypt.h>
#include <commctrl.h>
#include "Resource.h"
#include "DlgUnit.h"
#include "DigiCrypt.h"

unsigned char BtoH(char ch);
void HexBin(char * src, char * dest, int destlen);
void BinHex(char *Src, unsigned Len, char *Dest );
void in_BSTR2sz(BSTR copyFrom, char **copyTo);



BOOL GetRSAKeyFromCert(PCCERT_CONTEXT pCertContext,HCRYPTPROV *hProv,HCRYPTKEY *hPubKey,DWORD *dwKeySpec,BOOL *fFreeProv);
BOOL SignHashString(HCRYPTPROV hProv,HCRYPTKEY hPubKey,DWORD dwKeySpec,LPBYTE pbHash,LPBYTE pbSignature);

///////////// WinCrypt
static BOOL  oG_fDialogUserCancel;
#define DLGMAXIEMS 512
static char *psListItems[DLGMAXIEMS];
static PCCERT_CONTEXT psListItemsCert[DLGMAXIEMS];
static int iListItems=0;

////////////////////////////////////////////////////////////////////////////////////
unsigned char BtoH(char ch) 
////////////////////////////////////////////////////////////////////////////////////
// function for Binary to hex  coversion
{     
	if (ch >= '0' && ch <= '9') return (ch - '0');		// Handle numerals     
	if (ch >= 'A' && ch <= 'F') return (ch - 'A' + 0xA);  // Handle capitol hex digits     
	if (ch >= 'a' && ch <= 'f') return (ch - 'a' + 0xA);  // Handle small hex digits     
	return(255); 
}  
////////////////////////////////////////////////////////////////////////////////////
void HexBin(char * src, char * dest, int destlen) 
////////////////////////////////////////////////////////////////////////////////////
{	
// function for conversion hex -> bin strings
	
	char * srcptr;      
	srcptr = src;      
	while(destlen--)     
	{         
		*dest = BtoH(*srcptr++) << 4;     // Put 1st ascii byte in upper nibble. 
		*dest++ += BtoH(*srcptr++);      // Add 2nd ascii byte to above.     
	} 
}  

///////////////////////////////////////////////////////////////////
void BinHex(char *Src, unsigned Len, char *Dest )
///////////////////////////////////////////////////////////////////
{
	//function for binary to hex string conversion
	unsigned i, Len2;

	Len2 = Len<<1;
	
	for ( i = 0; i < Len; i++)
	{
		Dest[i<<1] = (Src[i] & 0xF0)>>4;
		Dest[(i<<1) + 1] = Src[i] & 0x0F;
	}

	for ( i = 0; i < Len2; i++)
	{
		Dest[i] = Dest[i] < 0x0a ? Dest[i] + 0x30: Dest[i] + 0x37;
	}

Dest[Len2] = 0;

	return;

}
///////////////////////////////////////////////////////////////////
void in_BSTR2sz(BSTR copyFrom, char **copyTo)
///////////////////////////////////////////////////////////////////
	{
	  unsigned int newLen = SysStringLen(copyFrom);
		
	  if (*copyTo != (char*)0)
	    delete *copyTo;
  

	  if (newLen > 0) {
	    USES_CONVERSION;
	    *copyTo = new char[newLen + 1];
	    lstrcpyA(*copyTo, OLE2A(copyFrom));
	  }
	}


///////////////////////////////////////////////////////////////////////////////////////
// Function: GetRSAKeyFromCert
// Description: RSA key fetching from the cert
///////////////////////////////////////////////////////////////////////////////////////
BOOL GetRSAKeyFromCert(
///////////////////////////////////////////////////////////////////////////////////////
                       PCCERT_CONTEXT pCertContext,
                       HCRYPTPROV *hProv,
                       HCRYPTKEY *hPubKey,
                       DWORD *dwKeySpec,
                       BOOL *fFreeProv)
{   
   BOOL fResult;
   BOOL fReturn = FALSE;

   __try
   {  
      if (hProv == NULL || hPubKey == NULL || 
          dwKeySpec == NULL || fFreeProv == NULL)
      {
		 __leave;

      }

      *hProv = NULL;
      *hPubKey = NULL;
      *fFreeProv = FALSE;
    
      fResult = CryptAcquireCertificatePrivateKey(pCertContext,
                                                     NULL,
                                                     NULL,
                                                     hProv,
                                                     dwKeySpec,
                                                     fFreeProv);
	 if (!fResult)
         {
            __leave;
         }


      fReturn = TRUE;
   }
   __finally
   {
      if (!fReturn)
      {
         if (*hPubKey != NULL) 
         {
            CryptDestroyKey(*hPubKey);
            *hPubKey = NULL;
         }

         if ((*fFreeProv == TRUE) && (*hProv != NULL)) 
         {
            CryptReleaseContext(*hProv, 0);
            *hProv = NULL;
            *fFreeProv = FALSE;
         }
      }
   }

   return fReturn;
}
///////////////////////////////////////////////////////////////////////////////////////
// Function: SignHashString
// Description: hash string signing
/////////////////////////////////////////////////////////////////////////////////////////
BOOL SignHashString(
/////////////////////////////////////////////////////////////////////////////////////////
					HCRYPTPROV hProv,
                    HCRYPTKEY hPubKey,
                    DWORD dwKeySpec,
					LPBYTE pbHash,
					LPBYTE pbSignature,
					DWORD *dwSignature)
{
   HCRYPTHASH hHash = NULL;
   
   BOOL fResult;
   BOOL fReturn = FALSE;

   __try
   {  
      // Create Hash
      fResult = CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash);
      if (!fResult)
        __leave;
     

  
	  // Set Hash
	  fResult=CryptSetHashParam(hHash,HP_HASHVAL,pbHash,0);

      
      fResult = CryptSignHash(hHash, dwKeySpec, NULL, 0, pbSignature, dwSignature);
         if (!fResult)
            __leave;

    fReturn = TRUE;
   }
   
 __finally
   {
      if (hHash != NULL) CryptDestroyHash(hHash);      
   }

   return fReturn;
}
/////////////////////////////////////////////////////////////////////////////
// CSign class methods
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSign::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ISign
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		//if (::ATL::InlineIsEqualGUID(*arr[i],riid))
		//	return S_OK;
	}
	return S_FALSE;
}
////////////////////////////////////////////////////////////////////////////////////
// The Information of Module Get Method
////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSign::getInfo(BSTR *infoText)
////////////////////////////////////////////////////////////////////////////////////
{
	

	*infoText=_bstr_t((LPCSTR)INFOTEXT).copy();

	return S_OK;
}
////////////////////////////////////////////////////////////////////////////////////
//The Signing Certificate Get Method
//out:
//		signing certificate in hex form
//Additionaly change values of attributes corresponding with signing cert (SigningKeyContainerName, SigningCSPName, SigningCertName, SigningCertIssuer)
////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSign::getSigningCertificate(BSTR *Certificate)
///////////////////////////////////////////////////////////////////////////////////
{
   
   TCHAR*       pbData = NULL; 
   DWORD selectedCert=-1;
   DWORD dwResult;
   PCCERT_CONTEXT pCertContext;
   int ret;

   ret = MessageBox(NULL, "This web site is trying to access your ID card.\nDo you want to continue?",
                          "Accessing ID Card",
                          MB_OKCANCEL | MB_ICONWARNING);

   if (ret == IDCANCEL)
       return S_FALSE;

   pCertContext=NULL;

   pCertContext=DigiCrypt_FindContext(0, &dwResult,&selectedCert);		   
   
   if(pCertContext!=NULL)
	{
	   pbData = (TCHAR*)malloc(pCertContext->cbCertEncoded*2+1);

	       if (pbData != NULL)
		   {   
			BinHex((char *)pCertContext->pbCertEncoded,pCertContext->cbCertEncoded,(char *)pbData);		
			*Certificate=_bstr_t((LPCTSTR)pbData).copy();
			gdwSelectedCertNumber=selectedCert;
			free(pbData);
		   }
		pCertContext=NULL;
	}
    //MessageBox(NULL,"Step10","SignDebug",0);

	return S_OK;
}
////////////////////////////////////////////////////////////////////////////////////
//The Signing Method
//Sign.getSignedHash
// in:
//		sHash -- hash string
//		selectedCert -- reserved (if 0 cleans the KeyContainerName cache)
// out:
//		signed hash in hex form
////Additionaly change values of attributes corresponding with signing cert (SigningKeyContainerName, SigningCSPName, SigningCertName, SigningCertIssuer)
////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSign::getSignedHash(BSTR sHash, long selectedCert, BSTR *SignedHash)
////////////////////////////////////////////////////////////////////////////////////
{
	
	PCCERT_CONTEXT pCertContext;
	BYTE pbHash[20];
	//char * sHashHex;
	char sHashHex[41];
	DWORD dwSelectedCert;

	DWORD dwSignature=1024; //RSA signatuuri pikkus - this parameter specifies the max size of signature value buffer         
	BYTE cSignature[1025]; //the buffer receiving signature value
 	TCHAR pbhSignature[2049]; //2*256 + 1 for null-terminating char

	BOOL fResult, fFreeProv;

	HCRYPTPROV hProv = NULL;
    HCRYPTKEY hPubKey = NULL; 
	DWORD dwKeySpec;
	DWORD dwResult;
	char pSignatureRev[1025]; //the buffer for reversed signature value
	int i; 

	memset(cSignature,0,1025);
	memset(pbhSignature,0,2049);
	memset(pSignatureRev,0,1025);
	memset(sHashHex,0,41);
	
	if(sHash=='\0')
       goto SIGDONE;

      for(i=0; i< 40; i++)
	  {
		sHashHex[i]=(TCHAR)sHash[i];
		if(sHashHex[i]==0) break;
	  }
    if(strlen(sHashHex)!=40)
	   goto SIGDONE;


	HexBin(sHashHex,(char *)pbHash,20);

	dwSelectedCert=selectedCert;
	pCertContext=DigiCrypt_FindContext(0, &dwResult, &dwSelectedCert);

    if(!pCertContext)
		goto SIGDONE;

    fResult = GetRSAKeyFromCert(pCertContext,&hProv,&hPubKey, &dwKeySpec,&fFreeProv);
	if(fResult==NULL) 
		goto SIGDONE;

	fResult = SignHashString(hProv,hPubKey,dwKeySpec,pbHash,cSignature,&dwSignature/*this parameter will be updated with the actual value of the signature*/);
	if(fResult==NULL) 
		goto SIGDONE;

	BinHex((char*)pbHash,20,sHashHex);

	for(i=dwSignature-1;i>=0;i--) //allowed indexes for an array of size N are from 0 to N-1, this is fixed now
	{
		pSignatureRev[dwSignature-1-i]=cSignature[i];
	}
	pSignatureRev[dwSignature]=0;


	BinHex((char *)pSignatureRev, dwSignature, pbhSignature);

    *SignedHash=_bstr_t((LPCTSTR)pbhSignature).copy();

SIGDONE:
   if(fFreeProv)
   {
	   if(hPubKey)
	   {
			CryptDestroyKey(hPubKey);
	   }
	   if(hProv)
			CryptReleaseContext(hProv, 0);
   }
	   pCertContext=NULL;

	return S_OK;
}
////////////////////////////////////////////////////////////////////////

STDMETHODIMP CSign::get_selectedCertNumber(long *pVal)
{
	// TODO: Add your implementation code 

	*pVal=gdwSelectedCertNumber;

	return S_OK;
}

STDMETHODIMP CSign::put_selectedCertNumber(long newVal)
{
	// TODO: Add your implementation code here

	gdwSelectedCertNumber=newVal;

	return S_OK;
}
