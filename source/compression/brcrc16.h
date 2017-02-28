/***************************************

	CRC16 hash manager

	Implemented following the documentation found in
	http://en.wikipedia.org/wiki/Cyclic_redundancy_check

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#ifndef __BRCRC16_H__
#define __BRCRC16_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
extern Word16 BURGER_API CalcCRC16IBM(const void *pInput,WordPtr uInputLength,Word16 uCRC=0);
extern void BURGER_API GenerateCRCTable(Word16 *pOutput,Word16 uPolynomial=0x8005,Word bBitReverse=TRUE);
}
/* END */

#endif
