/***************************************

	Incremental tick Manager Class, Xbox Classic version

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brtick.h"

#if defined(BURGER_XBOX)
#define NOD3D
#define NONET
#define NODSOUND
#include <xtl.h>

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
	if (uMilliseconds) {
		// Sleep until the time expires or something
		// occurs that could cause the main thread to take notice
		// like a I/O service routine
		::SleepEx(uMilliseconds, bAlertable);
	} else {
		SwitchToThread();
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
	// Get the time mark
	LARGE_INTEGER Freq;
	QueryPerformanceFrequency(&Freq);

	return static_cast<uint64_t>(Freq.QuadPart);
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
	LARGE_INTEGER NewTick;
	QueryPerformanceCounter(&NewTick);

	return static_cast<uint64_t>(NewTick.QuadPart);
}

#endif
