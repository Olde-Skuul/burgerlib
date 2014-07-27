/***************************************

	SHA-1 hash manager

	Implemented following the documentation found in
	http://en.wikipedia.org/wiki/SHA-1
	and http://tools.ietf.org/html/rfc3174

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
	Word8 m_Hash[20];	///< 160 bit hash value in RFC 3174 SHA-1 format
};
struct SHA1Hasher_t {
	SHA1_t m_Hash;				///< Current 160 bit value
	Word64 m_uByteCount;		///< Number of bytes processed (64 bit value)
	Word8 m_CacheBuffer[64];	///< input buffer for processing
	void Init(void);
	void Process(const Word8 *pBlock);
	void Process(const void *pInput,WordPtr uLength);
	void Finalize(void);
};
extern void BURGER_API Hash(SHA1_t *pOutput,const void *pInput,WordPtr uLength);
}
/* END */


#endif
