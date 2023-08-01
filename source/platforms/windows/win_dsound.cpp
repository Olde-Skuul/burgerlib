/***************************************

	Shims for dsound.dll

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "win_dsound.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "win_platformshims.h"
#include "win_version.h"

#if !defined(DOXYGEN)

// This crap is needed to get XAudio2.h to include with watcom
#if defined(BURGER_WATCOM)
#include <comdecl.h>
#undef DEFINE_CLSID
#undef DEFINE_IID
#define DEFINE_CLSID(className, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	DEFINE_GUID(CLSID_##className, 0x##l, 0x##w1, 0x##w2, 0x##b1, 0x##b2, \
		0x##b3, 0x##b4, 0x##b5, 0x##b6, 0x##b7, 0x##b8)

#define DEFINE_IID(interfaceName, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	DEFINE_GUID(IID_##interfaceName, 0x##l, 0x##w1, 0x##w2, 0x##b1, 0x##b2, \
		0x##b3, 0x##b4, 0x##b5, 0x##b6, 0x##b7, 0x##b8)
#define KSDATAFORMAT_SUBTYPE_PCM
#define KSDATAFORMAT_SUBTYPE_ADPCM
#define KSDATAFORMAT_SUBTYPE_IEEE_FLOAT
#define CLSID_IXAudio2 IID_IXAudio2
#define __uuidof(x) CLSID_##x
#endif

#include "win_windows.h"

#if defined(BURGER_METROWERKS)
#pragma warn_emptydecl off
#endif

// Must be included AFTER windows.h
#include <xaudio2.h>

#include <dsound.h>

#if defined(BURGER_METROWERKS)
#pragma warn_emptydecl reset
#endif

typedef HRESULT(WINAPI* DirectSoundCreatePtr)(
	LPCGUID pcGuidDevice, LPDIRECTSOUND* ppDS, LPUNKNOWN pUnkOuter);
typedef HRESULT(WINAPI* DirectSoundEnumerateAPtr)(
	LPDSENUMCALLBACKA pDSEnumCallback, LPVOID pContext);
typedef HRESULT(WINAPI* DirectSoundEnumerateWPtr)(
	LPDSENUMCALLBACKW pDSEnumCallback, LPVOID pContext);
typedef HRESULT(WINAPI* DirectSoundCaptureCreatePtr)(
	LPCGUID pcGuidDevice, LPDIRECTSOUNDCAPTURE* ppDSC, LPUNKNOWN pUnkOuter);
typedef HRESULT(WINAPI* DirectSoundCaptureEnumerateAPtr)(
	LPDSENUMCALLBACKA pDSEnumCallback, LPVOID pContext);
typedef HRESULT(WINAPI* DirectSoundCaptureEnumerateWPtr)(
	LPDSENUMCALLBACKW pDSEnumCallback, LPVOID pContext);
typedef HRESULT(WINAPI* DirectSoundCreate8Ptr)(
	LPCGUID pcGuidDevice, LPDIRECTSOUND8* ppDS8, LPUNKNOWN pUnkOuter);
typedef HRESULT(WINAPI* DirectSoundCaptureCreate8Ptr)(
	LPCGUID pcGuidDevice, LPDIRECTSOUNDCAPTURE8* ppDSC8, LPUNKNOWN pUnkOuter);
typedef HRESULT(WINAPI* DirectSoundFullDuplexCreatePtr)(
	LPCGUID pcGuidCaptureDevice, LPCGUID pcGuidRenderDevice,
	LPCDSCBUFFERDESC pcDSCBufferDesc, LPCDSBUFFERDESC pcDSBufferDesc, HWND hWnd,
	DWORD dwLevel, LPDIRECTSOUNDFULLDUPLEX* ppDSFD,
	LPDIRECTSOUNDCAPTUREBUFFER8* ppDSCBuffer8,
	LPDIRECTSOUNDBUFFER8* ppDSBuffer8, LPUNKNOWN pUnkOuter);
typedef HRESULT(WINAPI* GetDeviceIDPtr)(LPCGUID pGuidSrc, LPGUID pGuidDest);

// Unit tests for pointers
// DirectSoundCreatePtr gDirectSoundCreate = ::DirectSoundCreate;
// DirectSoundEnumerateAPtr gDirectSoundEnumerateA = ::DirectSoundEnumerateA;
// DirectSoundEnumerateWPtr gDirectSoundEnumerateW = ::DirectSoundEnumerateW;
// DirectSoundCaptureCreatePtr gDirectSoundCaptureCreate =
//	::DirectSoundCaptureCreate;
// DirectSoundCaptureEnumerateAPtr gDirectSoundCaptureEnumerateA =
//	::DirectSoundCaptureEnumerateA;
// DirectSoundCaptureEnumerateWPtr gDirectSoundCaptureEnumerateW =
//	::DirectSoundCaptureEnumerateW;
// DirectSoundCreate8Ptr gDirectSoundCreate8 = ::DirectSoundCreate8;
// DirectSoundCaptureCreate8Ptr gDirectSoundCaptureCreate8 =
//	::DirectSoundCaptureCreate8;
// DirectSoundFullDuplexCreatePtr gDirectSoundFullDuplexCreate =
//	::DirectSoundFullDuplexCreate;
// GetDeviceIDPtr gGetDeviceID = ::GetDeviceID;

#endif

//
// dsound.dll
//

/*! ************************************

	\brief Load in dsound.dll and call DirectSoundCreate

	To allow maximum compatibility, this function will manually load
	dsound.dll and then invoke DirectSoundCreate if present.

	\windowsonly

	\param pGuidDevice Requested audio device
	\param ppOutput Pointer to receive the pointer to the new IDirectSound
		instance
	\param pOuter Pointer to the IUnknown passed to DirectSoundCreate()

	\return DD_OK if no error. Any other value means an error occurred

***************************************/

