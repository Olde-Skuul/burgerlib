/***************************************

	MD4 hash manager

	Implemented following the documentation found in
	http://en.wikipedia.org/wiki/MD4
	and http://tools.ietf.org/html/rfc1320

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
	Word8 m_Hash[16];	///< 128 bit hash value in RFC 1320 MD4 format
};

struct MD4Hasher_t {
	MD4_t m_Hash;				///< Current 128 bit value
	Word64 m_uByteCount;		///< Number of bytes processed (64 bit value)
	Word8 m_CacheBuffer[64];	///< input buffer for processing

	void BURGER_API Init(void);
	void BURGER_API Process(const Word8 *pBlock);
	void BURGER_API Process(const void *pInput,WordPtr uLength);
	void BURGER_API Finalize(void);
};
extern void BURGER_API Hash(MD4_t *pOutput,const void *pInput,WordPtr uLength);
}
/* END */

#endif
