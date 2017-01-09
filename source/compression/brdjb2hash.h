/***************************************

	DJB2 hash manager (Dan Bernstein's algorithm)

	Implemented following the documentation found in
	http::/www.cs.yorku.ca/~oz/hash.html

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#ifndef __BRDJB2HASH_H__
#define __BRDJB2HASH_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
extern WordPtr BURGER_API DJB2HashAdd(const void *pInput,WordPtr uInputCount,Word32 uHashSeed = 5381U);
extern WordPtr BURGER_API DJB2HashAddCase(const void *pInput,WordPtr uInputCount,Word32 uHashSeed = 5381U);
extern WordPtr BURGER_API DJB2HashXor(const void *pInput,WordPtr uInputCount,Word32 uHashSeed = 5381U);
extern WordPtr BURGER_API DJB2HashXorCase(const void *pInput,WordPtr uInputCount,Word32 uHashSeed = 5381U);
}
/* END */

#endif

