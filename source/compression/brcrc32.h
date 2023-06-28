/***************************************

	CRC32 hash manager

	Implemented following the documentation found in
	http://en.wikipedia.org/wiki/Cyclic_redundancy_check
	and http://wiki.osdev.org/CRC32

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRCRC32_H__
#define __BRCRC32_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
extern uint32_t BURGER_API calc_crc32b(const void* pInput,
	uintptr_t uInputLength, uint32_t uCRC = 0) BURGER_NOEXCEPT;
extern uint32_t BURGER_API calc_crc32(const void* pInput,
	uintptr_t uInputLength, uint32_t uCRC = 0) BURGER_NOEXCEPT;
extern void BURGER_API generate_crc_table(uint32_t* pOutput,
	uint32_t uPolynomial = 0x04C11DB7U,
	uint_t bBitReverse = TRUE) BURGER_NOEXCEPT;
}
/* END */

#endif
