// testac.cpp : Defines the entry point for the console application.
//

#include "precompiled.h"
#include <windows.h>
#include <wincrypt.h>
#ifdef ATL
#include <atlbase.h>
#include <atlcrypt.h>
#include <atlstr.h>
#endif
#include "cryptContext.h"
#include "helpers.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>

#pragma comment(lib,"crypt32")

#define BASECSP L"EstEID NewCard CSP"
//#define BASECSP L"Microsoft Base Smart Card Crypto Provider"
//#define BASECSP L"EstEID Card CSP"
#define _ENC_ (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING)
#define BENH L"Microsoft Enhanced Cryptographic Provider v1.0"

BYTE signSSL[] = {
	 0xDA,0x53,0x0C,0x8C,0x46,0xe5,0x75,0x4b
	,0x41,0x30,0x91,0x9f,0x13,0x1a,0x77,0xfd
	,0x65,0x66,0x5e,0xe2,0xDA,0x53,0x0C,0x8C
	,0x46,0xe5,0x75,0x4b,0x41,0x30,0x91,0x9f
	,0x13,0x1a,0x77,0xfd
	};
BYTE signSHA1[] = {
	 0xDA,0x53,0x0C,0x8C,0x46,0xe5,0x75,0x4b
	,0x41,0x30,0x91,0x9f,0x13,0x1a,0x77,0xfd
	,0x65,0x66,0x5e,0xe2
	};
BYTE signMD5[] = {
	 0xDA,0x53,0x0C,0x8C,0x46,0xe5,0x75,0x4b
	,0x41,0x30,0x91,0x9f,0x13,0x1a,0x77,0xfd
	};

class store {
	HCERTSTORE hStore;
public:
	store(const TCHAR *sname) {
		hStore = CertOpenStore(sz_CERT_STORE_PROV_SYSTEM,X509_ASN_ENCODING,
			NULL,CERT_SYSTEM_STORE_CURRENT_USER,sname);
		if (!hStore) throw std::exception("no store");
		}
	~store() {
		CertCloseStore(hStore,0); 
		}
	operator const HCERTSTORE() const {return hStore;}
};


typedef vector<string> stringvec;

bool testsign(ALG_ID algo = CALG_SSL3_SHAMD5,DWORD flags = 0,DWORD cont=0,
			  wstring overrideContainer = L"") {
	wstring container;
	try {
		DWORD keySpec = cont == 0 ? AT_KEYEXCHANGE : AT_SIGNATURE ;
		wstring container(L"");
		if (overrideContainer == L"") {
			stringvec conts = cryptContext(BASECSP,overrideContainer,0).getContainers();
			for (stringvec::iterator s = conts.begin();s != conts.end(); s++ ) 
				cout << *s << endl;
			container = StrToWstr(conts[cont]);
		} else
			container = overrideContainer;
		cryptContext csp(BASECSP, container, 0);
		cryptHash hash = csp.createHash(algo);
		byteVec input;
		cout << "algo:";
		switch (algo) {
			case CALG_SSL3_SHAMD5:
				input = MAKEVECTOR(signSSL); cout << "SSL"; break;
			case CALG_SHA1:
				input = MAKEVECTOR(signSHA1); cout << "SHA1"; break;
			case CALG_MD5:
				input = MAKEVECTOR(signMD5); cout << "MD5"; break;
			default :
				throw runtime_error("what algo ??");
		};
		cout << " container:" << cont << " flags:" << flags << endl;
		byteVec sign = hash.sign(input, keySpec ,flags);
		byteVec pub = csp.ExportPub(keySpec);

		cryptContext verCsp(BENH,L"");
		HCRYPTKEY pbKey = verCsp.ImportPub(pub);
		cryptHash verifyH = verCsp.createHash(algo);
		verifyH.setValue(input);
		DWORD data,sz = sizeof(data);
		CryptGetKeyParam(pbKey,KP_BLOCKLEN,(LPBYTE)&data,&sz,0);
		if (verifyH.verify(sign,pbKey,flags)) 
			cout << "signature OK" << endl;
		else 
			cout << "signature FAILED" << endl;
		}
	catch (cryptError e) {
		cout << "cryptError " << e.getDesc() << endl;
		return false;
		}
	catch (runtime_error e) {
		cout << "runtime_error " << e.what() << endl;
		return false;
		}
	return true;
	}

