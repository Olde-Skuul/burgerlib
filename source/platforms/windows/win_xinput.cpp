/***************************************

	Shims for xinput1_3.dll and xinput1_4.dll

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "win_xinput.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "win_platformshims.h"

#if !defined(DOXYGEN)

//
// Handle some annoying defines that some windows SDKs may or may not have
//

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

// Windows 8 API level
#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0602
#endif

#include <Windows.h>

#include <Xinput.h>

// Function prototypes, lovingly stolen from xinput.h
typedef DWORD(WINAPI* XInputGetStatePtr)(
	DWORD dwUserIndex, XINPUT_STATE* pState);
typedef DWORD(WINAPI* XInputSetStatePtr)(
	DWORD dwUserIndex, XINPUT_VIBRATION* pVibration);
typedef DWORD(WINAPI* XInputGetCapabilitiesPtr)(
	DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities);
typedef void(WINAPI* XInputEnablePtr)(BOOL enable);
typedef DWORD(WINAPI* XInputGetDSoundAudioDeviceGuidsPtr)(
	DWORD dwUserIndex, GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid);
typedef DWORD(WINAPI* XInputGetBatteryInformationPtr)(DWORD dwUserIndex,
	BYTE devType, _XINPUT_BATTERY_INFORMATION* pBatteryInformation);
typedef DWORD(WINAPI* XInputGetKeystrokePtr)(
	DWORD dwUserIndex, DWORD dwReserved, _XINPUT_KEYSTROKE* pKeystroke);
typedef DWORD(WINAPI* XInputGetAudioDeviceIdsPtr)(DWORD dwUserIndex,
	LPWSTR pRenderDeviceId, UINT* pRenderCount, LPWSTR pCaptureDeviceId,
	UINT* pCaptureCount);

// Unit tests for pointers
// XInputGetStatePtr gXInputGetState = ::XInputGetState;
// XInputSetStatePtr gXInputSetState = ::XInputSetState;
// XInputGetCapabilitiesPtr gXInputGetCapabilities = ::XInputGetCapabilities;
// XInputEnablePtr gXInputEnable = ::XInputEnable;
// XInputGetDSoundAudioDeviceGuidsPtr gXInputGetDSoundAudioDeviceGuids =
// ::XInputGetDSoundAudioDeviceGuids;
// XInputGetBatteryInformationPtr gXInputGetBatteryInformation =
// ::XInputGetBatteryInformation;
// XInputGetKeystrokePtr gXInputGetKeystroke = ::XInputGetKeystroke;
// XInputGetAudioDeviceIdsPtr gXInputGetAudioDeviceIds =
// ::XInputGetAudioDeviceIds;
#endif

/*! ************************************

	\brief Load in xinput1_4.dll and call XInputGetState

	To allow maximum compatibility, this function will manually load
	xinput1_4.dll or xinput1_3.dll and then invoke XInputGetState if present.

	https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinputgetstate(v=vs.85).aspx

	\windowsonly

	\param dwUserIndex Index of the user's controller. Can be a value from 0
		to 3.
	\param pState Pointer to an XINPUT_STATE structure that receives the current
		state of the controller.

	\return Zero if no error. Any other value means an error occurred,
		ERROR_CALL_NOT_IMPLEMENTED means the function was not found

***************************************/

uint32_t BURGER_API Burger::Win32::XInputGetState(
	uint32_t dwUserIndex, _XINPUT_STATE* pState) BURGER_NOEXCEPT
{
	// Get the function pointer
	void* pXInputGetState = load_function(kCall_XInputGetState);
	uint32_t uResult = ERROR_CALL_NOT_IMPLEMENTED;
	if (pXInputGetState) {
		uResult = static_cast<XInputGetStatePtr>(pXInputGetState)(
			dwUserIndex, pState);
	}
	return uResult;
}

/*! ************************************

	\brief Load in xinput1_4.dll and call XInputSetState

	To allow maximum compatibility, this function will manually load
	xinput1_4.dll or xinput1_3.dll and then invoke XInputSetState if present.

	https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinputsetstate(v=vs.85).aspx

	\windowsonly

	\param dwUserIndex Index of the user's controller. Can be a value from 0
		to 3.
	\param pVibration Pointer to an XINPUT_VIBRATION structure containing the
		vibration information to send to the controller.

	\return Zero if no error. Any other value means an error occurred,
		ERROR_CALL_NOT_IMPLEMENTED means the function was not found

***************************************/

uint32_t BURGER_API Burger::Win32::XInputSetState(
	uint32_t dwUserIndex, _XINPUT_VIBRATION* pVibration) BURGER_NOEXCEPT
{
	// Get the function pointer
	void* pXInputSetState = load_function(kCall_XInputSetState);
	uint32_t uResult = ERROR_CALL_NOT_IMPLEMENTED;
	if (pXInputSetState) {
		uResult = static_cast<XInputSetStatePtr>(pXInputSetState)(
			dwUserIndex, pVibration);
	}
	return uResult;
}