HRESULT BURGER_API Burger::Win32::DirectSoundCreate(const GUID* pGuidDevice,
	IDirectSound** ppOutput, IUnknown* pOuter) BURGER_NOEXCEPT
{
	// Clear in case of error
	if (ppOutput) {
		ppOutput[0] = nullptr;
	}

	// Get the function pointer
	HRESULT lResult = DSERR_INVALIDCALL;
	void* pDirectSoundCreate = load_function(kCall_DirectSoundCreate);
	if (pDirectSoundCreate) {
		lResult = static_cast<DirectSoundCreatePtr>(pDirectSoundCreate)(
			pGuidDevice, ppOutput, pOuter);
	}
	return lResult;
}

/*! ************************************

	\brief Load in dsound.dll and call DirectSoundEnumerateA

	To allow maximum compatibility, this function will manually load
	dsound.dll and then invoke DirectSoundEnumerateA if present.

	https://msdn.microsoft.com/en-us/library/microsoft.directx_sdk.reference.directsoundenumerate(v=vs.85).aspx

	\windowsonly

	\param pDSEnumCallback Address of a DSENUMCALLBACKA function to be called
		with a description of each enumerated DirectSound-enabled hardware
		abstraction layer (HAL).
	\param pContext Address of an application-defined value to be passed to the
		enumeration callback function each time that it is called.

	\return DS_OK if no error. Any other value means an error occurred

***************************************/

HRESULT BURGER_API Burger::Win32::DirectSoundEnumerateA(
	void* pDSEnumCallback, void* pContext) BURGER_NOEXCEPT
{
	// Get the function pointer
	void* pDirectSoundEnumerateA = load_function(kCall_DirectSoundEnumerateA);
	HRESULT lResult = DSERR_NOINTERFACE;
	if (pDirectSoundEnumerateA) {
		lResult = static_cast<DirectSoundEnumerateAPtr>(pDirectSoundEnumerateA)(
			static_cast<LPDSENUMCALLBACKA>(pDSEnumCallback), pContext);
	}
	return lResult;
}

