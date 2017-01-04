/***************************************

	GOST hash manager

	Implemented following the documentation found in
	https://en.wikipedia.org/wiki/GOST_(hash_function)
	and http://tools.ietf.org/html/rfc5831

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
	Word8 m_Hash[32];	///< 256 bit hash value in RFC 5831 GOST format
};

struct GOSTHasher_t {
	GOST_t m_Hash;				///< Current 256 bit value
	Word64 m_uByteCount;		///< Number of bytes processed (64 bit value)
	Word64 m_uTotalCount;		///< Number of bits processed
	Word32 m_uNativeHash[8];	///< Hash in native endian
	Word32 m_uSum[8];			///< Running data sum
	Word8 m_CacheBuffer[32];	///< input buffer for processing

	static const Word32 g_TestParmsSBox[8][16];
	static const Word32 g_SBoxTable[4][256];
	void BURGER_API Init(void);
	void BURGER_API Process(const Word32 *pBlock);
	void BURGER_API Process(const Word8 *pBlock);
	void BURGER_API Process(const void *pInput,WordPtr uLength);
	void BURGER_API Finalize(void);
};

extern void BURGER_API Hash(GOST_t *pOutput,const void *pInput,WordPtr uLength);
extern void BURGER_API GenerateGOSTTable(Word32 *pOutput,const Word32 pSBoxTable[8][16] = GOSTHasher_t::g_TestParmsSBox);

}
/* END */


#endif