/*! ************************************

	\brief Load in xinput1_4.dll and call XInputGetCapabilities

	To allow maximum compatibility, this function will manually load
	xinput1_4.dll or xinput1_3.dll and then invoke XInputGetCapabilities if
	present.

	https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinputgetcapabilities(v=vs.85).aspx

	\windowsonly

	\param dwUserIndex Index of the user's controller. Can be a value from 0
		to 3.
	\param dwFlags Input flags that identify the controller type. If this
		value is 0, then the capabilities of all controllers connected to the
		system are returned.
	\param pCapabilities Pointer to an XINPUT_CAPABILITIES structure that
		receives the controller capabilities.

	\return Zero if no error. Any other value means an error occurred,
		ERROR_CALL_NOT_IMPLEMENTED means the function was not found

***************************************/

uint32_t BURGER_API Burger::Win32::XInputGetCapabilities(uint32_t dwUserIndex,
	uint32_t dwFlags, _XINPUT_CAPABILITIES* pCapabilities) BURGER_NOEXCEPT
{
	// Get the function pointer
	void* pXInputGetCapabilities = load_function(kCall_XInputGetCapabilities);
	uint32_t uResult = ERROR_CALL_NOT_IMPLEMENTED;
	if (pXInputGetCapabilities) {
		uResult = static_cast<XInputGetCapabilitiesPtr>(pXInputGetCapabilities)(
			dwUserIndex, dwFlags, pCapabilities);
	}
	return uResult;
}

/*! ************************************

	\brief Load in xinput1_4.dll and call XInputGetDSoundAudioDeviceGuids

	To allow maximum compatibility, this function will manually load
	xinput1_4.dll or xinput1_3.dll and then invoke
	XInputGetDSoundAudioDeviceGuids if present.

	https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinputgetdsoundaudiodeviceguids(v=vs.85).aspx

	\note This function is deprecated as of Windows 8 (DLL XInput 1.4 or later)

	\windowsonly

	\param dwUserIndex Index of the user's controller. Can be a value from 0
		to 3.
	\param pDSoundRenderGuid Pointer that receives the GUID of the headset
		sound rendering device.
	\param pDSoundCaptureGuid Pointer that receives the GUID of the headset
		sound capture device.

	\return Zero if no error. Any other value means an error occurred,
		ERROR_CALL_NOT_IMPLEMENTED means the function was not found

***************************************/

uint32_t BURGER_API Burger::Win32::XInputGetDSoundAudioDeviceGuids(
	uint32_t dwUserIndex, GUID* pDSoundRenderGuid,
	GUID* pDSoundCaptureGuid) BURGER_NOEXCEPT
{
	// Get the function pointer
	void* pXInputGetDSoundAudioDeviceGuids =
		load_function(kCall_XInputGetDSoundAudioDeviceGuids);
	uint32_t uResult = ERROR_CALL_NOT_IMPLEMENTED;
	if (pXInputGetDSoundAudioDeviceGuids) {
		uResult = static_cast<XInputGetDSoundAudioDeviceGuidsPtr>(
			pXInputGetDSoundAudioDeviceGuids)(
			dwUserIndex, pDSoundRenderGuid, pDSoundCaptureGuid);
	}
	return uResult;
}

/*! ************************************

	\brief Load in xinput1_4.dll and call XInputEnable

	To allow maximum compatibility, this function will manually load
	xinput1_4.dll or xinput1_3.dll and then invoke XInputEnable if present.

	https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinputenable(v=vs.85).aspx

	\windowsonly

	\param bEnable If enable is \ref FALSE, XInput will only send neutral data
		in response to XInputGetState() (all buttons up, axes centered, and
		triggers at 0). Sending any value other than \ref FALSE will restore
		reading and writing functionality to normal.

***************************************/

void BURGER_API Burger::Win32::XInputEnable(uint_t bEnable) BURGER_NOEXCEPT
{
	// Get the function pointer
	void* pXInputEnable = load_function(kCall_XInputEnable);
	if (pXInputEnable) {
		static_cast<XInputEnablePtr>(pXInputEnable)(bEnable != 0);
	}
}

