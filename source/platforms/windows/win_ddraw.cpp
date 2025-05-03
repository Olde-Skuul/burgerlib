/***************************************

	Shims for ddraw.dll

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "win_ddraw.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brmemoryfunctions.h"
#include "win_platformshims.h"

#if !defined(DOXYGEN)

//
// Handle some annoying defines that some windows SDKs may or may not have
//

#include "win_windows.h"

#include <ddraw.h>

// Function prototypes, lovingly stolen from ddraw.h
typedef HRESULT(WINAPI* DirectDrawCreatePtr)(
	GUID FAR* lpGUID, LPDIRECTDRAW FAR* lplpDD, IUnknown FAR* pUnkOuter);
typedef HRESULT(WINAPI* DirectDrawCreateExPtr)(
	GUID FAR* lpGuid, LPVOID* lplpDD, REFIID iid, IUnknown FAR* pUnkOuter);
typedef HRESULT(WINAPI* DirectDrawCreateClipperPtr)(DWORD dwFlags,
	LPDIRECTDRAWCLIPPER FAR* lplpDDClipper, IUnknown FAR* pUnkOuter);
typedef HRESULT(WINAPI* DirectDrawEnumerateWPtr)(
	LPDDENUMCALLBACKW lpCallback, LPVOID lpContext);
typedef HRESULT(WINAPI* DirectDrawEnumerateAPtr)(
	LPDDENUMCALLBACKA lpCallback, LPVOID lpContext);
typedef HRESULT(WINAPI* DirectDrawEnumerateExWPtr)(
	LPDDENUMCALLBACKEXW lpCallback, LPVOID lpContext, DWORD dwFlags);
typedef HRESULT(WINAPI* DirectDrawEnumerateExAPtr)(
	LPDDENUMCALLBACKEXA lpCallback, LPVOID lpContext, DWORD dwFlags);

/***************************************

	Unit tests for pointers, uncomment the values and see if there are compiler
	errors If they compile fine, the prototypes above are a match for the
	official headers

***************************************/

// DirectDrawCreatePtr gDirectDrawCreate = ::DirectDrawCreate;
// DirectDrawCreateExPtr gDirectDrawCreateEx = ::DirectDrawCreateEx;
// DirectDrawCreateClipperPtr gDirectDrawCreateClipper =
// ::DirectDrawCreateClipper; DirectDrawEnumerateWPtr gDirectDrawEnumerateW =
// ::DirectDrawEnumerateW; DirectDrawEnumerateAPtr gDirectDrawEnumerateA =
// ::DirectDrawEnumerateA; DirectDrawEnumerateExWPtr gDirectDrawEnumerateExW =
// ::DirectDrawEnumerateExW; DirectDrawEnumerateExAPtr gDirectDrawEnumerateExA =
// ::DirectDrawEnumerateExA;

#endif

/*! ************************************

	\brief Load in ddraw.dll and call DirectDrawCreate

	To allow maximum compatibility, this function will manually load
	ddraw.dll and then invoke DirectDrawCreate if present.

	https://msdn.microsoft.com/en-us/library/windows/desktop/gg426116(v=vs.85).aspx

	\windowsonly

	\param pGuid Guid of the version of DirectDraw requested
	\param ppOutput Pointer to the location where the valid IDirectDraw pointer
		will be stored. If the function fails, this value is guaranteed to be
		\ref NULL.
	\param pOuter Pointer to the IUnknown passed to DirectDrawCreate()

	\return Zero if no error. Any other value means an error occurred

***************************************/

uint_t BURGER_API Burger::Win32::DirectDrawCreate(
	const GUID* pGuid, IDirectDraw** ppOutput, IUnknown* pOuter) BURGER_NOEXCEPT
{
	// Clear in case of error
	if (ppOutput) {
		ppOutput[0] = nullptr;
	}

	// Get the function pointer
	void* pDirectDrawCreate = load_function(kCall_DirectDrawCreate);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pDirectDrawCreate) {
		uResult = static_cast<DirectDrawCreatePtr>(pDirectDrawCreate)(
			const_cast<GUID*>(pGuid), ppOutput, pOuter);
	}
	return static_cast<uint_t>(uResult);
}

