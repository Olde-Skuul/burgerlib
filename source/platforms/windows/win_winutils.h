/***************************************

	Window helper functions for Microsoft Windows

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __WIN_WINUTILS_H__
#define __WIN_WINUTILS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __WIN_TYPES_H__
#include "win_types.h"
#endif

/* BEGIN */
namespace Burger { namespace Win32 {
extern HWND__* BURGER_API get_window(void) BURGER_NOEXCEPT;
extern void BURGER_API set_window(HWND__* pInput) BURGER_NOEXCEPT;
extern const uint16_t* BURGER_API get_window_class_name(void) BURGER_NOEXCEPT;
extern void BURGER_API change_style(HWND__* hWindow, uint32_t uAddStyle,
	uint32_t uAddStyleEx, uint32_t uSubStyle,
	uint32_t uSubStyleEx) BURGER_NOEXCEPT;
extern void BURGER_API pump_messages(void) BURGER_NOEXCEPT;
extern void BURGER_API output_windows_message(
	uint_t uMessage, uintptr_t wParam, uintptr_t lParam) BURGER_NOEXCEPT;
}}
/* END */

#endif
