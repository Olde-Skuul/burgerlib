/***************************************

    Shims for ddraw.dll

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

#include <ddraw.h>

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

// Unit tests for pointers
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

Word BURGER_API Burger::Windows::DirectDrawCreate(
	const GUID* pGuid, IDirectDraw** ppOutput, IUnknown* pOuter)
{
	// Clear in case of error
	if (ppOutput) {
		ppOutput[0] = nullptr;
	}

	// Get the function pointer
	void* pDirectDrawCreate = LoadFunctionIndex(CALL_DirectDrawCreate);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pDirectDrawCreate) {
		uResult = static_cast<DirectDrawCreatePtr>(pDirectDrawCreate)(
			const_cast<GUID*>(pGuid), ppOutput, pOuter);
	}
	return static_cast<Word>(uResult);
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

Word BURGER_API Burger::Windows::DirectDrawCreateEx(
	const GUID* pGuid, void** ppOutput, const GUID& rGUID, IUnknown* pOuter)
{
	// Clear in case of error
	if (ppOutput) {
		ppOutput[0] = nullptr;
	}

	// Get the function pointer
	void* pDirectDrawCreateEx = LoadFunctionIndex(CALL_DirectDrawCreateEx);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pDirectDrawCreateEx) {
		uResult = static_cast<DirectDrawCreateExPtr>(pDirectDrawCreateEx)(
			const_cast<GUID*>(pGuid), ppOutput, rGUID, pOuter);
	}
	return static_cast<Word>(uResult);
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

Word BURGER_API Burger::Windows::DirectDrawCreateClipper(
	Word32 uFlags, IDirectDrawClipper** ppOutput, IUnknown* pOuter)
{
	// Clear in case of error
	if (ppOutput) {
		ppOutput[0] = nullptr; 
	}

	// Get the function pointer
	void* pDirectDrawCreateClipper =
		LoadFunctionIndex(CALL_DirectDrawCreateClipper);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pDirectDrawCreateClipper) {
		uResult = static_cast<DirectDrawCreateClipperPtr>(
			pDirectDrawCreateClipper)(uFlags, ppOutput, pOuter);
	}
	return static_cast<Word>(uResult);
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

Word BURGER_API Burger::Windows::DirectDrawEnumerateA(
	void* pCallback, void* pContext)
{
	// Get the function pointer
	void* pDirectDrawEnumerateA = LoadFunctionIndex(CALL_DirectDrawEnumerateA);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pDirectDrawEnumerateA) {
		uResult = static_cast<DirectDrawEnumerateAPtr>(pDirectDrawEnumerateA)(
			static_cast<LPDDENUMCALLBACKA>(pCallback), pContext);
	}
	return static_cast<Word>(uResult);
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

Word BURGER_API Burger::Windows::DirectDrawEnumerateW(
	void* pCallback, void* pContext)
{
	// Get the function pointer
	void* pDirectDrawEnumerateW = LoadFunctionIndex(CALL_DirectDrawEnumerateW);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pDirectDrawEnumerateW) {
		uResult = static_cast<DirectDrawEnumerateWPtr>(pDirectDrawEnumerateW)(
			static_cast<LPDDENUMCALLBACKW>(pCallback), pContext);
	}
	return static_cast<Word>(uResult);
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

Word BURGER_API Burger::Windows::DirectDrawEnumerateExA(
	void* pCallback, void* pContext, Word32 uFlags)
{
	// Get the function pointer
	void* pDirectDrawEnumerateExA =
		LoadFunctionIndex(CALL_DirectDrawEnumerateExA);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pDirectDrawEnumerateExA) {
		uResult =
			static_cast<DirectDrawEnumerateExAPtr>(pDirectDrawEnumerateExA)(
				static_cast<LPDDENUMCALLBACKEXA>(pCallback), pContext, uFlags);
	}
	return static_cast<Word>(uResult);
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

Word BURGER_API Burger::Windows::DirectDrawEnumerateExW(
	void* pCallback, void* pContext, Word32 uFlags)
{
	// Get the function pointer
	void* pDirectDrawEnumerateExW =
		LoadFunctionIndex(CALL_DirectDrawEnumerateExW);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pDirectDrawEnumerateExW) {
		uResult =
			static_cast<DirectDrawEnumerateExWPtr>(pDirectDrawEnumerateExW)(
				static_cast<LPDDENUMCALLBACKEXW>(pCallback), pContext, uFlags);
	}
	return static_cast<Word>(uResult);
}

#endif
