/***************************************

	Incremental tick Manager Class, Xbox 360 version

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brtick.h"

#if defined(BURGER_XBOX360)
#define NOD3D
#define NONET
#include <ppcintrinsics.h>
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

/***************************************

	\brief Constructor floating point timer

	Reads in the default data needed to maintain the timer
	and sets the elapsed time to 0.0f

	\sa GetTime(void) or Reset(void)

***************************************/

Burger::FloatTimer::FloatTimer() BURGER_NOEXCEPT: m_bPaused(FALSE)
{
	// QueryPerformanceFrequency() returns 50,000,000 however,
	// that's not really accurate. The real frequency is anywhere
	// between 49.875Mhz to 49.90Mhz due to differences between
	// runs of the XBox 360. This gets a much closer time to
	// real world seconds.

	// Get the frequency of the high precision timer
	m_dReciprocalFrequency = 1.0 / static_cast<double>(49875000);

	// Initialize the timer
	Reset();
}

/***************************************

	\brief Reset the timer

	Set m_uBaseTime to the current high precision time, however
	this function will not reset the elapsed time.

	\sa Reset(void)

***************************************/

void BURGER_API Burger::FloatTimer::SetBase(void) BURGER_NOEXCEPT
{
	// Read the counter
	uint64_t uTemp = __mftb();
	if (!static_cast<uint32_t>(uTemp)) {
		// Fix for a CPU error. If the lower 32 bits of the
		// counter is zero, the upper 32 bits has a 4 cycle window
		// where it's NOT properly incremented. By reading
		// a second time, the 4 second window will pass
		// and the 64 bit value is good.
		uTemp = __mftb();
	}
	m_uBaseTime = uTemp;
}

/*! ************************************

	\brief Read the timer in seconds

	Return the elapsed time in seconds from the last
	time this timer was reset. If the timer is paused, the
	value will be at the time mark when the pause was invoked.

	\sa Pause(void) or Reset(void)

***************************************/

float BURGER_API Burger::FloatTimer::GetTime(void) BURGER_NOEXCEPT
{
	float fResult;

	// If paused, just return the frozen elapsed time
	if (m_bPaused) {
		fResult = m_fElapsedTime;
	} else {

		// Windows XP or later will always succeed with this call

		// Read the counter
		uint64_t uMark = __mftb();
		if (!static_cast<uint32_t>(uMark)) {
			// Fix for a CPU error. If the lower 32 bits of the
			// counter is zero, the upper 32 bits has a 4 cycle window
			// where it's NOT properly incremented. By reading
			// a second time, the 4 second window will pass
			// and the 64 bit value is good.
			uMark = __mftb();
		}

		// Get the elapsed time

		uint64_t uElapsedTime = uMark - m_uBaseTime;
		m_uBaseTime = uMark;

		// Apply to seconds elapsed
		uElapsedTime += m_uElapsedTime;
		m_uElapsedTime = uElapsedTime;

		// Convert from integer to float, using a high precision integer
		// as the source to get around floating point imprecision.
		fResult = static_cast<float>(
			static_cast<double>(uElapsedTime) * m_dReciprocalFrequency);
		m_fElapsedTime = fResult;
	}
	return fResult;
}

#endif
