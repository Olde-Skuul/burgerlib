/***************************************

    Start up code for games based applications

    iOS version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

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
#define MEMORYSIZE Burger::MemoryManagerHandle::kSystemMemoryChuckSize
#endif

#if !defined(HANDLECOUNT)
#define HANDLECOUNT Burger::MemoryManagerHandle::kDefaultHandleCount
#endif

#if !defined(MINIMUMRESERVE)
#define MINIMUMRESERVE Burger::MemoryManagerHandle::kSystemMemoryReservedSize
#endif

/***************************************

    This is the main entry point for a iOS version of the game

***************************************/

extern "C" int BURGER_ANSIAPI main(void);

int BURGER_ANSIAPI main(void)
{
    // Create an application instance

    Burger::GameApp MyApp(MEMORYSIZE, HANDLECOUNT, MINIMUMRESERVE);

    // Error on startup?
    int iResult = Burger::Globals::GetErrorCode();

    if (!iResult) {
        // Call Objective-C with a NSAutorreleasePool
        // to start a run loop and then call CodeEntry(&MyApp)
        iResult = MyApp.Run();
    }
    return iResult;
}

#endif
