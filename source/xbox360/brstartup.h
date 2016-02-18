/***************************************

	Start up code for games based applications
	
	Xbox 360 version

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSTARTUP_H__
#define __BRSTARTUP_H__

#ifndef __BURGER__
#include <burger.h>
#endif

//
// Initialize defaults
//

#if !defined(MEMORYSIZE)
#define MEMORYSIZE Burger::MemoryManagerHandle::DEFAULTMEMORYCHUNK
#endif

#if !defined(HANDLECOUNT)
#define HANDLECOUNT Burger::MemoryManagerHandle::DEFAULTHANDLECOUNT
#endif

#if !defined(MINIMUMRESERVE)
#define MINIMUMRESERVE Burger::MemoryManagerHandle::DEFAULTMINIMUMRESERVE
#endif


/***************************************

	This is the main entry point for an XBox 360 version of the game

***************************************/

extern "C" void BURGER_ANSIAPI main(void);

void BURGER_ANSIAPI main(void)
{
	// Create an application instance

	Burger::GameApp MyApp(MEMORYSIZE,HANDLECOUNT,MINIMUMRESERVE);

	// Error on startup?
	int iResult = Burger::Globals::GetErrorCode();

	if (!iResult) {
		iResult = CodeEntry(&MyApp);
	}
}

#endif
