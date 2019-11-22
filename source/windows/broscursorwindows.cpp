/***************************************

	Static class to be a shim to an operating system cursor

	Windows specific code

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "broscursor.h"

#if defined(BURGER_WINDOWS)
#include "brglobals.h"
#include "brimage.h"

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

//
// Lookup table to convert Burgerlib cursors to system default cursors
//

static const Word g_uSystemCursors[Burger::OSCursor::CURSOR_COUNT-1] = {
	32512,		// IDC_ARROW
	32513,		// IDC_IBEAM
	32514,		// IDC_WAIT
	32515		// IDC_CROSS
};


Word BURGER_API Burger::OSCursorImage::CreateMonoChromeImage(const Word8 *pXor,const Word8 *pAnd,Word uWidth,Word uHeight,Int iHotX,Int iHotY)
{
	Shutdown();

	// Ask windows as to the maximum size allowed for a hardware cursor
	Word uAllowedX = static_cast<Word>(GetSystemMetrics(SM_CXCURSOR));
	Word uAllowedY = static_cast<Word>(GetSystemMetrics(SM_CYCURSOR));

	// Round up to the 8s
	Word uFinalWidth = (uWidth+7)&(~7);
	Word uFinalHeight = uHeight;
	Word uResult = 10;
	if ((uFinalWidth<=uAllowedX) && (uFinalHeight<=uAllowedY)) {
		m_uWidth = uFinalWidth;
		m_uHeight = uFinalHeight;
		m_iHotX = iHotX;
		m_iHotY = iHotY;
		HCURSOR hCursor = CreateCursor(Windows::GetInstance(),iHotX,iHotY,static_cast<int>(uFinalWidth),static_cast<int>(uFinalHeight),pAnd,pXor);
		if (hCursor) {
			m_pCursorImage = hCursor;
		}
	}
	return uResult;
}

/***************************************

	Release resources

***************************************/

void BURGER_API Burger::OSCursorImage::Shutdown(void)
{
	if (m_pCursorImage) {
		DestroyCursor(m_pCursorImage);
		m_pCursorImage = NULL;
	}
}

/***************************************

	Load an OS resource and set the cursor to it

***************************************/

void BURGER_API Burger::OSCursor::SetImageFromIDNumber(eCursor eCursorNumber)
{
	// Was there a change?

	if (eCursorNumber!=g_Global.m_eIDNumber) {
		WordPtr uCursorResource = 0;
		if (eCursorNumber!=CURSOR_NONE) {
			HINSTANCE hInstance;
			// Reset to the system cursor?
			if (eCursorNumber < CURSOR_COUNT) {
				uCursorResource = g_uSystemCursors[eCursorNumber-1];
				hInstance = NULL;
			} else {
				uCursorResource = static_cast<WordPtr>(eCursorNumber);
				hInstance = Windows::GetInstance();
			}
			// Try from the application's resource
			HCURSOR hCurs = LoadCursorW(hInstance,MAKEINTRESOURCEW(uCursorResource));
			// Did it load?
			if (!hCurs) {
				// Try the operating system instead
				hCurs = LoadCursorW(NULL,MAKEINTRESOURCEW(g_uSystemCursors[0]));
			}
			if (hCurs) {
				g_Global.m_eIDNumber = eCursorNumber;
				g_Global.m_pCursorImage = hCurs;
				g_Global.m_bActiveFlag = TRUE;
				// Tell windows to use this cursor
				SetCursor(hCurs);
				Show();
				return;
			}
		}
		// Force to a system cursor
		Hide();
		SetCursor(LoadCursorW(NULL,MAKEINTRESOURCEW(g_uSystemCursors[0])));
		g_Global.m_bActiveFlag = FALSE;
		g_Global.m_pCursorImage = NULL;
		g_Global.m_eIDNumber = CURSOR_NONE;
	}
}

/*! ************************************

	\brief Set the cursor to a generated cursor

	Given a custom cursor, set the cursor to it.
	\param pImage \ref NULL to hide the cursor, or a pointer to a generated cursor

***************************************/

void BURGER_API Burger::OSCursor::SetImage(const OSCursorImage *pImage)
{
	if (pImage) {
		HCURSOR hCurs = pImage->m_pCursorImage;
		if (hCurs) {
			g_Global.m_eIDNumber = CURSOR_CUSTOM;
			g_Global.m_pCursorImage = hCurs;
			g_Global.m_bActiveFlag = TRUE;
			// Tell windows to use this cursor
			SetCursor(hCurs);
			Show();
			return;
		}
	}
	// Force to a system cursor
	Hide();
	SetCursor(LoadCursorW(NULL,MAKEINTRESOURCEW(g_uSystemCursors[0])));
	g_Global.m_bActiveFlag = FALSE;
	g_Global.m_pCursorImage = NULL;
	g_Global.m_eIDNumber = CURSOR_NONE;
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
	SetImageFromIDNumber(CURSOR_ARROW);
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

#endif

/*! ************************************

	\brief If a custom cursor is defined, refresh the operating system

	\windowsonly

	Windows will sometimes change the desktop cursor when the cursor moves out of scope.
	This function will restore the cursor once it's back in scope. It's only called
	from within Windows window callback functions when mouse events are processed.

***************************************/

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
void BURGER_API Burger::OSCursor::Refresh(void)
{
	if (g_Global.m_pCursorImage) {
		SetCursor(g_Global.m_pCursorImage);
	}
}

#endif
