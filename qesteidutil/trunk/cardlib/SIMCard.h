#pragma once
#include "CardBase.h"

class SIMCard :
	public CardBase
{
public:
	SIMCard(ManagerInterface &ref);
	~SIMCard(void) {};
	void test(void) {};
};
