/***************************************

	Shims for user32.dll

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __WIN_USER32_H__
#define __WIN_USER32_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __WIN_TYPES_H__
#include "win_types.h"
#endif

/* BEGIN */
namespace Burger { namespace Win32 {
extern int BURGER_API TrackMouseEvent(
	tagTRACKMOUSEEVENT* pEventTrack) BURGER_NOEXCEPT;
extern int BURGER_API GetMonitorInfoA(
	HMONITOR__* hMonitor, tagMONITORINFO* pMonitorInfo) BURGER_NOEXCEPT;
extern int BURGER_API GetMonitorInfoW(
	HMONITOR__* hMonitor, tagMONITORINFO* pMonitorInfo) BURGER_NOEXCEPT;
extern HMONITOR__* BURGER_API MonitorFromWindow(
	HWND__* pWindow, uint_t uFlags) BURGER_NOEXCEPT;
extern HMONITOR__* BURGER_API MonitorFromRect(
	const tagRECT* pRect, uint_t uFlags) BURGER_NOEXCEPT;
}}

/* END */

#endif
