#if !defined(CRYPTCONTEXT_H_INCLUDED)
#define CRYPTCONTEXT_H_INCLUDED

#include <windows.h>
#include <wincrypt.h>
#include <stdexcept>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <strsafe.h>


#define LENOF(a) (sizeof(a)/sizeof(*(a)))
#define MAKEVECTOR(a) byteVec(a,a + LENOF(a) ) 

using std::string;

class cryptError: public std::runtime_error {
public:
	DWORD errCode;
	std::string desc;
	cryptError(DWORD code,const char *op): errCode(code), runtime_error("CryptoApiError") {
		std::ostringstream buf;
		buf << "exception: op:'"  << op << "' "
				" code:'0x" <<std::hex << std::setfill('0') <<
				std::setw(8) << code << "'";
		desc = buf.str();
		}
	static void check(BOOL err,const char *op="") { 
		if (err != TRUE)
			throw cryptError(GetLastError(),op); 
		}
	virtual const char * what() const throw() {	return desc.c_str();} 
	std::string getDesc() {
		char buf[1000];
		StringCbPrintfA(buf,sizeof(buf),
			" exception:'%s' code:'0x%08X'",
			runtime_error::what(),errCode);
/*		sprintf(buf," exception:'%s' code:'0x%08X'",
			runtime_error::what(),errCode);*/
		return buf;
		}
};

typedef	std::vector<BYTE> byteVec;

class cryptHash
{
	HCRYPTHASH hHash;
public:
	cryptHash(HCRYPTPROV hProv,ALG_ID alg=CALG_SHA1) {
		cryptError::check(CryptCreateHash(hProv,alg,0,0,&hHash));
		}
	~cryptHash(void) {	CryptDestroyHash(hHash);}

	bool hashData(byteVec input) {
		cryptError::check(CryptHashData(hHash,&input[0],(DWORD) input.size(),0));
		}

	bool verify(byteVec input,HCRYPTKEY pub,DWORD flags) {
		cryptError::check(CryptVerifySignature(
			hHash,&input[0],(DWORD)input.size(),pub,NULL,flags));
		return true;
		}

	byteVec calculate(byteVec input) {
		cryptError::check(CryptHashData(hHash,
			&input[0], (DWORD) input.size() ,0));
		DWORD hSize = 0;
		cryptError::check(CryptGetHashParam(hHash ,HP_HASHVAL,NULL,&hSize,0));
		byteVec retVal(hSize);
		cryptError::check(CryptGetHashParam(hHash,HP_HASHVAL,&retVal[0],&hSize,0));
		return retVal;
		}

	byteVec sign(DWORD keyspec = AT_KEYEXCHANGE,DWORD flags=0) {
		byteVec retVal(1024 / 8);
		DWORD hSize = (DWORD) retVal.size();
		cryptError::check(CryptSignHash(hHash,keyspec,NULL,flags,&retVal[0],&hSize));
		if (hSize > retVal.size() ) {
			retVal.reserve(hSize);
			cryptError::check(CryptSignHash(hHash,keyspec,NULL,flags,&retVal[0],&hSize));
			}
		return retVal;
		}

	void setValue(byteVec inHash) {
		cryptError::check(CryptSetHashParam(hHash,HP_HASHVAL,&inHash[0],0));
		}

	byteVec sign(byteVec inHash,DWORD keyspec = AT_KEYEXCHANGE,DWORD flags=0) {
		cryptError::check(CryptSetHashParam(hHash,HP_HASHVAL,&inHash[0],0));
		return sign(keyspec,flags);//NTE_BAD_HASH
		}

};

class cryptContext;

class cryptKey {
public:
	cryptKey(HCRYPTPROV hProv,DWORD keySpec = AT_KEYEXCHANGE) {
		cryptError::check(CryptGetUserKey(hProv,keySpec,&hKey));
		}
	cryptKey(HCRYPTKEY k):hKey(k) {};
	DWORD getLen() {
		DWORD dwKeyLen,dwSize= sizeof(DWORD);
		cryptError::check(CryptGetKeyParam(hKey,KP_KEYLEN,(LPBYTE)&dwKeyLen,&dwSize,0));
		return dwKeyLen;
		}
	~cryptKey() { CryptDestroyKey(hKey); }
	friend class cryptContext;
protected:
	HCRYPTKEY hKey;
private:
	cryptKey();
	};

class cryptContext
{
	HCRYPTPROV hProv;
	void init();
public:
	cryptContext(std::wstring provider = L"",std::wstring container=L"",DWORD flags= CRYPT_SILENT );
	cryptContext(HCRYPTPROV p):hProv(p) {}
	~cryptContext(void);
	cryptHash createHash(ALG_ID alg=CALG_SHA1) {return cryptHash(hProv,alg);}
	std::string getContainer();
	std::vector<std::string> getContainers();
	byteVec RSAEncrypt(byteVec input,HCRYPTKEY userKey);
	byteVec RSADecrypt(byteVec input,DWORD keySpec);
	byteVec ExportPub(DWORD keySpec = AT_KEYEXCHANGE);
	byteVec ExportCert(DWORD keySpec = AT_KEYEXCHANGE);
	HCRYPTKEY ImportPub(byteVec inKey);
};

#endif