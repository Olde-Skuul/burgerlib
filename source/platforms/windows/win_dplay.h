/***************************************

	Shims for dplayx.dll

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __WIN_DPLAY_H__
#define __WIN_DPLAY_H__

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
extern long BURGER_API DirectPlayCreate(GUID* pGuidSrc, IDirectPlay** ppOutput,
	IUnknown* pOuter = nullptr) BURGER_NOEXCEPT;
extern long BURGER_API CallDirectPlayEnumerate(
	void* pCallback, void* pContext) BURGER_NOEXCEPT;
extern long BURGER_API DirectPlayEnumerateA(
	void* pCallback, void* pContext) BURGER_NOEXCEPT;
extern long BURGER_API DirectPlayEnumerateW(
	void* pCallback, void* pContext) BURGER_NOEXCEPT;
extern long BURGER_API DirectPlayLobbyCreateA(GUID* pGuidSrc,
	IDirectPlayLobby** ppOutput, IUnknown* pOuter = nullptr,
	void* pData = nullptr, uint_t uDataSize = 0) BURGER_NOEXCEPT;
extern long BURGER_API DirectPlayLobbyCreateW(GUID* pGuidSrc,
	IDirectPlayLobby** ppOutput, IUnknown* pOuter = nullptr,
	void* pData = nullptr, uint_t uDataSize = 0) BURGER_NOEXCEPT;
}}

/* END */

#endif