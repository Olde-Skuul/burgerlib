/***************************************

	Incremental tick Manager Class, Playstation 3 version

	Copyright (c) 2015-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brtick.h"

#if defined(BURGER_PS3)
#include <sys/sys_time.h>
#include <sys/time_util.h>
#include <sys/timer.h>

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
	uint32_t uMilliseconds, uint_t /* bAlertable */) BURGER_NOEXCEPT
{
	usecond_t uMicroseconds;
	// Sleep in Microseconds, so, convert
	if (!uMilliseconds) {
		// Sleep for one tick, or it won't attempt to yield
		uMicroseconds = 1;
	} else {
		// Convert milliseconds to Microseconds
		uMicroseconds = static_cast<usecond_t>(uMilliseconds) * 1000U;
	}
	sys_timer_usleep(uMicroseconds);
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
	return sys_time_get_timebase_frequency();
}

/***************************************

	\brief Return the tick at the system's highest precision

	The value returns a tick that will increment at
	get_high_precision_frequency() ticks per second.

	\sa get_high_precision_frequency()

***************************************/

uint64_t BURGER_API Burger::Tick::read_high_precision(void) BURGER_NOEXCEPT
{
	// Get the time mark
	register uint64_t uNewTick;
	SYS_TIMEBASE_GET(uNewTick);

	return uNewTick;
}

#endif
