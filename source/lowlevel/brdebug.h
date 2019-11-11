/***************************************

	Debug manager

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRDEBUG_H__
#define __BRDEBUG_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {

namespace Debug {
extern Word BURGER_ANSIAPI Fatal(const char *pMessage,...);
extern void BURGER_ANSIAPI Warning(const char *pMessage,...);
extern void BURGER_ANSIAPI Message(const char *pMessage,...);
extern void BURGER_API PrintString(const char *pMessage);
extern void BURGER_API PrintString(Word32 uInput);
extern void BURGER_API PrintString(Word64 uInput);
#if defined(BURGER_NEED_UINTPTR_REMAP)
BURGER_INLINE void PrintString(uintptr_t uInput) { PrintString(static_cast<BURGER_NEED_UINTPTR_REMAP>(uInput)); }
#endif
extern Word BURGER_API IsDebuggerPresent(void);
extern void BURGER_API PrintErrorMessage(Word uErrorCode);
}

extern void BURGER_API OkAlertMessage(const char *pMessage,const char *pTitle=NULL);
extern Word BURGER_API OkCancelAlertMessage(const char *pMessage,const char *pTitle=NULL);

}
/* END */

#endif
