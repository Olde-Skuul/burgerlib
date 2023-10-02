/***************************************

    Start up code for games based applications

    Windows version

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

#if !defined(GAMENAME)
#define GAMENAME "Test Application"
#endif

#if !defined(WINDOWS_ICONID)
#define WINDOWS_ICONID 0
#endif

#if !defined(WINDOWS_CALLBACK)
#define WINDOWS_CALLBACK NULL
#endif

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

    This is the main entry point for a windows version of the game

***************************************/

extern "C" int __stdcall WinMain(HINSTANCE__* hInstance,
    HINSTANCE__* hPrevInstance, char* lpCmdLine, int nCmdShow);

int __stdcall WinMain(HINSTANCE__* /* hInstance */,
    HINSTANCE__* /* hPrevInstance */, char* /* lpCmdLine */, int /* nCmdShow */)
{
    int iResult = 0; // Exit without error if already running

    // Singular instance enabled?
#if defined(GAMELOCK)
    Burger::DetectMultiLaunch OneShot;
    if (!OneShot.is_multi_launched(GAMELOCK)) {
#endif
        // Create the game instance
        Burger::GameApp MyApp(MEMORYSIZE, HANDLECOUNT, MINIMUMRESERVE);
        iResult = Burger::Globals::GetErrorCode();
        if (!iResult) {

            // Create the initial window
            iResult =
                MyApp.InitWindow(GAMENAME, WINDOWS_CALLBACK, WINDOWS_ICONID);
            if (!iResult) {
                // Run the app
                iResult = CodeEntry(&MyApp);
            }
        }
        // Needed to properly close for the lock
#if defined(GAMELOCK)
    }
#endif
    return iResult;
}

#endif
