/***************************************

    Incremental tick Manager Class, Playstation Vita version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brtick.h"

#if defined(BURGER_VITA)
#include <kernel.h>
#include <libperf.h>

//
// It's come to my attention that using libperf in 
// shipping code is a TRC violation. Disabled the code
// until a need for high precision timing
// comes up where I need to spend time implementing
// code using  __builtin_mrc(15, 0, 9, 13, 0) to
// directly access the Cortex A9 cycle counter.
//

#if 0
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

	SceUInt32 uFrequency = scePerfGetTimebaseFrequency();
	m_dReciprocalFrequency = 1.0 / static_cast<double>(m_uFrequency);
	
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
	m_uBaseTime = scePerfGetTimebaseValue();
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

		uint64_t uMark = scePerfGetTimebaseValue();
		uint64_t uElapsedTime;

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
#endif

/***************************************

	Sleep the current thread

***************************************/

void BURGER_API Burger::Sleep(uint32_t uMilliseconds)
{
	// Zero is illegal for the VITA
	if (uMilliseconds) {
		// Convert to microseconds
		sceKernelDelayThread(uMilliseconds*1000);
	}
}

#endif
