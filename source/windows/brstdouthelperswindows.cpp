/***************************************

    Helper functions for printf and the like

    Windows specific code

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brstdouthelpers.h"

#if defined(BURGER_WINDOWS) && !defined(DOXYGEN)

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

/***************************************

    \brief Clear the text console

    For platforms that support a text console, clear out text console by sending
    a line feed or system appropriate set of calls to perform the same action

***************************************/

void BURGER_API Burger::ClearConsole(void) BURGER_NOEXCEPT
{
    // Get the handle to the current console
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Copy of the screen information
    CONSOLE_SCREEN_BUFFER_INFO ScreenInfo;
    if (GetConsoleScreenBufferInfo(hConsole, &ScreenInfo)) {

        // Figure out the number of cells in the buffer
        const DWORD uConsoleCellCount =
            static_cast<DWORD>(ScreenInfo.dwSize.X) *
            static_cast<DWORD>(ScreenInfo.dwSize.Y);

        // Fill the screen with spaces
        DWORD uCharactersWritten = 0;
        const COORD screen_origin = {0, 0};
        if (FillConsoleOutputCharacterW(hConsole, 0x20, uConsoleCellCount,
                screen_origin, &uCharactersWritten)) {

            // Get the attributes for the text character that was used to fill
            // the screen
            if (GetConsoleScreenBufferInfo(hConsole, &ScreenInfo)) {

                // Fill the screen with the attribute value (Flashing, color)
                if (FillConsoleOutputAttribute(hConsole, ScreenInfo.wAttributes,
                        uConsoleCellCount, screen_origin,
                        &uCharactersWritten)) {

                    // Home the cursor
                    SetConsoleCursorPosition(hConsole, screen_origin);
                }
            }
        }
    }
}

#endif
