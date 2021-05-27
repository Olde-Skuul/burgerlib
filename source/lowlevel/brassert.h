/***************************************

    Assert redirection class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
	typedef int (BURGER_API *CallbackProc)(void *pThis,const char *pCondition,const char *pFilename,uint_t uLineNumber);
	
	CallbackProc m_pCallback;		///< Function pointer to the redirected Assert function
	void *m_pThis;					///< "this" pointer passed to the m_pProcAssert function

	static Assert_t g_Instance;		///< Global instance of the Assert redirection

	void BURGER_API SetCallback(CallbackProc pCallback,void *pThis);
	static int BURGER_API DefaultAssert(void *pThis,const char *pCondition,const char *pFilename,uint_t uLineNumber);
};

extern int BURGER_API Assert(const char *pCondition,const char *pFilename,uint_t uLineNumber);
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

/* END */

#endif
