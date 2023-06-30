/***************************************

	SDBM hash manager (Service Data Base Management)

	Implemented following the documentation found in
	http::/www.cs.yorku.ca/~oz/hash.html

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSDBMHASH_H__
#define __BRSDBMHASH_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
extern uintptr_t BURGER_API SDBM_hash(const void* pInput, uintptr_t uInputCount,
	uint32_t uHashSeed = 0U) BURGER_NOEXCEPT;
extern uintptr_t BURGER_API SDBM_hash_case(const void* pInput,
	uintptr_t uInputCount, uint32_t uHashSeed = 0U) BURGER_NOEXCEPT;
}
/* END */

#endif
