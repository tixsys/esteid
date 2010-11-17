#pragma once
#include "../CardBase.h"

class SIMCard :
	public CardBase
{
	//data file numbers on SIM
	enum {
		DATAFILE_MF  = 0x3F00,
		DATAFILE_GSM  =	0x7F20,
		DATAFILE_TELECOM = 0x7F10,
	};
	//elementary file numbers on SIM
	enum {
		ELEMENTARY_ICC1 = 0x2FE2, // ALW/NEV
		ELEMENTARY_IMSI = 0x6F07, //CHV1/ADM
		ELEMENTARY_SPN	= 0x6F46, //
		ELEMENTARY_ADN	= 0x6F3A, //
	};

public:
	SIMCard(ManagerInterface &ref);
	~SIMCard(void);
	void test(void);

	virtual byte CLAByte() { return 0xA0; }

	bool isInReader(unsigned int idx);
	
	/// Reads the card ICC
	std::string readICC();
};
