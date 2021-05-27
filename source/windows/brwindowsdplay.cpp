/***************************************

    Shims for dplayx.dll

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

#include <dplay.h>
#include <dplay8.h>
#include <dplobby.h>
#include <dplobby8.h>

typedef HRESULT(WINAPI* DirectPlayEnumerateAPtr)(LPDPENUMDPCALLBACKA, LPVOID);
typedef HRESULT(WINAPI* DirectPlayEnumerateWPtr)(LPDPENUMDPCALLBACK, LPVOID);
typedef HRESULT(WINAPI* DirectPlayCreatePtr)(
	LPGUID lpGUID, LPDIRECTPLAY* lplpDP, IUnknown* pUnk);
typedef HRESULT(WINAPI* DirectPlayLobbyCreateWPtr)(
	LPGUID, LPDIRECTPLAYLOBBY*, IUnknown*, LPVOID, DWORD);
typedef HRESULT(WINAPI* DirectPlayLobbyCreateAPtr)(
	LPGUID, LPDIRECTPLAYLOBBYA*, IUnknown*, LPVOID, DWORD);

// Unit tests for pointers
// DirectPlayEnumerateAPtr gDirectPlayEnumerateA = ::DirectPlayEnumerateA;
// DirectPlayEnumerateWPtr gDirectPlayEnumerateW = ::DirectPlayEnumerateW;
// DirectPlayCreatePtr gDirectPlayCreate = ::DirectPlayCreate;
// DirectPlayLobbyCreateWPtr gDirectPlayLobbyCreateW = ::DirectPlayLobbyCreateW;
// DirectPlayLobbyCreateAPtr gDirectPlayLobbyCreateA = ::DirectPlayLobbyCreateA;

#endif

//
// dplayx.dll
//

/*! ************************************

	\brief Load in dplayx.dll and call DirectPlayCreate

	To allow maximum compatibility, this function will manually load
	dplayx.dll or dplay.dll and then invoke DirectPlayCreate if present.

	\windowsonly

	\param pGuidSrc Address of a variable that specifies a valid device
		identifier
	\param ppOutput Address of a variable that receives the unique identifier of
		the device
	\param pOuter Address of IUnknown, set to \ref NULL

	\return DP_OK if no error. Any other value means an error occurred

***************************************/

uint_t BURGER_API Burger::Windows::DirectPlayCreate(
	GUID* pGuidSrc, IDirectPlay** ppOutput, IUnknown* pOuter)
{
	// Get the function pointer
	void* pDirectPlayCreate = LoadFunctionIndex(CALL_DirectPlayCreate);
	HRESULT uResult = DPERR_UNSUPPORTED;
	if (pDirectPlayCreate) {
		uResult = static_cast<DirectPlayCreatePtr>(pDirectPlayCreate)(
			pGuidSrc, ppOutput, pOuter);
	}
	return static_cast<uint_t>(uResult);
}

/*! ************************************

	\brief Load in dplayx.dll and call DirectPlayEnumerate

	This function is called CallDirectPlayEnumerate instead of
	DirectPlayEnumerate to avoid a conflict of a macro found in the dplay.h
	header.

	To allow maximum compatibility, this function will manually load
	dplayx.dll or dplay.dll and then invoke DirectPlayEnumerate if present.

	\note This function is OBSOLETE, call DirectPlayEnumerateA() instead

	\windowsonly

	\param pCallback Address of the callback function of type
		LPDPENUMDPCALLBACKA
	\param pContext Address of user data

	\return DP_OK if no error. Any other value means an error occurred

***************************************/

uint_t BURGER_API Burger::Windows::CallDirectPlayEnumerate(
	void* pCallback, void* pContext)
{
	// Get the function pointer
	void* pDirectPlayEnumerate = LoadFunctionIndex(CALL_DirectPlayEnumerate);
	HRESULT uResult = DPERR_UNSUPPORTED;
	if (pDirectPlayEnumerate) {
		uResult = static_cast<DirectPlayEnumerateAPtr>(pDirectPlayEnumerate)(
			static_cast<LPDPENUMDPCALLBACKA>(pCallback), pContext);
	}
	return static_cast<uint_t>(uResult);
}

/*! ************************************

	\brief Load in dplayx.dll and call DirectPlayEnumerateA

	To allow maximum compatibility, this function will manually load
	dplayx.dll and then invoke DirectPlayEnumerateA if present.

	\windowsonly

	\param pCallback Address of the callback function of type
		LPDPENUMDPCALLBACKA
	\param pContext Address of user data

	\return DP_OK if no error. Any other value means an error occurred

***************************************/

