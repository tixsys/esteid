/*!
	\file		asnCertificate.h
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )    
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-04-17 16:19:22 +0300 (Fri, 17 Apr 2009) $
*/
// Revision $Revision: 255 $

#include "asnObject.h"

class asnCertificate : public asnObject {
	const asnCertificate &operator=(const asnCertificate &);
	asnObject * signatureValue;
	asnObject * signatureAlgorithm;

	asnObject * version;
	asnObject * serialNumber;
	asnObject * signatureAlg;
	asnObject * issuerName;
	asnObject * validityPeriod;
	asnObject * subjectName;
	asnObject * publicKeyInfo;
	asnObject * extensions;

	void init();
public:
	asnCertificate(byteVec &in,std::ostream &pout);
	asnObject *findExtension(std::string ext);
	string getSubjectAltName();
	bool isTimeValid(int numDaysFromNow = 0);
	string getValidFrom();
	string getValidTo();
	string getSubject();
	vector<byte> getSubjectCN();
	vector<byte> getSubjectO();
	vector<byte> getSubjectOU();
	vector<byte> getIssuerCN();
	vector<byte> getIssuerO();
	vector<byte> getIssuerOU();
	vector<byte> getIssuerBlob();
	vector<byte> getSerialBlob();
	vector<byte> getSubjectBlob();
	vector<byte> getPubKey();
	bool checkKeyUsage(std::string keyUsageId);
	bool hasExtKeyUsage();

};
