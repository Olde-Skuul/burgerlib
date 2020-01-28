/***************************************

	Shims for dsound.dll

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

#if !defined(DIRECTSOUND_VERSION)
#define DIRECTSOUND_VERSION 0x800
#endif

#include <Windows.h>

#include <MMReg.h>

#include <dsound.h>

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

Word BURGER_API Burger::Windows::DirectSoundCreate(
	const GUID* pGuidDevice, IDirectSound** ppOutput, IUnknown* pOuter)
{
	// Clear in case of error
	if (ppOutput) {
		ppOutput[0] = nullptr;
	}

	// Get the function pointer
	HRESULT uResult = DSERR_INVALIDCALL;
	void* pDirectSoundCreate = LoadFunctionIndex(CALL_DirectSoundCreate);
	if (pDirectSoundCreate) {
		uResult = static_cast<DirectSoundCreatePtr>(pDirectSoundCreate)(
			pGuidDevice, ppOutput, pOuter);
	}
	return static_cast<Word>(uResult);
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

Word BURGER_API Burger::Windows::DirectSoundEnumerateA(
	void* pDSEnumCallback, void* pContext)
{
	// Get the function pointer
	void* pDirectSoundEnumerateA =
		LoadFunctionIndex(CALL_DirectSoundEnumerateA);
	HRESULT uResult = DSERR_NOINTERFACE;
	if (pDirectSoundEnumerateA) {
		uResult = static_cast<DirectSoundEnumerateAPtr>(pDirectSoundEnumerateA)(
			static_cast<LPDSENUMCALLBACKA>(pDSEnumCallback), pContext);
	}
	return static_cast<Word>(uResult);
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

Word BURGER_API Burger::Windows::DirectSoundEnumerateW(
	void* pDSEnumCallback, void* pContext)
{
	// Get the function pointer
	void* pDirectSoundEnumerateW =
		LoadFunctionIndex(CALL_DirectSoundEnumerateW);
	HRESULT uResult = DSERR_NOINTERFACE;
	if (pDirectSoundEnumerateW) {
		uResult = static_cast<DirectSoundEnumerateWPtr>(pDirectSoundEnumerateW)(
			static_cast<LPDSENUMCALLBACKW>(pDSEnumCallback), pContext);
	}
	return static_cast<Word>(uResult);
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

Word BURGER_API Burger::Windows::DirectSoundCaptureCreate(
	const GUID* pGuidDevice, IDirectSoundCapture** ppOutput, IUnknown* pOuter)
{
	// Clear in case of error
	if (ppOutput) {
		ppOutput[0] = nullptr;
	}

	// Get the function pointer
	HRESULT uResult = DSERR_INVALIDCALL;
	void* pDirectSoundCaptureCreate =
		LoadFunctionIndex(CALL_DirectSoundCaptureCreate);
	if (pDirectSoundCaptureCreate) {
		uResult = static_cast<DirectSoundCaptureCreatePtr>(
			pDirectSoundCaptureCreate)(pGuidDevice, ppOutput, pOuter);
	}
	return static_cast<Word>(uResult);
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

Word BURGER_API Burger::Windows::DirectSoundCaptureEnumerateA(
	void* pDSEnumCallback, void* pContext)
{
	// Get the function pointer
	void* pDirectSoundCaptureEnumerateA =
		LoadFunctionIndex(CALL_DirectSoundCaptureEnumerateA);
	HRESULT uResult = DSERR_NOINTERFACE;
	if (pDirectSoundCaptureEnumerateA) {
		uResult = static_cast<DirectSoundCaptureEnumerateAPtr>(
			pDirectSoundCaptureEnumerateA)(
			static_cast<LPDSENUMCALLBACKA>(pDSEnumCallback), pContext);
	}
	return static_cast<Word>(uResult);
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

Word BURGER_API Burger::Windows::DirectSoundCaptureEnumerateW(
	void* pDSEnumCallback, void* pContext)
{
	// Get the function pointer
	void* pDirectSoundCaptureEnumerateW =
		LoadFunctionIndex(CALL_DirectSoundCaptureEnumerateW);
	HRESULT uResult = DSERR_NOINTERFACE;
	if (pDirectSoundCaptureEnumerateW) {
		uResult = static_cast<DirectSoundCaptureEnumerateWPtr>(
			pDirectSoundCaptureEnumerateW)(
			static_cast<LPDSENUMCALLBACKW>(pDSEnumCallback), pContext);
	}
	return static_cast<Word>(uResult);
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

Word BURGER_API Burger::Windows::DirectSoundCreate8(
	const GUID* pGuidDevice, IDirectSound8** ppOutput, IUnknown* pOuter)
{
	// Clear in case of error
	if (ppOutput) {
		ppOutput[0] = nullptr;
	}

	// Get the function pointer
	void* pDirectSoundCreate8 = LoadFunctionIndex(CALL_DirectSoundCreate8);
	HRESULT uResult = DSERR_INVALIDCALL;
	if (pDirectSoundCreate8) {
		uResult = static_cast<DirectSoundCreate8Ptr>(pDirectSoundCreate8)(
			pGuidDevice, ppOutput, pOuter);
	}
	return static_cast<Word>(uResult);
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

Word BURGER_API Burger::Windows::DirectSoundCaptureCreate8(
	const GUID* pGuidDevice, IDirectSoundCapture** ppOutput, IUnknown* pOuter)
{
	// Clear in case of error
	if (ppOutput) {
		ppOutput[0] = nullptr;
	}

	// Get the function pointer
	void* pDirectSoundCaptureCreate8 =
		LoadFunctionIndex(CALL_DirectSoundCaptureCreate8);
	HRESULT uResult = DSERR_INVALIDCALL;
	if (pDirectSoundCaptureCreate8) {
		uResult = static_cast<DirectSoundCaptureCreate8Ptr>(
			pDirectSoundCaptureCreate8)(pGuidDevice, ppOutput, pOuter);
	}
	return static_cast<Word>(uResult);
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

Word BURGER_API Burger::Windows::DirectSoundFullDuplexCreate(
	const GUID* pGuidCaptureDevice, const GUID* pGuidRenderDevice,
	const _DSCBUFFERDESC* pDSCBufferDesc, const _DSBUFFERDESC* pDSBufferDesc,
	HWND__* hWnd, Word32 uLevel, IDirectSoundFullDuplex** ppDSFD,
	IDirectSoundCaptureBuffer8** ppDSCBuffer8,
	IDirectSoundBuffer8** ppDSBuffer8, IUnknown* pOuter)
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
		LoadFunctionIndex(CALL_DirectSoundFullDuplexCreate);
	HRESULT uResult = DSERR_INVALIDCALL;
	if (pDirectSoundFullDuplexCreate) {
		uResult = static_cast<DirectSoundFullDuplexCreatePtr>(
			pDirectSoundFullDuplexCreate)(pGuidCaptureDevice, pGuidRenderDevice,
			pDSCBufferDesc, pDSBufferDesc, hWnd, uLevel, ppDSFD, ppDSCBuffer8,
			ppDSBuffer8, pOuter);
	}
	return static_cast<Word>(uResult);
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

Word BURGER_API Burger::Windows::GetDeviceID(
	const GUID* pGuidSrc, GUID* pGuidDest)
{
	// Get the function pointer
	void* pGetDeviceID = LoadFunctionIndex(CALL_GetDeviceID);
	HRESULT uResult = DSERR_INVALIDCALL;
	if (pGetDeviceID) {
		uResult =
			static_cast<GetDeviceIDPtr>(pGetDeviceID)(pGuidSrc, pGuidDest);
	}
	return static_cast<Word>(uResult);
}

#endif