/*! ************************************

	\brief Load in dsound.dll and call DirectSoundEnumerateW

	To allow maximum compatibility, this function will manually load
	dsound.dll and then invoke DirectSoundEnumerateW if present.

	https://msdn.microsoft.com/en-us/library/microsoft.directx_sdk.reference.directsoundenumerate(v=vs.85).aspx

	\windowsonly

	\param pDSEnumCallback Address of a DSENUMCALLBACKW function to be called
		with a description of each enumerated DirectSound-enabled hardware
		abstraction layer (HAL).
	\param pContext Address of an application-defined value to be passed to the
		enumeration callback function each time that it is called.

	\return DS_OK if no error. Any other value means an error occurred

***************************************/

HRESULT BURGER_API Burger::Win32::DirectSoundEnumerateW(
	void* pDSEnumCallback, void* pContext) BURGER_NOEXCEPT
{
	// Get the function pointer
	void* pDirectSoundEnumerateW = load_function(kCall_DirectSoundEnumerateW);
	HRESULT lResult = DSERR_NOINTERFACE;
	if (pDirectSoundEnumerateW) {
		lResult = static_cast<DirectSoundEnumerateWPtr>(pDirectSoundEnumerateW)(
			static_cast<LPDSENUMCALLBACKW>(pDSEnumCallback), pContext);
	}
	return lResult;
}

/*! ************************************

	\brief Load in dsound.dll and call DirectSoundCaptureCreate

	To allow maximum compatibility, this function will manually load
	dsound.dll and then invoke DirectSoundCreate if present.

	https://msdn.microsoft.com/en-us/library/ms939447.aspx

	\windowsonly

	\param pGuidDevice Requested audio device
	\param ppOutput Pointer to receive the pointer to the new
		IDirectSoundCapture instance
	\param pOuter Pointer to the IUnknown passed to DirectSoundCaptureCreate()

	\return DD_OK if no error. Any other value means an error occurred

***************************************/

HRESULT BURGER_API Burger::Win32::DirectSoundCaptureCreate(
	const GUID* pGuidDevice, IDirectSoundCapture** ppOutput,
	IUnknown* pOuter) BURGER_NOEXCEPT
{
	// Clear in case of error
	if (ppOutput) {
		ppOutput[0] = nullptr;
	}

	// Get the function pointer
	HRESULT lResult = DSERR_INVALIDCALL;
	void* pDirectSoundCaptureCreate =
		load_function(kCall_DirectSoundCaptureCreate);
	if (pDirectSoundCaptureCreate) {
		lResult = static_cast<DirectSoundCaptureCreatePtr>(
			pDirectSoundCaptureCreate)(pGuidDevice, ppOutput, pOuter);
	}
	return lResult;
}

/*! ************************************

	\brief Load in dsound.dll and call DirectSoundCaptureEnumerateA

	To allow maximum compatibility, this function will manually load
	dsound.dll and then invoke DirectSoundCaptureEnumerateA if present.

	https://msdn.microsoft.com/en-us/library/vs/alm/microsoft.directx_sdk.reference.directsoundcaptureenumerate(v=vs.85).aspx

	\windowsonly

	\param pDSEnumCallback Address of a DSENUMCALLBACKA function to be called
		with a description of each enumerated DirectSound-enabled hardware
		abstraction layer (HAL).
	\param pContext Address of an application-defined value to be passed to the
		enumeration callback function each time that it is called.

	\return DS_OK if no error. Any other value means an error occurred

***************************************/

HRESULT BURGER_API Burger::Win32::DirectSoundCaptureEnumerateA(
	void* pDSEnumCallback, void* pContext) BURGER_NOEXCEPT
{
	// Get the function pointer
	void* pDirectSoundCaptureEnumerateA =
		load_function(kCall_DirectSoundCaptureEnumerateA);
	HRESULT lResult = DSERR_NOINTERFACE;
	if (pDirectSoundCaptureEnumerateA) {
		lResult = static_cast<DirectSoundCaptureEnumerateAPtr>(
			pDirectSoundCaptureEnumerateA)(
			static_cast<LPDSENUMCALLBACKA>(pDSEnumCallback), pContext);
	}
	return lResult;
}

