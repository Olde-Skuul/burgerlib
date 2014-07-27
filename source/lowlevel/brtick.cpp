/***************************************

	Incremental tick Manager Class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brtick.h"
#if !defined(BURGER_WINDOWS) && !defined(BURGER_MAC) && !defined(BURGER_DS)
#include <time.h>
#endif

Word32 Burger::Tick::s_LastTick;

/*! ************************************

	\class Burger::Tick
	\brief 60 hertz timer
	
	Upon application start up, a global 60 hertz timer is created and via
	a background interrupt, it will increment 60 times a second. This is global
	and shared by all threads. The timer cannot stop.

	The value can be zero for 1/60th of a second, so do not assume that a zero is an uninitialized
	state.

	\sa Burger::Tick::Read()

***************************************/


/*! ************************************

	\brief Retrieve the 60 hertz timer system time
	
	Upon application start up, a 60 hertz timer is created and via
	a background interrupt, it will increment 60 times a second.

	The value can be zero for 1/60th of a second, so do not assume that a zero is an uninitialized
	state.

	\return 32 bit time value that increments 60 times a second
	\sa Burger::Tick::ResetLastTick()

***************************************/

#if !defined(BURGER_MSDOS) && !defined(BURGER_WINDOWS) && !defined(BURGER_MAC) && !defined(BURGER_BEOS) && !defined(BURGER_DS)

Word32 BURGER_API Burger::Tick::Read(void)
{
#if CLOCKS_PER_SEC==TICKSPERSEC
	return clock();
#else
	return static_cast<Word32>((clock()*TICKSPERSEC)/CLOCKS_PER_SEC);
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
	\sa Burger::Tick::Read() or Burger::Tick::GetLastTick()

***************************************/

/*! ************************************

	\fn Burger::Tick::GetLastTick(void)
	\brief Get the 60 hertz timer sync value

	When Burger::Tick::Wait(), Burger::Tick::WaitEvent() and Burger::Tick::ResetLastTick()
	are called, an internal sync value is updated. This function returns
	that value.

	\note This value will only change if any of the above calls are made. Otherwise
	the value will remain unchanging.

	\return 32 bit sync time value
	\sa Burger::Tick::Wait(), Burger::Tick::WaitEvent() or Burger::Tick::ResetLastTick()

***************************************/

/*! ************************************

	\fn Burger::Tick::WaitOneTick(void)
	\brief Sleep for a maximum of 1/60th of a second

	Sync to the current tick mark and sleep the application until
	the next time the timer increments.

	\sa Burger::Tick::Wait(), Burger::Tick::WaitEvent() or Burger::Tick::ResetLastTick()

***************************************/

/*! ************************************

	\brief Wait for a number of system ticks

	Sleep the application for approximately the number of units
	in 1/60ths of a second. The delay is a maximum.

	\note If zero is passed, this function does not issue a delay

	\param uCount Number of 1/60ths of a second to sleep.
	\sa Burger::Tick::WaitOneTick(), Burger::Tick::WaitEvent() or Burger::Tick::ResetLastTick()

***************************************/

#if !defined(BURGER_WINDOWS)
void BURGER_API Burger::Tick::Wait(Word uCount)
{
	//KeyboardKbhit();			// Handle any pending events
	Word32 uNewTick = Read();	// Read the timer
	if ((uNewTick-s_LastTick)<static_cast<Word32>(uCount)) {	// Should I wait?
		do {
			//KeyboardKbhit();	// Call the system task if needed
			uNewTick = Read();	// Read in the current time tick
		} while ((uNewTick-s_LastTick)<static_cast<Word32>(uCount));	// Time has elapsed?
	}
	s_LastTick = uNewTick;		// Mark the time
}
#endif

/*! ************************************

	\brief Sleep until a timeout or until a user input event

	\param uCount Number of 1/60ths of a second to sleep for a timeout. Zero is no timeout.
	\return \ref TRUE if an input event caused the function to end, \ref FALSE if it timed out
	\sa Burger::Tick::WaitOneTick(), Burger::Tick::Wait() or Burger::Tick::ResetLastTick()

***************************************/

Word BURGER_API Burger::Tick::WaitEvent(Word uCount)
{
	Word uTemp;

	Word32 uTimeMark = Read();	// Set the current time mark
	s_LastTick = uTimeMark;		// Set the global time mark

#if 0		// TODO Fix me
	Word MouseBits = MouseReadButtons();			// Get the current state of the mouse
#endif
	//Word JoyBits = 0;	//JoystickReadButtons(0);	// Get the current state of the joypad
	for (;;) {
#if 0
		uTemp = JoystickReadButtons(0);				// Pressed a joypad button?
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
		Wait(1);			// Wait 1 tick (Possibly calling an OS taskswitch)
		if (uCount) {		// Can I timeout?
			if ((s_LastTick-uTimeMark)>=static_cast<Word32>(uCount)) {	// Count down
				uTemp = 0;	// Timeout exit
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

	The value can be zero for 1/1,000,000th of a second, so do not assume that a zero is an uninitialized
	state.

	\note Due to hardware limitations, do NOT assume this timer is accurate to 1/1,000,000th of
	a second. The granularity could be much courser, however, it will be incrementing at
	a rate to remain in sync to 1,000,000 ticks a second.

	\return 32 bit time value that increments at 1Mhz
	\sa Burger::Tick::Read() or Burger::Tick::ReadMilliseconds()

***************************************/

#if !defined(BURGER_WINDOWS) && !defined(BURGER_MAC) && !defined(BURGER_DS)

Word32 BURGER_API Burger::Tick::ReadMicroseconds(void)
{
#if CLOCKS_PER_SEC==1000000
	return static_cast<Word32>(clock());
#else
	return static_cast<Word32>((clock()*1000000)/CLOCKS_PER_SEC);
#endif
}

#endif


/*! ************************************

	\brief Retrieve the 1Khz timer
	
	Upon application start up, a 1Khz hertz timer is created and via
	a hardward timer, it will increment 1Mhz times a second.

	The value can be zero for 1/1,000th of a second, so do not assume that a zero is an uninitialized
	state.

	\note Due to hardware limitations, do NOT assume this timer is accurate to 1/1,000th of
	a second. The granularity could be much courser, however, it will be incrementing at
	a rate to remain in sync to 1,000 ticks a second.

	\return 32 bit time value that increments at 1Khz
	\sa Burger::Tick::Read() or Burger::Tick::ReadMicroseconds()

***************************************/

#if !defined(BURGER_WINDOWS) && !defined(BURGER_MAC) && !defined(BURGER_DS)

Word32 BURGER_API Burger::Tick::ReadMilliseconds(void)
{
#if CLOCKS_PER_SEC==1000
	return static_cast<Word32>(clock());
#else
	return static_cast<Word32>((clock()*1000)/CLOCKS_PER_SEC);
#endif
}

#endif

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

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_XBOX360)) || defined(DOXYGEN)
void BURGER_API Burger::Sleep(Word32 /* uMilliseconds */)
{
}
#endif