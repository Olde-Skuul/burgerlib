/***************************************

	Class to handle threads, MacOS version

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brthread.h"

#if defined(BURGER_MAC)
#include <Threads.h>

/***************************************

	Note!

	True threads are only available for MacOS 8 or higher. When writing code for
	MacOS, and if 7.5 support is needed, code must be re-written to use async
	cooperative threading instead of true threads.

***************************************/

/***************************************

	\brief Return the ID of the current thread

	Query the operating system for the ID number of the currently executing
	thread.

	If zero is returned, this feature is not supported

	\returns ID of the thread or 0 on unsupported systems, like MSDOS.

	\sa \ref Thread

***************************************/

Burger::ThreadID BURGER_API Burger::get_ThreadID(void) BURGER_NOEXCEPT
{
	// ProcessSerialNumber is a 64 bit value
	::ThreadID ID;

	// Get the ThreadID
	if (MacGetCurrentThread(&ID)) {
		return 0;
	}

	// Return it
	return ID;
}

#endif
