/***************************************

	Incremental tick Manager Class, Windows version

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brtick.h"

#if defined(BURGER_WINDOWS)
#include "brglobals.h"
#include "win_winmm.h"

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include <mmsystem.h>

#if defined(BURGER_WATCOM)
// Needed for timeBeginPeriod
#pragma library("Winmm.lib")
#else
#pragma comment(lib, "Winmm.lib")
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
	// Sleep until the time expires or something
	// occurs that could cause the main thread to take notice
	// like a I/O service routine
	::SleepEx(uMilliseconds, static_cast<BOOL>(bAlertable));
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

	Wait for a number of system ticks

***************************************/

void BURGER_API Burger::Tick::wait(uint_t uCount) BURGER_NOEXCEPT
{
	// Read the timer
	uint32_t uNewTick = read();

	// Should I wait?
	if ((uNewTick - g_Tick.m_uLast60HertzMark) <
		static_cast<uint32_t>(uCount)) {
		do {
			// Sleep until a tick occurs
			WaitMessage();
			// Call the system task if needed
			// KeyboardKbhit();
			// Read in the current time tick
			uNewTick = read();
			// Time has elapsed?
		} while ((uNewTick - g_Tick.m_uLast60HertzMark) <
			static_cast<uint32_t>(uCount));
	}
	// Mark the time
	g_Tick.m_uLast60HertzMark = uNewTick;
}

/***************************************

	\brief Constructor floating point timer

	Reads in the default data needed to maintain the timer
	and sets the elapsed time to 0.0f

	\sa GetTime(void) or Reset(void)

***************************************/

Burger::FloatTimer::FloatTimer() BURGER_NOEXCEPT: m_bPaused(FALSE)
{
	// Get the frequency of the high precision timer

	// No need to test for error, since BurgerLib requires Windows XP or higher,
	// this function is guaranteed to succeed

	LARGE_INTEGER uFrequency;
	QueryPerformanceFrequency(&uFrequency);
	m_dReciprocalFrequency = 1.0 / static_cast<double>(uFrequency.QuadPart);

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
	QueryPerformanceCounter(
		static_cast<LARGE_INTEGER*>(static_cast<void*>(&m_uBaseTime)));
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

		LARGE_INTEGER uTick;
		QueryPerformanceCounter(&uTick);

		// Here is a HUGE gotcha!!! According to my
		// disassembly of QueryPerformanceCounter(), it's NOT guaranteed
		// to be returning a fully 64 bit value. On one machine, the top
		// 10 bits were always zero, this causes issues in the very
		// rare occasion of value wraparound. How rare? Doing the math,
		// at 3.2 million ticks a second, a 54 bit value wraps in 178 years,
		// assuming the timer started at zero. Since it's a bad idea
		// to assume the timer started at zero, and it's also not
		// a good idea to assume the timer is fully 64 bit, a sanity
		// check for wrap around is in order.

		// Blame Microsoft.

		uint64_t uElapsedTime;

		uint64_t uMark = static_cast<uint64_t>(uTick.QuadPart);
		if (uMark < m_uBaseTime) {

			// The timer wrapped around.

			// Start with the wrapped value, this is the new "zero"
			uElapsedTime = uMark;

			// Discard the time that was "wrapped" because without any knowledge
			// exactly where it considered a wrap around point (It can't be
			// assumed that the wrap around point was on a power of 2), this
			// excess time will be dropped on the floor. Since the amount of
			// lost time is usually less than a second (1/60th of a second is
			// typical), it's an acceptable compromise especially at the rarity
			// of the wrap around case.

		} else {
			// 99.99999% of the time, this is the code executed
			uElapsedTime = uMark - m_uBaseTime;
		}
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
