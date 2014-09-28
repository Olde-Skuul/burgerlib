/***************************************

	SDBM hash manager (Service Data Base Management)

	Implemented following the documentation found in
	http::/www.cs.yorku.ca/~oz/hash.html

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#ifndef __BRSDBMHASH_H__
#define __BRSDBMHASH_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
extern WordPtr BURGER_API SDBMHash(const void *pInput,WordPtr uInputCount,Word32 uHashSeed = 0U);
extern WordPtr BURGER_API SDBMHashCase(const void *pInput,WordPtr uInputCount,Word32 uHashSeed = 0U);
}
/* END */

#endif

