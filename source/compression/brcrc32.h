/***************************************

	CRC32 hash manager

	Implemented following the documentation found in
	http://en.wikipedia.org/wiki/Cyclic_redundancy_check
	and http://wiki.osdev.org/CRC32

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#ifndef __BRCRC32_H__
#define __BRCRC32_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
extern Word32 BURGER_API CalcCRC32B(const void *pInput,WordPtr uInputLength,Word32 uCRC=0);
}
/* END */

#endif
