/***************************************

	Shims for user32.dll

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "win_user32.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "win_platformshims.h"

#if !defined(DOXYGEN)

//
// Handle some annoying defines that some windows SDKs may or may not have
//

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0501 // Windows XP
#endif

#include <Windows.h>

// Function prototypes, lovingly stolen from user32.h
typedef BOOL(WINAPI* TrackMouseEventPtr)(LPTRACKMOUSEEVENT lpEventTrack);
typedef BOOL(WINAPI* GetMonitorInfoAPtr)(HMONITOR hMonitor, LPMONITORINFO lpmi);
typedef BOOL(WINAPI* GetMonitorInfoWPtr)(HMONITOR hMonitor, LPMONITORINFO lpmi);
typedef HMONITOR(WINAPI* MonitorFromRectPtr)(LPCRECT lprc, DWORD dwFlags);
typedef HMONITOR(WINAPI* MonitorFromWindowPtr)(HWND hwnd, DWORD dwFlags);

// Unit tests for pointers
// TrackMouseEventPtr gTrackMouseEvent = ::TrackMouseEvent;
// GetMonitorInfoAPtr gGetMonitorInfoA = ::GetMonitorInfoA;
// GetMonitorInfoWPtr gGetMonitorInfoW = ::GetMonitorInfoW;
// MonitorFromRectPtr gMonitorFromRect = ::MonitorFromRect;
// MonitorFromWindowPtr gMonitorFromWindow = ::MonitorFromWindow;

/***************************************

	Self destructing timer that tracks the mouse and will send a WM_MOUSELEAVE
	event to the application if the mouse left the application window. This is
	necessary because if the mouse leaves the window, no mouse events are passed
	to the window, so this timer will monitor the mouse and if detected it's off
	the window, it will fire the event and end.

	This code is used for Win95/XP and NT to support the feature that appeared
	in XP Service Pack 1

***************************************/

static VOID CALLBACK TrackMouseTimerProc(
	HWND pWindow, UINT /* uMsg */, UINT_PTR uEventID, DWORD /* dwTime */)
{
	// Get the rect of the tracked window

	// Note: The only way the call to GetClientRect() to fail is in the rare
	// case where the application is in shutdown while this time function was
	// active (I.E. The Window was released before this timer was externally
	// killed). To handle that 1 in million case, test for GetClientRect()'s
	// success

	RECT TheClientRect;
	if (GetClientRect(pWindow, &TheClientRect)) {

		// Remap it to the desktop (2 points make a rect)
		MapWindowPoints(pWindow, HWND_DESKTOP,
			static_cast<POINT*>(static_cast<void*>(&TheClientRect)), 2);

		// Where is the cursor?
		POINT TheCursorLocation;
		GetCursorPos(&TheCursorLocation);

		// Check if outside
		if (!PtInRect(&TheClientRect, TheCursorLocation) ||
			(WindowFromPoint(TheCursorLocation) != pWindow)) {

			// My work is done, commit suicide
			KillTimer(pWindow, uEventID);

			// Post the message to the app, to alert it that the mouse has
			// gone AWOL
			PostMessageW(pWindow, WM_MOUSELEAVE, 0, 0);
		}
	}
}

/***************************************

	Compatibility function for old versions of windows that do not implement
	TrackMouseEvent()

	\note This is not a full implementation, only the functionality that is
	needed for Burgerlib

***************************************/

static BOOL WINAPI Win95TrackMouseEvent(TRACKMOUSEEVENT* pEventTrack)
{
	BOOL uResult = FALSE;
	if (pEventTrack->dwFlags == TME_LEAVE) {
		// Install the timer
		uResult = SetTimer(pEventTrack->hwndTrack, pEventTrack->dwFlags, 100,
					  TrackMouseTimerProc) != 0;
	}
	return uResult;
}

#endif

/*! ************************************

	\brief Load in user32.dll and call TrackMouseEvent

	Manually load user32.dll if needed and call the Windows function
	TrackMouseEvent()

	On versions of windows that do not have TrackMouseEvent(), use a
	compatibility function that performs the same task

	https://msdn.microsoft.com/en-us/library/windows/desktop/ms646265(v=vs.85).aspx

	\windowsonly

	\param pEventTrack A pointer to a TRACKMOUSEEVENT structure that contains
		tracking information.

	\return If the function succeeds, the return value is nonzero.

***************************************/

BOOL BURGER_API Burger::Win32::TrackMouseEvent(
	tagTRACKMOUSEEVENT* pEventTrack) BURGER_NOEXCEPT
{
	void* pTrackMouseEvent = load_function(kCall_TrackMouseEvent);

	// No?
	if (!pTrackMouseEvent) {
		// Use a compatibility function
		pTrackMouseEvent = Win95TrackMouseEvent;
		override_function(kCall_TrackMouseEvent, pTrackMouseEvent);
	}
	return static_cast<TrackMouseEventPtr>(pTrackMouseEvent)(pEventTrack);
}

/*! ************************************

	\brief Load in user32.dll and call TrackMouseEvent

	Manually load user32.dll if needed and call the Windows function
	GetMonitorInfoA()

	On versions of windows that do not have GetMonitorInfoA(), use a
	compatibility function that performs the same task

	https://msdn.microsoft.com/en-us/library/windows/desktop/dd144901(v=vs.85).aspx

	\windowsonly

	\param hMonitor A handle to the display monitor of interest.
	\param pMonitorInfo A pointer to a MONITORINFO or MONITORINFOEX structure
		that receives information about the specified display monitor.

	\return If the function succeeds, the return value is nonzero.

***************************************/

