// EstEID.cpp : Implementation of CEstEID
#include "stdafx.h"
#include "EIDCard.h"
#include "EstEID.h"

#include <Winscard.h>

#include <comdef.h>
#define SCARD_PROTOCOL_TYPE SCARD_PROTOCOL_T0
#define SCARD_PCI_TYPE SCARD_PCI_T0
#define sModuleInfoText "EstCARD EstEID class v1.0, 03.02.2003"

///////////////////////////////////////////////////////////////////////////
//EstEID related
///////////////////////////////////////////////////////////////////////////
EstEID::EstEID(int InterfaceType)
///////////////////////////////////////////////////////////////////
{
	this->GlobalInterfaceType = InterfaceType;
	strcpy( this->SelectedReader, "NO Reader");
	return;
}
///////////////////////////////////////////////////////////////////
EstEID::~EstEID()
///////////////////////////////////////////////////////////////////
{
return;
}
///////////////////////////////////////////////////////////////////
int EstEID::OpenCard()
///////////////////////////////////////////////////////////////////
{
	LONG            lReturn, i, rdrCount, lReturn1;
	DWORD           dwAP;
	//extern int OpenInsertCardWindow();
	//void PutLog1 ( char* Message );

	
	if ( this->GlobalInterfaceType == PCSC )
	{

		unsigned long cchReaders;
		char mszReaders[1000], *currStr;
		SCARD_READERSTATE rgscState[20];
		TCHAR szCard[1000];
		char ATR[20];
		void Translate(char *Src, char *Dest);

		Translate("0068D276000028FF051E3180009000",ATR);


		lReturn = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hSC);
		if ( lReturn )	return lReturn;
 
		char pmszCards[2000];
		DWORD cchCards;
		cchCards = 2000;
		lReturn = SCardListCards(NULL, NULL, NULL, NULL, pmszCards, &cchCards );
		if ( lReturn )
		{
			lReturn1 = SCardReleaseContext(hSC);
			return lReturn;
		}

		currStr = pmszCards;

/*		int AlreadyInserted = 0;
		while ( (pmszCards + cchCards) > (currStr + 1) )
		{
			if ( strlen( (const char *) currStr ) == 0 ) break;
			if ( ! strcmp( (const char *) currStr, "EstEID" ) )
			{
				AlreadyInserted = 1;
				break;
			}
			currStr = &currStr[strlen((const char *) currStr) + 1];
		}
		*/
//kui süsteem ei tunne sihukest kaarti, siis püüame tutvustada
/*		if ( ! AlreadyInserted )
		{
			GUID  MyGuid = { 0xABCDEF00,0xABCD, 0xABCD, 0xAA, 0xBB, 0xCC, 0xDD, 0xAA, 0xBB, 0xCC, 0xDD };

			BYTE MyATR[0x1A];
			Translate("3BFE9400FF80B1FA451F034573744549442076657220312E3043", (char *)MyATR);
			BYTE MyATRMask[0x1A];
			memset((void *) MyATRMask, 0xFF, 0x1A );

			lReturn = SCardIntroduceCardType(hSC, "EstEID", &MyGuid, NULL, 0, MyATR, MyATRMask, sizeof(MyATR));
			if ( lReturn )	return lReturn;
		}*/

		cchReaders = 999;
		lReturn = SCardListReaders( NULL, NULL, mszReaders, &cchReaders);
		if ( lReturn )
		{
			lReturn1 = SCardReleaseContext(hSC);
			return lReturn;
		}

		currStr = mszReaders;
		lstrcpy( szCard, TEXT("EstEID National ID Card"));
		szCard[lstrlen(szCard) + 1] = 0;

		rdrCount = 0;
		while ( (mszReaders + cchReaders) > (currStr + 1) )
		{
		    rgscState[rdrCount].szReader = currStr;
			rgscState[rdrCount++].dwCurrentState = SCARD_STATE_UNAWARE;
			currStr = &currStr[strlen((const char *) currStr) + 1];
		}

		lReturn = SCardLocateCards(hSC, szCard, rgscState, rdrCount );
		if ( lReturn != 0 )
		{
			lReturn1 = SCardReleaseContext(hSC);
			return lReturn;
		}

LookForCard:
		for ( i = 0; i < rdrCount; i++)
		{
			if ( ! memcmp ( (void *) &(rgscState[i].rgbAtr[0xB]), (const void *) "EstEID ver 1.0", 14) )
			{
				strcpy( SelectedReader, (const char *) rgscState[i].szReader);
				break;
			}
			if ( ! memcmp ( (void *) &(rgscState[i].rgbAtr[0x4]), (const void *) "EstEID ver 1.0", 14) )
			{
				strcpy( SelectedReader, (const char *) rgscState[i].szReader);
				break;
			}
		}

		if ( i == rdrCount )
		{
			for ( i = 0; i < rdrCount; i++ ) rgscState[i].dwCurrentState = rgscState[i].dwEventState;
			lReturn = SCardGetStatusChange ( hSC, 7000, rgscState, rdrCount );
			if ( lReturn == SCARD_S_SUCCESS ) goto LookForCard;
			else
			{
				lReturn1 = SCardReleaseContext(hSC);
				return 0x0F000001;
			}
		} //End if ( i == rdrCount )

		lReturn = SCardConnect( hSC, SelectedReader, SCARD_SHARE_SHARED,
                        SCARD_PROTOCOL_TYPE,
						&hCardHandle, &dwAP );
		if ( lReturn )
		{
			lReturn1 = SCardReleaseContext(hSC);
			return 0x0F000001;
		}

		lReturn = ( dwAP != SCARD_PROTOCOL_TYPE );
		if ( lReturn )
		{
			lReturn1 = SCardDisconnect( hCardHandle, SCARD_LEAVE_CARD);
			lReturn1 = SCardReleaseContext(hSC);
			return 0x0F000001;
		}

		lReturn = SCardBeginTransaction( hCardHandle );
		if ( lReturn )
		{
			lReturn1 = SCardDisconnect( hCardHandle, SCARD_LEAVE_CARD);
			lReturn1 = SCardReleaseContext(hSC);
			return 0x0F000001;
		}

	}

	return 0;
}
///////////////////////////////////////////////////////////////////
void Translate(char *Src, char *Dest)
///////////////////////////////////////////////////////////////////
{
	unsigned int i, Len;
	unsigned char c1, c2;

	Len = strlen((const char *) Src)/2;
	
	for ( i = 0; i < Len; i++)
	{
		c1 = Src[i<<1];
		c1 = c1 < 0x40 ? c1 - 0x30 : c1 - 0x37;
		c2 = Src[(i<<1)+1];
		c2 = c2 < 0x40 ? c2 - 0x30 : c2 - 0x37;

		Dest[i] = (c1 << 4) | c2;
	}
	return;
}

