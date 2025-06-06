/***************************************

	Shims for hid.dll

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __WIN_HID_H__
#define __WIN_HID_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRGUID_H__
#include "brguid.h"
#endif

/* BEGIN */
namespace Burger { namespace Win32 {
extern void BURGER_API HidD_GetHidGuid(GUID* pHidGuid) BURGER_NOEXCEPT;
}}
/* END */

#endif
