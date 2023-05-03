/***************************************

	Shims for d3d9.dll or d3dx9_43.dll

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __WIN_D3D9_H__
#define __WIN_D3D9_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __WIN_TYPES_H__
#include "win_types.h"
#endif

/* BEGIN */
namespace Burger {
namespace Win32 {

extern IDirect3D9* BURGER_API Direct3DCreate9(
	uint_t uSDKVersion) BURGER_NOEXCEPT;
extern int BURGER_API D3DPERF_BeginEvent(
	uint32_t col, const uint16_t* wszName) BURGER_NOEXCEPT;
extern int BURGER_API D3DPERF_EndEvent(void) BURGER_NOEXCEPT;
extern void BURGER_API D3DPERF_SetMarker(
	uint32_t col, const uint16_t* wszName) BURGER_NOEXCEPT;
extern void BURGER_API D3DPERF_SetRegion(
	uint32_t col, const uint16_t* wszName) BURGER_NOEXCEPT;
extern int BURGER_API D3DPERF_QueryRepeatFrame(void) BURGER_NOEXCEPT;
extern void BURGER_API D3DPERF_SetOptions(uint32_t dwOptions) BURGER_NOEXCEPT;
extern uint_t BURGER_API D3DPERF_GetStatus(void) BURGER_NOEXCEPT;
extern long BURGER_API D3DXCreateMatrixStack(
	uint_t uFlags, ID3DXMatrixStack** ppStack) BURGER_NOEXCEPT;

}

class DirectXPerfMarker {
	BURGER_DISABLE_COPY(DirectXPerfMarker);

public:
	BURGER_INLINE DirectXPerfMarker(
		uint32_t uColor, const uint16_t* pMessage) BURGER_NOEXCEPT
	{
		Win32::D3DPERF_BeginEvent(uColor, pMessage);
	}

	BURGER_INLINE ~DirectXPerfMarker() BURGER_NOEXCEPT
	{
		Win32::D3DPERF_EndEvent();
	}
};

}
/* END */

#endif
