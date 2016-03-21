/***************************************

	Incremental tick Manager Class, Android version

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brtick.h"

#if defined(BURGER_ANDROID)
#include <time.h>


/***************************************

	\brief Reset the timer

	Set m_uBaseTime to the current high precision time, however
	this function will not reset the elapsed time.

	\sa Reset(void)

***************************************/

void BURGER_API Burger::FloatTimer::SetBase(void)
{
	struct timespec uTime;
	clock_gettime(CLOCK_MONOTONIC,&uTime);
	m_uBaseTime = uTime.tv_sec;
	m_uBaseTimeNano = uTime.tv_nsec;
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

		struct timespec uTick;
		clock_gettime(CLOCK_MONOTONIC,&uTick);

		Word64 uElapsedTime;
		Word64 uElapsedTimeNano;

		Word64 uMark = static_cast<Word64>(uTick.tv_sec);
		Word64 uMarkNano = static_cast<Word64>(uTick.tv_nsec);
		if (uMark<m_uBaseTime) {

			// The timer wrapped around.

			// Start with the wrapped value, this is the new "zero"
			uElapsedTime = uMark;
			uElapsedTimeNano = uMarkNano;
			// Discard the time that was "wrapped" because without any knowledge
			// exactly where it considered a wrap around point (It can't be assumed
			// that the wrap around point was on a power of 2), this excess time
			// will be dropped on the floor. Since the amount of lost time is usually less
			// than a second (1/60th of a second is typical), it's an acceptable compromise
			// especially at the rarity of the wrap around case.

		} else {
			// 99.99999% of the time, this is the code executed
			uElapsedTime = uMark-m_uBaseTime;
			uElapsedTimeNano = uMarkNano-m_uBaseTimeNano;
		}
		m_uBaseTime = uMark;
		m_uBaseTimeNano = uMarkNano;

		// Apply to seconds elapsed
		uElapsedTime += m_uElapsedTime;
		uElapsedTimeNano += m_uElapsedTimeNano;

		// Handle wrap around
		if (uElapsedTimeNano>=1000000000) {
			++uElapsedTime;
			uElapsedTimeNano-=1000000000;
		}
		m_uElapsedTime = uElapsedTime;
		m_uElapsedTimeNano = uElapsedTimeNano;

		// Convert from integer to float, using a high precision integer
		// as the source to get around floating point imprecision.
		fResult = static_cast<float>(static_cast<double>(uElapsedTime) + (static_cast<double>(uElapsedTimeNano)*0.000000001));
		m_fElapsedTime = fResult;
	}
	return fResult;
}

/***************************************

	Sleep the current thread

***************************************/

void BURGER_API Burger::Sleep(Word32 uMilliseconds)
{
	// Old code
	// usleep(uMilliseconds*1000);

	// Sleep until the time expires or something
	// occurs that could cause the main thread to take notice
	// like a I/O service routine

	timespec SleepTime;
	// Seconds to sleep
	SleepTime.tv_sec = uMilliseconds/1000U;
	// Nanoseconds to sleep
	SleepTime.tv_nsec = (uMilliseconds-(SleepTime.tv_sec*1000U))*1000000U;
	nanosleep(&SleepTime,NULL);
}

#endif