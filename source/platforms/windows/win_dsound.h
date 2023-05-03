/***************************************

	Shims for dsound.dll

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __WIN_DSOUND_H__
#define __WIN_DSOUND_H__

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

extern long BURGER_API DirectSoundCreate(const GUID* pGuidDevice,
	IDirectSound** ppOutput, IUnknown* pOuter = nullptr) BURGER_NOEXCEPT;
extern long BURGER_API DirectSoundEnumerateA(
	void* pDSEnumCallback, void* pContext) BURGER_NOEXCEPT;
extern long BURGER_API DirectSoundEnumerateW(
	void* pDSEnumCallback, void* pContext) BURGER_NOEXCEPT;
extern long BURGER_API DirectSoundCaptureCreate(const GUID* pGuidDevice,
	IDirectSoundCapture** ppOutput, IUnknown* pOuter = nullptr) BURGER_NOEXCEPT;
extern long BURGER_API DirectSoundCaptureEnumerateA(
	void* pDSEnumCallback, void* pContext) BURGER_NOEXCEPT;
extern long BURGER_API DirectSoundCaptureEnumerateW(
	void* pDSEnumCallback, void* pContext) BURGER_NOEXCEPT;
extern long BURGER_API DirectSoundCreate8(const GUID* pGuidDevice,
	IDirectSound8** ppOutput, IUnknown* pOuter = nullptr) BURGER_NOEXCEPT;
extern long BURGER_API DirectSoundCaptureCreate8(const GUID* pGuidDevice,
	IDirectSoundCapture** ppOutput, IUnknown* pOuter = nullptr) BURGER_NOEXCEPT;
extern long BURGER_API DirectSoundFullDuplexCreate(
	const GUID* pGuidCaptureDevice, const GUID* pGuidRenderDevice,
	const _DSCBUFFERDESC* pDSCBufferDesc, const _DSBUFFERDESC* pDSBufferDesc,
	HWND__* hWnd, uint32_t uLevel, IDirectSoundFullDuplex** ppDSFD,
	IDirectSoundCaptureBuffer8** ppDSCBuffer8,
	IDirectSoundBuffer8** ppDSBuffer8,
	IUnknown* pOuter = nullptr) BURGER_NOEXCEPT;
extern long BURGER_API GetDeviceID(
	const GUID* pGuidSrc, GUID* pGuidDest) BURGER_NOEXCEPT;
}}
/* END */

#endif