/*! ************************************

	\brief Load in ddraw.dll and call DirectDrawCreateEx

	To allow maximum compatibility, this function will manually load
	ddraw.dll and then invoke DirectDrawCreateEx if present.

	http://msdn.microsoft.com/en-us/library/windows/desktop/gg426118(v=vs.85).aspx

	\windowsonly

	\param pGuid Guid of the version of DirectDraw requested
	\param ppOutput Pointer to the location where the valid IDirectDraw7 pointer
		will be stored. If the function fails, this value is guaranteed to be
		\ref NULL.
	\param rGUID Reference to the GUID of the DirectInput8 object to create
	\param pOuter Pointer to the IUnknown passed to DirectDrawCreateEx()

	\return Zero if no error. Any other value means an error occurred

***************************************/

uint_t BURGER_API Burger::Win32::DirectDrawCreateEx(const GUID* pGuid,
	void** ppOutput, const GUID& rGUID, IUnknown* pOuter) BURGER_NOEXCEPT
{
	// Clear in case of error
	if (ppOutput) {
		ppOutput[0] = nullptr;
	}

	// Get the function pointer
	void* pDirectDrawCreateEx = load_function(kCall_DirectDrawCreateEx);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pDirectDrawCreateEx) {
		uResult = static_cast<DirectDrawCreateExPtr>(pDirectDrawCreateEx)(
			const_cast<GUID*>(pGuid), ppOutput, rGUID, pOuter);
	}
	return static_cast<uint_t>(uResult);
}

/*! ************************************

	\brief Load in ddraw.dll and call DirectDrawCreateClipper

	To allow maximum compatibility, this function will manually load
	ddraw.dll and then invoke DirectDrawCreateClipper if present.

	https://msdn.microsoft.com/en-us/library/windows/desktop/gg426117(v=vs.85).aspx

	\windowsonly

	\param uFlags Bit flags passed
	\param ppOutput Pointer to the location where the valid IDirectDrawClipper
		pointer will be stored. If the function fails, this value is guaranteed
		to be \ref NULL.
	\param pOuter Pointer to the IUnknown passed to DirectDrawCreateClipper()

	\return Zero if no error. Any other value means an error occurred

***************************************/

uint_t BURGER_API Burger::Win32::DirectDrawCreateClipper(uint32_t uFlags,
	IDirectDrawClipper** ppOutput, IUnknown* pOuter) BURGER_NOEXCEPT
{
	// Clear in case of error
	if (ppOutput) {
		ppOutput[0] = nullptr;
	}

	// Get the function pointer
	void* pDirectDrawCreateClipper =
		load_function(kCall_DirectDrawCreateClipper);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pDirectDrawCreateClipper) {
		uResult = static_cast<DirectDrawCreateClipperPtr>(
			pDirectDrawCreateClipper)(uFlags, ppOutput, pOuter);
	}
	return static_cast<uint_t>(uResult);
}

/*! ************************************

	\brief Load in ddraw.dll and call DirectDrawEnumerateA

	To allow maximum compatibility, this function will manually load
	ddraw.dll and then invoke DirectDrawEnumerateA if present.

	https://msdn.microsoft.com/en-us/library/windows/desktop/gg426119(v=vs.85).aspx

	\windowsonly

	\param pCallback Address of a DDENUMCALLBACKA function to be called with a
		description of each enumerated DirectDraw-enabled hardware abstraction
		layer (HAL).
	\param pContext Address of an application-defined value to be passed to the
		enumeration callback function each time that it is called.

	\return DD_OK if no error. Any other value means an error occurred

***************************************/

