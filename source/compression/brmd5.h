/***************************************

	MD5 hash manager

	Implemented following the documentation found in
	http://en.wikipedia.org/wiki/MD5
	and http://tools.ietf.org/html/rfc1321

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMD5_H__
#define __BRMD5_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {

struct MD5_t {
	/** 128 bit hash value in RFC 1321 MD5 format */
	uint8_t m_Hash[16];
};

struct MD5Hasher_t {
	/** Current 128 bit value */
	MD5_t m_Hash;
	/** Number of bytes processed (64 bit value) */
	uint64_t m_uByteCount;
	/** Input buffer for processing */
	uint8_t m_CacheBuffer[64];

	void BURGER_API Init(void) BURGER_NOEXCEPT;
	void BURGER_API Process(const uint8_t* pBlock) BURGER_NOEXCEPT;
	void BURGER_API Process(
		const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT;
	void BURGER_API Finalize(void) BURGER_NOEXCEPT;
};

extern void BURGER_API Hash(
	MD5_t* pOutput, const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT;

}
/* END */

#endif
