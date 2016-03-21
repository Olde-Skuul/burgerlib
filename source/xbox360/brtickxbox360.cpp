/***************************************

	Incremental tick Manager Class

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#include "brtick.h"

#if defined(BURGER_XBOX360)
#define NOD3D
#define NONET
#include <xtl.h>

/***************************************

	\brief Constructor floating point timer
	
	Reads in the default data needed to maintain the timer
	and sets the elapsed time to 0.0f

	\sa GetTime(void) or Reset(void)

***************************************/

Burger::FloatTimer::FloatTimer() :
	m_bPaused(FALSE)
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

void BURGER_API Burger::FloatTimer::SetBase(void)
{
	// Read the counter
	Word64 uTemp = __mftb();
	if (!static_cast<Word32>(uTemp)) {
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

float BURGER_API Burger::FloatTimer::GetTime(void)
{
	float fResult;

	// If paused, just return the frozen elapsed time
	if (m_bPaused) {
		fResult = m_fElapsedTime;
	} else {

		// Windows XP or later will always succeed with this call

		// Read the counter
		Word64 uMark = __mftb();
		if (!static_cast<Word32>(uMark)) {
			// Fix for a CPU error. If the lower 32 bits of the 
			// counter is zero, the upper 32 bits has a 4 cycle window
			// where it's NOT properly incremented. By reading
			// a second time, the 4 second window will pass
			// and the 64 bit value is good.
			uMark = __mftb();
		}
		
		// Get the elapsed time

		Word64 uElapsedTime = uMark-m_uBaseTime;
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