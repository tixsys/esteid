/*!
	\file		PCSCManager.h
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-07-15 21:16:04 +0300 (Wed, 15 Jul 2009) $
*/
// Revision $Revision: 361 $
#pragma once
#include <smartcardpp/ManagerInterface.h>

struct SmartCardManagerPriv;
struct SmartCardConnectionPriv;

struct SmartCardConnection : public ConnectionBase {
	SmartCardConnectionPriv *d;
	SmartCardConnection(int manager,ManagerInterface &iface,unsigned int index,
		bool force,ManagerInterface &orig);
	~SmartCardConnection();
	virtual bool isSecure();
};

/// Unified class for smarcard managers
/** SmartCardManager combines all system smartcard managers into one view
 Currently included are PCSCManager and CTAPIManager.
 */
class SmartCardManager : public ManagerInterface {
	SmartCardManagerPriv *d;

	void makeConnection(ConnectionBase *c,uint idx);
	void deleteConnection(ConnectionBase *c);
	void beginTransaction(ConnectionBase *c);
	void endTransaction(ConnectionBase *c,bool forceReset = false);
	void execCommand(ConnectionBase *c,std::vector<byte> &cmd,std::vector<byte> &recv,
		unsigned int &recvLen);
	bool isT1Protocol(ConnectionBase *c);
	virtual void execPinEntryCommand(ConnectionBase *c,std::vector<byte> &cmd);

public:
	SmartCardManager(void);
	~SmartCardManager(void);

	uint getReaderCount(bool forceRefresh = false);
	std::string getReaderName(uint idx);
	std::string getReaderState(uint idx);
	std::string getATRHex(uint idx);
	SmartCardConnection * connect(uint idx,bool forceT0);
	SmartCardConnection * reconnect(ConnectionBase *c,bool forceT0);
};