bool allSign() {
	bool retcode = true;
	retcode &= testsign(CALG_MD5,0,0);
	retcode &= testsign(CALG_SHA1,0,0);
	retcode &= testsign(CALG_SSL3_SHAMD5,0,0);
	retcode &= testsign(CALG_MD5,CRYPT_NOHASHOID,0);
	retcode &= testsign(CALG_SHA1,CRYPT_NOHASHOID,0);

	retcode &= testsign(CALG_MD5,0,1);
	retcode &= testsign(CALG_SHA1,0,1);
	return true;
	}

bool testEncDec() {
	try {
		BYTE testVec[] = {1,2,3};
		byteVec data(MAKEVECTOR(testVec));
		byteVec test;

		cryptContext *ct = NULL;

		try {
			ct = new cryptContext(MS_ENHANCED_PROV_W,L"",0);
		} catch (cryptError e) {
			cout << "no default key" << endl;
			if (e.errCode != NTE_BAD_KEYSET) throw e;
			}
		if (!ct) {
			cout << "try to create default key" << endl;
			ct = new cryptContext(MS_ENHANCED_PROV_W,L"",CRYPT_NEWKEYSET);
		}
		cout << "enhanced context acquired" << endl;

		cryptContext a(BASECSP,L"",0);
		byteVec tmpKey = a.ExportPub(AT_KEYEXCHANGE);
		cout << "key exported" << endl;

		HCRYPTKEY k = ct->ImportPub(tmpKey);
		cout << "key reimported" << endl;
		byteVec encr = ct->RSAEncrypt(data,k);
		delete ct;
		cout << "encrypted" << endl;
		test = a.RSADecrypt(encr,AT_KEYEXCHANGE);
		cout << "decrypted" << endl;
		if (data != test ) 
			throw runtime_error("input doesnt match output");
		cout << "all ok" << endl;
		}
	catch (cryptError e) {
		cout << "cryptError " << e.getDesc() << endl;
		return false;
		}
	catch (runtime_error e) {
		cout << "runtime_error " << e.what() << endl;
		return false;
		}

	return true;
	}

bool regCert(int which) {
	wstring doh;
	try {

		{
		cryptContext a(BASECSP,L"",0);
		stringvec conts = a.getContainers();
		doh = StrToWstr(conts[which]);
		}

		cryptContext a(BASECSP,doh);
		DWORD keySpec = which == 0 ? AT_KEYEXCHANGE : AT_SIGNATURE ;
		byteVec cert = a.ExportCert(keySpec);

		HCERTSTORE hCertStore = CertOpenStore(CERT_STORE_PROV_SYSTEM_A,_ENC_,
								 0,CERT_SYSTEM_STORE_CURRENT_USER,"MY");
		if (hCertStore  == NULL) return FALSE;

		PCCERT_CONTEXT pCertContext;
		BOOL fResult = CertAddEncodedCertificateToStore(hCertStore,
												_ENC_,&cert[0],(DWORD)cert.size(),
												CERT_STORE_ADD_REPLACE_EXISTING,
												&pCertContext);
		if (fResult != TRUE) return FALSE;

		CRYPT_KEY_PROV_INFO KeyProvInfo;
		ZeroMemory(&KeyProvInfo, sizeof(KeyProvInfo));
		KeyProvInfo.pwszProvName = BASECSP;
		LPCWSTR x = doh.c_str();
		KeyProvInfo.pwszContainerName = (LPWSTR)x;
		KeyProvInfo.dwKeySpec = keySpec;
		KeyProvInfo.dwProvType = PROV_RSA_FULL;
		fResult = CertSetCertificateContextProperty(pCertContext,
													CERT_KEY_PROV_INFO_PROP_ID,
													0,
													&KeyProvInfo);
		if (fResult != TRUE) return FALSE;
	}
	catch (cryptError e) {
		cout << "cryptError " << e.getDesc() << endl;
		return false;
		}
	catch (runtime_error e) {
		cout << "runtime_error " << e.what() << endl;
		return false;
		}

	return true;
}

#include <winscard.h>
#pragma comment(lib,"winscard")
#define SCAPI __stdcall

