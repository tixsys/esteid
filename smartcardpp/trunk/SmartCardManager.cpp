/*!
	\file		PCSCManager.cpp
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-07-15 21:16:04 +0300 (Wed, 15 Jul 2009) $
*/
// Revision $Revision: 361 $
#include <smartcardpp/common.h>
#include <smartcardpp/SmartCardManager.h>
#include <smartcardpp/PCSCManager.h>
#include <smartcardpp/CTAPIManager.h>

enum mgrType {
	MANAGER_PCSC,
	MANAGER_CTAPI
	};

struct SmartCardConnectionPriv {
	int m_manager;
	PCSCConnection *pcscConn;
	CTAPIConnection *ctConn;
	ManagerInterface &m_managerOriginal;
	SmartCardConnectionPriv(int manager,ManagerInterface &iface,unsigned int index,bool force,
		ManagerInterface &origMgr) :
		m_manager(manager),pcscConn(NULL),ctConn(NULL)
			,m_managerOriginal(origMgr) {
		UNUSED_ARG(iface);
		if (m_manager == MANAGER_PCSC) pcscConn = new PCSCConnection(m_managerOriginal,index,force);
		if (m_manager == MANAGER_CTAPI) ctConn = new CTAPIConnection(m_managerOriginal,index,force);
		}
	~SmartCardConnectionPriv() {
		if (m_manager == MANAGER_PCSC) delete pcscConn;
		if (m_manager == MANAGER_CTAPI) delete ctConn;
		}
	ConnectionBase * getConnection() {
		if (m_manager == MANAGER_PCSC) return pcscConn;
		if (m_manager == MANAGER_CTAPI) return ctConn;
		throw std::runtime_error("Invalid smartcardconnection");
		}
	bool isSecure() {
		if (m_manager == MANAGER_PCSC) return pcscConn->isSecure();
		if (m_manager == MANAGER_CTAPI) return ctConn->isSecure();
		return false;
		}
private:
	const SmartCardConnectionPriv& operator=(const SmartCardConnectionPriv &);
};

SmartCardConnection::SmartCardConnection(int manager,ManagerInterface &iface,unsigned int index,bool force
										 ,ManagerInterface &orig)
	:ConnectionBase(iface) {
	d = new SmartCardConnectionPriv(manager,iface,index,force,orig);
	}

SmartCardConnection::~SmartCardConnection() {
	d->m_managerOriginal.deleteConnection(this);
	delete d;
	}

bool SmartCardConnection::isSecure() {
	return d->isSecure();
	}


struct SmartCardManagerPriv {
	PCSCManager pcscMgr;
	CTAPIManager ctMgr;
	ManagerInterface *connIf;
	uint pcscCount;
	uint ctCount;
	SmartCardManagerPriv() : connIf(&pcscMgr), pcscCount(0),ctCount(0) {
		try { // avoid throws from constructor
			pcscCount = pcscMgr.getReaderCount();
			ctCount = ctMgr.getReaderCount();
		} catch(...) {}
		}
	ManagerInterface & getIndex(uint &i) {
		if (i < pcscCount )
			return pcscMgr;
		i-= pcscCount;
		return ctMgr;
		}
	};

SmartCardManager::SmartCardManager(void) {
	d = new SmartCardManagerPriv();
}

SmartCardManager::~SmartCardManager(void) {
	delete d;
}

void SmartCardManager::makeConnection(ConnectionBase *c,uint idx) {
//	SmartCardConnection *pc = (SmartCardConnection *)c;
	d->connIf->makeConnection( c , idx);
	}

void SmartCardManager::deleteConnection(ConnectionBase *c) {
	SmartCardConnection *pc = (SmartCardConnection *)c;
	pc->mManager.deleteConnection(pc->d->getConnection());
	pc->d->getConnection()->mOwnConnection = false; // ensure no duplicate delete
	}

void SmartCardManager::beginTransaction(ConnectionBase *c) {
	SmartCardConnection *pc = (SmartCardConnection *)c;
	pc->mManager.beginTransaction(pc->d->getConnection());
	}

void SmartCardManager::endTransaction(ConnectionBase *c,bool forceReset) {
	UNUSED_ARG(forceReset);
	SmartCardConnection *pc = (SmartCardConnection *)c;
	pc->mManager.endTransaction(pc->d->getConnection());
	}

void SmartCardManager::execCommand(ConnectionBase *c,std::vector<BYTE> &cmd,std::vector<BYTE> &recv,
								   unsigned int &recvLen) {
	SmartCardConnection *pc = (SmartCardConnection *)c;
	pc->mManager.execCommand(pc->d->getConnection(),cmd,
		recv,recvLen);
}

bool SmartCardManager::isT1Protocol(ConnectionBase *c) {
	SmartCardConnection *pc = (SmartCardConnection *)c;
	return pc->mManager.isT1Protocol(pc->d->getConnection());
	}

void SmartCardManager::execPinEntryCommand(ConnectionBase *c,std::vector<byte> &cmd) {
	SmartCardConnection *pc = (SmartCardConnection *)c;
	return pc->mManager.execPinEntryCommand(pc->d->getConnection(),
		cmd);
	}

uint SmartCardManager::getReaderCount(bool forceRefresh) {
	d->pcscCount = d->pcscMgr.getReaderCount(forceRefresh);
	d->ctCount = d->ctMgr.getReaderCount(forceRefresh);
	return d->pcscCount + d->ctCount;
	}

std::string SmartCardManager::getReaderName(uint idx) {
	uint t_idx = idx;
	return d->getIndex(t_idx).getReaderName(t_idx);
	}

std::string SmartCardManager::getReaderState(uint idx) {
	uint t_idx = idx;
	return d->getIndex(t_idx).getReaderState(t_idx);
	}

std::string SmartCardManager::getATRHex(uint idx) {
	uint t_idx = idx;
	return d->getIndex(t_idx).getATRHex(t_idx);
	}

SmartCardConnection * SmartCardManager::connect(uint idx,bool forceT0) {
	uint t_idx = idx;
	ManagerInterface &mgr = d->getIndex(t_idx);
	d->connIf = &mgr; //hack, passing down to makeConnection
	SmartCardConnection *retConn = NULL;
	if (&mgr == &d->pcscMgr)
		retConn = new SmartCardConnection(MANAGER_PCSC,mgr,t_idx,forceT0,*this);
	if (&mgr == &d->ctMgr)
		retConn = new SmartCardConnection(MANAGER_CTAPI,mgr,t_idx,forceT0,*this);
	return retConn;
	}

SmartCardConnection * SmartCardManager::reconnect(ConnectionBase *c,bool forceT0) {
	SmartCardConnection *pc = (SmartCardConnection *)c;
	if (&pc->mManager == &d->pcscMgr) {
		pc->mManager.reconnect(pc->d->getConnection(),forceT0);
		return pc;
		}
	else
		throw std::runtime_error("err, unimplemented");
	return 0;
	}
