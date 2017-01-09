/***************************************

	Helper functions for printf and the like

	Windows specific code

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#include "brstdouthelpers.h"

#if defined(BURGER_WINDOWS)
#if !defined(WIN32_LEAN_AND_MEAN) && !defined(DOXYGEN)
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

/***************************************

	\brief Clear the text console

	For platforms that support a text console, clear out
	text console by sending a line feed or system
	appropriate set of calls to perform the same action

***************************************/

void BURGER_API Burger::ClearConsole(void)
{
	const COORD g_ScreenOrigin = { 0, 0 };

	// Get the handle to the current console
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	// Copy of the screen information
	CONSOLE_SCREEN_BUFFER_INFO ScreenInfo;
	if (GetConsoleScreenBufferInfo(hConsole,&ScreenInfo)) {
		
		// Figure out the number of cells in the buffer
		DWORD uConsoleCellCount = static_cast<DWORD>(ScreenInfo.dwSize.X) * static_cast<DWORD>(ScreenInfo.dwSize.Y);

		// Fill the screen with spaces
		DWORD uCharactersWritten;
		if (FillConsoleOutputCharacterW(hConsole,0x20,uConsoleCellCount,g_ScreenOrigin,&uCharactersWritten)) {

			// Get the attributes for the text character that was used to fill the screen
			if (GetConsoleScreenBufferInfo(hConsole,&ScreenInfo)) {

				// Fill the screen with the attribute value (Flashing, color)
				if (FillConsoleOutputAttribute(hConsole,ScreenInfo.wAttributes,uConsoleCellCount,g_ScreenOrigin,&uCharactersWritten)) {

					// Home the cursor
					SetConsoleCursorPosition(hConsole,g_ScreenOrigin);

				}
			}
		}
	}
}
#endif
