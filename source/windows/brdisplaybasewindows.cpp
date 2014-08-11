/***************************************

	Display base class

	Windows version

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brdisplay.h"

#if defined(BURGER_WINDOWS)
#include "brwindowsapp.h"
#include "brstring16.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

/***************************************

	If a window is present, set the text to a specific string

***************************************/

void Burger::Display::SetWindowTitle(const char *pTitle)
{
	HWND pWindow = static_cast<WindowsApp *>(m_pGameApp)->GetWindow();
	// Is the window present?
	if (pWindow) {
		String16 MyString(pTitle);
		// Set the new title text
		SetWindowTextW(pWindow,reinterpret_cast<LPCWSTR>(MyString.c_str()));
	}
}

#endif

