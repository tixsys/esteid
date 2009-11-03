#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <smartcard++/types.h>

#ifdef _WIN32
#include <tchar.h>
#endif

#ifdef __APPLE__
typedef void *LPVOID;
#define __COREFOUNDATION_CFPLUGINCOM__
#endif
/*! \mainpage Smartcard++ documentation
*
* \section intro_sec Introduction
*
* Smartcard++ reference documentation. Smartcard++ is a set of c++ classes to manage
* smartcard communications and implement basic command primitives.
*/
