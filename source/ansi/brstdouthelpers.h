/***************************************

	Helper functions for printf and the like

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSTDOUTHELPERS_H__
#define __BRSTDOUTHELPERS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
extern void BURGER_API PrintHexDigit(Word uInput);
extern void BURGER_API PrintHex(Word8 uInput);
extern void BURGER_API PrintHex(Word16 uInput);
extern void BURGER_API PrintHex(Word32 uInput);
extern void BURGER_API PrintHex(Word64 uInput);
extern void BURGER_API PrintHex(float fInput);
extern void BURGER_API PrintHex(double dInput);
extern void BURGER_API ClearConsole(void);
}
/* END */

#endif