bool enumcont() {
	try {
		cryptContext a(BASECSP,L"",CRYPT_VERIFYCONTEXT);
		stringvec conts = a.getContainers();
		for (unsigned i=0;i < conts.size();i++ ) {
			cout << "container " << i << ": " << conts[i] << endl;
			}
		}
	catch (cryptError e) {
		cout << "cryptError " << e.getDesc() << endl;
		return false;
		}
	catch (runtime_error e) {
		cout << "runtime_error " << e.what() << endl;
		return false;
		}
	return true;
}

class cert {
	PCCERT_CONTEXT ctx;
	bool m_own;
public:
	operator const PCCERT_CONTEXT() const {return ctx;}
	cert(const std::vector<BYTE> &dataBlob) : m_own(true) {
		ctx= CertCreateCertificateContext(_ENC_,&dataBlob[0],(DWORD)dataBlob.size());
		}
	cert(PCCERT_CONTEXT &refBy) : m_own(false),ctx(refBy) {}
	cert(store& store,const TCHAR *byName) : m_own(true) {
		ctx = CertFindCertificateInStore(store,X509_ASN_ENCODING,0,CERT_FIND_SUBJECT_STR,
			byName,NULL);
		if (!ctx) throw std::runtime_error("cert (name) not found");
		}
	cert(store &store,cert &refBy) : m_own(true) {
		ctx = CertFindCertificateInStore(store, _ENC_ , 
									0, CERT_FIND_EXISTING, refBy, NULL); 
		if (!ctx) throw std::runtime_error("cert (copy) not found in store");
		}
	~cert() {
		if (m_own) CertFreeCertificateContext(ctx);
		}
	template<class T> 
	void doGetProp(T &refVal,DWORD propId,DWORD size = 0) {
		DWORD sz = sizeof(refVal);
		if (size) sz = size;
		cryptError::check(CertGetCertificateContextProperty(ctx,propId,&refVal,&sz));
		}
};

bool testMultiple(ALG_ID algo = CALG_SSL3_SHAMD5,DWORD flags = 0,DWORD cont=0) {
	try {
		store mStore(_T("MY"));
		PCCERT_CONTEXT ctx = NULL;
		while(ctx= CertEnumCertificatesInStore(mStore,ctx)) {
			BYTE *buffer = new BYTE[sizeof(CRYPT_KEY_PROV_INFO) * 8];
			cert mCert(ctx);
			CRYPT_KEY_PROV_INFO *provInfo = (CRYPT_KEY_PROV_INFO *)buffer;
			mCert.doGetProp(*provInfo,CERT_KEY_PROV_INFO_PROP_ID,sizeof(*provInfo) * 8);
			if (provInfo->pwszProvName == std::wstring(BASECSP)) {
				std::wstring container(provInfo->pwszContainerName);
				wcout << "BaseCSP cert, container:" << 
					container << " keyspec:" << provInfo->dwKeySpec << endl;
				if (provInfo->dwKeySpec == AT_SIGNATURE) {
					wcout << "testing SHA1 sign .." << endl;
					testsign(CALG_SHA1,flags,1,container);
					}

				}
			} 
	} catch (cryptError e) {
		cout << "cryptError " << e.getDesc() << endl;
		return false;
	} catch (runtime_error e) {
		cout << "runtime_error " << e.what() << endl;
		return false;
		}
	return true;
	}

void readFile(std::string fn, byteVec &data) {
	std::ifstream strm;
	strm.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	strm.open(fn.c_str(),std::ios::in|std::ios::binary|std::ios::ate);
	data.resize(strm.tellg());
	strm.seekg(0,std::ios::beg);
	strm.read((char *)&data[0],(std::streamsize) data.size());
	}

bool testFileDec() {
	try {
		store myStore(_T("MY"));
		byteVec certBlob,keyBlob;
		readFile("cert.cer",certBlob);
		readFile("key.dat",keyBlob);

		cert pCertFindContext(certBlob); //cert from file
		cert pCertContext(myStore,pCertFindContext); //cert from store

		HCRYPTPROV hProv;
		DWORD keySpec;
		BOOL mustFree;
		cryptError::check(CryptAcquireCertificatePrivateKey(pCertContext,
									NULL,NULL,&hProv,&keySpec, &mustFree),"get private key");

		HCRYPTKEY hPrivKey;
		cryptError::check(CryptGetUserKey(hProv, keySpec, &hPrivKey),"get user key");

		DWORD keyLen = (DWORD)keyBlob.size();
		cryptError::check(CryptDecrypt(hPrivKey, 0, TRUE, 0, &keyBlob[0], &keyLen),"decrypt");

	} catch(std::exception &err) {
		std::cout << "exception : " << err.what() << std::endl;
		return false;
		}
	return true;
	}

