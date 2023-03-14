/***************************************

	Console manager, Linux

	Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brconsolemanager.h"

#if defined(BURGER_LINUX)
#include "brnumberstring.h"
#include "brstring.h"

/***************************************

	\brief Set the title of the console window.

	On platforms that present console text output in a desktop window, this
	function will set the title of that window to the specified UTF-8 string.
	On platforms that don't support such a string, this function does nothing
	and returns no error.

	\param pTitle UTF-8 "C" string for new terminal title string.

	\return The current error code. Can be returned to the operating system.
	\sa WasDesktopLaunched()

***************************************/

Burger::eError BURGER_API Burger::ConsoleApp::SetWindowTitle(const char* pTitle)
{
	// Pack to a single string
	String Converter(
		"\x1B"
		"]0;",
		pTitle, "\x07");
	// Issue a single call to the terminal to update the header
	fwrite(Converter.c_str(), 1, Converter.length(), stdout);

	// Just say there was no error.
	return kErrorNone;
}

/***************************************

	\brief Set the size of the console window.

	On platforms that present console text output in a desktop window, this
	function will resize the window to the specified width and height in text
	cells. On platforms that don't text window resizing, this function does
	nothing and returns no error.

	\param uWidth Number of text cells wide of the new window
	\param uHeight Number of text cells high of the new window

	\return The current error code. Can be returned to the operating system.
	\sa WasDesktopLaunched()

***************************************/

Burger::eError BURGER_API Burger::ConsoleApp::SetWindowSize(
	uint_t uWidth, uint_t uHeight)
{
	NumberString Width(uWidth);
	NumberString Height(uHeight);
	// Pack to a single string
	String Converter(
		"\x1B"
		"[8;");
	Converter += Height.c_str();
	Converter += ';';
	Converter += Width.c_str();
	Converter += 't';
	// Issue a single call to the terminal to update the header
	fwrite(Converter.c_str(), 1, Converter.length(), stdout);

	return kErrorNone;
}

#endif
