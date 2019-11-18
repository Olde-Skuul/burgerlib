/***************************************

	Static class to be a shim to an operating system cursor

	MacOS Carbon specific code

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "broscursor.h"

#if defined(BURGER_MAC)
#include <Quickdraw.h>

/***************************************

	Load an OS resource and set the cursor to it

***************************************/

void BURGER_API Burger::OSCursor::SetImageFromIDNumber(eCursor eCursorNumber)
{
	if (eCursorNumber != g_Global.m_eIDNumber) {
		// Load the cursor
		CCrsrHandle hCursor = GetCCursor(eCursorNumber);
		// Did it load?
		if (hCursor) {
			g_Global.m_eIDNumber = eCursorNumber;
			// Set the cursor
			SetCCursor(hCursor);
			// Was there a cursor before?
			if (g_Global.m_pCursorImage) {
				// Dispose of the cursor
				DisposeCCursor(g_Global.m_pCursorImage);
			}
			// Save the cursor record to delete
			g_Global.m_pCursorImage = hCursor;
		}
	}
}

/***************************************

	Make an OS cursor visible

***************************************/

Word BURGER_API Burger::OSCursor::Show(void)
{
	Word uResult = g_Global.m_bVisibleFlag;
	if (!uResult) {
		// It's visible
		g_Global.m_bVisibleFlag = TRUE;
		ShowCursor();
	}
	return uResult;
}

/***************************************

	Make an OS cursor visible or invisible

***************************************/

Word BURGER_API Burger::OSCursor::Show(Word bVisible)
{
	bVisible = (bVisible != 0);
	Word uResult = g_Global.m_bVisibleFlag;
	if (uResult != bVisible) {
		// It's visible now!
		g_Global.m_bVisibleFlag = bVisible;
		if (bVisible) {
			ShowCursor();
		} else {
			HideCursor();
		}
	}
	return uResult;
}

/***************************************

	Make an OS cursor disappear

***************************************/

Word BURGER_API Burger::OSCursor::Hide(void)
{
	Word uResult = g_Global.m_bVisibleFlag;
	if (uResult) {
		// It's visible
		g_Global.m_bVisibleFlag = FALSE;
		HideCursor();
	}
	return uResult;
}

/***************************************

	Reset an OS cursor to an arrow and show it

***************************************/

void BURGER_API Burger::OSCursor::Init(void)
{
	InitCursor();
	g_Global.m_bVisibleFlag = TRUE;
	g_Global.m_eIDNumber = CURSOR_ARROW;
}

/***************************************

	Release all resources allocated by the cursor

***************************************/

void BURGER_API Burger::OSCursor::Shutdown(void)
{
	// Was there a cursor before?
	if (g_Global.m_pCursorImage) {
		// Dispose of the cursor
		DisposeCCursor(g_Global.m_pCursorImage);
		g_Global.m_pCursorImage = NULL;
	}
}

#endif
