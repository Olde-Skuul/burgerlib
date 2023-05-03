/***************************************

	Shims for ddraw.dll

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __WIN_DDRAW_H__
#define __WIN_DDRAW_H__

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
extern uint_t BURGER_API DirectDrawCreate(const GUID* pGuid,
	IDirectDraw** ppOutput, IUnknown* pOuter = nullptr) BURGER_NOEXCEPT;
extern uint_t BURGER_API DirectDrawCreateEx(const GUID* pGuid, void** ppOutput,
	const GUID& rGUID, IUnknown* pOuter = nullptr) BURGER_NOEXCEPT;
extern uint_t BURGER_API DirectDrawCreateClipper(uint32_t uFlags,
	IDirectDrawClipper** ppOutput, IUnknown* pOuter = nullptr) BURGER_NOEXCEPT;
extern long BURGER_API DirectDrawEnumerateA(
	void* pCallback, void* pContext) BURGER_NOEXCEPT;
extern long BURGER_API DirectDrawEnumerateW(
	void* pCallback, void* pContext) BURGER_NOEXCEPT;
extern long BURGER_API DirectDrawEnumerateExA(
	void* pCallback, void* pContext, uint32_t uFlags) BURGER_NOEXCEPT;
extern long BURGER_API DirectDrawEnumerateExW(
	void* pCallback, void* pContext, uint32_t uFlags) BURGER_NOEXCEPT;
extern long BURGER_API get_video_GUID(
	GUID* pOutput, uint32_t uDevNum) BURGER_NOEXCEPT;
}}
/* END */

#endif
