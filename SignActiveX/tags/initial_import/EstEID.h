// EstEID.h : Declaration of the CEstEID

#ifndef __ESTEID_H_
#define __ESTEID_H_
      // main symbols

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Winscard.h>
#include <atlctl.h>
#include "resource.h" 

#define PCSC 1
#define CTAPI 2
#define MUEHLBAUER 3

#define MF 1
#define EEEE 2

#define AUTH_CE 1
#define SIGN_CE 2
#define AUTH_KEY 3
#define SIGN_KEY 4
#define AUTH_KEY1 31
#define SIGN_KEY1 41
#define UNWRAP 5
#define UNWRAP_AUTH 51
#define UNWRAP_SIGN 52
#define CHALLENGE_DECRYPTION 6
#define CA_CE 7
#define TEST_CE 8
#define EF_PD 0x5044
#define EF_Pwd 0x12
#define EF_PwdC 0x16
#define EF_Key 0x10
#define EF_KeyD 0x13
#define EF_Rule 0x30
#define EF_SE 0x33

#define PUB_KEY1_FILE 101
#define PUB_KEY2_FILE 102
#define SIGN_KEY1_FILE 103
#define SIGN_KEY2_FILE 104
#define AUTH_KEY1_FILE 105
#define AUTH_KEY2_FILE 106

#define CMK1 1
#define CMK2a 2
#define CMK2b 3
#define CMK3 4

#define HASH_SONUM 0
#define HASH_BCDHASH 1

#define HASH_ON_CARD 0
#define HASH_IN_COMPUTER 1

#define CBC 0
#define CFB 1

#define SM_PRO 1
#define SM_ENC 2
#define SM_PRO_ENC 3

#define KEY_VERSION_CURRENT 0
#define KEY_VERSION_PREVIOUS 1


/////////////////////////////////////////////////////////////////////////////
// CEstEID
class ATL_NO_VTABLE CEstEID : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CEstEID, &CLSID_EstEID>,
	public ISupportErrorInfo,
	public IObjectSafetyImpl<CEstEID, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
	public IConnectionPointContainerImpl<CEstEID>,
	public IDispatchImpl<IEstEID, &IID_IEstEID, &LIBID_EIDCARDLib>
{
public:
	CEstEID()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ESTEID)
DECLARE_NOT_AGGREGATABLE(CEstEID)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CEstEID)
	COM_INTERFACE_ENTRY(IEstEID)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IObjectSafety)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()
BEGIN_CONNECTION_POINT_MAP(CEstEID)
END_CONNECTION_POINT_MAP()


// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IEstEID
public:
	STDMETHOD(getCardNo)(/*[out, retval]*/ BSTR *sCardNo);
	STDMETHOD(getInfo)(/*[out, retval]*/ BSTR *sInfo);
};

class EstEID
{
public:
	int SetCrossMF_EEEE_SE(int MF_SE_No, int EEEE_SE_No, int FID);
	int SelectDF( int FID );
	int GetCardChallenge(char* CardChallenge);
	int ReadRecord(int DirFID,int FileFID,int RecNo, char* Data, int* RecLen);
	int SelectFile(int DirFID, int FileFID);
	unsigned short int ctn;
	int Reader;
	char SelectedReader[200];
	int ComPort;
	int GlobalInterfaceType;
	int CloseCard();
	SCARDHANDLE hCardHandle;
	SCARDCONTEXT hSC;
	int OpenCard();
	EstEID(int InterfaceType);
	virtual ~EstEID();
	int SetSE(int SE_No);
	int VerifyPIN(int PIN_No, char* PIN);
	int SelectEF(int);
	int APDUCommand( char *DataIn,
						 unsigned char DataInLen,
						 char *DataOut,
						 unsigned char *OutLen);
	int SelectMasterFile();
private:


};
#endif //__ESTEID_H_


