/***************************************

	Assert redirection class

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRASSERT_H__
#define __BRASSERT_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRWATCOM_H__
#include "brwatcom.h"
#endif

/* BEGIN */
namespace Burger {
	typedef int (BURGER_API *ProcAssert)(void *pThis,const char *pCondition,const char *pFilename,Word uLineNumber);
	extern int BURGER_API Assert(const char *pCondition,const char *pFilename,Word uLineNumber);
	extern void BURGER_API AssertRedirect(ProcAssert pAssert,void *pThis);
	extern void BURGER_API Halt(void);
}
#if defined(BURGER_WATCOM) && !defined(DOXYGEN)
// Use WatcomAssertNothing() as a null function to shut up Watcom's warnings
#if defined(_DEBUG)
#define BURGER_ASSERT(conditional) ((conditional) ? WatcomAssertNothing() : static_cast<void>(::Burger::Assert(#conditional,__FILE__,__LINE__)))
#define BURGER_ASSERTTEST(conditional) ((conditional) ? TRUE : ::Burger::Assert(#conditional,__FILE__,__LINE__))
#else
#define BURGER_ASSERT(conditional) WatcomAssertNothing()
#define BURGER_ASSERTTEST(conditional) ((conditional) ? TRUE : FALSE)
#endif
#elif defined(_DEBUG) || defined(DOXYGEN)
#define BURGER_ASSERT(conditional) ((conditional) ? static_cast<void>(NULL) : static_cast<void>(::Burger::Assert(#conditional,__FILE__,__LINE__)))
#define BURGER_ASSERTTEST(conditional) ((conditional) ? TRUE : ::Burger::Assert(#conditional,__FILE__,__LINE__))
#else
#define BURGER_ASSERT(conditional) static_cast<void>(NULL)
#define BURGER_ASSERTTEST(conditional) ((conditional) ? TRUE : FALSE)
#endif
#define BURGER_COMPILE_TIME_ASSERT(x) typedef int ThisIsABogusTypeDef ## __LINE__ [(x) * 2 - 1]

/* END */

#endif