BOOL BURGER_API Burger::Win32::GetMonitorInfoA(
	HMONITOR__* hMonitor, tagMONITORINFO* pMonitorInfo) BURGER_NOEXCEPT
{
	void* pGetMonitorInfoA = load_function(kCall_GetMonitorInfoA);

	// Failure
	BOOL uResult = FALSE;
	if (pGetMonitorInfoA) {
		uResult = static_cast<GetMonitorInfoAPtr>(pGetMonitorInfoA)(
			hMonitor, pMonitorInfo);
	} else {
		// Compatibility function
		RECT TempRect;
		if ((hMonitor == ((HMONITOR)0x12340042)) && pMonitorInfo &&
			(pMonitorInfo->cbSize >= sizeof(MONITORINFO)) &&
			SystemParametersInfoA(SPI_GETWORKAREA, 0, &TempRect, 0)) {

			pMonitorInfo->rcMonitor.left = 0;
			pMonitorInfo->rcMonitor.top = 0;
			pMonitorInfo->rcMonitor.right = GetSystemMetrics(SM_CXSCREEN);
			pMonitorInfo->rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
			pMonitorInfo->rcWork = TempRect;
			pMonitorInfo->dwFlags = MONITORINFOF_PRIMARY;
			uResult = TRUE;
		}
	}
	return uResult;
}

/*! ************************************

	\brief Load in user32.dll and call TrackMouseEvent

	Manually load user32.dll if needed and call the Windows function
	GetMonitorInfoW()

	On versions of windows that do not have GetMonitorInfoW(), use a
	compatibility function that performs the same task

	https://msdn.microsoft.com/en-us/library/windows/desktop/dd144901(v=vs.85).aspx

	\windowsonly

	\param hMonitor A handle to the display monitor of interest.
	\param pMonitorInfo A pointer to a MONITORINFO or MONITORINFOEX structure
		that receives information about the specified display monitor.

	\return If the function succeeds, the return value is nonzero.

***************************************/

BOOL BURGER_API Burger::Win32::GetMonitorInfoW(
	HMONITOR__* hMonitor, tagMONITORINFO* pMonitorInfo) BURGER_NOEXCEPT
{
	void* pGetMonitorInfoW = load_function(kCall_GetMonitorInfoW);

	// Failure
	BOOL uResult = FALSE;
	if (pGetMonitorInfoW) {
		uResult = static_cast<GetMonitorInfoWPtr>(pGetMonitorInfoW)(
			hMonitor, pMonitorInfo);
	} else {
		// Compatibility function
		RECT TempRect;
		if ((hMonitor == ((HMONITOR)0x12340042)) && pMonitorInfo &&
			(pMonitorInfo->cbSize >= sizeof(MONITORINFO)) &&
			SystemParametersInfoA(SPI_GETWORKAREA, 0, &TempRect, 0)) {

			pMonitorInfo->rcMonitor.left = 0;
			pMonitorInfo->rcMonitor.top = 0;
			pMonitorInfo->rcMonitor.right = GetSystemMetrics(SM_CXSCREEN);
			pMonitorInfo->rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
			pMonitorInfo->rcWork = TempRect;
			pMonitorInfo->dwFlags = MONITORINFOF_PRIMARY;
			uResult = TRUE;
		}
	}
	return uResult;
}

/*! ************************************

	\brief Load in user32.dll and call MonitorFromWindow

	Manually load user32.dll if needed and call the Windows function
	MonitorFromWindow()

	On versions of windows that do not have MonitorFromWindow(), use a
	compatibility function that performs the same task

	https://msdn.microsoft.com/en-us/library/windows/desktop/dd145064(v=vs.85).aspx

	\windowsonly

	\param pWindow A handle to the window of interest.
	\param uFlags Determines the function's return value if the window does not
		intersect any display monitor.

	\return \ref nullptr on failure, HMONITOR on success.

***************************************/

HMONITOR__* BURGER_API Burger::Win32::MonitorFromWindow(
	HWND__* pWindow, uint_t uFlags) BURGER_NOEXCEPT
{
	void* pMonitorFromWindow = load_function(kCall_MonitorFromWindow);

	// Failure
	HMONITOR pResult = ((HMONITOR)0x12340042);
	if (pMonitorFromWindow) {
		pResult = static_cast<MonitorFromWindowPtr>(pMonitorFromWindow)(
			pWindow, uFlags);
	}
	return pResult;
}

/*! ************************************

	\brief Load in user32.dll and call MonitorFromRect

	Manually load user32.dll if needed and call the Windows function
	MonitorFromRect()

	On versions of windows that do not have MonitorFromRect(), use a
	compatibility function that performs the same task

	https://msdn.microsoft.com/en-us/library/windows/desktop/dd145063(v=vs.85).aspx

	\windowsonly

	\param pRect A pointer to a RECT structure that specifies the rectangle of
		interest in virtual-screen coordinates.
	\param uFlags Determines the function's return value if the window does not
		intersect any display monitor.

	\return \ref nullptr on failure, HMONITOR on success.

***************************************/

HMONITOR__* BURGER_API Burger::Win32::MonitorFromRect(
	const tagRECT* pRect, uint_t uFlags) BURGER_NOEXCEPT
{
	void* pMonitorFromRect = load_function(kCall_MonitorFromRect);
	HMONITOR pResult = ((HMONITOR)0x12340042); // Failure
	if (pMonitorFromRect) {
		pResult =
			static_cast<MonitorFromRectPtr>(pMonitorFromRect)(pRect, uFlags);
	}
	return pResult;
}

#endif
