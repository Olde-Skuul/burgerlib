/***************************************

	Incremental tick Manager Class, Playstation Vita version

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brtick.h"

#if defined(BURGER_VITA)
#include <kernel.h>

/***************************************

	Sleep the current thread

***************************************/

void BURGER_API Burger::Sleep(Word32 uMilliseconds)
{
	// Zero is illegal for the VITA
	if (uMilliseconds) {
		// Convert to microseconds
		sceKernelDelayThread(uMilliseconds*1000);
	}
}

#endif