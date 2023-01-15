/***************************************

	Incremental tick Manager Class

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brtick.h"

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MAC) || defined(BURGER_DS) || \
	defined(BURGER_MACOSX) || defined(BURGER_IOS))
#include <time.h>
#endif

uint32_t Burger::Tick::g_uLastTick;

/*! ************************************

	\class Burger::Tick
	\brief 60 hertz timer

	Upon application start up, a global 60 hertz timer is created and via
	a background interrupt, it will increment 60 times a second. This is global
	and shared by all threads. The timer cannot stop.

	The value can be zero for 1/60th of a second, so do not assume that a zero
	is an uninitialized state.

	\note: The timer will wrap around 2 years. Do not assume it will not wrap
		around.

	\sa Read() or \ref FloatTimer

***************************************/

/*! ************************************

	\brief Retrieve the 60 hertz timer system time

	Upon application start up, a 60 hertz timer is created and via
	a background interrupt, it will increment 60 times a second.

	The value can be zero for 1/60th of a second, so do not assume that a zero
	is an uninitialized state.

	\return 32 bit time value that increments 60 times a second
	\sa ResetLastTick()

***************************************/

#if !(defined(BURGER_MSDOS) || defined(BURGER_WINDOWS) || \
	defined(BURGER_MAC) || defined(BURGER_BEOS) || defined(BURGER_DS) || \
	defined(BURGER_MACOSX) || defined(BURGER_IOS)) || \
	defined(DOXYGEN)

uint32_t BURGER_API Burger::Tick::Read(void) BURGER_NOEXCEPT
{
#if defined(BURGER_LINUX) || defined(BURGER_STADIA) || defined(BURGER_XBOXONE)
	return static_cast<uint32_t>((clock() * TICKSPERSEC) / CLOCKS_PER_SEC);
#elif CLOCKS_PER_SEC == TICKSPERSEC
	return clock();
#else
	return static_cast<uint32_t>((clock() * TICKSPERSEC) / CLOCKS_PER_SEC);
#endif
}
#endif

/*! ************************************

	\fn Burger::Tick::ResetLastTick(void)
	\brief Sync the 60 hertz timer

	Read the current tick value and set the internal tick value
	it. Functions like Wait() and WaitEvent() use this value to begin
	a time delay.

	\return 32 bit time value that increments 60 times a second
	\sa Read() or GetLastTick()

***************************************/

/*! ************************************

	\fn Burger::Tick::GetLastTick(void)
	\brief Get the 60 hertz timer sync value

	When Wait(), WaitEvent() and ResetLastTick() are called, an internal sync
	value is updated. This function returns that value.

	\note This value will only change if any of the above calls are made.
		Otherwise the value will remain unchanging.

	\return 32 bit sync time value
	\sa Wait(), WaitEvent() or ResetLastTick()

***************************************/

/*! ************************************

	\fn Burger::Tick::WaitOneTick(void)
	\brief Sleep for a maximum of 1/60th of a second

	Sync to the current tick mark and sleep the application until
	the next time the timer increments.

	\sa Wait(), WaitEvent() or ResetLastTick()

***************************************/

/*! ************************************

	\brief Wait for a number of system ticks

	Sleep the application for approximately the number of units
	in 1/60ths of a second. The delay is a maximum.

	\note If zero is passed, this function does not issue a delay

	\param uCount Number of 1/60ths of a second to sleep.
	\sa WaitOneTick(), WaitEvent() or ResetLastTick()

***************************************/

#if !(defined(BURGER_WINDOWS)) || defined(DOXYGEN)
void BURGER_API Burger::Tick::Wait(uint_t uCount) BURGER_NOEXCEPT
{

	// Read the timer
	uint32_t uNewTick = Read();

	// Should I wait?
	if ((uNewTick - g_uLastTick) < static_cast<uint32_t>(uCount)) {
		do {
			// Read in the current time tick
			uNewTick = Read();

			// Time has elapsed?
		} while ((uNewTick - g_uLastTick) < static_cast<uint32_t>(uCount));
	}

	// Mark the time
	g_uLastTick = uNewTick;
}
#endif

/*! ************************************

	\brief Sleep until a timeout or until a user input event

	\param uCount Number of 1/60ths of a second to sleep for a timeout. Zero is
		no timeout.

	\return \ref TRUE if an input event caused the function to end, \ref
		FALSE if it timed out

	\sa WaitOneTick(), Wait() or ResetLastTick()

***************************************/

