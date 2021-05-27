/***************************************

    Incremental tick Manager Class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

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
	static uint32_t s_LastTick;				///< Previously set tick value
public:
	static const uint_t TICKSPERSEC=60;		///< Number of ticks per second
	static uint32_t BURGER_API Read(void) BURGER_NOEXCEPT;
	static BURGER_INLINE uint32_t ResetLastTick(void) BURGER_NOEXCEPT { const uint32_t uTick=Read(); s_LastTick=uTick; return uTick; }
	static BURGER_INLINE uint32_t GetLastTick(void) BURGER_NOEXCEPT { return s_LastTick; }
	static BURGER_INLINE void WaitOneTick(void) { s_LastTick=Read(); Wait(); }
	static void BURGER_API Wait(uint_t uCount=1);
	static uint_t BURGER_API WaitEvent(uint_t uCount=0);
	static uint32_t BURGER_API ReadMicroseconds(void);
	static uint32_t BURGER_API ReadMilliseconds(void);
};

class FloatTimer {
#if (defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
	double m_dReciprocalFrequency;	///< 1.0 / CPU timebase
#endif
#if (defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || defined(BURGER_MACOSX) || defined(BURGER_IOS) || defined(BURGER_ANDROID)) || defined(DOXYGEN)
	uint64_t m_uBaseTime;			///< Integer time mark of the last read time in high precision
	uint64_t m_uElapsedTime;		///< Integer time mark of the elapsed time in high precision
#if defined(BURGER_ANDROID) || defined(DOXYGEN)
	uint64_t m_uBaseTimeNano;		///< Nanosecond time for time mark
	uint64_t m_uElapsedTimeNano;	///< Nanosecond time for elapsed time
#endif
#endif
#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || defined(BURGER_MACOSX) || defined(BURGER_IOS) || defined(BURGER_ANDROID)) || defined(DOXYGEN)
	uint32_t m_uBaseTime;			///< Microsecond mark of the last read time
#endif
	float m_fElapsedTime;		///< Last read time
	uint_t m_bPaused;				///< \ref TRUE if this timer is currently paused
	void BURGER_API SetBase(void);
public:
	FloatTimer();
	BURGER_INLINE float GetLastTime(void) const { return m_fElapsedTime; }
	BURGER_INLINE uint_t IsPaused(void) const { return m_bPaused; }
	void BURGER_API Reset(void);
	float BURGER_API GetTime(void) BURGER_NOEXCEPT;
	void BURGER_API Pause(void);
	void BURGER_API Unpause(void);
};

enum {
	SLEEP_YIELD=0,					///< Used by Sleep(uint32_t) to yield the current time quantum
	SLEEP_INFINITE=0xFFFFFFFFU		///< Used by Sleep(uint32_t) for an infinite time delay
};
extern void BURGER_API Sleep(uint32_t uMilliseconds);
}
/* END */

#endif