HRESULT BURGER_API Burger::Win32::DirectDrawEnumerateA(
	void* pCallback, void* pContext) BURGER_NOEXCEPT
{
	// Get the function pointer
	void* pDirectDrawEnumerateA = load_function(kCall_DirectDrawEnumerateA);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pDirectDrawEnumerateA) {
		uResult = static_cast<DirectDrawEnumerateAPtr>(pDirectDrawEnumerateA)(
			static_cast<LPDDENUMCALLBACKA>(pCallback), pContext);
	}
	return uResult;
}

/*! ************************************

	\brief Load in ddraw.dll and call DirectDrawEnumerateW

	To allow maximum compatibility, this function will manually load
	ddraw.dll and then invoke DirectDrawEnumerateW if present.

	https://msdn.microsoft.com/en-us/library/windows/desktop/gg426119(v=vs.85).aspx

	\windowsonly

	\param pCallback Address of a DDENUMCALLBACKW function to be called with a
		description of each enumerated DirectDraw-enabled hardware abstraction
		layer (HAL).
	\param pContext Address of an application-defined value to be passed to the
		enumeration callback function each time that it is called.

	\return DD_OK if no error. Any other value means an error occurred

***************************************/

HRESULT BURGER_API Burger::Win32::DirectDrawEnumerateW(
	void* pCallback, void* pContext) BURGER_NOEXCEPT
{
	// Get the function pointer
	void* pDirectDrawEnumerateW = load_function(kCall_DirectDrawEnumerateW);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pDirectDrawEnumerateW) {
		uResult = static_cast<DirectDrawEnumerateWPtr>(pDirectDrawEnumerateW)(
			static_cast<LPDDENUMCALLBACKW>(pCallback), pContext);
	}
	return uResult;
}

/*! ************************************

	\brief Load in ddraw.dll and call DirectDrawEnumerateExA

	To allow maximum compatibility, this function will manually load
	ddraw.dll and then invoke DirectDrawEnumerateExA if present.

	http://msdn.microsoft.com/en-us/library/windows/desktop/gg426120(v=vs.85).aspx

	\windowsonly

	\param pCallback Address of a DDENUMCALLBACKEXA function to be called with a
		description of each enumerated DirectDraw-enabled hardware abstraction
		layer (HAL).
	\param pContext Address of an application-defined value to be passed to the
		enumeration callback function each time that it is called.
	\param uFlags Flags that specify the enumeration scope. This parameter can
		be 0 or a combination of the following flags. If the value is 0, the
		function enumerates only the primary display device.

	\return DD_OK if no error. Any other value means an error occurred

***************************************/

HRESULT BURGER_API Burger::Win32::DirectDrawEnumerateExA(
	void* pCallback, void* pContext, uint32_t uFlags) BURGER_NOEXCEPT
{
	// Get the function pointer
	void* pDirectDrawEnumerateExA = load_function(kCall_DirectDrawEnumerateExA);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pDirectDrawEnumerateExA) {
		uResult =
			static_cast<DirectDrawEnumerateExAPtr>(pDirectDrawEnumerateExA)(
				static_cast<LPDDENUMCALLBACKEXA>(pCallback), pContext, uFlags);
	}
	return uResult;
}

/*! ************************************

	\brief Load in ddraw.dll and call DirectDrawEnumerateExW

	To allow maximum compatibility, this function will manually load
	ddraw.dll and then invoke DirectDrawEnumerateExA if present.

	http://msdn.microsoft.com/en-us/library/windows/desktop/gg426120(v=vs.85).aspx

	\note Some video cards do not support this function call. Notably the nVidia
		GT 545

	\windowsonly

	\param pCallback Address of a DDENUMCALLBACKEXW function to be called with a
		description of each enumerated DirectDraw-enabled hardware abstraction
		layer (HAL).
	\param pContext Address of an application-defined value to be passed to the
		enumeration callback function each time that it is called.
	\param uFlags Flags that specify the enumeration scope. This parameter can
		be 0 or a combination of the following flags. If the value is 0, the
		function enumerates only the primary display device.

	\return DD_OK if no error. Any other value means an error occurred

***************************************/