uint_t BURGER_API Burger::Windows::DirectPlayEnumerateA(
	void* pCallback, void* pContext)
{
	// Get the function pointer
	void* pDirectPlayEnumerateA = LoadFunctionIndex(CALL_DirectPlayEnumerateA);
	HRESULT uResult = DPERR_UNSUPPORTED;
	if (pDirectPlayEnumerateA) {
		uResult = static_cast<DirectPlayEnumerateAPtr>(pDirectPlayEnumerateA)(
			static_cast<LPDPENUMDPCALLBACKA>(pCallback), pContext);
	}
	return static_cast<uint_t>(uResult);
}

/*! ************************************

	\brief Load in dplayx.dll and call DirectPlayEnumerateW

	To allow maximum compatibility, this function will manually load
	dplayx.dll and then invoke DirectPlayEnumerateW if present.

	\windowsonly

	\param pCallback Address of the callback function of type LPDPENUMDPCALLBACK
	\param pContext Address of user data

	\return DP_OK if no error. Any other value means an error occurred

***************************************/

uint_t BURGER_API Burger::Windows::DirectPlayEnumerateW(
	void* pCallback, void* pContext)
{
	// Get the function pointer
	void* pDirectPlayEnumerateW = LoadFunctionIndex(CALL_DirectPlayEnumerateW);
	HRESULT uResult = DPERR_UNSUPPORTED;
	if (pDirectPlayEnumerateW) {
		uResult = static_cast<DirectPlayEnumerateWPtr>(pDirectPlayEnumerateW)(
			static_cast<LPDPENUMDPCALLBACK>(pCallback), pContext);
	}
	return static_cast<uint_t>(uResult);
}

/*! ************************************

	\brief Load in dplayx.dll and call DirectPlayLobbyCreateA

	To allow maximum compatibility, this function will manually load
	dplayx.dll and then invoke DirectPlayLobbyCreateA if present.

	\windowsonly
	\param pGuidSrc Address of a variable that specifies a valid device
		identifier
	\param ppOutput Address of a variable that receives the pointer to the
		IDirectPlayLobby
	\param pOuter Address of IUnknown, set to \ref NULL
	\param pData Address of custom data needed for the lobby
	\param uDataSize Size in bytes of the custom data for the lobby

	\return DP_OK if no error. Any other value means an error occurred

***************************************/

uint_t BURGER_API Burger::Windows::DirectPlayLobbyCreateA(GUID* pGuidSrc,
	IDirectPlayLobby** ppOutput, IUnknown* pOuter, void* pData, uint_t uDataSize)
{
	// Get the function pointer
	void* pDirectPlayLobbyCreateA =
		LoadFunctionIndex(CALL_DirectPlayLobbyCreateA);
	HRESULT uResult = DPERR_UNSUPPORTED;
	if (pDirectPlayLobbyCreateA) {
		uResult =
			static_cast<DirectPlayLobbyCreateAPtr>(pDirectPlayLobbyCreateA)(
				pGuidSrc, ppOutput, pOuter, pData, uDataSize);
	}
	return static_cast<uint_t>(uResult);
}

/*! ************************************

	\brief Load in dplayx.dll and call DirectPlayLobbyCreateW

	To allow maximum compatibility, this function will manually load
	dplayx.dll and then invoke DirectPlayLobbyCreateW if present.

	\windowsonly

	\param pGuidSrc Address of a variable that specifies a valid device
		identifier
	\param ppOutput Address of a variable that receives the pointer to
		the IDirectPlayLobby
	\param pOuter Address of IUnknown, set to \ref NULL
	\param pData Address of custom data needed for the lobby
	\param uDataSize Size in bytes of the custom data for the lobby

	\return DP_OK if no error. Any other value means an error occurred

***************************************/

uint_t BURGER_API Burger::Windows::DirectPlayLobbyCreateW(GUID* pGuidSrc,
	IDirectPlayLobby** ppOutput, IUnknown* pOuter, void* pData, uint_t uDataSize)
{
	// Get the function pointer
	void* pDirectPlayLobbyCreateW =
		LoadFunctionIndex(CALL_DirectPlayLobbyCreateW);
	HRESULT uResult = DPERR_UNSUPPORTED;
	if (pDirectPlayLobbyCreateW) {
		uResult =
			static_cast<DirectPlayLobbyCreateWPtr>(pDirectPlayLobbyCreateW)(
				pGuidSrc, ppOutput, pOuter, pData, uDataSize);
	}
	return static_cast<uint_t>(uResult);
}

#endif
