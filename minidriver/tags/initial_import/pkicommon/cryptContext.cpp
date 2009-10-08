#include "StdAfx.h"
#include "cryptContext.h"

#define MAXCONTAINER 200

using std::wstring;
using std::string;
using std::vector;

cryptContext::cryptContext(wstring provider,wstring container,DWORD flags) {
	LPCWSTR provName =  provider.length() ? provider.c_str() : NULL;
	LPCWSTR contName =  container.length() ? container.c_str() : NULL;

	cryptError::check( 
		CryptAcquireContext(&hProv, contName ,provName  , PROV_RSA_FULL , flags)
		);


/*	DWORD keySpec;
	DWORD dwSize = sizeof(keySpec);
	cryptError::check(CryptGetProvParam(hProv,PP_KEYSPEC,(LPBYTE)&keySpec,&dwSize,0));
	string duh;

	if (keySpec == AT_KEYEXCHANGE) {
		duh += "|keyexchange";
		}
	if (keySpec == AT_SIGNATURE) {
		duh += "|signature";
		}
	HCRYPTKEY userKey;
	cryptError::check(CryptGetUserKey(hProv,keySpec,&userKey));
*/}

cryptContext::~cryptContext(void){
	CryptReleaseContext(hProv,0);
	}

string cryptContext::getContainer() {
	DWORD len=MAXCONTAINER;
	string retVal(len,'\0');
	cryptError::check( CryptGetProvParam(hProv,PP_CONTAINER,(LPBYTE)&retVal[0],&len,0));
	retVal.resize(len - 1);
	return retVal;
	}

vector<string> cryptContext::getContainers() {
	vector<string> retVal(0);

	DWORD flag = CRYPT_FIRST;
	DWORD len = MAXCONTAINER ;
	string data(len,'\0');

	while(CryptGetProvParam(hProv , PP_ENUMCONTAINERS,(LPBYTE) &data[0],&len, flag)) {
		data.resize(len - 1);
		retVal.push_back( ( data )  );
		len = MAXCONTAINER;
		data.resize(MAXCONTAINER);
		flag = 0;
		}

	return retVal;
	} 

byteVec cryptContext::RSAEncrypt(byteVec input,HCRYPTKEY userKey) {
	DWORD dwKeyLen,dwSize = sizeof(dwKeyLen);
	cryptError::check(CryptGetKeyParam(userKey,KP_KEYLEN,(LPBYTE)&dwKeyLen,&dwSize,0));

	LPBYTE cryptBuf = new BYTE[dwKeyLen / 8]; 
	CopyMemory(cryptBuf,&input[0],input.size());
	dwSize = (DWORD) input.size();
	cryptError::check(CryptEncrypt(userKey,0,TRUE,0, cryptBuf,&dwSize,dwKeyLen / 8));
	byteVec retVal(cryptBuf,cryptBuf+dwSize);
	delete [] cryptBuf;
	return retVal;
}
byteVec cryptContext::RSADecrypt(byteVec input,DWORD keySpec) {
	HCRYPTKEY userKey;
	cryptKey user(hProv,keySpec);

	cryptError::check(CryptGetUserKey(hProv,keySpec,&userKey));

	LPBYTE cryptBuf = new BYTE[ user.getLen() / 8]; 
	CopyMemory(cryptBuf,&input[0],input.size());
	DWORD dwSize = (DWORD) input.size();
	cryptError::check(CryptDecrypt(user.hKey,0,TRUE,0,cryptBuf,&dwSize));
	byteVec retVal(cryptBuf,cryptBuf+dwSize);
	delete [] cryptBuf;
	return retVal;
	}

byteVec cryptContext::ExportPub(DWORD keySpec) {
	byteVec retVal(1024);
	HCRYPTKEY userKey;
	cryptError::check(CryptGetUserKey(hProv,keySpec,&userKey));
	DWORD dwSize = (DWORD) retVal.size();
	cryptError::check(CryptExportKey(userKey,0,PUBLICKEYBLOB,0,&retVal[0],&dwSize));
	retVal.resize(dwSize);
	cryptError::check(CryptDestroyKey(userKey));
	return retVal;
	}

byteVec cryptContext::ExportCert(DWORD keySpec) {
	byteVec retVal(16384);

	HCRYPTKEY userKey;
	cryptError::check(CryptGetUserKey(hProv,keySpec,&userKey));
	DWORD dwSize = (DWORD) retVal.size();
	cryptError::check(CryptGetKeyParam(userKey,KP_CERTIFICATE,&retVal[0],&dwSize,0));
	retVal.resize(dwSize);
	cryptError::check(CryptDestroyKey(userKey));
	return retVal;
	}


HCRYPTKEY cryptContext::ImportPub(byteVec inKey) {
	HCRYPTKEY Key;
	cryptError::check(CryptImportKey(hProv,&inKey[0],(DWORD) inKey.size(),0,0,&Key));
	return Key;
	}
