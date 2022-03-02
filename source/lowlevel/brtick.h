/***************************************

	Incremental tick Manager Class

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
	/** Previously set tick value */
	static uint32_t g_uLastTick;

public:
	/** Number of ticks per second */
	static const uint_t TICKSPERSEC = 60;
	static uint32_t BURGER_API Read(void) BURGER_NOEXCEPT;
	static BURGER_INLINE uint32_t ResetLastTick(void) BURGER_NOEXCEPT
	{
		const uint32_t uTick = Read();
		g_uLastTick = uTick;
		return uTick;
	}
	static BURGER_INLINE uint32_t GetLastTick(void) BURGER_NOEXCEPT
	{
		return g_uLastTick;
	}
	static BURGER_INLINE void WaitOneTick(void) BURGER_NOEXCEPT
	{
		g_uLastTick = Read();
		Wait();
	}
	static void BURGER_API Wait(uint_t uCount = 1) BURGER_NOEXCEPT;
	static uint_t BURGER_API WaitEvent(uint_t uCount = 0) BURGER_NOEXCEPT;
	static uint32_t BURGER_API ReadMicroseconds(void) BURGER_NOEXCEPT;
	static uint32_t BURGER_API ReadMilliseconds(void) BURGER_NOEXCEPT;
};

class FloatTimer {
#if (defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || \
	defined(BURGER_MACOSX) || defined(BURGER_IOS)) || \
	defined(DOXYGEN)
	double m_dReciprocalFrequency; ///< 1.0 / CPU timebase
#endif
#if (defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || \
	defined(BURGER_MACOSX) || defined(BURGER_IOS) || \
	defined(BURGER_ANDROID)) || \
	defined(DOXYGEN)
	/** Integer time mark of the last read time in high precision */
	uint64_t m_uBaseTime;
	/** Integer time mark of the elapsed time in high precision */
	uint64_t m_uElapsedTime;

#if defined(BURGER_ANDROID) || defined(DOXYGEN)
	/** Nanosecond time for time mark */
	uint64_t m_uBaseTimeNano;
	/** Nanosecond time for elapsed time */
	uint64_t m_uElapsedTimeNano;
#endif
#endif
#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || \
	defined(BURGER_MACOSX) || defined(BURGER_IOS) || \
	defined(BURGER_ANDROID)) || \
	defined(DOXYGEN)
	/** Microsecond mark of the last read time */
	uint32_t m_uBaseTime;
#endif
	/** Last read time */
	float m_fElapsedTime;
	/** \ref TRUE if this timer is currently paused */
	uint_t m_bPaused;
	void BURGER_API SetBase(void) BURGER_NOEXCEPT;

public:
	FloatTimer() BURGER_NOEXCEPT;
	BURGER_INLINE float GetLastTime(void) const BURGER_NOEXCEPT
	{
		return m_fElapsedTime;
	}
	BURGER_INLINE uint_t IsPaused(void) const BURGER_NOEXCEPT
	{
		return m_bPaused;
	}
	void BURGER_API Reset(void) BURGER_NOEXCEPT;
	float BURGER_API GetTime(void) BURGER_NOEXCEPT;
	void BURGER_API Pause(void) BURGER_NOEXCEPT;
	void BURGER_API Unpause(void) BURGER_NOEXCEPT;
};

enum {
	/** Used by Sleep(uint32_t) to yield the current time quantum */
	SLEEP_YIELD = 0,
	/** Used by Sleep(uint32_t) for an infinite time delay */
	SLEEP_INFINITE = 0xFFFFFFFFU
};

extern void BURGER_API Sleep(uint32_t uMilliseconds) BURGER_NOEXCEPT;
}
/* END */

#endif
