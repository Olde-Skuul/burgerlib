/***************************************

	String handlers for MacRomanUS support

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMACROMANUS_H__
#define __BRMACROMANUS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
class MacRomanUS {
public:
	enum {BAD=-1};
	static const Word8 BURGER_ALIGN(UpperCaseTable[256],16);
	static const Word8 BURGER_ALIGN(LowerCaseTable[256],16);
	static const Word8 BURGER_ALIGN(ToUTF8Table[128][4],16);	
	static Word BURGER_API FromUTF8(const char *pInput);
	static Word BURGER_API FromUTF8(char *pOutput,WordPtr uOutputSize,const char *pInput);
	static Word BURGER_API FromUTF8(char *pOutput,WordPtr uOutputSize,const char *pInput,WordPtr uInputSize);
};
}
/* END */

#endif
