#ifndef CARDLIB_H_
#define CARDLIB_H_

/* EstEID cardlib includes */
#include "common.h"
#include "PCSCManager.h"
#include "EstEidCard.h"
#include "SCError.h"
#include "helperMacro.h" // MAKEVECTOR
#include <vector>
#include <string>

#define PDATA_MIN EstEidCard::SURNAME
#define PDATA_MAX EstEidCard::COMMENT4

#endif /* CARDLIB_H_ */
