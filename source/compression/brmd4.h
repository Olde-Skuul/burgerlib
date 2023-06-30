/***************************************

	MD4 hash manager

	Implemented following the documentation found in
	http://en.wikipedia.org/wiki/MD4
	and http://tools.ietf.org/html/rfc1320

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMD4_H__
#define __BRMD4_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {

struct MD4_t {
	/** 128 bit hash value in RFC 1320 MD4 format */
	uint8_t m_Hash[16];
};

struct MD4Hasher_t {

	/** Current 128 bit value */
	MD4_t m_Hash;

	/** Number of bytes processed (64 bit value) */
	uint64_t m_uByteCount;

	/** input buffer for processing */
	uint8_t m_CacheBuffer[64];

	void BURGER_API init(void) BURGER_NOEXCEPT;
	void BURGER_API process(const uint8_t pBlock[64]) BURGER_NOEXCEPT;
	void BURGER_API process(
		const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT;
	void BURGER_API finalize(void) BURGER_NOEXCEPT;
};

extern void BURGER_API hash(
	MD4_t* pOutput, const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT;
}
/* END */

#endif
