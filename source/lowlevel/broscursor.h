/***************************************

	Static class to be a shim to an operating system cursor

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
class OSCursor;
class OSCursorImage {
	friend class OSCursor;
	BURGER_DISABLECOPYCONSTRUCTORS(OSCursorImage);
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	HICON__ *m_pCursorImage;	///< Windows only, handle to the cursor image
#endif
	Word m_uWidth;			///< Width of the cursor in pixels
	Word m_uHeight;			///< Height of the cursor in pixels
	Int m_iHotX;			///< Anchor X coordinate of the cursor
	Int m_iHotY;			///< Anchor Y coordinate of the cursor
public:
	OSCursorImage();
	~OSCursorImage();
	Word BURGER_API CreateMonoChromeImage(const Word8 *pXor,const Word8 *pAnd,Word uWidth,Word uHeight,Int iHotX,Int iHotY);
	void BURGER_API Shutdown(void);
};

class OSCursor {
public:
	enum eCursor {
		CURSOR_NONE,	///< No cursor at all
		CURSOR_ARROW,	///< Standard arrow cursor
		CURSOR_IBEAM,	///< Text edit i-beam cursor
		CURSOR_WAIT,	///< Beachball cursor
		CURSOR_CROSS,	///< Crosshairs cursor
		CURSOR_COUNT,	///< Total number of system cursors
		CURSOR_CUSTOM	///< Cursor set by a call with SetImage()
	};
private:
	BURGER_DISABLECOPYCONSTRUCTORS(OSCursor);
#if defined(BURGER_MAC) || defined(DOXYGEN)
	CCrsr **m_pCursorImage;		///< MacOS only, handle to the cursor image
#endif
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	HICON__ *m_pCursorImage;	///< Windows only, handle to the cursor image
#endif
	eCursor m_eIDNumber;		///< ID Number of the currently set cursor
	Word m_bVisibleFlag;		///< \ref TRUE if the cursor is visible
	Word m_bActiveFlag;			///< \ref TRUE if a game cursor is loaded
	static OSCursor g_Global;
	OSCursor();
public:
	~OSCursor();
	static BURGER_INLINE Word IsVisible(void) { return g_Global.m_bVisibleFlag; }
	static BURGER_INLINE Word IsActive(void) { return g_Global.m_bActiveFlag; }
	static BURGER_INLINE Word GetImageIDNumber(void) { return g_Global.m_eIDNumber; }
	static BURGER_INLINE Word IsThisADesktop(void) {
#if defined(BURGER_WINDOWS) || defined(BURGER_MACOS) || defined(BURGER_LINUX)
		return TRUE;
#else
		return FALSE;
#endif
	}
	static void BURGER_API SetImageFromIDNumber(eCursor eCursorNumber);
	static void BURGER_API SetImage(const OSCursorImage *pImage);
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
