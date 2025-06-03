/***************************************

	Incremental tick Manager Class

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brtick.h"

#include <time.h>

// Global instance
Burger::Tick Burger::Tick::g_Tick;

/*! ************************************

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

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX) || defined(BURGER_XBOX360) || \
	defined(BURGER_XBOXONE) || defined(BURGER_VITA) || defined(BURGER_PS3) || \
	defined(BURGER_PS4) || defined(BURGER_PS5) || defined(BURGER_WIIU) || \
	defined(BURGER_SWITCH) || defined(BURGER_UNIX)) || \
	defined(DOXYGEN)
void BURGER_API Burger::sleep_ms(
	uint32_t /* uMilliseconds */, uint_t /* bAlertable */) BURGER_NOEXCEPT
{
}
#endif

/*! ************************************

	\class Burger::Tick::Scaler
	\brief Time scaler

	This class is used by Burger::Tick to scale a high precision timer to a
	different precision timer without losing precision.

	\note: All timers can wrap around. Do not assume it will not wrap
		around.

	\sa Tick, Tick::read_high_precision()

***************************************/

/*! ************************************

	\brief Constuctor

	Initialize the scaler with the desired tick rate in ticks per second. Any
	value but zero can be used.

	\param uDesiredRate Number of ticks per second to scale to

***************************************/

void BURGER_API Burger::Tick::Scaler_t::init(
	uint32_t uDesiredRate) BURGER_NOEXCEPT
{
	m_uDelta = 0;
	m_uTick = 1;
	m_uDesiredRate = uDesiredRate;

	// After clearing the delta and starting the tick at one, capture the
	// current high precision time mark to "start" the tick
	m_uMark = Tick::read_high_precision();
}

/*! ************************************

	\brief Read a tick value at the desired scale

	Read a high precision timer and through some math magic, convert it to the
	desired tick rate in ticks per second. The code is accurate and handles a
	full 32 bit range of values.

	\return Value that increments at the desireed ticks per second

***************************************/

uint32_t BURGER_API Burger::Tick::Scaler_t::read(void) BURGER_NOEXCEPT
{
	// Get the amount of time that has elapsed from the time mark, scaled for
	// m_uDesiredRate
	const uint64_t uNewTick = Tick::read_high_precision();
	const uint64_t uElapsed =
		((uNewTick - m_uMark) * m_uDesiredRate) + m_uDelta;

	// Save the current high precision tick for the next pass
	m_uMark = uNewTick;

	// Divide by ticks per second to get the ticks elapsed
	// Note: Since the value is pre-scaled, it's in ticks at the rate of
	// interest
	const uint64_t uFrequency = Tick::get_high_precision_frequency();
	const uint64_t uNewTicks = uElapsed / uFrequency;

	// Perform a modulo to capture the excess high frequency ticks for the next
	// pass
	m_uDelta = uElapsed - (uNewTicks * uFrequency);

	// Add in the elapsed ticks to the master counter
	// It's done this way to ensure wraparound is handled properly
	const uint32_t uTick = m_uTick + static_cast<uint32_t>(uNewTicks);
	m_uTick = uTick;

	// Done!
	return uTick;
}

/*! ************************************

	\class Burger::Tick
	\brief Low level timer manager

	Upon this class being started up, a global 60 hertz timer is created and via
	a background interrupt, it will increment 60 times a second. This is global
	and shared by all threads. The timer cannot stop.

	The value can be zero for 1/60th of a second, so do not assume that a zero
	is an uninitialized state.

	In addition to starting up this timer, higher precision timers are
	initialized so ticks accurate to milliseconds, microseconds and system
	precision.

	\note: The timer will wrap around 2 years. Do not assume it will not wrap
		around.

	\sa read(), read_ms(), read_us() or \ref FloatTimer

***************************************/

/*! ************************************

	\fn Burger::Tick::get_instance(void)
	\brief Get the global instance of the timer system

	\sa init(), or shutdown()

***************************************/

