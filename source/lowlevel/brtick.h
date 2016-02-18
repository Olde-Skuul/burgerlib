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

class FloatTimer {
#if (defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
	double m_dReciprocalFrequency;	///< 1.0 / CPU timebase
#endif
#if (defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || defined(BURGER_MACOSX) || defined(BURGER_IOS) || defined(BURGER_ANDROID)) || defined(DOXYGEN)
	Word64 m_uBaseTime;			///< Integer time mark of the last read time in high precision
	Word64 m_uElapsedTime;		///< Integer time mark of the elapsed time in high precision
#if defined(BURGER_ANDROID) || defined(DOXYGEN)
	Word64 m_uBaseTimeNano;		///< Nanosecond time for time mark
	Word64 m_uElapsedTimeNano;	///< Nanosecond time for elapsed time
#endif
#endif
#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || defined(BURGER_MACOSX) || defined(BURGER_IOS) || defined(BURGER_ANDROID)) || defined(DOXYGEN)
	Word32 m_uBaseTime;			///< Microsecond mark of the last read time
#endif
	float m_fElapsedTime;		///< Last read time
	Word m_bPaused;				///< \ref TRUE if this timer is currently paused
	void BURGER_API SetBase(void);
public:
	FloatTimer();
	BURGER_INLINE float GetLastTime(void) const { return m_fElapsedTime; }
	BURGER_INLINE Word IsPaused(void) const { return m_bPaused; }
	void BURGER_API Reset(void);
	float BURGER_API GetTime(void);
	void BURGER_API Pause(void);
	void BURGER_API Unpause(void);
};

enum {
	SLEEP_YIELD=0,					///< Used by Sleep(Word32) to yield the current time quantum
	SLEEP_INFINITE=0xFFFFFFFFU		///< Used by Sleep(Word32) for an infinite time delay
};
extern void BURGER_API Sleep(Word32 uMilliseconds);
}
/* END */

#endif
