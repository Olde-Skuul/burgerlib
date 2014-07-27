/***************************************

	Static class to be a shim to an operating system cursor

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BROSCURSOR_H__
#define __BROSCURSOR_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

#if defined(BURGER_MAC) && !defined(__BRMACTYPES_H__)
#include "brmactypes.h"
#endif

/* BEGIN */
namespace Burger {
class OSCursor {
	BURGER_DISABLECOPYCONSTRUCTORS(OSCursor);
#if defined(BURGER_MAC) || defined(DOXYGEN)
	CCrsr **m_pCursorImage;		///< MacOS only, handle to the cursor image
#endif
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	HICON__ *m_pCursorImage;	///< Windows only, handle to the cursor image
#endif
	Word m_uIDNumber;			///< ID Number of the currently set cursor
	Word m_bVisibleFlag;		///< \ref TRUE if the cursor is visible
	static OSCursor g_Global;
	OSCursor();
public:
	~OSCursor();
	static BURGER_INLINE Word IsVisible(void) { return g_Global.m_bVisibleFlag; }
	static BURGER_INLINE Word GetImageIDNumber(void) { return g_Global.m_uIDNumber; }
	static BURGER_INLINE Word IsThisADesktop(void) {
#if defined(BURGER_WINDOWS) || defined(BURGER_MAC)
		return TRUE;
#else
		return FALSE;
#endif
	}
	static void BURGER_API SetImageFromIDNumber(Word uCursorNumber);
	static Word BURGER_API Show(void);
	static Word BURGER_API Show(Word bVisible);
	static Word BURGER_API Hide(void);
	static void BURGER_API Init(void);
	static void BURGER_API Shutdown(void);
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	static void BURGER_API Refresh(void);
#endif
};
}
/* END */

#endif
