/***************************************

	Static class to be a shim to an operating system cursor

	Windows specific code

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "broscursor.h"

#if defined(BURGER_WINDOWS)
#include "brglobals.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

/***************************************

	Load an OS resource and set the cursor to it

***************************************/

void BURGER_API Burger::OSCursor::SetImageFromIDNumber(Word uCursorNumber)
{
	// Was there a change?

	if (uCursorNumber!=g_Global.m_uIDNumber) {

		// Reset to the system cursor?
		if (uCursorNumber) {
			// Try from the application's resource
			HCURSOR hCurs = LoadCursorW(Globals::GetInstance(),MAKEINTRESOURCEW(uCursorNumber));
			// Did it load?
			if (hCurs==NULL) {
				// Try the operating system instead
				hCurs = LoadCursorW(NULL,MAKEINTRESOURCEW(uCursorNumber));
			}
			if (hCurs!=NULL) {
				g_Global.m_uIDNumber = uCursorNumber;
				g_Global.m_pCursorImage = hCurs;
				// Tell windows to use this cursor
				SetCursor(hCurs);
				return;
			}
		}
		SetCursor(LoadCursorW(NULL,MAKEINTRESOURCEW(IDC_ARROW)));
		g_Global.m_pCursorImage = NULL;
	}
}

/***************************************

	Make an OS cursor visible

***************************************/

Word BURGER_API Burger::OSCursor::Show(void)
{
	Word uResult = g_Global.m_bVisibleFlag;
	if (!uResult) {
		// It's visible now!
		g_Global.m_bVisibleFlag = TRUE;
		int iState;
		do {
			// Keep calling until it's visible
			iState=ShowCursor(TRUE);
		} while (iState<0);
	}
	return uResult;
}

/***************************************

	Make an OS cursor visible

***************************************/

Word BURGER_API Burger::OSCursor::Show(Word bVisible)
{
	bVisible = (bVisible!=0);
	Word uResult = g_Global.m_bVisibleFlag;
	if (uResult!=bVisible) {
		// It's visible now!
		g_Global.m_bVisibleFlag = bVisible;
		int iState;
		if (bVisible) {
			do {
				// Keep calling until it's visible
				iState=ShowCursor(TRUE);
			} while (iState<0);
		} else {
			do {
				// Keep calling until it's hidden
				iState=ShowCursor(FALSE);
			} while (iState>=0);
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
		g_Global.m_bVisibleFlag = FALSE;
		int iState;
		do {
			// Keep calling until it's hidden
			iState=ShowCursor(FALSE);
		} while (iState>=0);
	}
	return uResult;
}

/***************************************

	Reset an OS cursor to an arrow and show it

***************************************/

void BURGER_API Burger::OSCursor::Init(void)
{
	// Set to the arrow
	SetImageFromIDNumber(0);
	// Force the cursor to be shown
	g_Global.m_bVisibleFlag = FALSE;
	// Make sure it's visible!
	Show();
}

/***************************************

	Release all resources allocated by the cursor

***************************************/

void BURGER_API Burger::OSCursor::Shutdown(void)
{
	Init();
}

/*! ************************************

	\brief If a custom cursor is defined, refresh the operating system

	\note Windows Only!

	Windows will sometimes change the desktop cursor when the cursor moves out of scope.
	This function will restore the cursor once it's back in scope. It's only called
	from within Windows window callback functions when mouse events are processed.

***************************************/

void BURGER_API Burger::OSCursor::Refresh(void)
{
	if (g_Global.m_pCursorImage) {
		SetCursor(g_Global.m_pCursorImage);
	}
}

#endif