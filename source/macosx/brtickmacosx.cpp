/***************************************

	Incremental tick Manager Class

	MacOSX version

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brtick.h"

#if defined(BURGER_MACOSX)
#include <mach/mach_time.h>
#include <time.h>

/***************************************

	I create a thread that gets CPU time about 60 times a second, but since I
	can't rely on this I then use GetTickCount() to get the TRUE elapsed time

***************************************/

/***************************************

	Read the current system tick value

***************************************/

static Word bStarted60Hz = FALSE;
static Word32 uTick;
static Word64 uAnchor60Hz;
static Word64 uFraction60Hz;
static Word64 uStep60Hz;

Word32 BURGER_API Burger::Tick::Read(void)
{
	Word64 uMark = mach_absolute_time();
	if (!bStarted60Hz) {
		bStarted60Hz = TRUE;
		uAnchor60Hz = uMark;
		uTick = 1;
		uFraction60Hz = 0;
		mach_timebase_info_data_t sTimebaseInfo;
		mach_timebase_info(&sTimebaseInfo);
		uStep60Hz = (50000000ULL * static_cast<Word64>(sTimebaseInfo.denom)) /
			static_cast<Word64>(sTimebaseInfo.numer * 3);
		return 1;
	}
	Word64 uElapsed = (uMark - uAnchor60Hz) + uFraction60Hz;
	uAnchor60Hz = uMark;
	Word64 uTicks = uElapsed / uStep60Hz;
	uFraction60Hz = uElapsed - (uTicks * uStep60Hz);
	uTick = static_cast<Word32>(uTick + uTicks);
	return uTick;
}

/***************************************

	Read the time in microsecond increments

***************************************/

static Word bStarted1MHz = FALSE;
static Word32 uTick1MHz;
static Word64 uAnchor1MHz;
static Word64 uFraction1MHz;
static Word64 uStep1MHz;

Word32 BURGER_API Burger::Tick::ReadMicroseconds(void)
{
	Word64 uMark = mach_absolute_time();
	if (!bStarted1MHz) {
		bStarted1MHz = TRUE;
		uAnchor1MHz = uMark;
		uTick1MHz = 1;
		uFraction1MHz = 0;
		mach_timebase_info_data_t sTimebaseInfo;
		mach_timebase_info(&sTimebaseInfo);
		uStep1MHz = (1000ULL * static_cast<Word64>(sTimebaseInfo.denom)) /
			static_cast<Word64>(sTimebaseInfo.numer);
		return 1;
	}
	Word64 uElapsed = (uMark - uAnchor1MHz) + uFraction1MHz;
	uAnchor1MHz = uMark;
	Word64 uTicks = uElapsed / uStep1MHz;
	uFraction1MHz = uElapsed - (uTicks * uStep1MHz);
	uTick1MHz = static_cast<Word32>(uTick1MHz + uTicks);
	return uTick1MHz;
}

/***************************************

	Read the time in millisecond increments

***************************************/

static Word bStarted1KHz = FALSE;
static Word32 uTick1KHz;
static Word64 uAnchor1KHz;
static Word64 uFraction1KHz;
static Word64 uStep1KHz;

Word32 BURGER_API Burger::Tick::ReadMilliseconds(void)
{
	Word64 uMark = mach_absolute_time();
	if (!bStarted1KHz) {
		bStarted1KHz = TRUE;
		uAnchor1KHz = uMark;
		uTick1KHz = 1;
		uFraction1KHz = 0;
		mach_timebase_info_data_t sTimebaseInfo;
		mach_timebase_info(&sTimebaseInfo);
		uStep1KHz = (1000000ULL * static_cast<Word64>(sTimebaseInfo.denom)) /
			static_cast<Word64>(sTimebaseInfo.numer);
		return 1;
	}
	Word64 uElapsed = (uMark - uAnchor1KHz) + uFraction1KHz;
	uAnchor1KHz = uMark;
	Word64 uTicks = uElapsed / uStep1KHz;
	uFraction1KHz = uElapsed - (uTicks * uStep1KHz);
	uTick1KHz = static_cast<Word32>(uTick1KHz + uTicks);
	return uTick1KHz;
}

/***************************************

	\brief Constructor floating point timer

	Reads in the default data needed to maintain the timer and sets the elapsed
	time to 0.0f

	\sa GetTime(void) or Reset(void)

***************************************/

Burger::FloatTimer::FloatTimer() : m_bPaused(FALSE)
{
	// Get the frequency of the high precision timer

	mach_timebase_info_data_t sTimebaseInfo;
	mach_timebase_info(&sTimebaseInfo);

	// The value gives the scale for nanoseconds. Convert to a scale for seconds

	m_dReciprocalFrequency = static_cast<double>(sTimebaseInfo.denom) /
		(static_cast<double>(sTimebaseInfo.numer) * 1000000000);

	// Initialize the timer
	Reset();
}

/***************************************

	\brief Reset the timer

	Set m_uBaseTime to the current high precision time, however this function
	will not reset the elapsed time.

	\sa Reset(void)

***************************************/

void BURGER_API Burger::FloatTimer::SetBase(void)
{
	m_uBaseTime = mach_absolute_time();
}

/*! ************************************

	\brief Read the timer in seconds

	Return the elapsed time in seconds from the last time this timer was reset.
	If the timer is paused, the value will be at the time mark when the pause
	was invoked.

	\sa Pause(void) or Reset(void)

***************************************/

float BURGER_API Burger::FloatTimer::GetTime(void)
{
	float fResult;

	// If paused, just return the frozen elapsed time
	if (m_bPaused) {
		fResult = m_fElapsedTime;
	} else {

		Word64 uMark = mach_absolute_time();
		Word64 uElapsedTime;

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

		// Convert from integer to float, using a high precision integer as the
		// source to get around floating point imprecision.
		fResult = static_cast<float>(
			static_cast<double>(uElapsedTime) * m_dReciprocalFrequency);
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

	timespec SleepTime;
	// Seconds to sleep
	SleepTime.tv_sec = uMilliseconds / 1000U;
	// Nanoseconds to sleep
	SleepTime.tv_nsec = (uMilliseconds - (SleepTime.tv_sec * 1000U)) * 1000000U;
	nanosleep(&SleepTime, nullptr);
}

#endif