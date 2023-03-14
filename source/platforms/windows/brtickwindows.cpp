/***************************************

	Incremental tick Manager Class

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brtick.h"

#if defined(BURGER_WINDOWS)
#include "brglobals.h"

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

/***************************************

	Use timeGetTime() as the master timer.

***************************************/

/***************************************

	Read the current system tick value
	in 60hz ticks

***************************************/

static uint_t g_b60HertzTimerStarted = FALSE;
static uint32_t g_u60HertzTick;         // Tick value
static uint32_t g_u60HertzMSTime;       // 1000ms time
static uint32_t g_u60HertzTickFraction; // 3000/60 time fraction

uint32_t BURGER_API Burger::Tick::Read(void) BURGER_NOEXCEPT
{
	const uint32_t uMark = Windows::timeGetTime();
	uint32_t uTick;
	// Never initialized?
	if (!g_b60HertzTimerStarted) {
		g_b60HertzTimerStarted = TRUE;
		g_u60HertzMSTime = uMark;
		g_u60HertzTickFraction = 0;
		g_u60HertzTick = 1;
		uTick = 1;
	} else {
		// Get the elapsed time in Millisecond
		const uint32_t uElapsed = uMark - g_u60HertzMSTime;
		uTick = g_u60HertzTick;
		if (uElapsed) {
			// Update the time mark
			g_u60HertzMSTime = uMark;
			// Convert 1000 ticks per second to 60 while
			// trying to avoid losing any precision

			// Convert to 3000 ticks per second, so it's evenly divided by 60
			const uint32_t uFraction = g_u60HertzTickFraction + (uElapsed * 3U);

			// Get the 60hz elapsed time
			uTick = uTick + (uFraction / (3000U / 60U));

			// Keep the fraction
			g_u60HertzTickFraction = uFraction % (3000U / 60U);
			g_u60HertzTick = uTick;
		}
	}
	return uTick;
}

/***************************************

	Wait for a number of system ticks

***************************************/

void BURGER_API Burger::Tick::Wait(uint_t uCount) BURGER_NOEXCEPT
{
	// Handle any pending events
	// KeyboardKbhit();
	// Read the timer
	uint32_t uNewTick = Read();
	// Should I wait?
	if ((uNewTick - g_uLastTick) < static_cast<uint32_t>(uCount)) {
		do {
			// Sleep until a tick occurs
			WaitMessage();
			// Call the system task if needed
			// KeyboardKbhit();
			// Read in the current time tick
			uNewTick = Read();
			// Time has elapsed?
		} while ((uNewTick - g_uLastTick) < static_cast<uint32_t>(uCount));
	}
	// Mark the time
	g_uLastTick = uNewTick;
}

/***************************************

	Read the time in microsecond increments

***************************************/

static uint_t g_bStartedMicroseconds; /* Got the frequency */
static double g_dWinTicks = 1.0;      /* Frequency adjust */

uint32_t BURGER_API Burger::Tick::ReadMicroseconds(void) BURGER_NOEXCEPT
{
	LARGE_INTEGER Temp;
	// Is the divisor initialized?
	if (!g_bStartedMicroseconds) {
		g_bStartedMicroseconds = TRUE;
		// Get the constants
		if (QueryPerformanceFrequency(&Temp)) {
			// Timer change
			g_dWinTicks = 1000000.0 / static_cast<double>(Temp.QuadPart);
		}
	}

	// Get the timer from Win95
	if (QueryPerformanceCounter(&Temp)) {
		// Save the result
		return static_cast<uint32_t>(
			static_cast<double>(Temp.QuadPart) * g_dWinTicks);
	}
	// Just zap it! (Error)
	return 0;
}

/***************************************

	Read the time in millisecond increments

***************************************/

uint32_t BURGER_API Burger::Tick::ReadMilliseconds(void) BURGER_NOEXCEPT
{
	// Call windows 95/NT
	return Windows::timeGetTime();
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

/***************************************

	Sleep the current thread

***************************************/

void BURGER_API Burger::Sleep(uint32_t uMilliseconds) BURGER_NOEXCEPT
{
	// Sleep until the time expires or something
	// occurs that could cause the main thread to take notice
	// like a I/O service routine
	::SleepEx(uMilliseconds, TRUE);
}

#endif