/*! ************************************

	\brief Load in dsound.dll and call DirectSoundCaptureEnumerateW

	To allow maximum compatibility, this function will manually load
	dsound.dll and then invoke DirectSoundCaptureEnumerateW if present.

	https://msdn.microsoft.com/en-us/library/vs/alm/microsoft.directx_sdk.reference.directsoundcaptureenumerate(v=vs.85).aspx

	\windowsonly

	\param pDSEnumCallback Address of a DSENUMCALLBACKW function to be called
		with a description of each enumerated DirectSound-enabled hardware
		abstraction layer (HAL).
	\param pContext Address of an application-defined value to be passed to the
		enumeration callback function each time that it is called.

	\return DS_OK if no error. Any other value means an error occurred

***************************************/

HRESULT BURGER_API Burger::Win32::DirectSoundCaptureEnumerateW(
	void* pDSEnumCallback, void* pContext) BURGER_NOEXCEPT
{
	// Get the function pointer
	void* pDirectSoundCaptureEnumerateW =
		load_function(kCall_DirectSoundCaptureEnumerateW);
	HRESULT lResult = DSERR_NOINTERFACE;
	if (pDirectSoundCaptureEnumerateW) {
		lResult = static_cast<DirectSoundCaptureEnumerateWPtr>(
			pDirectSoundCaptureEnumerateW)(
			static_cast<LPDSENUMCALLBACKW>(pDSEnumCallback), pContext);
	}
	return lResult;
}

/*! ************************************

	\brief Load in dsound.dll and call DirectSoundCreate8

	To allow maximum compatibility, this function will manually load
	dsound.dll and then invoke DirectSoundCreate8 if present.

	https://msdn.microsoft.com/en-us/library/vs/alm/microsoft.directx_sdk.reference.directsoundcreate8(v=vs.85).aspx

	\windowsonly

	\param pGuidDevice Requested audio device
	\param ppOutput Pointer to receive the pointer to the new IDirectSound8
		instance
	\param pOuter Pointer to the IUnknown passed to DirectSoundCreate8()

	\return DD_OK if no error. Any other value means an error occurred

***************************************/

HRESULT BURGER_API Burger::Win32::DirectSoundCreate8(const GUID* pGuidDevice,
	IDirectSound8** ppOutput, IUnknown* pOuter) BURGER_NOEXCEPT
{
	// Clear in case of error
	if (ppOutput) {
		ppOutput[0] = nullptr;
	}

	// Get the function pointer
	void* pDirectSoundCreate8 = load_function(kCall_DirectSoundCreate8);
	HRESULT lResult = DSERR_INVALIDCALL;
	if (pDirectSoundCreate8) {
		lResult = static_cast<DirectSoundCreate8Ptr>(pDirectSoundCreate8)(
			pGuidDevice, ppOutput, pOuter);
	}
	return lResult;
}

/*! ************************************

	\brief Load in dsound.dll and call DirectSoundCaptureCreate8

	To allow maximum compatibility, this function will manually load
	dsound.dll and then invoke DirectSoundCaptureCreate8 if present.

	https://msdn.microsoft.com/en-us/library/vs/alm/microsoft.directx_sdk.reference.directsoundcapturecreate8(v=vs.85).aspx

	\windowsonly

	\param pGuidDevice Requested audio device
	\param ppOutput Pointer to receive the pointer to the new
		IDirectSoundCapture8 instance
	\param pOuter Pointer to the IUnknown passed to DirectSoundCaptureCreate8()

	\return DD_OK if no error. Any other value means an error occurred

***************************************/

HRESULT BURGER_API Burger::Win32::DirectSoundCaptureCreate8(
	const GUID* pGuidDevice, IDirectSoundCapture** ppOutput,
	IUnknown* pOuter) BURGER_NOEXCEPT
{
	// Clear in case of error
	if (ppOutput) {
		ppOutput[0] = nullptr;
	}

	// Get the function pointer
	void* pDirectSoundCaptureCreate8 =
		load_function(kCall_DirectSoundCaptureCreate8);
	HRESULT lResult = DSERR_INVALIDCALL;
	if (pDirectSoundCaptureCreate8) {
		lResult = static_cast<DirectSoundCaptureCreate8Ptr>(
			pDirectSoundCaptureCreate8)(pGuidDevice, ppOutput, pOuter);
	}
	return lResult;
}

