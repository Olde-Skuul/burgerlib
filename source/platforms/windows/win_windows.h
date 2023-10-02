/***************************************

	Include Windows.h in a compatible way

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __WIN_WINDOWS_H__
#define __WIN_WINDOWS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/***************************************

	Codewarrior, Watcom and other older IDEs are missing features
	that are present in modern versions of the Windows SDK

	This header will include Windows.h and restore more modern APIs
	and allow other SDKs, like DirectX which require advanced windows
	API features

***************************************/

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

// Windows 8 API level
#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0602
#endif

#if !defined(DIRECTINPUT_VERSION)
#define DIRECTINPUT_VERSION 0x800
#endif

#if !defined(DIRECTDRAW_VERSION)
#define DIRECTDRAW_VERSION 0x700
#endif

#if !defined(DIRECT3D_VERSION)
#define DIRECT3D_VERSION 0x900
#endif

#if !defined(DIRECTSOUND_VERSION)
#define DIRECTSOUND_VERSION 0x800
#endif

// Metrowerks requires inclusion of sal.h since it's Windows headers are so old
// and without this, DirectX will not include properly
// This file is found in source\platform\windows\vc7compat
#if defined(BURGER_METROWERKS)
#include <sal.h>
#endif

// Include Windows.h from the compiler's headers
#include <Windows.h>

//
// Old Windows SDKs doesn't have these defined.
//

#if !defined(LOAD_LIBRARY_SEARCH_SYSTEM32)
#define LOAD_LIBRARY_SEARCH_SYSTEM32 0x00000800
#endif

#if !defined(WAVE_FORMAT_IEEE_FLOAT)
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#endif

#if !defined(GET_SC_WPARAM)
#define GET_SC_WPARAM(wParam) ((int)wParam & 0xFFF0)
#endif

#if !defined(PBT_APMQUERYSUSPEND)
#define PBT_APMQUERYSUSPEND 0x0000
#endif

#if !defined(PBT_APMRESUMESUSPEND)
#define PBT_APMRESUMESUSPEND 0x0007
#endif

#if !defined(WM_MOUSEHWHEEL)
#define WM_MOUSEHWHEEL 0x020E
#endif

#if !defined(WM_NCMOUSELEAVE)
#define WM_NCMOUSELEAVE 0x02A2
#endif

#if !defined(INVALID_ATOM)
#define INVALID_ATOM ((ATOM)0)
#endif

#if !defined(WM_KEYF1)
#define WM_KEYF1 0x004D
#endif

#if !defined(WM_UAHDESTROYWINDOW)
#define WM_UAHDESTROYWINDOW 0x0090
#endif

#if !defined(WM_UAHDRAWMENU)
#define WM_UAHDRAWMENU 0x0091
#endif

#if !defined(WM_UAHDRAWMENUITEM)
#define WM_UAHDRAWMENUITEM 0x0092
#endif

#if !defined(WM_UAHINITMENU)
#define WM_UAHINITMENU 0x0093
#endif

#if !defined(WM_UAHMEASUREMENUITEM)
#define WM_UAHMEASUREMENUITEM 0x0094
#endif

#if !defined(WM_UAHNCPAINTMENUPOPUP)
#define WM_UAHNCPAINTMENUPOPUP 0x0095
#endif

#if !defined(WM_NCUAHDRAWCAPTION)
#define WM_NCUAHDRAWCAPTION 0x00AE
#endif

#if !defined(WM_NCUAHDRAWFRAME)
#define WM_NCUAHDRAWFRAME 0x00AF
#endif

#if !defined(WM_INPUT_DEVICE_CHANGE)
#define WM_INPUT_DEVICE_CHANGE 0x00FE
#endif

#if !defined(WM_INPUT)
#define WM_INPUT 0x00FF
#endif

#if !defined(WM_GESTURE)
#define WM_GESTURE 0x0119
#endif

#if !defined(WM_GESTURENOTIFY)
#define WM_GESTURENOTIFY 0x011A
#endif

#if !defined(WM_TOUCH)
#define WM_TOUCH 0x0240
#endif

#if !defined(WM_NCMOUSEHOVER)
#define WM_NCMOUSEHOVER 0x02A0
#endif

#if !defined(WM_CLIPBOARDUPDATE)
#define WM_CLIPBOARDUPDATE 0x031D
#endif

#if !defined(WM_DWMCOMPOSITIONCHANGED)
#define WM_DWMCOMPOSITIONCHANGED 0x031E
#endif

#if !defined(WM_DWMNCRENDERINGCHANGED)
#define WM_DWMNCRENDERINGCHANGED 0x031F
#endif

#if !defined(WM_DWMCOLORIZATIONCOLORCHANGED)
#define WM_DWMCOLORIZATIONCOLORCHANGED 0x0320
#endif

#if !defined(WM_DWMWINDOWMAXIMIZEDCHANGE)
#define WM_DWMWINDOWMAXIMIZEDCHANGE 0x0321
#endif

#if !defined(GAA_FLAG_SKIP_MULTICAST)
#if defined(BURGER_WATCOM)
#define GAA_FLAG_SKIP_MULTICAST 0x00000004L
#else
#define GAA_FLAG_SKIP_MULTICAST 0x0004
#endif
#endif

#if !defined(GAA_FLAG_SKIP_DNS_SERVER)
#if defined(BURGER_WATCOM)
#define GAA_FLAG_SKIP_DNS_SERVER 0x00000008L
#else
#define GAA_FLAG_SKIP_DNS_SERVER 0x0008
#endif
#endif

#if !defined(GAA_FLAG_INCLUDE_PREFIX)
#if defined(BURGER_WATCOM)
#define GAA_FLAG_INCLUDE_PREFIX 0x00000010L
#else
#define GAA_FLAG_INCLUDE_PREFIX 0x0010
#endif
#endif

#if !defined(GAA_FLAG_SKIP_FRIENDLY_NAME)
#if defined(BURGER_WATCOM)
#define GAA_FLAG_SKIP_FRIENDLY_NAME 0x00000020L
#else
#define GAA_FLAG_SKIP_FRIENDLY_NAME 0x0020
#endif
#endif

#if !defined(_WIN32_WINNT_VISTA) && !defined(_WIN32_WINNT_LONGHORN)
#define TokenElevation static_cast<TOKEN_INFORMATION_CLASS>(20)
struct _TOKEN_ELEVATION {
	DWORD TokenIsElevated;
};
#endif

#endif
