/***************************************

	GOST hash manager

	Implemented following the documentation found in
	https://en.wikipedia.org/wiki/GOST_(hash_function)
	and http://tools.ietf.org/html/rfc5831

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRGOST_H__
#define __BRGOST_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {

struct GOST_t {
	/** 256 bit hash value in RFC 5831 GOST format */
	uint8_t m_Hash[32];
};

struct GOSTHasher_t {
	/** Current 256 bit value */
	GOST_t m_Hash;
	/** Number of bytes processed (64 bit value) */
	uint64_t m_uByteCount;
	/** Number of bits processed */
	uint64_t m_uTotalCount;
	/** Hash in native endian */
	uint32_t m_uNativeHash[8];
	/** Running data sum */
	uint32_t m_uSum[8];
	/** Input buffer for processing */
	uint8_t m_CacheBuffer[32];

	static const uint32_t g_TestParmsSBox[8][16];
	static const uint32_t g_SBoxTable[4][256];
	void BURGER_API Init(void) BURGER_NOEXCEPT;
	void BURGER_API Process(const uint32_t* pBlock) BURGER_NOEXCEPT;
	void BURGER_API Process(const uint8_t* pBlock) BURGER_NOEXCEPT;
	void BURGER_API Process(
		const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT;
	void BURGER_API Finalize(void) BURGER_NOEXCEPT;
};

extern void BURGER_API Hash(
	GOST_t* pOutput, const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT;
extern void BURGER_API GenerateGOSTTable(uint32_t* pOutput,
	const uint32_t pSBoxTable[8][16] = GOSTHasher_t::g_TestParmsSBox)
	BURGER_NOEXCEPT;

}
/* END */

#endif
