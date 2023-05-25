/***************************************

	Incremental tick Manager Class, Apple Darwin version

	Copyright (c) 2015-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brtick.h"

#if defined(BURGER_DARWIN)
#include <mach/mach_time.h>

/***************************************

	\brief Initialize the low level timer manager

	Start up the low level timer

	\sa shutdown()

***************************************/

void BURGER_API Burger::Tick::init(void) BURGER_NOEXCEPT
{
	Tick* pThis = &g_Tick;
	if (!pThis->m_bInitialized) {
		// Get the speed of the most accurate timer
		pThis->m_uHighPrecisionFrequency = get_high_precision_rate();
		pThis->m_uLast60HertzMark = 1;

		// Init the sub-timers
		pThis->m_60Hertz.init(kTicksPerSecond);
		pThis->m_1KHertz.init(1000U);
		pThis->m_1MHertz.init(1000000U);

		// Get the Darwin specific data
		mach_timebase_info_data_t MyInfo;
		mach_timebase_info(&MyInfo);
		pThis->m_uNumerator = MyInfo.numer;
		pThis->m_uDenominator = MyInfo.denom;
		pThis->m_uTickHighPrecisionMark = mach_absolute_time();
		pThis->m_uTickHighPrecisionDelta = 0;
		pThis->m_uTickHighPrecision = 1;

		pThis->m_bInitialized = TRUE;
	}
}

/***************************************

	\brief Return the ticks per second at the system's highest precision

	This platform specific code will ask the operating system what is the
	highest precision timer tick rate and then will return that value.

	This value is cached and is available from get_high_precision_frequency()

	\sa get_high_precision_frequency(), or read_high_precision()

***************************************/

uint64_t BURGER_API Burger::Tick::get_high_precision_rate(void) BURGER_NOEXCEPT
{
	// Nanoseconds
	return 1000000000U;
}

/***************************************

	\brief Return the tick at the system's highest precision

	The value returns a tick that will increment at
	get_high_precision_frequency() ticks per second.

	\sa get_high_precision_frequency()

***************************************/

uint64_t BURGER_API Burger::Tick::read_high_precision(void) BURGER_NOEXCEPT
{
	Tick* pThis = &g_Tick;
	uint64_t uTick = mach_absolute_time();

	// If it needs conversion to nanoseconds, do so.
	if (pThis->m_uNumerator != pThis->m_uDenominator) {

		// Get the amount of time that has elapsed from the time mark, scaled
		// for m_uNumerator from Darwin
		const uint64_t uNewTick = uTick;
		const uint64_t uElapsed =
			((uNewTick - pThis->m_uTickHighPrecisionMark) *
				pThis->m_uNumerator) +
			pThis->m_uTickHighPrecisionDelta;

		// Save the current high precision tick for the next pass
		pThis->m_uTickHighPrecisionMark = uTick;

		// Divide by ticks per second to get the ticks elapsed
		// Note: Since the value is pre-scaled, it's in ticks at the rate of
		// interest
		const uint64_t uNewTicks = uElapsed / pThis->m_uDenominator;

		// Perform a modulo to capture the excess high frequency ticks for the
		// next pass
		pThis->m_uTickHighPrecisionDelta =
			uElapsed - (uNewTicks * pThis->m_uDenominator);

		// Add in the elapsed ticks to the master counter
		// It's done this way to ensure wraparound is handled properly
		uTick = pThis->m_uTickHighPrecision + uNewTicks;
		pThis->m_uTickHighPrecision = uTick;
	}
	return uTick;
}
#endif
