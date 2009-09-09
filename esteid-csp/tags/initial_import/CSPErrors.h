/*!
	\file		CSPErrors.h
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )    
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2008-12-23 16:54:11 +0200 (Tue, 23 Dec 2008) $
*/
// Revision $Revision: 152 $

#pragma once

class err_base : public std::runtime_error {
	DWORD m_code;
public:
	err_base(const char *desc,DWORD code) : m_code(code),std::runtime_error(desc) {
		SetLastError(code);
		}
	};

class err_crypto: public err_base {
	std::string desc;
public:
	err_crypto(const char *op,DWORD code): err_base("CryptoApiError",code) {
		std::ostringstream buf;
		buf << "exception: op:'"  << op << "' "
				" code:'0x" <<std::hex << std::setfill('0') <<
				std::setw(8) << code << "'";
		desc = buf.str();
		}
	static void check(BOOL err,const char *op="") { 
		if (err != TRUE) throw err_crypto(op,GetLastError()); 
		}
	virtual const char * what() const throw() {	return desc.c_str();} 
	};

class err_badFlags : public err_base {
	public: err_badFlags() : err_base("bad flags",NTE_BAD_FLAGS) {}
};

class err_badKeyset : public err_base {
	public: err_badKeyset() : err_base("bad keyset",NTE_BAD_KEYSET) {}
};

class err_badContext : public err_base {
	public: err_badContext() : err_base("bad context handle",NTE_BAD_UID) {}
};
class err_badHash : public err_base {
	public: err_badHash() : err_base("bad hash",NTE_BAD_HASH) {}
};

class err_badKey : public err_base {
	public: err_badKey() : err_base("bad key",NTE_BAD_KEY) {}
};

class err_badType : public err_base {
	public: err_badType() : err_base("bad type specified",NTE_BAD_TYPE) {}
};

class err_bufferTooSmall : public err_base {
	public: err_bufferTooSmall() : err_base("buffer too small",ERROR_MORE_DATA) {}
};

class err_noMoreItems : public err_base {
	public: err_noMoreItems () : err_base("no more items",ERROR_NO_MORE_ITEMS) {}
};

class err_noKey : public err_base {
	public: err_noKey () : err_base("key does not exist",NTE_NO_KEY) {}
};
