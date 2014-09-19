/***************************************

	Debug manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

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
class Debug {
public:
	static Word BURGER_ANSIAPI Fatal(const char *pMessage,...);
	static void BURGER_ANSIAPI Warning(const char *pMessage,...);
	static void BURGER_ANSIAPI Message(const char *pMessage,...);
	static void BURGER_API String(const char *pMessage);
	static void BURGER_API String(Word32 uInput);
	static void BURGER_API String(Word64 uInput);
	static Word BURGER_API IsDebuggerPresent(void);
};
extern void BURGER_API OkAlertMessage(const char *pMessage,const char *pTitle=NULL);
extern Word BURGER_API OkCancelAlertMessage(const char *pMessage,const char *pTitle=NULL);
}
/* END */

#endif
