/***************************************

    Debug manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

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
extern uint_t BURGER_ANSIAPI Fatal(const char *pMessage,...);
extern void BURGER_ANSIAPI Warning(const char *pMessage,...);
extern void BURGER_ANSIAPI Message(const char *pMessage,...);
extern void BURGER_API PrintString(const char *pMessage) BURGER_NOEXCEPT;
extern void BURGER_API PrintString(uint32_t uInput) BURGER_NOEXCEPT;
extern void BURGER_API PrintString(uint64_t uInput) BURGER_NOEXCEPT;
#if defined(BURGER_NEED_UINTPTR_REMAP)
BURGER_INLINE void PrintString(uintptr_t uInput) BURGER_NOEXCEPT { PrintString(static_cast<BURGER_NEED_UINTPTR_REMAP>(uInput)); }
#endif
extern uint_t BURGER_API IsDebuggerPresent(void) BURGER_NOEXCEPT;
extern void BURGER_API PrintErrorMessage(uint_t uErrorCode);
}

extern void BURGER_API OkAlertMessage(const char *pMessage,const char *pTitle=NULL);
extern uint_t BURGER_API OkCancelAlertMessage(const char *pMessage,const char *pTitle=NULL);

}
/* END */

#endif
