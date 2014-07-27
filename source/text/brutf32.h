/***************************************

	String handlers for UTF32 support

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRUTF32_H__
#define __BRUTF32_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
class UTF32 {
public:
	enum {BAD=-1,ENDIANMARK=0xFEFF,
#if defined(BURGER_LITTLEENDIAN)
	BE=0xFFFE0000,LE=0xFEFF};
#else
	BE=0xFEFF,LE=0xFFFE0000};
#endif
	static Word BURGER_API IsValid(Word32 uInput);
	static Word BURGER_API IsValid(const Word32 *pInput);
	static Word BURGER_API IsValid(const Word32 *pInput,WordPtr uInputSize);
	static Word32 BURGER_API FromUTF8(const char *pInput);
	static WordPtr BURGER_API FromUTF8(Word32 *pOutput,WordPtr uOutputSize,const char *pInput);
	static WordPtr BURGER_API FromUTF8(Word32 *pOutput,WordPtr uOutputSize,const char *pInput,WordPtr uInputSize);
};
}
/* END */

#endif

