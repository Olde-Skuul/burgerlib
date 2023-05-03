/***************************************

	Shims for d3d11.dll or dxgi.dll

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __WIN_D3D11_H__
#define __WIN_D3D11_H__

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
extern long BURGER_API D3D11CreateDevice(IDXGIAdapter* pAdapter,
	uint_t DriverType, HINSTANCE__* Software, uint_t Flags,
	const uint_t* pFeatureLevels, uint_t FeatureLevels, uint_t SDKVersion,
	ID3D11Device** ppDevice, uint_t* pFeatureLevel,
	ID3D11DeviceContext** ppImmediateContext) BURGER_NOEXCEPT;
extern long BURGER_API CreateDXGIFactory(
	const GUID* pGuidFactory, void** ppFactory) BURGER_NOEXCEPT;
extern long BURGER_API CreateDXGIFactory1(
	const GUID* pGuidFactory, void** ppFactory) BURGER_NOEXCEPT;
extern long BURGER_API CreateDXGIFactory2(
	uint_t uFlags, const GUID* pGuidFactory, void** ppFactory) BURGER_NOEXCEPT;
}}
/* END */

#endif
