/***************************************

	Shims for dinput.dll and dinput8.dll

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __WIN_DINPUT_H__
#define __WIN_DINPUT_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRGUID_H__
#include "brguid.h"
#endif

#ifndef __WIN_TYPES_H__
#include "win_types.h"
#endif

/* BEGIN */
namespace Burger { namespace Win32 {

extern long BURGER_API DirectInputCreateA(HINSTANCE__* hInst, uint32_t uVersion,
	IDirectInputA** ppOutput, IUnknown* pOuter = nullptr) BURGER_NOEXCEPT;
extern long BURGER_API DirectInputCreateW(HINSTANCE__* hInst, uint32_t uVersion,
	IDirectInputW** ppOutput, IUnknown* pOuter = nullptr) BURGER_NOEXCEPT;
extern long BURGER_API DirectInput8Create(HINSTANCE__* hInst, uint32_t uVersion,
	const GUID& rGUID, void** ppOutput,
	IUnknown* pOuter = nullptr) BURGER_NOEXCEPT;

extern IDirectInputW* BURGER_API get_DirectInput_singleton(
	uint32_t uVersion = 0x700U) BURGER_NOEXCEPT;
extern IDirectInput8W* BURGER_API get_DirectInput8_singleton(
	void) BURGER_NOEXCEPT;
}}

/* END */

#endif