int doTest(string test,bool value) {
	cout << test;
	if (value) 
		cout << "ok";
	else 
		cout << "failed";
	cout << endl;
	return 0;
	}

#define CSPNAME BASECSP
struct HR { //wrapper class to throw exceptions on failed HR calls
     HR(HRESULT hr) { if (FAILED(hr)) throw hr; }
     HR operator = (HRESULT hr) { if (FAILED(hr)) throw hr; return hr; }
};

#ifdef ATL
void get_CSPDiagnostics(CStringA &result) {
	try {
		result+= "doing CSP diagnosis .. \r\n";
		DWORD certLen = 0,contLen = 200;
		CCryptProv scProv;
		HR(scProv.Initialize(PROV_RSA_FULL,NULL,CSPNAME));
		result+="initialized..\r\n";
		CAtlStringA contName;
		HR(scProv.GetContainer(CStrBufA(contName,contLen),&contLen));
		result+= "CSP Context acquired, name '" + contName + "'\r\n";
		CCryptUserExKey authKey;
		HR(authKey.Initialize(scProv));
		result+="authKey acquired..\r\n";
		HR(authKey.GetParam(KP_CERTIFICATE,NULL			,&certLen));
		CHeapPtr<BYTE> certificate;
		certificate.Allocate(certLen);
		HR(authKey.GetParam(KP_CERTIFICATE,certificate	,&certLen));
		contName.Format("Certificate read, len %d",certLen);
		result+= contName + "\r\n";
	} catch (HRESULT &exception) {
		CStringA desc;
		desc.Format("HR exception, 0x%08X",exception);
		result+= desc + "\r\n";
		}
	}
#endif
#ifdef UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc !=2 ) {
		cout << "use with param \n\t"
				"enumcont - enumerate containers\n\t"
				"regcert - reg auth cert\n\t"
				"regsigncert - reg signature cert\n\t"
				"testdec - test decryption\n\t"
				"testsign - test ssl signing\n\t"
				"nonrepu - test sha hash with signing key\n\t"
				"testvpn - test sha has, signing key, NOHASHOID ( vpn )\n\t"
#ifdef ATL
				"diag - diag run, get certificates through CSP\n\t"
#endif
				"allsigningtests - all sign tests\n\t"
				"testmultiple - test SSL signing with multiple cards\n\t"
				"decfile - decrypt key from file ( cert.cer and key.dat )";
		return 1;
		}
	tstring param = argv[1];
	if (param == _T("enumcont")) { return doTest("enumcont ",enumcont()); }
	if (param == _T("regcert")) { return doTest("regcert ",regCert(0)); }
	if (param == _T("regsigncert")) { return doTest("regsigncert ",regCert(1)); }
	if (param == _T("testdec") ) {return doTest("RSA encrypt/decrypt test ",testEncDec()); }
	if (param == _T("testsign")) {return doTest("SSL signing test ",testsign()); }
	if (param == _T("testvpn")) {return doTest("SHA1 signing test with CRYPT_NOHASHOID ",
			testsign(CALG_SHA1,CRYPT_NOHASHOID)); }
	if (param == _T("nonrepu") ) {return doTest("non-repu key signing test ",
			testsign(CALG_SHA1,0,1));}
#ifdef ATL
	if (doh == L"diag") {
		CStringA result;
		get_CSPDiagnostics(result);
		char * pp = result.GetBuffer() ;
		cout << pp << endl;
		return 0;
		}
#endif
	if (param == _T("allsigningtests")) {
		return doTest("all signing tests:",allSign());
		}
	if (param == _T("testmultiple")) {
		return doTest("SSL signing with multiple cards ",
			testMultiple());
		}
	if (param == _T("decfile")) {
		return doTest("decrypt data from file ",
			testFileDec());
		}

	cout << "wrong param";
	return 1;
}

