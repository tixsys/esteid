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
	void DigiOpen();
	void DigiInit();
	int ret;
};