/*! ************************************

	\brief Initialize the low level timer manager

	Start up the low level timer

	\sa shutdown()

***************************************/

#if !(defined(BURGER_MSDOS) || defined(BURGER_MAC) || defined(BURGER_UNIX)) || \
	defined(DOXYGEN)
void BURGER_API Burger::Tick::init(void) BURGER_NOEXCEPT
{
	Tick* pThis = &g_Tick;
	if (!pThis->m_bInitialized) {
		// Get the speed of the most accurate timer
		pThis->m_uHighPrecisionFrequency = get_high_precision_rate();
		pThis->m_uLast60HertzMark = 1;

		// Init the sub-timers

		pThis->m_60Hertz.init(kTicksPerSecond);
		pThis->m_1KHertz.init(1000U);
		pThis->m_1MHertz.init(1000000U);

		pThis->m_bInitialized = TRUE;
	}
}
#endif

/*! ************************************

	\brief Shut down the low level timer manager

	Shut down the low level timer

	\sa init()

***************************************/

#if !(defined(BURGER_MSDOS)) || defined(DOXYGEN)
void BURGER_API Burger::Tick::shutdown(void) BURGER_NOEXCEPT
{
	Tick* pThis = &g_Tick;
	pThis->m_bInitialized = FALSE;
}
#endif

/*! ************************************

	\fn Burger::Tick::get_high_precision_frequency(void)
	\brief Get the frequency of the high precision timer

	This value is in units per second. Divide the output of
	read_high_precision() by this number to determine the time in seconds.

	\sa read(), read_ms(), read_us()

***************************************/

/*! ************************************

	\fn Burger::Tick::is_initialized(void)
	\brief Check if the timer manager is initialized

	If \ref TRUE, the timer has been initialized.

	\sa init()

***************************************/

/*! ************************************

	\brief Return the ticks per second at the system's highest precision

	This platform specific code will ask the operating system what is the
	highest precision timer tick rate and then will return that value.

	This value is cached and is available from get_high_precision_frequency()

	\sa get_high_precision_frequency(), or read_high_precision()

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX) || defined(BURGER_XBOX360) || \
	defined(BURGER_XBOXONE) || defined(BURGER_VITA) || defined(BURGER_PS3) || \
	defined(BURGER_PS4) || defined(BURGER_PS5) || defined(BURGER_WIIU) || \
	defined(BURGER_SWITCH) || defined(BURGER_UNIX) || defined(BURGER_MAC)) || \
	defined(DOXYGEN)
uint64_t BURGER_API Burger::Tick::get_high_precision_rate(void) BURGER_NOEXCEPT
{
	return CLOCKS_PER_SEC;
}

/*! ************************************

	\brief Return the tick at the system's highest precision

	The value returns a tick that will increment at
	get_high_precision_frequency() ticks per second.

	\sa get_high_precision_frequency()

***************************************/

uint64_t BURGER_API Burger::Tick::read_high_precision(void) BURGER_NOEXCEPT
{
	return static_cast<uint64_t>(clock());
}
#endif

/*! ************************************

	\brief Retrieve the 60 hertz timer system time

	When init() is called, a 60 hertz timer is created and via a background
	interrupt or other means, will increment 60 times a second.

	The value can be zero for 1/60th of a second, so do not assume that a zero
	is an uninitialized state.

	\return 32 bit time value that increments 60 times a second
	\sa read_and_mark()

***************************************/

#if !(defined(BURGER_MSDOS) || defined(BURGER_MAC) || defined(BURGER_BEOS) || \
	defined(BURGER_DS)) || \
	defined(DOXYGEN)