uint_t BURGER_API Burger::Tick::WaitEvent(uint_t uCount) BURGER_NOEXCEPT
{
	uint_t uTemp;

	// Set the current time mark
	const uint32_t uTimeMark = Read();

	// Set the global time mark
	g_uLastTick = uTimeMark;

#if 0 // TODO Fix me
	// Get the current state of the mouse
	uint_t MouseBits = MouseReadButtons();
#endif
	// uint_t JoyBits = 0;	//JoystickReadButtons(0);	// Get the current state
	// of the joypad
	for (;;) {
#if 0
		// Pressed a joypad button?
		uTemp = JoystickReadButtons(0);			
		if (((uTemp^JoyBits)&Temp) & (PadButton1|PadButton2|PadButton3|PadButton4|PadButton5|PadButton6)) {
			uTemp = 1;					// Joypad event
			break;
		}
		JoyBits = uTemp;				// Save it
		uTemp = MouseReadButtons();		// Read the mouse
		if ((uTemp^MouseBits)&Temp) {	// Pressed a mouse button?
			uTemp = 1;
			break;
		}
		MouseBits = uTemp;			// Save it
		uTemp = KeyboardGet();		// Try the keyboard
		if (uTemp) {
			break;					// Return the key event
		}
#endif
		Wait(1);      // Wait 1 tick (Possibly calling an OS taskswitch)
		if (uCount) { // Can I timeout?
			if ((g_uLastTick - uTimeMark) >=
				static_cast<uint32_t>(uCount)) { // Count down
				uTemp = 0;                       // Timeout exit
				break;
			}
		}
	}
	return uTemp;
}

/*! ************************************

	\brief Retrieve the 1Mhz timer

	Upon application start up, a 1Mhz hertz timer is created and via
	a hardward timer, it will increment 1Mhz times a second.

	The value can be zero for 1/1,000,000th of a second, so do not assume that a
	zero is an uninitialized state.

	\note Due to hardware limitations, do NOT assume this timer is accurate to
		1/1,000,000th of a second. The granularity could be much courser,
		however, it will be incrementing at a rate to remain in sync to
		1,000,000 ticks a second.

	\return 32 bit time value that increments at 1Mhz
	\sa Read() or ReadMilliseconds()

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MAC) || defined(BURGER_DS) || \
	defined(BURGER_MACOSX) || defined(BURGER_IOS)) || \
	defined(DOXYGEN)

uint32_t BURGER_API Burger::Tick::ReadMicroseconds(void) BURGER_NOEXCEPT
{
#if defined(BURGER_LINUX) || defined(BURGER_XBOXONE)
	return static_cast<uint32_t>((clock() * 1000000) / CLOCKS_PER_SEC);
#elif defined(BURGER_STADIA)
	return static_cast<uint32_t>(clock());
#elif (CLOCKS_PER_SEC == 1000000)
	return static_cast<uint32_t>(clock());
#else
	return static_cast<uint32_t>((clock() * 1000000) / CLOCKS_PER_SEC);
#endif
}

#endif

/*! ************************************

	\brief Retrieve the 1Khz timer

	Upon application start up, a 1Khz hertz timer is created and via
	a hardware timer, it will increment 1Mhz times a second.

	The value can be zero for 1/1,000th of a second, so do not assume that a
	zero is an uninitialized state.

	\note Due to hardware limitations, do NOT assume this timer is accurate to
		1/1,000th of a second. The granularity could be much courser, however,
		it will be incrementing at a rate to remain in sync to 1,000 ticks a
		second.

	\return 32 bit time value that increments at 1Khz
	\sa Read() or ReadMicroseconds()

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MAC) || defined(BURGER_DS) || \
	defined(BURGER_MACOSX) || defined(BURGER_IOS)) || \
	defined(DOXYGEN)

uint32_t BURGER_API Burger::Tick::ReadMilliseconds(void) BURGER_NOEXCEPT
{
#if defined(BURGER_LINUX) || defined(BURGER_STADIA) || defined(BURGER_XBOXONE)
	return static_cast<uint32_t>((clock() * 1000) / CLOCKS_PER_SEC);
#elif CLOCKS_PER_SEC == 1000
	return static_cast<uint32_t>(clock());
#else
	return static_cast<uint32_t>((clock() * 1000) / CLOCKS_PER_SEC);
#endif
}

#endif

/*! ************************************

	\class Burger::FloatTimer
	\brief Floating point timer

	Upon class start up, a high accuracy timer is read and this moment is
	considered 0.0f elapsed time. When the timer is queried, it will return
	a floating point number in seconds. 0.5f is a half second, etc...

	The value is as high an accuracy as the platform supports.

	\sa GetTime(void) or \ref Tick

***************************************/

/*! ************************************

	\brief Constructor for the floating point timer

	Reads in the default data needed to maintain the timer
	and sets the elapsed time to 0.0f

	\sa GetTime(void) or Reset(void)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || \
	defined(BURGER_MACOSX) || defined(BURGER_IOS)) || \
	defined(DOXYGEN)
Burger::FloatTimer::FloatTimer() BURGER_NOEXCEPT: m_bPaused(FALSE)
{
	// Initialize the timer
	Reset();
}
#endif

/*! ************************************

	\fn float Burger::FloatTimer::GetLastTime(void) const
	\brief Get the last read time

	Returns the last time value without actually asking the platform for
	a time update.

	Call GetTime(void) to read the timer and return a updating timer.

	\return Elapsed time in seconds

	\sa GetTime(void) or Reset(void)

***************************************/