/*! ************************************

	\brief Load in xinput1_4.dll and call XInputGetAudioDeviceIds

	To allow maximum compatibility, this function will manually load
	xinput1_4.dll or xinput1_3.dll and then invoke XInputGetAudioDeviceIds if
	present.

	https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinputgetaudiodeviceids(v=vs.85).aspx

	\note This function is only available as of Windows 8 (DLL XInput 1.4 or
	later)

	\windowsonly

	\param dwUserIndex Index of the gamer associated with the device.
	\param pRenderDeviceId Pointer that receives Windows Core Audio device ID
		string for render (speakers).
	\param pRenderCount Pointer that receives the size, in wide-chars, of the
		render device ID string buffer.
	\param pCaptureDeviceId Pointer that receives Windows Core Audio device ID
		string for capture (microphone).
	\param pCaptureCount Pointer that receives the size, in wide-chars, of
		capture device ID string buffer.

	\return Zero if no error. Any other value means an error occurred,
		ERROR_CALL_NOT_IMPLEMENTED means the function was not found

***************************************/

uint32_t BURGER_API Burger::Win32::XInputGetAudioDeviceIds(uint32_t dwUserIndex,
	uint16_t* pRenderDeviceId, uint_t* pRenderCount, uint16_t* pCaptureDeviceId,
	uint_t* pCaptureCount) BURGER_NOEXCEPT
{
	// Get the function pointer
	void* pXInputGetAudioDeviceIds =
		load_function(kCall_XInputGetAudioDeviceIds);
	uint32_t uResult = ERROR_CALL_NOT_IMPLEMENTED;
	if (pXInputGetAudioDeviceIds) {
		uResult = static_cast<XInputGetAudioDeviceIdsPtr>(
			pXInputGetAudioDeviceIds)(dwUserIndex, (LPWSTR)pRenderDeviceId,
			pRenderCount, (LPWSTR)pCaptureDeviceId, pCaptureCount);
	}
	return uResult;
}

/*! ************************************

	\brief Load in xinput1_4.dll and call XInputGetBatteryInformation

	To allow maximum compatibility, this function will manually load
	xinput1_4.dll or xinput1_3.dll and then invoke XInputGetBatteryInformation
	if present.

	https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinputgetbatteryinformation(v=vs.85).aspx

	\windowsonly

	\param dwUserIndex Index of the signed-in gamer associated with the device.
		Can be a value in the range 0-XUSER_MAX_COUNT - 1.
	\param devType Input Specifies which device associated with this user index
		should be queried. Must be BATTERY_DEVTYPE_GAMEPAD or
		BATTERY_DEVTYPE_HEADSET.
	\param pBatteryInformation Pointer to an _XINPUT_BATTERY_INFORMATION
		structure that receives the battery information.

	\return Zero if no error. Any other value means an error occurred,
		ERROR_CALL_NOT_IMPLEMENTED means the function was not found

***************************************/

uint32_t BURGER_API Burger::Win32::XInputGetBatteryInformation(
	uint32_t dwUserIndex, uint8_t devType,
	_XINPUT_BATTERY_INFORMATION* pBatteryInformation) BURGER_NOEXCEPT
{
	// Get the function pointer
	void* pXInputGetBatteryInformation =
		load_function(kCall_XInputGetBatteryInformation);
	uint32_t uResult = ERROR_CALL_NOT_IMPLEMENTED;
	if (pXInputGetBatteryInformation) {
		uResult = static_cast<XInputGetBatteryInformationPtr>(
			pXInputGetBatteryInformation)(
			dwUserIndex, devType, pBatteryInformation);
	}
	return uResult;
}

/*! ************************************

	\brief Load in xinput1_4.dll and call XInputGetKeystroke

	To allow maximum compatibility, this function will manually load
	xinput1_4.dll or xinput1_3.dll and then invoke XInputGetKeystroke if
	present.

	https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinputgetkeystroke(v=vs.85).aspx

	\windowsonly

	\param dwUserIndex Index of the signed-in gamer associated with the device.
		Can be a value in the range 0-XUSER_MAX_COUNT - 1 or XUSER_INDEX_ANY to
		fetch the next available input event from any user.
	\param dwReserved Set to zero.
	\param pKeystroke Pointer to an XINPUT_KEYSTROKE structure that receives
		an input event.

	\return Zero if no error. Any other value means an error occurred,
		ERROR_CALL_NOT_IMPLEMENTED means the function was not found

***************************************/

uint32_t BURGER_API Burger::Win32::XInputGetKeystroke(uint32_t dwUserIndex,
	uint32_t dwReserved, _XINPUT_KEYSTROKE* pKeystroke) BURGER_NOEXCEPT
{
	// Get the function pointer
	void* pXInputGetKeystroke = load_function(kCall_XInputGetKeystroke);
	uint32_t uResult = ERROR_CALL_NOT_IMPLEMENTED;
	if (pXInputGetKeystroke) {
		uResult = static_cast<XInputGetKeystrokePtr>(pXInputGetKeystroke)(
			dwUserIndex, dwReserved, pKeystroke);
	}
	return uResult;
}

#endif
