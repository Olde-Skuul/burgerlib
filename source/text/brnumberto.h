/***************************************

	Number to ASCII functions

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRNUMBERTO_H__
#define __BRNUMBERTO_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {

extern char* BURGER_API NumberToAsciiHex(char* pOutput, Word8 uInput);
extern char* BURGER_API NumberToAsciiHex(char* pOutput, Word16 uInput);
extern char* BURGER_API NumberToAsciiHex(char* pOutput, Word32 uInput);
extern char* BURGER_API NumberToAsciiHex(char* pOutput, Word64 uInput);
extern char* BURGER_API NumberToAsciiHex(char* pOutput, float fInput);
extern char* BURGER_API NumberToAsciiHex(char* pOutput, double dInput);
extern char* BURGER_API NumberToAsciiHex(
	char* pOutput, Word32 uInput, Word uDigits);
extern char* BURGER_API NumberToAsciiHex(
	char* pOutput, Word64 uInput, Word uDigits);
extern char* BURGER_API NumberToAscii(
	char* pOutput, Word32 uInput, Word uDigits = 0);
extern char* BURGER_API NumberToAscii(
	char* pOutput, Int32 iInput, Word uDigits = 0);
extern char* BURGER_API NumberToAscii(
	char* pOutput, Word64 uInput, Word uDigits = 0);
extern char* BURGER_API NumberToAscii(
	char* pOutput, Int64 iInput, Word uDigits = 0);
extern char* BURGER_API NumberToAscii(char* pOutput, float fInput);
extern char* BURGER_API NumberToAscii(char* pOutput, double dInput);
extern Word BURGER_API NumberStringLength(Word32 uInput);
extern Word BURGER_API NumberStringLength(Int32 iInput);
extern Word BURGER_API NumberStringLength(Word64 uInput);
extern Word BURGER_API NumberStringLength(Int64 iInput);
extern Word BURGER_API NumberHexStringLength(Word32 uInput);
extern Word BURGER_API NumberHexStringLength(Word64 uInput);
extern Word BURGER_API NumberOctalStringLength(Word32 uInput);
extern Word BURGER_API NumberOctalStringLength(Word64 uInput);

}

/* END */

#endif
