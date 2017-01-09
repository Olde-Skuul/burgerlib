/***************************************

	Incremental tick Manager Class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brtick.h"

#if defined(BURGER_WINDOWS)
#include "brglobals.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

/***************************************

	I create a thread that gets CPU time about 60 times
	a second, but since I can't rely on this I then
	use GetTickCount() to get the TRUE elapsed time

***************************************/

/***************************************

	Read the current system tick value
	in 60hz ticks

***************************************/

static Word g_b60HertzTimerStarted = FALSE;
static Word32 g_u60HertzTick;		// Tick value
static Word32 g_u60HertzMSTime;		// 1000ms time
static Word32 g_u60HertzTickFraction;	// 3000/60 time fraction

Word32 BURGER_API Burger::Tick::Read(void)
{
	Word32 uMark = Globals::timeGetTime();
	Word32 uTick;
	if (!g_b60HertzTimerStarted) {			// Never initialized?
		g_b60HertzTimerStarted = TRUE;
		g_u60HertzMSTime = uMark;
		g_u60HertzTickFraction = 0;
		g_u60HertzTick = 1;
		uTick = 1;
	} else {
		// Get the elapsed time in Millisecond
		Word32 uElapsed = uMark-g_u60HertzMSTime;
		uTick = g_u60HertzTick;
		if (uElapsed) {
			// Update the time mark
			g_u60HertzMSTime = uMark;
			// Convert 1000 ticks per second to 60 while 
			// trying to avoid losing any precision

			// Convert to 3000 ticks per second, so it's evenly divided by 60
			Word32 uFraction = g_u60HertzTickFraction + (uElapsed*3U);

			// Get the 60hz elapsed time
			uTick = uTick + (uFraction/(3000U/60U));

			// Keep the fraction
			g_u60HertzTickFraction = uFraction % (3000U/60U);
			g_u60HertzTick = uTick;
		}
	}
	return uTick;
}

/***************************************

	Wait for a number of system ticks

***************************************/

void BURGER_API Burger::Tick::Wait(Word uCount)
{
	//KeyboardKbhit();				/* Handle any pending events */
	Word32 NewTick = Read();	/* Read the timer */
	if ((NewTick-s_LastTick)<(Word32)uCount) {	/* Should I wait? */
		do {
			WaitMessage();		/* Sleep until a tick occurs */
			//KeyboardKbhit();			/* Call the system task if needed */
			NewTick = Read();	/* Read in the current time tick */
		} while ((NewTick-s_LastTick)<(Word32)uCount);	/* Time has elapsed? */
	}
	s_LastTick = NewTick;		/* Mark the time */
}


/***************************************

	Read the time in microsecond increments

***************************************/

static Word g_bStartedMicroseconds;	/* Got the frequency */
static double g_dWinTicks = 1.0;	/* Frequency adjust */

Word32 BURGER_API Burger::Tick::ReadMicroseconds(void)
{
	LARGE_INTEGER Temp;
	if (!g_bStartedMicroseconds) {		/* Is the divisor initialized? */
		g_bStartedMicroseconds = TRUE;
		if (QueryPerformanceFrequency(&Temp)) {	/* Get the constants */
			g_dWinTicks = 1000000.0/(double)Temp.QuadPart;	/* Timer change */
		}
	}
	if (QueryPerformanceCounter(&Temp)) {	/* Get the timer from Win95 */
		return (Word32)((double)Temp.QuadPart*g_dWinTicks);	/* Save the result */
	}
	return 0;		/* Just zap it! (Error) */
}

/***************************************

	Read the time in millisecond increments

***************************************/

Word32 BURGER_API Burger::Tick::ReadMilliseconds(void)
{
	return Globals::timeGetTime();		/* Call windows 95/NT */
}

/***************************************

	\brief Constructor floating point timer
	
	Reads in the default data needed to maintain the timer
	and sets the elapsed time to 0.0f

	\sa GetTime(void) or Reset(void)

***************************************/

Burger::FloatTimer::FloatTimer() :
	m_bPaused(FALSE)
{
	// Get the frequency of the high precision timer

	// No need to test for error, since BurgerLib requires Windows XP or higher, this function
	// is guaranteed to succeed

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

void BURGER_API Burger::FloatTimer::SetBase(void)
{
	QueryPerformanceCounter(static_cast<LARGE_INTEGER *>(static_cast<void *>(&m_uBaseTime)));
}

/*! ************************************

	\brief Read the timer in seconds

	Return the elapsed time in seconds from the last
	time this timer was reset. If the timer is paused, the
	value will be at the time mark when the pause was invoked.

	\sa Pause(void) or Reset(void)

***************************************/

float BURGER_API Burger::FloatTimer::GetTime(void)
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

		Word64 uElapsedTime;

		Word64 uMark = static_cast<Word64>(uTick.QuadPart);
		if (uMark<m_uBaseTime) {

			// The timer wrapped around.

			// Start with the wrapped value, this is the new "zero"
			uElapsedTime = uMark;

			// Discard the time that was "wrapped" because without any knowledge
			// exactly where it considered a wrap around point (It can't be assumed
			// that the wrap around point was on a power of 2), this excess time
			// will be dropped on the floor. Since the amount of lost time is usually less
			// than a second (1/60th of a second is typical), it's an acceptable compromise
			// especially at the rarity of the wrap around case.

		} else {
			// 99.99999% of the time, this is the code executed
			uElapsedTime = uMark-m_uBaseTime;
		}
		m_uBaseTime = uMark;

		// Apply to seconds elapsed
		uElapsedTime += m_uElapsedTime;
		m_uElapsedTime = uElapsedTime;

		// Convert from integer to float, using a high precision integer
		// as the source to get around floating point imprecision.
		fResult = static_cast<float>(static_cast<double>(uElapsedTime) * m_dReciprocalFrequency);
		m_fElapsedTime = fResult;
	}
	return fResult;
}


/***************************************

	Sleep the current thread

***************************************/

void BURGER_API Burger::Sleep(Word32 uMilliseconds)
{
	// Sleep until the time expires or something
	// occurs that could cause the main thread to take notice
	// like a I/O service routine
	::SleepEx(uMilliseconds,TRUE);
}

#endif