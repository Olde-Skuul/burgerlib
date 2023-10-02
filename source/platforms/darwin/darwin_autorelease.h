/***************************************

	Autorelease class for Darwin

	Copyright (c) 2021-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __DARWIN_AUTORELEASE_H__
#define __DARWIN_AUTORELEASE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
#if defined(__OBJC__) || defined(DOXYGEN)
#define BURGER_SCOPED_AUTORELEASE \
	const Burger::AutoreleasePool BURGER_JOIN2(TempPool, __LINE__)

namespace Burger {
class AutoreleasePool {
	/** Pointer to the allocated NSAutoreleasePool */
	void* m_pNSAutoreleasePool;

public:
	AutoreleasePool() BURGER_NOEXCEPT;
	~AutoreleasePool();
};
}
#else

#define BURGER_SCOPED_AUTORELEASE ((void)0)
#endif
/* END */

#endif
