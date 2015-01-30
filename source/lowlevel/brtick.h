/***************************************

	Incremental tick Manager Class

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRTICK_H__
#define __BRTICK_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
class Tick {
	static Word32 s_LastTick;				///< Previously set tick value
public:
	static const Word TICKSPERSEC=60;		///< Number of ticks per second
	static Word32 BURGER_API Read(void);
	static BURGER_INLINE Word32 ResetLastTick(void) { Word32 uTick=Read(); s_LastTick=uTick; return uTick; }
	static BURGER_INLINE Word32 GetLastTick(void) { return s_LastTick; }
	static BURGER_INLINE void WaitOneTick(void) { s_LastTick=Read(); Wait(); }
	static void BURGER_API Wait(Word uCount=1);
	static Word BURGER_API WaitEvent(Word uCount=0);
	static Word32 BURGER_API ReadMicroseconds(void);
	static Word32 BURGER_API ReadMilliseconds(void);
};
enum {
	SLEEP_YIELD=0,					///< Used by Sleep(Word32) to yield the current time quantum
	SLEEP_INFINITE=0xFFFFFFFFU		///< Used by Sleep(Word32) for an infinite time delay
};
extern void BURGER_API Sleep(Word32 uMilliseconds);
}
/* END */

#endif
