/***************************************

	MD2 hash manager

	Implemented following the documentation found in
	http://en.wikipedia.org/wiki/MD2_(cryptography)
	and http://www.ietf.org/rfc/rfc1319.txt 

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
	Word8 m_Hash[16];	///< 128 bit hash value in RFC 1319 MD2 format
};
struct MD2Hasher_t {
	MD2_t m_Hash;			///< Calculated hash
	Word8 m_Checksum[16];	///< Running checksum
	Word8 m_CacheBuffer[16];	///< Cached input data for multi-pass hashing
	WordPtr m_uCount;		///< Number of bytes in the cache (0-15)
	void Init(void);
	void Process(const Word8 *pBlock);
	void Process(const void *pInput,WordPtr uLength);
	void Finalize(void);
};
extern void BURGER_API Hash(MD2_t *pOutput,const void *pInput,WordPtr uLength);
}
/* END */

#endif
