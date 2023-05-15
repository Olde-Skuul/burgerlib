/***************************************

    Static class to be a shim to an operating system cursor

    Windows specific code

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "broscursor.h"

#if defined(BURGER_WINDOWS)
#include "brglobals.h"
#include "brimage.h"
#include "win_globals.h"

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

//
// Lookup table to convert Burgerlib cursors to system default cursors
//

static const uint_t g_uSystemCursors[Burger::OSCursor::CURSOR_COUNT - 1] = {
    32512, // IDC_ARROW
    32513, // IDC_IBEAM
    32514, // IDC_WAIT
    32515  // IDC_CROSS
};

Burger::eError BURGER_API Burger::OSCursorImage::CreateMonoChromeImage(
    const uint8_t* pXor, const uint8_t* pAnd, uint_t uWidth, uint_t uHeight,
    int_t iHotX, int_t iHotY) BURGER_NOEXCEPT
{
    Shutdown();

    // Ask windows as to the maximum size allowed for a hardware cursor
    const uint_t uAllowedX = static_cast<uint_t>(GetSystemMetrics(SM_CXCURSOR));
    const uint_t uAllowedY = static_cast<uint_t>(GetSystemMetrics(SM_CYCURSOR));

    // Round up to the 8s
    const uint_t uFinalWidth = (uWidth + 7) & (~7);
    const uint_t uFinalHeight = uHeight;
    eError uResult = kErrorOutOfBounds;
    if ((uFinalWidth <= uAllowedX) && (uFinalHeight <= uAllowedY)) {
        m_uWidth = uFinalWidth;
        m_uHeight = uFinalHeight;
        m_iHotX = iHotX;
        m_iHotY = iHotY;
        HCURSOR hCursor = CreateCursor(Win32::get_instance(), iHotX, iHotY,
            static_cast<int>(uFinalWidth), static_cast<int>(uFinalHeight), pAnd,
            pXor);
        if (!hCursor) {
            uResult = kErrorInvalidParameter;
        } else {
            m_pCursorImage = hCursor;
            uResult = kErrorNone;
        }
    }
    return uResult;
}

/***************************************

    Release resources

***************************************/

void BURGER_API Burger::OSCursorImage::Shutdown(void) BURGER_NOEXCEPT
{
    if (m_pCursorImage) {
        DestroyCursor(m_pCursorImage);
        m_pCursorImage = nullptr;
    }
}

/***************************************

    Load an OS resource and set the cursor to it

***************************************/

void BURGER_API Burger::OSCursor::SetImageFromIDNumber(
    eCursor eCursorNumber) BURGER_NOEXCEPT
{
    // Was there a change?

    if (eCursorNumber != g_Global.m_eIDNumber) {
        uintptr_t uCursorResource = 0;
        if (eCursorNumber != CURSOR_NONE) {
            HINSTANCE hInstance;
            // Reset to the system cursor?
            if (eCursorNumber < CURSOR_COUNT) {
                uCursorResource = g_uSystemCursors[eCursorNumber - 1];
                hInstance = nullptr;
            } else {
                uCursorResource = static_cast<uintptr_t>(eCursorNumber);
                hInstance = Win32::get_instance();
            }
            // Try from the application's resource
            HCURSOR hCurs =
                LoadCursorW(hInstance, MAKEINTRESOURCEW(uCursorResource));
            // Did it load?
            if (!hCurs) {
                // Try the operating system instead
                hCurs =
                    LoadCursorW(nullptr, MAKEINTRESOURCEW(g_uSystemCursors[0]));
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
        SetCursor(LoadCursorW(nullptr, MAKEINTRESOURCEW(g_uSystemCursors[0])));
        g_Global.m_bActiveFlag = FALSE;
        g_Global.m_pCursorImage = nullptr;
        g_Global.m_eIDNumber = CURSOR_NONE;
    }
}

/*! ************************************

    \brief Set the cursor to a generated cursor

    Given a custom cursor, set the cursor to it.
    \param pImage nullptr to hide the cursor, or a pointer to a generated
        cursor

***************************************/

void BURGER_API Burger::OSCursor::SetImage(
    const OSCursorImage* pImage) BURGER_NOEXCEPT
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
    SetCursor(LoadCursorW(nullptr, MAKEINTRESOURCEW(g_uSystemCursors[0])));
    g_Global.m_bActiveFlag = FALSE;
    g_Global.m_pCursorImage = nullptr;
    g_Global.m_eIDNumber = CURSOR_NONE;
}

/***************************************

    Make an OS cursor visible

***************************************/

uint_t BURGER_API Burger::OSCursor::Show(void) BURGER_NOEXCEPT
{
    uint_t uResult = g_Global.m_bVisibleFlag;
    if (!uResult) {
        // It's visible now!
        g_Global.m_bVisibleFlag = TRUE;
        int iState;
        do {
            // Keep calling until it's visible
            iState = ShowCursor(TRUE);
        } while (iState < 0);
    }
    return uResult;
}

/***************************************

    Make an OS cursor visible

***************************************/

uint_t BURGER_API Burger::OSCursor::Show(uint_t bVisible) BURGER_NOEXCEPT
{
    bVisible = (bVisible != 0);
    uint_t uResult = g_Global.m_bVisibleFlag;
    if (uResult != bVisible) {
        // It's visible now!
        g_Global.m_bVisibleFlag = bVisible;
        int iState;
        if (bVisible) {
            do {
                // Keep calling until it's visible
                iState = ShowCursor(TRUE);
            } while (iState < 0);
        } else {
            do {
                // Keep calling until it's hidden
                iState = ShowCursor(FALSE);
            } while (iState >= 0);
        }
    }
    return uResult;
}

/***************************************

    Make an OS cursor disappear

***************************************/

uint_t BURGER_API Burger::OSCursor::Hide(void) BURGER_NOEXCEPT
{
    uint_t uResult = g_Global.m_bVisibleFlag;
    if (uResult) {
        g_Global.m_bVisibleFlag = FALSE;
        int iState;
        do {
            // Keep calling until it's hidden
            iState = ShowCursor(FALSE);
        } while (iState >= 0);
    }
    return uResult;
}

/***************************************

    Reset an OS cursor to an arrow and show it

***************************************/

void BURGER_API Burger::OSCursor::Init(void) BURGER_NOEXCEPT
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

void BURGER_API Burger::OSCursor::Shutdown(void) BURGER_NOEXCEPT
{
    Init();
}

#endif

/*! ************************************

    \brief If a custom cursor is defined, refresh the operating system

    \windowsonly

    Windows will sometimes change the desktop cursor when the cursor moves out
    of scope. This function will restore the cursor once it's back in scope.
    It's only called from within Windows window callback functions when mouse
    events are processed.

***************************************/

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
void BURGER_API Burger::OSCursor::Refresh(void) BURGER_NOEXCEPT
{
    if (g_Global.m_pCursorImage) {
        SetCursor(g_Global.m_pCursorImage);
    }
}

#endif
