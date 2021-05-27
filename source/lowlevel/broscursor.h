/***************************************

    Static class to be a shim to an operating system cursor

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BROSCURSOR_H__
#define __BROSCURSOR_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRERROR_H__
#include "brerror.h"
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
    BURGER_DISABLE_COPY(OSCursorImage);

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
    HICON__* m_pCursorImage; ///< Windows only, handle to the cursor image
#endif

    uint_t m_uWidth;  ///< Width of the cursor in pixels
    uint_t m_uHeight; ///< Height of the cursor in pixels
    int_t m_iHotX;    ///< Anchor X coordinate of the cursor
    int_t m_iHotY;    ///< Anchor Y coordinate of the cursor
public:
    OSCursorImage() BURGER_NOEXCEPT;
    ~OSCursorImage();
    eError BURGER_API CreateMonoChromeImage(const uint8_t* pXor,
        const uint8_t* pAnd, uint_t uWidth, uint_t uHeight, int_t iHotX, int_t iHotY) BURGER_NOEXCEPT;
    void BURGER_API Shutdown(void) BURGER_NOEXCEPT;
};

class OSCursor {
public:
    enum eCursor {
        CURSOR_NONE,  ///< No cursor at all
        CURSOR_ARROW, ///< Standard arrow cursor
        CURSOR_IBEAM, ///< Text edit i-beam cursor
        CURSOR_WAIT,  ///< Beachball cursor
        CURSOR_CROSS, ///< Crosshairs cursor
        CURSOR_COUNT, ///< Total number of system cursors
        CURSOR_CUSTOM ///< Cursor set by a call with SetImage()
    };

private:
    BURGER_DISABLE_COPY(OSCursor);

#if defined(BURGER_MAC) || defined(DOXYGEN)
    CCrsr** m_pCursorImage; ///< MacOS only, handle to the cursor image
#endif

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
    HICON__* m_pCursorImage; ///< Windows only, handle to the cursor image
#endif

    uint_t m_bVisibleFlag; ///< \ref TRUE if the cursor is visible
    uint_t m_bActiveFlag;  ///< \ref TRUE if a game cursor is loaded
    eCursor m_eIDNumber; ///< ID Number of the currently set cursor

    static OSCursor g_Global;
    OSCursor() BURGER_NOEXCEPT;

public:
    ~OSCursor();
    static BURGER_INLINE uint_t IsVisible(void) BURGER_NOEXCEPT
    {
        return g_Global.m_bVisibleFlag;
    }
    static BURGER_INLINE uint_t IsActive(void) BURGER_NOEXCEPT
    {
        return g_Global.m_bActiveFlag;
    }
    static BURGER_INLINE uint_t GetImageIDNumber(void) BURGER_NOEXCEPT
    {
        return g_Global.m_eIDNumber;
    }
    static BURGER_INLINE uint_t IsThisADesktop(void) BURGER_NOEXCEPT
    {
#if defined(BURGER_WINDOWS) || defined(BURGER_MACOS) || defined(BURGER_LINUX)
        return TRUE;
#else
        return FALSE;
#endif
    }
    static void BURGER_API SetImageFromIDNumber(eCursor eCursorNumber) BURGER_NOEXCEPT;
    static void BURGER_API SetImage(const OSCursorImage* pImage) BURGER_NOEXCEPT;
    static uint_t BURGER_API Show(void) BURGER_NOEXCEPT;
    static uint_t BURGER_API Show(uint_t bVisible) BURGER_NOEXCEPT;
    static uint_t BURGER_API Hide(void) BURGER_NOEXCEPT;
    static void BURGER_API Init(void) BURGER_NOEXCEPT;
    static void BURGER_API Shutdown(void) BURGER_NOEXCEPT;
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
    static void BURGER_API Refresh(void) BURGER_NOEXCEPT;
#endif
};
}
/* END */

#endif
