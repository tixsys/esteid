/*!
	\file		pinDialog.h
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-07-15 21:20:50 +0300 (Wed, 15 Jul 2009) $
*/
// Revision $Revision: 363 $

///forward-declared opsys-specific components
struct pinDialogPriv;
#include <smartcard++/esteid/EstEidCard.h>
#include "pinDialog_res.h"

class mutexObj;

class pinOpInterface
{
	const pinOpInterface &operator=(const pinOpInterface &o);
protected:
	friend class pinDialog;
	EstEidCard &m_card;
	mutexObj &m_mutex;
public:
	pinOpInterface(EstEidCard &card,mutexObj &mutex): m_card(card),m_mutex(mutex) {}
	virtual void call(EstEidCard &,const PinString &pin,EstEidCard::KeyType)=0;
};

class pinDialog {
	pinDialogPriv *d;
protected:
	EstEidCard::KeyType m_key;
	std::string m_prompt;
	int m_minLen;
	friend struct pinDialogPriv;
public:
	EstEidCard::KeyType keyType() { return m_key; }
	pinDialog(const void * opsysParam,std::string prompt);
	pinDialog(const void * opsysParam,EstEidCard::KeyType key);
	~pinDialog();
	bool doDialog();
	bool showPrompt(std::string,bool allowRetry = false);
	bool doDialogInloop(pinOpInterface &operation,PinString &authPinCache);
	PinString getPin();
};

