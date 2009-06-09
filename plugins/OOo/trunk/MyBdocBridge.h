// MyDdocBridge.h
// Korralikud HEADERI define-d
// -------------------------------------------------------------------------------------------------
// -- Comments from Antix:									  --
// -- Siin failis EI TOHI OLLA MITTE YHTEGI BDOCi ega TAVALISE STLi andmetüüpi.			  --
// -- Kuna OOo-l on oma samm kampunn STL, siis muidu on kohe symbolite ja prototyypidega ikaldus. --
// -------------------------------------------------------------------------------------------------
	
class MyBdocBridge {
protected:
	MyBdocBridge();
public:
	static MyBdocBridge * getInstance();
	void teemingilollus1();
	void DigiSign(char* pPath, char* pParam, char* pPin);
	void DigiOpen(char* pPath);
	void DigiInit();
	void DigiCheckCert();
	int ret;
	int iSignCnt;
	const char* pRetPath;
	const char* pSignName;
	const char* pSignTime;
	const char* pSignCity;
	const char* pSignState;
	const char* pSignPostal;
	const char* pSignCountry;
	const char* pSignRole;
	const char* pSignAddRole;
};