/*! ************************************

	\brief Load in dsound.dll and call DirectSoundFullDuplexCreate

	To allow maximum compatibility, this function will manually load
	dsound.dll and then invoke DirectSoundFullDuplexCreate if present.

	https://msdn.microsoft.com/en-us/library/vs/alm/microsoft.directx_sdk.reference.directsoundfullduplexcreate8(v=vs.85).aspx

	\windowsonly

	\param pGuidCaptureDevice Address of the GUID that identifies the sound
		capture device
	\param pGuidRenderDevice Address of the GUID that identifies the sound
		render device for full-duplex output
	\param pDSCBufferDesc Address of a DSCBUFFERDESC structure that specifies
		the characteristics of the capture buffer
	\param pDSBufferDesc Address of a DSCBUFFERDESC structure that specifies the
		characteristics of the render buffer
	\param hWnd Handle to the application window
	\param uLevel Cooperative level for the device
	\param ppDSFD Pointer to receive the pointer to the new
		IDirectSoundFullDuplex instance
	\param ppDSCBuffer8 Pointer to receive the pointer to the new
		IDirectSoundCaptureBuffer8 instance
	\param ppDSBuffer8 Pointer to receive the pointer to the new
		IDirectSoundBuffer8 instance
	\param pOuter Pointer to the IUnknown passed to
		DirectSoundFullDuplexCreate()

	\return DD_OK if no error. Any other value means an error occurred

***************************************/

HRESULT BURGER_API Burger::Win32::DirectSoundFullDuplexCreate(
	const GUID* pGuidCaptureDevice, const GUID* pGuidRenderDevice,
	const _DSCBUFFERDESC* pDSCBufferDesc, const _DSBUFFERDESC* pDSBufferDesc,
	HWND__* hWnd, uint32_t uLevel, IDirectSoundFullDuplex** ppDSFD,
	IDirectSoundCaptureBuffer8** ppDSCBuffer8,
	IDirectSoundBuffer8** ppDSBuffer8, IUnknown* pOuter) BURGER_NOEXCEPT
{
	// Clear in case of error
	if (ppDSFD) {
		ppDSFD[0] = nullptr;
	}
	if (ppDSCBuffer8) {
		ppDSCBuffer8[0] = nullptr;
	}
	if (ppDSBuffer8) {
		ppDSBuffer8[0] = nullptr;
	}

	// Get the function pointer
	void* pDirectSoundFullDuplexCreate =
		load_function(kCall_DirectSoundFullDuplexCreate);
	HRESULT lResult = DSERR_INVALIDCALL;
	if (pDirectSoundFullDuplexCreate) {
		lResult = static_cast<DirectSoundFullDuplexCreatePtr>(
			pDirectSoundFullDuplexCreate)(pGuidCaptureDevice, pGuidRenderDevice,
			pDSCBufferDesc, pDSBufferDesc, hWnd, uLevel, ppDSFD, ppDSCBuffer8,
			ppDSBuffer8, pOuter);
	}
	return lResult;
}

/*! ************************************

	\brief Load in dsound.dll and call GetDeviceID

	To allow maximum compatibility, this function will manually load
	dsound.dll and then invoke GetDeviceID if present.

	https://msdn.microsoft.com/en-us/library/vs/alm/microsoft.directx_sdk.reference.getdeviceid(v=vs.85).aspx

	\windowsonly

	\param pGuidSrc Address of a variable that specifies a valid device
		identifier
	\param pGuidDest Address of a variable that receives the unique identifier
		of the device

	\return DD_OK if no error. Any other value means an error occurred

***************************************/

