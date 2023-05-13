/***************************************

	Incremental tick Manager Class

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

enum {
	/** Used by sleep_ms(uint32_t) to yield the current time quantum */
	kSleepYield = 0,

	/** Used by sleep_ms(uint32_t) for an infinite time delay */
	kSleepInfinite = 0xFFFFFFFFU
};

extern void BURGER_API sleep_ms(
	uint32_t uMilliseconds, uint_t bAlertable = FALSE) BURGER_NOEXCEPT;

class Tick {

	struct Scaler_t {
		/** Time mark for scaled hertz tick */
		uint64_t m_uMark;

		/** Pending time delta tick */
		uint64_t m_uDelta;

		/** Current tick */
		uint32_t m_uTick;

		/** Desired tick rate 60, 1000, 1000000 */
		uint32_t m_uDesiredRate;

		void BURGER_API init(uint32_t uDesiredRate) BURGER_NOEXCEPT;
		uint32_t BURGER_API read(void) BURGER_NOEXCEPT;
	};

public:
	/** Number of ticks per second */
	static const uint32_t kTicksPerSecond = 60U;

	/** High precision timer resolution in units per second */
	uint64_t m_uHighPrecisionFrequency;

	/** Previously set 60 hertz tick value */
	uint32_t m_uLast60HertzMark;

	/** \ref TRUE if initialized */
	uint_t m_bInitialized;

	// Platform specific variables
#if !defined(DOXYGEN)

	// MSDos only
#if defined(BURGER_MSDOS)
	/** Old INT 8 vector \msdosonly  */
	void(__interrupt __far* m_pPreviousINT8)();

	/** System units between interrupts in 1192030UL per second units \msdosonly
	 */
	volatile uint32_t m_uStepUnits;

	/** Cumulative units elapsed since the last interrupt, counted up to
	 * 0x10000U for original timer \msdosonly */
	volatile uint32_t m_uDelta;

	/** Increment every 1/60th of a second by an interrupt \msdosonly */
	volatile uint32_t m_u60HertzTick;

	// MSDos X32 Dos extender only
#if defined(BURGER_X32) || defined(DOXYGEN)
	/** Old real mode INT 8 vector X32 \msdosonly */
	uint32_t m_uPreviousRealService;
#endif
#endif

#if defined(BURGER_DARWIN)
	/** Numerator from mach_base_info */
	uint32_t m_uNumerator;

	/** Denominator from mach_base_info */
	uint32_t m_uDenominator;

	/** Last read high precision value */
	uint64_t m_uTickHighPrecisionMark;

	/** Rounding leftovers past mark */
	uint64_t m_uTickHighPrecisionDelta;

	/** Tick value */
	uint64_t m_uTickHighPrecision;

#elif defined(BURGER_UNIX)
	/** Android and Linux needs to test for monotonic clock */
	uint_t m_bHasMonotonicClock;
	
#elif defined(BURGER_MAC) && defined(BURGER_PPC)
	/** Which type of timer is being used for high precision */
	uint32_t m_uMethod;
	
	/** Pointer to UpTime() if available */
	void *m_pUpTime;
#endif

#if !(defined(BURGER_MSDOS) || defined(BURGER_MAC))
	/** Tick scaler for 60 hertz */
	Scaler_t m_60Hertz;
#endif

	/** Tick scaler for 1KHz */
	Scaler_t m_1KHertz;

#if !(defined(BURGER_MAC) && defined(BURGER_68K))
	/** Tick scaler for 1Mhz */
	Scaler_t m_1MHertz;
#endif

#endif

protected:
	/** Global instance of all data for timers */
	static Tick g_Tick;

public:
	static BURGER_INLINE Tick* get_instance(void) BURGER_NOEXCEPT
	{
		return &g_Tick;
	}

	static void BURGER_API init(void) BURGER_NOEXCEPT;
	static void BURGER_API shutdown(void) BURGER_NOEXCEPT;

	static BURGER_INLINE uint64_t get_high_precision_frequency(
		void) BURGER_NOEXCEPT
	{
		return g_Tick.m_uHighPrecisionFrequency;
	}

	static BURGER_INLINE uint_t is_initialized(void) BURGER_NOEXCEPT
	{
		return g_Tick.m_bInitialized;
	}

protected:
	static uint64_t BURGER_API get_high_precision_rate(void) BURGER_NOEXCEPT;

public:
	static uint64_t BURGER_API read_high_precision(void) BURGER_NOEXCEPT;

	static uint32_t BURGER_API read(void) BURGER_NOEXCEPT;
	static uint32_t BURGER_API read_ms(void) BURGER_NOEXCEPT;
	static uint32_t BURGER_API read_us(void) BURGER_NOEXCEPT;
	static uint32_t BURGER_API read_and_mark(void) BURGER_NOEXCEPT;

	static BURGER_INLINE uint32_t get_mark(void) BURGER_NOEXCEPT
	{
		return g_Tick.m_uLast60HertzMark;
	}

	static void BURGER_API wait_one_tick(void) BURGER_NOEXCEPT;
	static void BURGER_API wait(uint_t uCount = 1) BURGER_NOEXCEPT;
	static uint_t BURGER_API wait_event(uint_t uCount = 0) BURGER_NOEXCEPT;
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

}
/* END */

#endif