uint32_t BURGER_API Burger::Tick::read(void) BURGER_NOEXCEPT
{
	return g_Tick.m_60Hertz.read();
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
	\sa Read() or read_us()

***************************************/

#if !(defined(BURGER_DS)) || defined(DOXYGEN)
uint32_t BURGER_API Burger::Tick::read_ms(void) BURGER_NOEXCEPT
{
	return g_Tick.m_1KHertz.read();
}
#endif

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
	\sa read() or read_ms()

***************************************/

#if !((defined(BURGER_MAC) && defined(BURGER_68K)) || defined(BURGER_DS)) || \
	defined(DOXYGEN)
uint32_t BURGER_API Burger::Tick::read_us(void) BURGER_NOEXCEPT
{
	return g_Tick.m_1MHertz.read();
}
#endif

/*! ************************************

	\brief Sync the 60 hertz timer

	Read the current tick value and set the internal tick value
	it. Functions like wait() and wait_event() use this value to begin
	a time delay.

	\return 32 bit time value that increments 60 times a second
	\sa read() or get_mark()

***************************************/

uint32_t BURGER_API Burger::Tick::read_and_mark(void) BURGER_NOEXCEPT
{
	// Get the time
	const uint32_t uTick = read();

	// Save it and then return
	g_Tick.m_uLast60HertzMark = uTick;
	return uTick;
}

/*! ************************************

	\fn Burger::Tick::get_mark(void)
	\brief Get the 60 hertz timer sync value

	When wait(), wait_event() and read_and_mark() are called, an internal sync
	value is updated. This function returns that value.

	\note This value will only change if any of the above calls are made.
		Otherwise the value will remain unchanging.

	\return 32 bit sync time value

	\sa wait(), wait_event() or read_and_mark()

***************************************/

/*! ************************************

	\brief Sleep for a maximum of 1/60th of a second

	Sync to the current tick mark and sleep the application until
	the next time the timer increments.

	\sa wait(), wait_event() or read_and_mark()

***************************************/

void BURGER_API Burger::Tick::wait_one_tick(void) BURGER_NOEXCEPT
{
	g_Tick.m_uLast60HertzMark = read();
	wait();
}

/*! ************************************

	\brief Wait for a number of system ticks

	Sleep the application for approximately the number of units
	in 1/60ths of a second. The delay is a maximum.

	\note If zero is passed, this function does not issue a delay

	\param uCount Number of 1/60ths of a second to sleep.
	\sa wait_one_tick(), wait_event() or read_and_mark()

***************************************/

#if !(defined(BURGER_WINDOWS)) || defined(DOXYGEN)
void BURGER_API Burger::Tick::wait(uint_t uCount) BURGER_NOEXCEPT
{

	// Read the timer
	uint32_t uNewTick = read();

	// Should I wait?
	if ((uNewTick - g_Tick.m_uLast60HertzMark) <
		static_cast<uint32_t>(uCount)) {
		do {
			// Read in the current time tick
			uNewTick = read();

			// Time has elapsed?
		} while ((uNewTick - g_Tick.m_uLast60HertzMark) <
			static_cast<uint32_t>(uCount));
	}

	// Mark the time
	g_Tick.m_uLast60HertzMark = uNewTick;
}
#endif

/*! ************************************

	\brief Sleep until a timeout or until a user input event

	\param uCount Number of 1/60ths of a second to sleep for a timeout. Zero is
		no timeout.

	\return \ref TRUE if an input event caused the function to end, \ref
		FALSE if it timed out

	\sa wait_one_tick(), wait() or read_and_mark()

***************************************/

uint_t BURGER_API Burger::Tick::wait_event(uint_t uCount) BURGER_NOEXCEPT
{
	uint_t uTemp;

	// Set the current time mark
	const uint32_t uTimeMark = read();

	// Set the global time mark
	g_Tick.m_uLast60HertzMark = uTimeMark;

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

		// Wait 1 tick (Possibly calling an OS taskswitch)
		wait(1);
		if (uCount) { // Can I timeout?
			if ((g_Tick.m_uLast60HertzMark - uTimeMark) >=
				static_cast<uint32_t>(uCount)) { // Count down
				uTemp = 0;                       // Timeout exit
				break;
			}
		}
	}
	return uTemp;
}

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
	m_uBaseTime = Tick::read_us();
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

		uint32_t uTick = Tick::read_us();
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
