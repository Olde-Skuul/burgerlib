/***************************************

    Shims for d3d9.dll or d3dx9_43.dll

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

#include <d3d9.h>
#include <d3dx9math.h>

typedef IDirect3D9*(WINAPI* Direct3DCreate9Ptr)(UINT SDKVersion);
typedef int(WINAPI* D3DPERF_BeginEventPtr)(D3DCOLOR col, LPCWSTR wszName);
typedef int(WINAPI* D3DPERF_EndEventPtr)(void);
typedef void(WINAPI* D3DPERF_SetMarkerPtr)(D3DCOLOR col, LPCWSTR wszName);
typedef void(WINAPI* D3DPERF_SetRegionPtr)(D3DCOLOR col, LPCWSTR wszName);
typedef BOOL(WINAPI* D3DPERF_QueryRepeatFramePtr)(void);
typedef void(WINAPI* D3DPERF_SetOptionsPtr)(DWORD dwOptions);
typedef DWORD(WINAPI* D3DPERF_GetStatusPtr)(void);
typedef HRESULT(WINAPI* D3DXCreateMatrixStackPtr)(
	DWORD Flags, LPD3DXMATRIXSTACK* ppStack);

// Unit tests for pointers
// Direct3DCreate9Ptr gDirect3DCreate9 = ::Direct3DCreate9;
// D3DPERF_BeginEventPtr gD3DPERF_BeginEvent = ::D3DPERF_BeginEvent;
// D3DPERF_EndEventPtr gD3DPERF_EndEvent = ::D3DPERF_EndEvent;
// D3DPERF_SetMarkerPtr gD3DPERF_SetMarker = ::D3DPERF_SetMarker;
// D3DPERF_SetRegionPtr gD3DPERF_SetRegion = ::D3DPERF_SetRegion;
// D3DPERF_QueryRepeatFramePtr gD3DPERF_QueryRepeatFrame =
// ::D3DPERF_QueryRepeatFrame;
// D3DPERF_SetOptionsPtr gD3DPERF_SetOptions = ::D3DPERF_SetOptions;
// D3DPERF_GetStatusPtr gD3DPERF_GetStatus = ::D3DPERF_GetStatus;
// D3DXCreateMatrixStackPtr gD3DXCreateMatrixStack = ::D3DXCreateMatrixStack;

#endif

//
// d3d9.dll
//

/*! ************************************

	\brief Load in d3d9.dll and call Direct3DCreate9

	To allow maximum compatibility, this function will manually load
	d3d9.dll and then invoke Direct3DCreate9 if present.

	\windowsonly

	\param uSDKVersion Requested version of Direct3D 9

	\return \ref NULL if DirectX 9 is not present. A valid Direct3D9 pointer
		otherwise

***************************************/

IDirect3D9* BURGER_API Burger::Windows::Direct3DCreate9(uint_t uSDKVersion)
{
	// Get the function pointer
	void* pDirect3DCreate9 = LoadFunctionIndex(CALL_Direct3DCreate9);
	IDirect3D9* pDirect3D9 = nullptr;
	if (pDirect3DCreate9) {
		pDirect3D9 =
			static_cast<Direct3DCreate9Ptr>(pDirect3DCreate9)(uSDKVersion);
	}
	return pDirect3D9;
}

/*! ************************************

	\brief Load in d3d9.dll and call D3DPERF_BeginEvent

	To allow maximum compatibility, this function will manually load
	d3d9.dll and then invoke D3DPERF_BeginEvent if present.

	\windowsonly

	\param col The color of the event
	\param wszName Pointer to UTF-16 string of the name of the event

	\return Is the level starting from 0 in the hierarchy to start this event.
		If an error occurs, the return value is negative.

***************************************/

int BURGER_API Burger::Windows::D3DPERF_BeginEvent(
	uint32_t col, const uint16_t* wszName)
{
	// Get the function pointer
	void* pD3DPERF_BeginEvent = LoadFunctionIndex(CALL_D3DPERF_BeginEvent);
	int iResult = -1;
	if (pD3DPERF_BeginEvent) {
		iResult = static_cast<D3DPERF_BeginEventPtr>(pD3DPERF_BeginEvent)(
			col, static_cast<LPCWSTR>(static_cast<const void*>(wszName)));
	}
	return iResult;
}

/*! ************************************

	\brief Load in d3d9.dll and call D3DPERF_EndEvent

	To allow maximum compatibility, this function will manually load
	d3d9.dll and then invoke D3DPERF_EndEvent if present.

	\windowsonly

	\return Is the level starting from 0 in the hierarchy to start this event.
		If an error occurs, the return value is negative.

***************************************/

int BURGER_API Burger::Windows::D3DPERF_EndEvent(void)
{
	// Get the function pointer
	void* pD3DPERF_EndEvent = LoadFunctionIndex(CALL_D3DPERF_EndEvent);
	int iResult = -1;
	if (pD3DPERF_EndEvent) {
		iResult = static_cast<D3DPERF_EndEventPtr>(pD3DPERF_EndEvent)();
	}
	return iResult;
}

