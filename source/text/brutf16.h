/***************************************

	String handlers for UTF16 support

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRUTF16_H__
#define __BRUTF16_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRENDIAN_H__
#include "brendian.h"
#endif

/* BEGIN */
namespace Burger {
class UTF16 {
public:

	enum {
		BAD=-1,
		ENDIANMARK=0xFEFF,
		BE=BURGER_LITTLEENDIAN16(0xFFFE),
		LE=BURGER_LITTLEENDIAN16(0xFEFF)
	};

	static const Word16 BURGER_ALIGN(FromMacRomanUSTable[128],16);
	static const Word16 BURGER_ALIGN(FromWin1252Table[128],16);
	static const Word16 BURGER_ALIGN(FromISOLatin1Table[128],16);
	static Word BURGER_API IsValid(Word16 uInput);
	static Word BURGER_API IsValid(const Word16 *pInput);
	static Word BURGER_API IsValid(const Word16 *pInput,WordPtr uInputSize);
	static Word32 BURGER_API FromUTF8(const char *pInput);
	static WordPtr BURGER_API FromUTF8(Word16 *pOutput,WordPtr uOutputSize,const char *pInput);
	static WordPtr BURGER_API FromUTF8(Word16 *pOutput,WordPtr uOutputSize,const char *pInput,WordPtr uInputSize);
};
}
/* END */

#endif

