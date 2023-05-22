/***************************************

	Incremental tick Manager Class, Unix version

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brtick.h"

#if defined(BURGER_UNIX)
#include "unix_sleep.h"

#include <sys/time.h>
#include <time.h>

#if !defined(DOXYGEN)

// If CLOCK_MONOTONIC_RAW is available, prefer using it
#if defined(CLOCK_MONOTONIC_RAW)
// Found on Stadia, Android and most flavors of Linux
#define BURGER_CLOCK_MONOTONIC CLOCK_MONOTONIC_RAW
#else
#define BURGER_CLOCK_MONOTINIC CLOCK_MONOTONIC
#endif
#endif

/***************************************

	\brief Sleep the current thread

	On multithreaded systems, if \ref kSleepYield is passed to this function
	it will yield the thread's remaining time quantum.

	If \ref kSleepInfinite is passed then the thread will sleep
	forever unless an Remote Procedure Call or an I/O event occurs.
	Otherwise, pass the number of milliseconds that are desired
	for the thread to sleep. Zero is mapped to \ref kSleepYield.

	If bAlertable is \ref FALSE, the pause will not be interruptable.

	\param uMilliseconds \ref kSleepYield, \ref kSleepInfinite or number
		of non-zero milliseconds to sleep
	\param bAlertable \ref TRUE if the sleep can be interrupted

	\note On non-multithreaded systems, this function does nothing

***************************************/

void BURGER_API Burger::sleep_ms(
	uint32_t uMilliseconds, uint_t bAlertable) BURGER_NOEXCEPT
{
	timespec SleepTime;

	// Sleep in Microseconds, so, convert
	if (!uMilliseconds) {

		// Sleep for one tick, or it won't attempt to yield
		SleepTime.tv_nsec = 1;
		SleepTime.tv_sec = 0;

		// Is the number too high?
	} else if (uMilliseconds == 0xFFFFFFFFU) {
		SleepTime.tv_nsec = 999999999ULL;
		SleepTime.tv_sec = static_cast<time_t>(0x7FFFFFFFU);
	} else {
		// Convert milliseconds to nanoseconds
		uint64_t uNanoSeconds =
			static_cast<uint64_t>(uMilliseconds) * 1000000ULL;

		SleepTime.tv_nsec = static_cast<long>(uNanoSeconds % 1000000000ULL);
		SleepTime.tv_sec = static_cast<time_t>(uNanoSeconds / 1000000000ULL);
	}

	Unix::nanosleep_delay(&SleepTime, bAlertable);
}

/***************************************

	\brief Initialize the low level timer manager

	Start up the low level timer

	\sa shutdown()

***************************************/

#if !defined(BURGER_DARWIN)
void BURGER_API Burger::Tick::init(void) BURGER_NOEXCEPT
{
	Tick* pThis = &g_Tick;
	if (!pThis->m_bInitialized) {

		// IMPORTANT!! m_bHasMonotonicClock MUST be initialized first, or the
		// code below it will fail because of mismatched clock timing.
		timespec ClockCheck;
		if (clock_gettime(BURGER_CLOCK_MONOTONIC, &ClockCheck)) {
			pThis->m_bHasMonotonicClock = FALSE;
		} else {
			pThis->m_bHasMonotonicClock = TRUE;
		}

		// Get the speed of the most accurate timer
		pThis->m_uHighPrecisionFrequency = get_high_precision_rate();
		pThis->m_uLast60HertzMark = 1;

		// Init the sub-timers
		pThis->m_60Hertz.init(kTicksPerSecond);
		pThis->m_1KHertz.init(1000U);
		pThis->m_1MHertz.init(1000000U);

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
	// If it has a high precision clock, use it
	if (g_Tick.m_bHasMonotonicClock) {
		return 1000000000U;
	}
	return 1000000U;
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
	if (pThis->m_bHasMonotonicClock) {

		// Read the nanosecond clock
		timespec CurrentTime;
		clock_gettime(BURGER_CLOCK_MONOTONIC, &CurrentTime);

		// Note: tv_nsec is in the range of 0-999999999 which fits in a uint32_t
		return (static_cast<uint64_t>(CurrentTime.tv_sec) * 1000000000ULL) +
			static_cast<uint32_t>(CurrentTime.tv_nsec);
	}

	timeval CurrentTimeVal;
	gettimeofday(&CurrentTimeVal, nullptr);

	// Note: tv_usec is in the range of 0-999999 which fits in a uint32_t
	return (static_cast<uint64_t>(CurrentTimeVal.tv_sec) * 1000000ULL) +
		static_cast<uint32_t>(CurrentTimeVal.tv_usec);
}
#endif

#endif