/*! ************************************

	\brief Load in d3d9.dll and call D3DPERF_SetMarker

	To allow maximum compatibility, this function will manually load
	d3d9.dll and then invoke D3DPERF_SetMarker if present.

	\windowsonly

	\param col The color of the event
	\param wszName Pointer to UTF-16 string of the name of the marker

***************************************/

void BURGER_API Burger::Windows::D3DPERF_SetMarker(
	uint32_t col, const uint16_t* wszName)
{
	// Get the function pointer
	void* pD3DPERF_SetMarker = LoadFunctionIndex(CALL_D3DPERF_SetMarker);
	if (pD3DPERF_SetMarker) {
		static_cast<D3DPERF_SetMarkerPtr>(pD3DPERF_SetMarker)(
			col, static_cast<LPCWSTR>(static_cast<const void*>(wszName)));
	}
}

/*! ************************************

	\brief Load in d3d9.dll and call D3DPERF_SetRegion

	To allow maximum compatibility, this function will manually load
	d3d9.dll and then invoke D3DPERF_SetRegion if present.

	\windowsonly

	\param col The color of the event
	\param wszName Pointer to UTF-16 string of the name of the region

***************************************/

void BURGER_API Burger::Windows::D3DPERF_SetRegion(
	uint32_t col, const uint16_t* wszName)
{
	// Get the function pointer
	void* pD3DPERF_SetRegion = LoadFunctionIndex(CALL_D3DPERF_SetRegion);
	if (pD3DPERF_SetRegion) {
		static_cast<D3DPERF_SetRegionPtr>(pD3DPERF_SetRegion)(
			col, static_cast<LPCWSTR>(static_cast<const void*>(wszName)));
	}
}

/*! ************************************

	\brief Load in d3d9.dll and call D3DPERF_QueryRepeatFrame

	To allow maximum compatibility, this function will manually load
	d3d9.dll and then invoke D3DPERF_QueryRepeatFrame if present.

	\windowsonly

	\return When the return value is \ref TRUE, the caller will need to repeat
		the same sequence of calls. If \ref FALSE, the caller needs to move
		forward.

***************************************/

int BURGER_API Burger::Windows::D3DPERF_QueryRepeatFrame(void)
{
	// Get the function pointer
	void* pD3DPERF_QueryRepeatFrame =
		LoadFunctionIndex(CALL_D3DPERF_QueryRepeatFrame);
	int iResult = FALSE;
	if (pD3DPERF_QueryRepeatFrame) {
		iResult = static_cast<D3DPERF_QueryRepeatFramePtr>(
			pD3DPERF_QueryRepeatFrame)();
	}
	return iResult;
}

/*! ************************************

	\brief Load in d3d9.dll and call D3DPERF_SetOptions

	To allow maximum compatibility, this function will manually load
	d3d9.dll and then invoke D3DPERF_SetOptions if present.

	\windowsonly

	\param dwOptions Set to 1 if PIX should be turned off

***************************************/

void BURGER_API Burger::Windows::D3DPERF_SetOptions(uint32_t dwOptions)
{
	// Get the function pointer
	void* pD3DPERF_SetOptions = LoadFunctionIndex(CALL_D3DPERF_SetOptions);
	if (pD3DPERF_SetOptions) {
		static_cast<D3DPERF_SetOptionsPtr>(pD3DPERF_SetOptions)(dwOptions);
	}
}

/*! ************************************

	\brief Load in d3d9.dll and call D3DPERF_GetStatus

	To allow maximum compatibility, this function will manually load
	d3d9.dll and then invoke D3DPERF_GetStatus if present.

	\windowsonly

	\return Non-zero if profiled by PIX. 0 if PIX is not present.

***************************************/

uint_t BURGER_API Burger::Windows::D3DPERF_GetStatus(void)
{
	// Get the function pointer
	void* pD3DPERF_GetStatus = LoadFunctionIndex(CALL_D3DPERF_GetStatus);
	uint_t uResult = 0;
	if (pD3DPERF_GetStatus) {
		uResult = static_cast<D3DPERF_GetStatusPtr>(pD3DPERF_GetStatus)();
	}
	return uResult;
}

//
// d3dx9_43.dll
//

/*! ************************************

	\brief Load in d3dx9.dll and call D3DXCreateMatrixStack

	To allow maximum compatibility, this function will manually load
	d3dx9.dll if needed and then invoke D3DXCreateMatrixStack.

	\windowsonly

	\param uFlags Requested version of Direct3D 9
	\param ppStack Pointer to a pointer to receive the created ID3DXMatrixStack

	\return S_OK if the call succeeded. Windows error if otherwise

***************************************/

uint_t BURGER_API Burger::Windows::D3DXCreateMatrixStack(
	uint_t uFlags, ID3DXMatrixStack** ppStack)
{
	// Clear in case of error
	if (ppStack) {
		ppStack[0] = nullptr;
	}

	// Get the function pointer
	void* pD3DXCreateMatrixStack =
		LoadFunctionIndex(CALL_D3DXCreateMatrixStack);
	HRESULT uResult = D3DERR_NOTFOUND;
	if (pD3DXCreateMatrixStack) {
		uResult = static_cast<D3DXCreateMatrixStackPtr>(pD3DXCreateMatrixStack)(
			uFlags, ppStack);
	}
	return static_cast<uint_t>(uResult);
}

#endif
