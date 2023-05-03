/***************************************

	Shims for xinput1_3.dll and xinput1_4.dll

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __WIN_XINPUT_H__
#define __WIN_XINPUT_H__

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
extern uint32_t BURGER_API XInputGetState(
	uint32_t dwUserIndex, _XINPUT_STATE* pState) BURGER_NOEXCEPT;
extern uint32_t BURGER_API XInputSetState(
	uint32_t dwUserIndex, _XINPUT_VIBRATION* pVibration) BURGER_NOEXCEPT;
extern uint32_t BURGER_API XInputGetCapabilities(uint32_t dwUserIndex,
	uint32_t dwFlags, _XINPUT_CAPABILITIES* pCapabilities) BURGER_NOEXCEPT;
extern uint32_t BURGER_API XInputGetDSoundAudioDeviceGuids(uint32_t dwUserIndex,
	GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid) BURGER_NOEXCEPT;
extern void BURGER_API XInputEnable(uint_t bEnable) BURGER_NOEXCEPT;
extern uint32_t BURGER_API XInputGetAudioDeviceIds(uint32_t dwUserIndex,
	uint16_t* pRenderDeviceId, uint_t* pRenderCount, uint16_t* pCaptureDeviceId,
	uint_t* pCaptureCount) BURGER_NOEXCEPT;
extern uint32_t BURGER_API XInputGetBatteryInformation(uint32_t dwUserIndex,
	uint8_t devType,
	_XINPUT_BATTERY_INFORMATION* pBatteryInformation) BURGER_NOEXCEPT;
extern uint32_t BURGER_API XInputGetKeystroke(uint32_t dwUserIndex,
	uint32_t dwReserved, _XINPUT_KEYSTROKE* pKeystroke) BURGER_NOEXCEPT;
}}
/* END */

#endif
