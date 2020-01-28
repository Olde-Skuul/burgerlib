/***************************************

	Shims for xinput1_4.dll or xinput1_3.dll

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brwindowstypes.h"
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)

#if !defined(DOXYGEN)

//
// Handle some annoying defines that some windows SDKs may or may not have
//

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0501 // Windows XP
#endif

#include <Windows.h>

#include <Xinput.h>

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
	BYTE devType, XINPUT_BATTERY_INFORMATION* pBatteryInformation);
typedef DWORD(WINAPI* XInputGetKeystrokePtr)(
	DWORD dwUserIndex, DWORD dwReserved, PXINPUT_KEYSTROKE pKeystroke);
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

Word32 BURGER_API Burger::Windows::XInputGetState(
	Word32 dwUserIndex, _XINPUT_STATE* pState)
{
	// Get the function pointer
	void* pXInputGetState = LoadFunctionIndex(CALL_XInputGetState);
	Word32 uResult = ERROR_CALL_NOT_IMPLEMENTED;
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

Word32 BURGER_API Burger::Windows::XInputSetState(
	Word32 dwUserIndex, _XINPUT_VIBRATION* pVibration)
{
	// Get the function pointer
	void* pXInputSetState = LoadFunctionIndex(CALL_XInputSetState);
	Word32 uResult = ERROR_CALL_NOT_IMPLEMENTED;
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

Word32 BURGER_API Burger::Windows::XInputGetCapabilities(
	Word32 dwUserIndex, Word32 dwFlags, _XINPUT_CAPABILITIES* pCapabilities)
{
	// Get the function pointer
	void* pXInputGetCapabilities =
		LoadFunctionIndex(CALL_XInputGetCapabilities);
	Word32 uResult = ERROR_CALL_NOT_IMPLEMENTED;
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
		sound rendering device. \param pDSoundCaptureGuid Pointer that receives
		the GUID of the headset sound capture device.
	\return Zero if no error. Any other value means an error occurred,
		ERROR_CALL_NOT_IMPLEMENTED means the function was not found

***************************************/

Word32 BURGER_API Burger::Windows::XInputGetDSoundAudioDeviceGuids(
	Word32 dwUserIndex, GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid)
{
	// Get the function pointer
	void* pXInputGetDSoundAudioDeviceGuids =
		LoadFunctionIndex(CALL_XInputGetDSoundAudioDeviceGuids);
	Word32 uResult = ERROR_CALL_NOT_IMPLEMENTED;
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

void BURGER_API Burger::Windows::XInputEnable(Word bEnable)
{
	// Get the function pointer
	void* pXInputEnable = LoadFunctionIndex(CALL_XInputEnable);
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

Word32 BURGER_API Burger::Windows::XInputGetAudioDeviceIds(Word32 dwUserIndex,
	Word16* pRenderDeviceId, Word* pRenderCount, Word16* pCaptureDeviceId,
	Word* pCaptureCount)
{
	// Get the function pointer
	void* pXInputGetAudioDeviceIds =
		LoadFunctionIndex(CALL_XInputGetAudioDeviceIds);
	Word32 uResult = ERROR_CALL_NOT_IMPLEMENTED;
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
	\param pBatteryInformation Pointer to an XINPUT_BATTERY_INFORMATION
		structure that receives the battery information.

	\return Zero if no error. Any other value means an error occurred,
		ERROR_CALL_NOT_IMPLEMENTED means the function was not found

***************************************/

Word32 BURGER_API Burger::Windows::XInputGetBatteryInformation(
	Word32 dwUserIndex, Word devType,
	_XINPUT_BATTERY_INFORMATION* pBatteryInformation)
{
	// Get the function pointer
	void* pXInputGetBatteryInformation =
		LoadFunctionIndex(CALL_XInputGetBatteryInformation);
	Word32 uResult = ERROR_CALL_NOT_IMPLEMENTED;
	if (pXInputGetBatteryInformation) {
		uResult = static_cast<XInputGetBatteryInformationPtr>(
			pXInputGetBatteryInformation)(
			dwUserIndex, static_cast<BYTE>(devType), pBatteryInformation);
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

Word32 BURGER_API Burger::Windows::XInputGetKeystroke(
	Word32 dwUserIndex, Word32 dwReserved, _XINPUT_KEYSTROKE* pKeystroke)
{
	// Get the function pointer
	void* pXInputGetKeystroke = LoadFunctionIndex(CALL_XInputGetKeystroke);
	Word32 uResult = ERROR_CALL_NOT_IMPLEMENTED;
	if (pXInputGetKeystroke) {
		uResult = static_cast<XInputGetKeystrokePtr>(pXInputGetKeystroke)(
			dwUserIndex, dwReserved, pKeystroke);
	}
	return uResult;
}

#endif
