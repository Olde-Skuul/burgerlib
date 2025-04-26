/***************************************

	Assert redirection class

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

/* BEGIN */

namespace Burger {

struct assert_t {
	typedef int(BURGER_API* callback_t)(void* pThis, const char* pCondition,
		const char* pFilename, uint32_t uLineNumber);

	/** Function pointer to the redirected do_assert function */
	callback_t m_pCallback;
	/** "this" pointer passed to the m_pCallback function */
	void* m_pThis;

	/** Global instance of the do_assert redirection */
	static assert_t g_Instance;

	void BURGER_API set_callback(
		callback_t pCallback, void* pThis) BURGER_NOEXCEPT;
	static int BURGER_API default_assert(void* pThis, const char* pCondition,
		const char* pFilename, uint32_t uLineNumber) BURGER_NOEXCEPT;
};

extern int BURGER_API do_assert(const char* pCondition, const char* pFilename,
	uint32_t uLineNumber) BURGER_NOEXCEPT;
extern void BURGER_API invoke_debugger(void) BURGER_NOEXCEPT;

}

// Use watcom_assert() as a null function to shut up Watcom's warnings
// Clang requires a pragma to get rid of a compiler warning triggered by
// constant folding

#if defined(BURGER_WATCOM) && !defined(DOXYGEN)
extern void watcom_assert(void);
#pragma aux watcom_assert = modify exact[] nomemory;
#endif

#if defined(_DEBUG) || defined(DOXYGEN)

#if defined(BURGER_WATCOM) && !defined(DOXYGEN)
#define BURGER_ASSERT(conditional) \
	((conditional) ? watcom_assert() : \
					 static_cast<void>(::Burger::do_assert( \
						 #conditional, __FILE__, __LINE__)))

#elif defined(BURGER_CLANG) && !defined(DOXYGEN)
#define BURGER_ASSERT(conditional) \
	_Pragma("clang diagnostic push") _Pragma( \
		"clang diagnostic ignored \"-Wtautological-compare\"")((conditional) ? \
			static_cast<void>(0) : \
			static_cast<void>(::Burger::do_assert(#conditional, __FILE__, \
				__LINE__))) _Pragma("clang diagnostic pop")
#else

#define BURGER_ASSERT(conditional) \
	((conditional) ? static_cast<void>(0) : \
					 static_cast<void>(::Burger::do_assert( \
						 #conditional, __FILE__, __LINE__)))
#endif

#define BURGER_ASSERTTEST(conditional) \
	((conditional) ? TRUE : \
					 ::Burger::do_assert(#conditional, __FILE__, __LINE__))

#else

// Disable BURGER_ASSERT
#if defined(BURGER_WATCOM)
#define BURGER_ASSERT(conditional) watcom_assert()
#else
#define BURGER_ASSERT(conditional) static_cast<void>(0)
#endif

#define BURGER_ASSERTTEST(conditional) ((conditional) ? TRUE : FALSE)

#endif

/* END */

#endif
