/************************************************************************
* Created By Mark Erlich, Smartlink OÜ
* 2009
*************************************************************************/


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
	void DigiSign();
	void DigiOpen(const char* pPath);
	void DigiInit();
	void DigiCheckCert();
	void Terminate();

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
	const char* pPath;
	const char* pParam;
	char c_Pin[13]; //max length for pin2 is 12
	char* pSerialNr;

	const char* pcErrMsg;
	struct {
		const char* pcEMsg;
	} eMessages [20];

	int iCounter;
	volatile int iPinReq;
    const char * locErrMess;
};
