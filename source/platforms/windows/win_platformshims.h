/***************************************

	System function loader specific to Windows

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __WIN_PLATFORMSHIMS_H__
#define __WIN_PLATFORMSHIMS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __WIN_TYPES_H__
#include "win_types.h"
#endif

/* BEGIN */

namespace Burger { namespace Win32 {

enum eDLLIndex {
	/** Index for ddraw.dll */
	kDLLIndex_DDRAW,
	/** Index for dinput.dll */
	kDLLIndex_DINPUT,
	/** Index for dinput8.dll */
	kDLLIndex_DINPUT8,
	/** Index for xinput1_4.dll */
	kDLLIndex_XINPUT1_4,
	/** Index for xinput1_3.dll */
	kDLLIndex_XINPUT1_3,
	/** Index for d3d9.dll */
	kDLLIndex_D3D9,
	/** Index for d3dx9_43.dll */
	kDLLIndex_D3DX9_43,
	/** Index for d3d11.dll */
	kDLLIndex_D3D11,
	/** Index for dxgi.dll */
	kDLLIndex_DXGI,
	/** Index for dsound.dll */
	kDLLIndex_DSOUND,
	/** Index for dplayx.dll */
	kDLLIndex_DPLAYX,
	/** Index for dplay.dll */
	kDLLIndex_DPLAY,
	/** Index for rpcrt4.dll */
	kDLLIndex_RPCRT4,
	/** Index for winmm.dll */
	kDLLIndex_WINMM,
	/** Index for shlwapi.dll */
	kDLLIndex_SHLWAPI,
	/** Index for version.dll */
	kDLLIndex_VERSION,
	/** Index for hid.dll */
	kDLLIndex_HID,
	/** Index for setupapi.dll */
	kDLLIndex_SETUPAPI,
	/** Index for user32.dll */
	kDLLIndex_USER32,
	/** Index for kernel32.dll */
	kDLLIndex_KERNEL32,
	/** Index for shell32.dll */
	kDLLIndex_SHELL32,
	/** Index for API-MS-Win-Core-Synch-l1-2-0.dll */
	kDLLIndex_SYNCAPI,
	/** Total number of DLLs to be managed */
	kDLLIndexCount
};

enum eCallIndex {
	/** Index for DirectInputCreateA() */
	kCall_DirectInputCreateA,
	/** Index for DirectInputCreateW() */
	kCall_DirectInputCreateW,
	/** Index for DirectInput8Create() */
	kCall_DirectInput8Create,
	/** Index for XInputGetState() */
	kCall_XInputGetState,
	/** Index for XInputSetState() */
	kCall_XInputSetState,
	/** Index for XInputGetCapabilities() */
	kCall_XInputGetCapabilities,
	/** Index for XInputGetDSoundAudioDeviceGuids(). */
	kCall_XInputGetDSoundAudioDeviceGuids,
	/** Index for XInputEnable() */
	kCall_XInputEnable,
	/** Index for XInputGetAudioDeviceIds() */
	kCall_XInputGetAudioDeviceIds,
	/** Index for XInputGetBatteryInformation() */
	kCall_XInputGetBatteryInformation,
	/** Index for XInputGetKeystroke() */
	kCall_XInputGetKeystroke,
	/** Index for DirectDrawCreate() */
	kCall_DirectDrawCreate,
	/** Index for DirectDrawCreateEx() */
	kCall_DirectDrawCreateEx,
	/** Index for DirectDrawCreateClipper() */
	kCall_DirectDrawCreateClipper,
	/** Index for DirectDrawEnumerateA() */
	kCall_DirectDrawEnumerateA,
	/** Index for DirectDrawEnumerateW() */
	kCall_DirectDrawEnumerateW,
	/** Index for DirectDrawEnumerateExA() */
	kCall_DirectDrawEnumerateExA,
	/** Index for DirectDrawEnumerateExW() */
	kCall_DirectDrawEnumerateExW,
	/** Index for Direct3DCreate9() */
	kCall_Direct3DCreate9,
	/** Index for D3DPERF_BeginEvent() */
	kCall_D3DPERF_BeginEvent,
	/** Index for D3DPERF_EndEvent() */
	kCall_D3DPERF_EndEvent,
	/** Index for D3DPERF_SetMarker() */
	kCall_D3DPERF_SetMarker,
	/** Index for D3DPERF_SetRegion() */
	kCall_D3DPERF_SetRegion,
	/** Index for D3DPERF_QueryRepeatFrame() */
	kCall_D3DPERF_QueryRepeatFrame,
	/** Index for D3DPERF_SetOptions() */
	kCall_D3DPERF_SetOptions,
	/** Index for D3DPERF_GetStatus() */
	kCall_D3DPERF_GetStatus,
	/** Index for D3DXCreateMatrixStack() */
	kCall_D3DXCreateMatrixStack,
	/** Index for D3D11CreateDevice() */
	kCall_D3D11CreateDevice,
	/** Index for CreateDXGIFactory() */
	kCall_CreateDXGIFactory,
	/** Index for CreateDXGIFactory1() */
	kCall_CreateDXGIFactory1,
	/** Index for CreateDXGIFactory2() */
	kCall_CreateDXGIFactory2,
	/** Index for DirectSoundCreate() */
	kCall_DirectSoundCreate,
	/** Index for DirectSoundEnumerateA() */
	kCall_DirectSoundEnumerateA,
	/** Index for DirectSoundEnumerateW() */
	kCall_DirectSoundEnumerateW,
	/** Index for DirectSoundCaptureCreate() */
	kCall_DirectSoundCaptureCreate,
	/** Index forDirectSoundCaptureEnumerateA() */
	kCall_DirectSoundCaptureEnumerateA,
	/** Index for DirectSoundCaptureEnumerateW() */
	kCall_DirectSoundCaptureEnumerateW,
	/** Index for DirectSoundCreate8() */
	kCall_DirectSoundCreate8,
	/** Index for DirectSoundCaptureCreate8() */
	kCall_DirectSoundCaptureCreate8,
	/** Index for DirectSoundFullDuplexCreate() */
	kCall_DirectSoundFullDuplexCreate,
	/** Index for DirectPlayCreate() */
	kCall_DirectPlayCreate,
	/** Index for DirectPlayEnumerate() Note, this is REALLY obsolete */
	kCall_DirectPlayEnumerate,
	/** Index for DirectPlayEnumerateA() */
	kCall_DirectPlayEnumerateA,
	/** Index for DirectPlayEnumerateW() */
	kCall_DirectPlayEnumerateW,
	/** Index for DirectPlayLobbyCreateA() */
	kCall_DirectPlayLobbyCreateA,
	/** Index for DirectPlayLobbyCreateW() */
	kCall_DirectPlayLobbyCreateW,
	/** Index for GetDeviceID() */
	kCall_GetDeviceID,
	/** Index for UuidCreateSequential() */
	kCall_UuidCreateSequential,
	/** Index for timeGetTime() */
	kCall_timeGetTime,
	/** Index for PathSearchAndQualifyA() */
	kCall_PathSearchAndQualifyA,
	/** Index for PathSearchAndQualifyW() */
	kCall_PathSearchAndQualifyW,
	/** Index for VerQueryValueA() */
	kCall_VerQueryValueA,
	/** Index for VerQueryValueW() */
	kCall_VerQueryValueW,
	/** Index for GetFileVersionInfoA() */
	kCall_GetFileVersionInfoA,
	/** Index for GetFileVersionInfoW() */
	kCall_GetFileVersionInfoW,
	/** Index for GetFileVersionInfoSizeA() */
	kCall_GetFileVersionInfoSizeA,
	/** Index for GetFileVersionInfoSizeW() */
	kCall_GetFileVersionInfoSizeW,
	/** Index for HidD_GetHidGuid() */
	kCall_HidD_GetHidGuid,
	/** Index for SetupDiGetClassDevsA() */
	kCall_SetupDiGetClassDevsA,
	/** Index for SetupDiGetClassDevsW() */
	kCall_SetupDiGetClassDevsW,
	/** Index for SetupDiGetDeviceInterfaceDetailA() */
	kCall_SetupDiGetDeviceInterfaceDetailA,
	/** Index for SetupDiGetDeviceInterfaceDetailW() */
	kCall_SetupDiGetDeviceInterfaceDetailW,
	/** Index for SetupDiEnumDeviceInterfaces() */
	kCall_SetupDiEnumDeviceInterfaces,
	/** Index for SetupDiDestroyDeviceInfoList() */
	kCall_SetupDiDestroyDeviceInfoList,
	/** Index for TrackMouseEvent() */
	kCall_TrackMouseEvent,
	/** Index for GetMonitorInfoA() */
	kCall_GetMonitorInfoA,
	/** Index for GetMonitorInfoW() */
	kCall_GetMonitorInfoW,
	/** Index for MonitorFromWindow() */
	kCall_MonitorFromWindow,
	/** Index for MonitorFromRect() */
	kCall_MonitorFromRect,
	/** Index for GetSystemWow64DirectoryA() */
	kCall_GetSystemWow64DirectoryA,
	/** Index for GetSystemWow64DirectoryW() */
	kCall_GetSystemWow64DirectoryW,
	/** Index for IsDebuggerPresent() */
	kCall_IsDebuggerPresent,
	/** Index for SetThreadDescription() */
	kCall_SetThreadDescription,
	/** Index for InitializeSRWLock() */
	kCall_InitializeSRWLock,
	/** Index for AcquireSRWLockExclusive() */
	kCall_AcquireSRWLockExclusive,
	/** Index for TryAcquireSRWLockExclusive() */
	kCall_TryAcquireSRWLockExclusive,
	/** Index for ReleaseSRWLockExclusive() */
	kCall_ReleaseSRWLockExclusive,
	/** Index for SHGetKnownFolderPath() */
	kCall_SHGetKnownFolderPath,
	/** Index for WaitOnAddress() */
	kCall_WaitOnAddress,
	/** Index for WakeByAddressSingle() */
	kCall_WakeByAddressSingle,
	/** Index for WakeConditionVariable() */
	kCall_WakeConditionVariable,
	/** Index for WakeAllConditionVariable() */
	kCall_WakeAllConditionVariable,
	/** Index for SleepConditionVariableSRW() */
	kCall_SleepConditionVariableSRW,
	/** Index for SleepConditionVariableCS() */
	kCall_SleepConditionVariableCS,
	/** Total number of function calls to be managed */
	kCallIndexCount
};

extern HINSTANCE__* BURGER_API load_library(eDLLIndex uIndex) BURGER_NOEXCEPT;
extern void* BURGER_API load_function(eCallIndex uIndex) BURGER_NOEXCEPT;
extern void BURGER_API override_function(
	eCallIndex uIndex, void* pFunction) BURGER_NOEXCEPT;

extern uint_t BURGER_API has_DirectInput(void) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_DirectInput8(void) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_XInput(void) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_DirectDraw(void) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_D3D9(void) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_DirectSound(void) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_DirectPlay(void) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_XAudio2(void) BURGER_NOEXCEPT;
extern uint_t BURGER_API is_windows64Bit(void) BURGER_NOEXCEPT;

}}
/* END */

#endif
