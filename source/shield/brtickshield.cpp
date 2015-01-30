/***************************************

	Incremental tick Manager Class, Android version

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brtick.h"

#if defined(BURGER_ANDROID)
#include <time.h>

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