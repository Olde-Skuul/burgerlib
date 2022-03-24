/***************************************

	MD2 hash manager

	Implemented following the documentation found in
	http://en.wikipedia.org/wiki/MD2_(cryptography)
	and http://www.ietf.org/rfc/rfc1319.txt

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMD2_H__
#define __BRMD2_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {

struct MD2_t {
	/** 128 bit hash value in RFC 1319 MD2 format */
	uint8_t m_Hash[16];
};

struct MD2Hasher_t {
	/** Calculated hash */
	MD2_t m_Hash;
	/** Running checksum */
	uint8_t m_Checksum[16];
	/** Cached input data for multi-pass hashing */
	uint8_t m_CacheBuffer[16];
	/** Number of bytes in the cache (0-15) */
	uintptr_t m_uCount;

	void BURGER_API Init(void) BURGER_NOEXCEPT;
	void BURGER_API Process(const uint8_t* pBlock) BURGER_NOEXCEPT;
	void BURGER_API Process(
		const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT;
	void BURGER_API Finalize(void) BURGER_NOEXCEPT;
};
extern void BURGER_API Hash(
	MD2_t* pOutput, const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT;
}
/* END */

#endif