/*! ************************************

	\fn uint_t Burger::FloatTimer::IsPaused(void) const
	\brief Returns \ref TRUE if the timer is paused

	If the timer was paused with a call to Pause(void), the
	timer will not increment. This function will return \ref TRUE
	if the timer is in pause mode.

	\return \ref FALSE if the timer is running, \ref TRUE if paused

	\sa Pause(void) or Unpause(void)

***************************************/

/*! ************************************

	\brief Reset the timer

	Set m_uBaseTime to the current high precision time, however
	this function will not reset the elapsed time.

	\sa Reset(void)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || \
	defined(BURGER_ANDROID) || defined(BURGER_MACOSX) || \
	defined(BURGER_IOS)) || \
	defined(DOXYGEN)
void BURGER_API Burger::FloatTimer::SetBase(void) BURGER_NOEXCEPT
{
	// Generic version
	m_uBaseTime = Tick::ReadMicroseconds();
}
#endif

/*! ************************************

	\brief Reset the timer

	Clear the timer to 0.0f.

	\sa GetTime(void)

***************************************/

void BURGER_API Burger::FloatTimer::Reset(void) BURGER_NOEXCEPT
{
	SetBase(); // Set the platform specific time values
	m_fElapsedTime = 0.0f;
#if defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || \
	defined(BURGER_ANDROID) || defined(BURGER_MACOSX) || defined(BURGER_IOS)
	m_uElapsedTime = 0; // Clear the high precision value
#endif
#if defined(BURGER_ANDROID)
	m_uElapsedTimeNano = 0;
#endif
}

/*! ************************************

	\brief Read the timer in seconds

	Return the elapsed time in seconds from the last
	time this timer was reset. If the timer is paused, the
	value will be at the time mark when the pause was invoked.

	\sa Pause(void) or Reset(void)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || \
	defined(BURGER_ANDROID) || defined(BURGER_MACOSX) || \
	defined(BURGER_IOS)) || \
	defined(DOXYGEN)
float BURGER_API Burger::FloatTimer::GetTime(void) BURGER_NOEXCEPT
{
	float fResult;

	// If paused, just return the frozen elapsed time
	if (m_bPaused) {
		fResult = m_fElapsedTime;
	} else {
		// Generic code

		uint32_t uTick = Tick::ReadMicroseconds();
		uint32_t uElapsed = uTick - m_uBaseTime;
		m_uBaseTime = uTick;

		// Convert to seconds
		fResult = static_cast<float>(uElapsed) * (1.0f / 1000000.0f);
		fResult += m_fElapsedTime;
		m_fElapsedTime = fResult;
	}
	return fResult;
}
#endif

/*! ************************************

	\brief Pause the timer

	If the timer was not paused, accumulate the current time
	into the elapsed time and freeze the timer at that value.

	If it was already paused, this function will do nothing.

	\sa Unpause(void)

***************************************/

void BURGER_API Burger::FloatTimer::Pause(void) BURGER_NOEXCEPT
{
	if (!m_bPaused) {
		// Accumulate the time up to this moment
		GetTime();
		// No more time will be acquired
		m_bPaused = TRUE;
	}
}

/*! ************************************

	\brief Unpause the timer

	If the timer was paused, mark the current time as the moment
	timing will commence.

	If it was not paused, this function will do nothing.

	\sa Pause(void)

***************************************/

void BURGER_API Burger::FloatTimer::Unpause(void) BURGER_NOEXCEPT
{
	if (m_bPaused) {
		// Mark as running
		m_bPaused = FALSE;
		// Grab the current time mark so time is accumulated properly
		SetBase();
	}
}

/*! ************************************

	\brief Sleep the current thread

	On multithreaded systems, if \ref SLEEP_YIELD is passed to this function
	it will yield the thread's remaining time quantum.
	If \ref SLEEP_INFINITE is passed then the thread will sleep
	forever unless an Remote Procedure Call or an I/O event occurs.
	Otherwise, pass the number of milliseconds that are desired
	for the thread to sleep (Zero is not permitted, it's mapped
	to \ref SLEEP_YIELD )

	\param uMilliseconds \ref SLEEP_YIELD, \ref SLEEP_INFINITE or number
		of non-zero milliseconds to sleep

	\note On non-multithreaded systems, this function does nothing

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || \
	defined(BURGER_SHIELD) || defined(BURGER_VITA) || \
	defined(BURGER_MACOSX) || defined(BURGER_IOS)) || \
	defined(DOXYGEN)
void BURGER_API Burger::Sleep(uint32_t /* uMilliseconds */) BURGER_NOEXCEPT {}
#endif