HRESULT BURGER_API Burger::Win32::GetDeviceID(
	const GUID* pGuidSrc, GUID* pGuidDest) BURGER_NOEXCEPT
{
	// Get the function pointer
	void* pGetDeviceID = load_function(kCall_GetDeviceID);
	HRESULT lResult = DSERR_INVALIDCALL;
	if (pGetDeviceID) {
		lResult =
			static_cast<GetDeviceIDPtr>(pGetDeviceID)(pGuidSrc, pGuidDest);
	}
	return lResult;
}

/*! ************************************

	\brief XAudio2Create() for older compilers

	To allow maximum compatibility, this function will manually load
	xaudio2_9.dll, xaudio2_8.dll, up to xaudio2_7.dll and stop when one is
	found.

	It will then invoke XAudio2CreateWithVersionInfo() or XAudio2Create()

	\windowsonly

	\param ppXAudio2 Address of a variable that will receive the IXAudio2
		instance

	\param Flags Flags to pass to XAudio2Create()

	\param XAudio2Processor Value to pass to XAudio2Create()

	\return DD_OK if no error. Any other value means an error occurred

***************************************/

#if !defined(DOXYGEN)
static const char* s_DLLNames[3] = {
	"xaudio2_9.dll", "xaudio2_8.dll", "xaudio2_7.dll"};
#endif

HRESULT Burger::Win32::XAudio2Create(
	IXAudio2** ppXAudio2, uint_t Flags, uint_t XAudio2Processor) BURGER_NOEXCEPT
{
	// Set to TRUE once tested
	static uint_t s_bTested = FALSE;

	// Check if the newer XAudio2CreateWithVersionInfo() exists, and if so, call
	// it. Otherwise, fall back to XAudio2Create()
	typedef HRESULT(__stdcall * XAudio2CreateWithVersionInfoProc)(
		IXAudio2**, UINT32, XAUDIO2_PROCESSOR, DWORD);
	typedef HRESULT(__stdcall * XAudio2CreateInfoProc)(
		IXAudio2**, UINT32, XAUDIO2_PROCESSOR);
	static XAudio2CreateWithVersionInfoProc s_pXAudio2CreateWithVersion =
		nullptr;
	static XAudio2CreateInfoProc s_pXAudio2Create = nullptr;

	// Was there an attempt to load the DLL?
	if (!s_bTested) {

		// Don't do it again
		s_bTested = TRUE;

		// Check if running under XP. If so, the flag can't be used
		uint32_t uFlags = 0;
		if (is_vista_or_higher()) {
			uFlags = LOAD_LIBRARY_SEARCH_SYSTEM32;
		}

		// Scan the DLLs until one is hit
		uintptr_t i = 0;
		HMODULE hXAudio2Instance;
		do {
			hXAudio2Instance = LoadLibraryExA(s_DLLNames[i], nullptr, uFlags);
			if (hXAudio2Instance) {
				break;
			}
		} while (++i < BURGER_ARRAYSIZE(s_DLLNames));

		// Nothing loaded at all?
		if (!hXAudio2Instance) {

			// We're boned
			return HRESULT_FROM_WIN32(GetLastError());
		}

		// Load in the functions
		s_pXAudio2CreateWithVersion =
			reinterpret_cast<XAudio2CreateWithVersionInfoProc>(GetProcAddress(
				hXAudio2Instance, "XAudio2CreateWithVersionInfo"));

		s_pXAudio2Create = reinterpret_cast<XAudio2CreateInfoProc>(
			GetProcAddress(hXAudio2Instance, "XAudio2Create"));
	}

	// If the newer function exists, use it
	if (s_pXAudio2CreateWithVersion) {

		// Fake Windows 10 SDK
		return s_pXAudio2CreateWithVersion(ppXAudio2, Flags,
			static_cast<XAUDIO2_PROCESSOR>(XAudio2Processor), 0x0A00000C);
	}

	// Use the older function
	if (s_pXAudio2Create) {
		return s_pXAudio2Create(
			ppXAudio2, Flags, static_cast<XAUDIO2_PROCESSOR>(XAudio2Processor));
	}

	// XAudio2 does not exit
	return ERROR_PROC_NOT_FOUND;
}

#endif
