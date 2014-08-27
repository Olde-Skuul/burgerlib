/***************************************

	Incremental tick Manager Class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

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

***************************************/

static Word Started = FALSE;
static Word32 uTime;
static Word32 uAnchor;
static Word32 uFraction;

Word32 BURGER_API Burger::Tick::Read(void)
{
	Word32 uMark = Globals::timeGetTime();
	if (!Started) {			/* Already started? */
		Started = TRUE;
		uAnchor = uMark;
		uTime = 1;
		uFraction = 0;
		return uTime;		//
	}
	Word32 uElapsed = uMark-uAnchor;
	if (uElapsed) {
		uAnchor = uMark;
		uFraction += uElapsed*(3000/1000);
		uTime += uFraction/(3000/60);
		uFraction = uFraction % (3000/60);
	}
	return uTime;
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

static Word GotMilk;	/* Got the frequency */
static double WinTicks = 1.0;	/* Frequency adjust */

Word32 BURGER_API Burger::Tick::ReadMicroseconds(void)
{
	LARGE_INTEGER Temp;
	if (!GotMilk) {		/* Is the divisor initialized? */
		GotMilk = TRUE;
		if (QueryPerformanceFrequency(&Temp)) {	/* Get the constants */
			WinTicks = 1000000.0/(double)Temp.QuadPart;	/* Timer change */
		}
	}
	if (QueryPerformanceCounter(&Temp)) {	/* Get the timer from Win95 */
		return (Word32)((double)Temp.QuadPart*WinTicks);	/* Save the result */
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