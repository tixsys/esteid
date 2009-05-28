// MyDdocBridge.h
// Korralikud HEADERI define-d
// Siin failis EI TOHI OLLA MITTE YHTEGI BDOCi ega TAVALISE STLi andmetüüpi.
// Kuna OOo-l on oma samm kampunn STL, siis muidu on kohe symbolite ja prototyypidega ikaldus.

class MyBdocBridge {
protected:
	MyBdocBridge();
public:
	static MyBdocBridge * getInstance();
	void teemingilollus1();
};
