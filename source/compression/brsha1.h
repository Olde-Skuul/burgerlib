/***************************************

	SHA-1 hash manager

	Implemented following the documentation found in
	http://en.wikipedia.org/wiki/SHA-1
	and http://tools.ietf.org/html/rfc3174

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSHA1_H__
#define __BRSHA1_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {

struct SHA1_t {
	/** 160 bit hash value in RFC 3174 SHA-1 format */
	uint8_t m_Hash[20];
};

struct SHA1Hasher_t {
	/** Number of bytes processed (64 bit value) */
	uint64_t m_uByteCount;
	/** Current 160 bit value */
	SHA1_t m_Hash;
	/** Input buffer for processing */
	uint8_t m_CacheBuffer[64];

	void BURGER_API Init(void) BURGER_NOEXCEPT;
	void BURGER_API Process(const uint8_t* pBlock) BURGER_NOEXCEPT;
	void BURGER_API Process(
		const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT;
	void BURGER_API Finalize(void) BURGER_NOEXCEPT;
};

extern void BURGER_API Hash(
	SHA1_t* pOutput, const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT;

}
/* END */

#endif
