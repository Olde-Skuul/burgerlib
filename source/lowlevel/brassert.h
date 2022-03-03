/***************************************

	Assert redirection class

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

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

struct Assert_t {
	typedef int(BURGER_API* CallbackProc)(void* pThis, const char* pCondition,
		const char* pFilename, uint_t uLineNumber);

	/** Function pointer to the redirected Assert function */
	CallbackProc m_pCallback;
	/** "this" pointer passed to the m_pProcAssert function */
	void* m_pThis;

	/** Global instance of the Assert redirection */
	static Assert_t g_Instance;

	void BURGER_API SetCallback(
		CallbackProc pCallback, void* pThis) BURGER_NOEXCEPT;
	static int BURGER_API DefaultAssert(void* pThis, const char* pCondition,
		const char* pFilename, uint_t uLineNumber) BURGER_NOEXCEPT;
};

extern int BURGER_API Assert(const char* pCondition, const char* pFilename,
	uint_t uLineNumber) BURGER_NOEXCEPT;
extern void BURGER_API Halt(void) BURGER_NOEXCEPT;

}

// Use WatcomAssertNothing() as a null function to shut up Watcom's warnings
// Clang requires a pragma to get rid of a compiler warning triggered by
// constant folding

#if defined(_DEBUG) || defined(DOXYGEN)

#if defined(BURGER_WATCOM) && !defined(DOXYGEN)
#define BURGER_ASSERT(conditional) \
	((conditional) ? WatcomAssertNothing() : \
                     static_cast<void>( \
						 ::Burger::Assert(#conditional, __FILE__, __LINE__)))
#elif defined(BURGER_CLANG) && !defined(DOXYGEN)
#define BURGER_ASSERT(conditional) \
	_Pragma("clang diagnostic push") _Pragma( \
		"clang diagnostic ignored \"-Wtautological-compare\"")((conditional) ? \
            static_cast<void>(0) : \
            static_cast<void>(::Burger::Assert(#conditional, __FILE__, \
				__LINE__))) _Pragma("clang diagnostic pop")
#else
#define BURGER_ASSERT(conditional) \
	((conditional) ? static_cast<void>(0) : \
                     static_cast<void>( \
						 ::Burger::Assert(#conditional, __FILE__, __LINE__)))
#endif
#define BURGER_ASSERTTEST(conditional) \
	((conditional) ? TRUE : ::Burger::Assert(#conditional, __FILE__, __LINE__))

#else

#if defined(BURGER_WATCOM)
#define BURGER_ASSERT(conditional) WatcomAssertNothing()
#else
#define BURGER_ASSERT(conditional) static_cast<void>(0)
#endif
#define BURGER_ASSERTTEST(conditional) ((conditional) ? TRUE : FALSE)

#endif

/* END */

#endif