///////////////////////////////////////////////////////////////////
int EstEID::APDUCommand( 
///////////////////////////////////////////////////////////////////
						char *DataIn,
						 unsigned char DataInLen,
						 char *DataOut,
						 unsigned char *OutLen)
{
	int lReturn, i;
	char bRecv[1000];
	DWORD SendLen;
	DWORD  RecvLen;

	RecvLen = 1000;
	SendLen = (DWORD) DataInLen;


	if ( SCARD_PROTOCOL_TYPE == SCARD_PROTOCOL_T0 )
	{
		if ( ( SendLen > 4 ) && ((unsigned char) DataIn[4] < (SendLen - 5 )) ) SendLen--;
		if ( (SendLen == 5) && ( (unsigned char) DataIn[1] != (unsigned char)0xB0) ) SendLen = 4;
	}

	if ( this->GlobalInterfaceType == PCSC )
	{
		//PutLog ( 1, 0, DataIn, SendLen );
		lReturn = SCardTransmit(hCardHandle, SCARD_PCI_TYPE,
                        (const unsigned char *)DataIn, SendLen,
						NULL, (unsigned char *)bRecv, &RecvLen );
		//PutLog ( 2, lReturn, bRecv, RecvLen );
	}

	if ( lReturn )	return lReturn;



	unsigned char SW1 = bRecv[RecvLen-2];
	unsigned char SW2 = bRecv[RecvLen-1];
	if ( !( SW1 == 0x90 || SW1 == 0x61 || SW1 == 0x62 || SW1 == 0x63 ) )
	{
		lReturn = (unsigned int)0xFF000000 + (unsigned int)0x00000100*SW1 + SW2;
		return lReturn;
	}

	if ( SW1 == 0x61 )
	{
		RecvLen = 1000;
		DataIn[0] = (TCHAR)0x00; DataIn[1] = (TCHAR)0xC0; DataIn[2] = (TCHAR)0x00; DataIn[3] = (TCHAR)0x00;
		DataIn[4] = SW2; 
		SendLen = (DWORD) 5;

		if ( this->GlobalInterfaceType == PCSC )
		{
			//PutLog ( 3, 0, DataIn, SendLen );
			lReturn = SCardTransmit(hCardHandle, SCARD_PCI_TYPE,
                        (const unsigned char *)DataIn, SendLen,
						NULL, (unsigned char *)bRecv, &RecvLen );
			//PutLog ( 4, lReturn, bRecv, RecvLen );
		}
		

		if ( lReturn )	return lReturn;
	}

	if ( OutLen != NULL ) *OutLen = (int)RecvLen - 2;

	if ( RecvLen > 2 && DataOut != NULL)
	{
		for ( i = 0; i < (int)RecvLen - 2; i++)
			DataOut[i] = bRecv[i];
	}

	return 0;

}
///////////////////////////////////////////////////////////////////
void TranslateBack(char *Src, unsigned Len, char *Dest )
///////////////////////////////////////////////////////////////////
{
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
int EstEID::CloseCard()
///////////////////////////////////////////////////////////////////
{
	LONG lReturn;

	lReturn = 0;

	if ( this->GlobalInterfaceType == PCSC )
	{
		lReturn = SCardEndTransaction(hCardHandle, SCARD_LEAVE_CARD);

		lReturn = SCardDisconnect( hCardHandle, SCARD_LEAVE_CARD);

		lReturn = SCardReleaseContext(hSC);
	}


	return lReturn;
}


///////////////////////////////////////////////////////////////////
int EstEID::SelectDF(int FID)
///////////////////////////////////////////////////////////////////
{
	int lReturn;
	TCHAR bSend[1000];

	switch ( FID )
	{
	case MF: 
		Translate ( "00A4000C", bSend);
		lReturn = this->APDUCommand(bSend,4,NULL,NULL);
		break;
	case EEEE:
		Translate ( "00A4010C02", bSend);
		bSend[5] = (TCHAR)0xEE; bSend[6] = (TCHAR)0xEE;
		lReturn = this->APDUCommand(bSend,7,NULL,NULL);
		break;
	default:
		return 1;
	}

	return lReturn;
}
///////////////////////////////////////////////////////////////////
int EstEID::SelectEF(int Type)
///////////////////////////////////////////////////////////////////
{
	int lReturn;
	char bSend[1000];

	Translate ( "00A4000C", bSend);
	lReturn = this->APDUCommand(bSend,4,NULL,NULL);

//	Translate ( "00A4040C0C544573744549442076312E30", bSend);
//	lReturn = this->APDUCommand(bSend,17,NULL,NULL);
	Translate ( "00A4010C02EEEE", bSend);
	lReturn = this->APDUCommand(bSend,7,NULL,NULL);
	if ( lReturn ) return lReturn;

	Translate( "00A4020C02", bSend );
	switch ( Type )
	{
	case EF_PD:   bSend[5] = (TCHAR)0x50; bSend[6] = (TCHAR)0x44; break;
	case EF_Pwd:  bSend[5] = (TCHAR)0x00; bSend[6] = (TCHAR)0x12; break;
	case EF_Key:  bSend[5] = (TCHAR)0x00; bSend[6] = (TCHAR)0x10; break;
	case EF_KeyD: bSend[5] = (TCHAR)0x00; bSend[6] = (TCHAR)0x13; break;
	case EF_SE:   bSend[5] = (TCHAR)0x00; bSend[6] = (TCHAR)0x33; break;
	case CA_CE:   bSend[5] = (TCHAR)0xCA; bSend[6] = (TCHAR)0xCE; break;
	case AUTH_CE: bSend[5] = (TCHAR)0xAA; bSend[6] = (TCHAR)0xCE; break;
	case SIGN_CE: bSend[5] = (TCHAR)0xDD; bSend[6] = (TCHAR)0xCE; break;
	case AUTH_KEY:bSend[5] = (TCHAR)0xEF; bSend[6] = (TCHAR)0x01; break;
	case SIGN_KEY:bSend[5] = (TCHAR)0xEF; bSend[6] = (TCHAR)0x02; break;
	case PUB_KEY1_FILE: bSend[5] = (TCHAR)0x10; bSend[6] = (TCHAR)0x0; break;
	case PUB_KEY2_FILE: bSend[5] = (TCHAR)0x20; bSend[6] = (TCHAR)0x0; break;
	case SIGN_KEY1_FILE: bSend[5] = (TCHAR)0x10; bSend[6] = (TCHAR)0x1; break;
	case SIGN_KEY2_FILE: bSend[5] = (TCHAR)0x10; bSend[6] = (TCHAR)0x2; break;
	case AUTH_KEY1_FILE: bSend[5] = (TCHAR)0x10; bSend[6] = (TCHAR)0x11; break;
	case AUTH_KEY2_FILE: bSend[5] = (TCHAR)0x10; bSend[6] = (TCHAR)0x12; break;
	}
	lReturn = this->APDUCommand(bSend,7,NULL,NULL);
	return lReturn;
}

///////////////////////////////////////////////////////////////////
int EstEID::VerifyPIN(int PIN_No, char *PIN)
///////////////////////////////////////////////////////////////////
{
	int lReturn;
	char bSend[1000];

	Translate( "002000", bSend );
	bSend[3] = (unsigned char) PIN_No;
	bSend[4] = (unsigned char) strlen(PIN);

	memcpy( (void *) &(bSend[5]), (const void *) PIN, bSend[4] );

	lReturn = this->APDUCommand(bSend,5+bSend[4],NULL,NULL);

	return lReturn;
}

///////////////////////////////////////////////////////////////////
int EstEID::SetSE(int SE_No)
///////////////////////////////////////////////////////////////////
{
	int lReturn;
	char bSend[10];
	
	Translate( "0022F3", bSend );
	bSend[3] = (char) SE_No;
	lReturn = this->APDUCommand(bSend,4,NULL,NULL);

	return lReturn;
}
///////////////////////////////////////////////////////////////////
int EstEID::SelectFile(int DirFID, int FileFID)
///////////////////////////////////////////////////////////////////
{
	int lReturn;
	char bSend[1000];

	Translate ( "00A4000C", bSend);
	lReturn = this->APDUCommand(bSend,4,NULL,NULL);
	if ( lReturn ) return lReturn;

	if ( DirFID && ( DirFID != MF) )
	{
		Translate ( "00A4010C02", bSend);
		switch ( DirFID )
		{
		case EEEE: bSend[5] = (TCHAR)0xEE; bSend[6] = (TCHAR)0xEE; break;
		default:
			return 1;
		}
		lReturn = this->APDUCommand(bSend,7,NULL,NULL);
		if ( lReturn ) return lReturn;
	}

	if ( FileFID )
	{
		Translate ( "00A4020C02", bSend);
		switch ( FileFID )
		{
		case EF_PD:   bSend[5] = (TCHAR)0x50; bSend[6] = (TCHAR)0x44; break;
		case EF_Pwd:  bSend[5] = (TCHAR)0x00; bSend[6] = (TCHAR)0x12; break;
		case EF_PwdC: bSend[5] = (TCHAR)0x00; bSend[6] = (TCHAR)0x16; break;
		case EF_Key:  bSend[5] = (TCHAR)0x00; bSend[6] = (TCHAR)0x10; break;
		case EF_KeyD: bSend[5] = (TCHAR)0x00; bSend[6] = (TCHAR)0x13; break;
		case EF_Rule: bSend[5] = (TCHAR)0x00; bSend[6] = (TCHAR)0x30; break;
		case EF_SE:   bSend[5] = (TCHAR)0x00; bSend[6] = (TCHAR)0x33; break;
		case CA_CE:   bSend[5] = (TCHAR)0xCA; bSend[6] = (TCHAR)0xCE; break;
		case AUTH_CE: bSend[5] = (TCHAR)0xAA; bSend[6] = (TCHAR)0xCE; break;
		case SIGN_CE: bSend[5] = (TCHAR)0xDD; bSend[6] = (TCHAR)0xCE; break;
		case PUB_KEY1_FILE: bSend[5] =(TCHAR) 0x10; bSend[6] = (TCHAR)0x0; break;
		default:
			return 2;
		}
		lReturn = this->APDUCommand(bSend,7,NULL,NULL);
		if ( lReturn ) return lReturn;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////
int EstEID::SetCrossMF_EEEE_SE(int MF_SE_No, int EEEE_SE_No, int FID)
///////////////////////////////////////////////////////////////////
{
	int lReturn;

	lReturn = this->SelectDF( MF );
	if ( lReturn ) return lReturn;

	lReturn = this->SetSE(MF_SE_No);
	if ( lReturn ) return lReturn;
	
	lReturn = this->SelectDF( EEEE );
	if ( lReturn ) return lReturn;

	if ( FID != 0 )
	{
		lReturn = this->SelectEF(FID);
		if ( lReturn ) return lReturn;
	}

	lReturn = this->SetSE(EEEE_SE_No);
	if ( lReturn ) return lReturn;

	return 0;
}
///////////////////////////////////////////////////////////////////
int EstEID::GetCardChallenge(char *CardChallenge)
///////////////////////////////////////////////////////////////////
{
	int lReturn;
	unsigned char recvLen;
	char bSend[1000], bRecv[1000];

	Translate( "0084000008", bSend );
	lReturn = this->APDUCommand(bSend,5,bRecv,&recvLen);
	if ( recvLen != 8 ) return 1;
	if ( lReturn ) return lReturn;

	memcpy ( (void *) CardChallenge, (const void *) bRecv, 8);

	return 0;
}
///////////////////////////////////////////////////////////////////
int EstEID::ReadRecord(int DirFID, int FileFID, int RecNo, char *Data, int* RecLen)
///////////////////////////////////////////////////////////////////
{
	int lReturn;
	char bSend[1000], bRecv[1000];
	unsigned char recvLen;
	
	lReturn = this->SelectFile(DirFID, FileFID);
	if ( lReturn ) return lReturn;

	Translate( "00B2000400", bSend );

	bSend[2] = (unsigned char) RecNo;

	lReturn = this->APDUCommand(bSend,5,bRecv,&recvLen);
	if ( lReturn ) return lReturn;

	*RecLen = recvLen;
	memcpy((void *) Data, (const void *) bRecv, recvLen);

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CEstEID
///////////////////////////////////////////////////////////////////
STDMETHODIMP CEstEID::InterfaceSupportsErrorInfo(REFIID riid)
///////////////////////////////////////////////////////////////////
{
	static const IID* arr[] = 
	{
		&IID_IEstEID
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (::ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

///////////////////////////////////////////////////////////////////
STDMETHODIMP CEstEID::getInfo(BSTR *sInfo)
///////////////////////////////////////////////////////////////////
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// TODO: Add your implementation code here
	*sInfo = _bstr_t((LPCTSTR)sModuleInfoText).copy();
	return S_OK;
}
///////////////////////////////////////////////////////////////////
STDMETHODIMP CEstEID::getCardNo(BSTR *sCardNo)
///////////////////////////////////////////////////////////////////
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// TODO: Add your implementation code here

	
	LONG lReturn;
	TCHAR sKaart[10];
	int KaartLen=0;

    EstEID *Kaart;

    Kaart = new EstEID(PCSC);

	lReturn = Kaart->OpenCard();
/*
    //1. Valime juurkataloogi MF
	lReturn = Kaart->SelectDF(MF);

	//2. Valime kataloogi EEEE
	lReturn = Kaart->SelectDF(EEEE);

	//3. Valime faili 5044 
	lReturn = Kaart->SelectFile(EEEE,EF_PD);
*/
    //4. Kaardinumbri lugemine
	lReturn = Kaart->ReadRecord(EEEE,EF_PD,8, sKaart, &KaartLen);
	sKaart[KaartLen]=0;


	lReturn = Kaart->CloseCard();
	
	delete Kaart;
	
   *sCardNo=_bstr_t((LPCSTR)sKaart).copy();

	return S_OK;
}
///////////////////////////////////////////////////////////////////