HRESULT BURGER_API Burger::Win32::DirectDrawEnumerateExW(
	void* pCallback, void* pContext, uint32_t uFlags) BURGER_NOEXCEPT
{
	// Get the function pointer
	void* pDirectDrawEnumerateExW = load_function(kCall_DirectDrawEnumerateExW);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pDirectDrawEnumerateExW) {
		uResult =
			static_cast<DirectDrawEnumerateExWPtr>(pDirectDrawEnumerateExW)(
				static_cast<LPDDENUMCALLBACKEXW>(pCallback), pContext, uFlags);
	}
	return uResult;
}

/***************************************

	This callback is used to find a specific GUID for
	an enumerated device

	Note: Do NOT use the pName or pDeviceName parameters since it's
	unknown if the ASCII or wchar_t call was issued to call this
	function

***************************************/

#if !defined(DOXYGEN)
struct DeviceGuid_t {
	// Buffer to store the located GUID
	GUID* m_pGUID;
	// Count down
	uint32_t m_uDevNum;
};
#endif

static int CALLBACK FindDeviceCallback(GUID* pGUID, LPSTR /* pName */,
	LPSTR /* pDeviceName */, void* pThis, void* /* pMonitor */)
{
	// Deref the pointer
	DeviceGuid_t* pRef = static_cast<DeviceGuid_t*>(pThis);

	// Keep going
	int iResult = DDENUMRET_OK;

	// Found the device yet?
	if (!--pRef->m_uDevNum) {

		// Specific device?
		if (pGUID) {
			// Copy the GUID
			Burger::memory_copy(pRef->m_pGUID, pGUID, sizeof(GUID));
		} else {
			// Clear the GUID
			Burger::memory_clear(pRef->m_pGUID, sizeof(GUID));
		}

		// Stop now
		iResult = DDENUMRET_CANCEL;
	}
	return iResult;
}

/*! ************************************

	\brief Given a specific device number, return the DirectX GUID

	Scan the device list for the GUID of the requested device.
	Device #0 returns the global display device (All screens)

	\windowsonly

	\param pOutput Pointer to a buffer to accept the returned GUID. Cannot be
		\ref nullptr
	\param uDevNum 0 for the master global device, 1-??? for the enumerated
		displays

	\return Zero if no error, non-zero if an error has occurred

***************************************/

HRESULT BURGER_API Burger::Win32::get_video_GUID(
	GUID* pOutput, uint32_t uDevNum) BURGER_NOEXCEPT
{
	HRESULT uError = E_FAIL;
	if (pOutput) {

		// Data for the callback
		DeviceGuid_t Ref;

		// Set the pointer to the GUID to store the result
		Ref.m_pGUID = pOutput;
		// Scan for this device
		Ref.m_uDevNum = ++uDevNum;

		// Try the UTF16 version of the enumerator
		uError = Win32::DirectDrawEnumerateExW(FindDeviceCallback, &Ref,
			DDENUM_ATTACHEDSECONDARYDEVICES | DDENUM_DETACHEDSECONDARYDEVICES |
				DDENUM_NONDISPLAYDEVICES);

		// The nVidia GT 545 fails on this call, so call using the 8 Bit Ascii
		// version instead
		if (uError == E_NOTIMPL) {

			// Scan for this device
			Ref.m_uDevNum = uDevNum;
			uError = Win32::DirectDrawEnumerateExA(FindDeviceCallback, &Ref,
				DDENUM_ATTACHEDSECONDARYDEVICES |
					DDENUM_DETACHEDSECONDARYDEVICES | DDENUM_NONDISPLAYDEVICES);
		}

		if (uError == DD_OK) {

			// Got it?
			if (Ref.m_uDevNum) {

				// Force an error
				uError = E_FAIL;
			}
		}
	}

	// Return zero if no error
	return uError;
}

#endif
