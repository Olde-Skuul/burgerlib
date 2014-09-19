/***************************************

	Global variable manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brglobals.h"
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brstring16.h"
#include "brstring.h"
#include "brstringfunctions.h"
#include "brfilemanager.h"

#if !defined(DOXYGEN)
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#if !defined(DIRECTINPUT_VERSION)
#define DIRECTINPUT_VERSION 0x800
#endif

#if !defined(DIRECT3D_VERSION)
#define DIRECT3D_VERSION 0x700
#endif

#if !defined(DIRECTDRAW_VERSION)
#define DIRECTDRAW_VERSION 0x700
#endif

#include <Windows.h>
#include <MMReg.h>
#include <dinput.h>
#include <d3d.h>
#include <dsound.h>
#include <shellapi.h>
#include <shlobj.h>
#include <SetupAPI.h>

static const char g_SoftwareClasses[] = "Software\\Classes\\";

/***************************************

	DLLs that can be dynamically loaded at runtime (So the 
	application can launch if they are missing or missing functions)
	
***************************************/

static const char *s_LibaryNames[Burger::Globals::DLL_COUNT] = {
	"ddraw.dll",
	"dinput.dll",
	"dinput8.dll",
	"d3d9.dll",
	"dsound.dll",
	"rpcrt4.dll",
	"winmm.dll",
	"shlwapi.dll",
	"version.dll",
	"hid.dll",
	"setupapi.dll"
};

struct CallNames_t {
	Burger::Globals::eWindowsDLLIndex eDLL;
	const char *m_pName;
};

static const CallNames_t g_CallNames[Burger::Globals::CALL_COUNT] = {
	{Burger::Globals::DINPUT8_DLL,"DirectInput8Create"},
	{Burger::Globals::DINPUT_DLL,"DirectInputCreateW"},
	{Burger::Globals::DDRAW_DLL,"DirectDrawCreate"},
	{Burger::Globals::DDRAW_DLL,"DirectDrawCreateEx"},
	{Burger::Globals::DDRAW_DLL,"DirectDrawCreateClipper"},
	{Burger::Globals::DDRAW_DLL,"DirectDrawEnumerateA"},
	{Burger::Globals::DDRAW_DLL,"DirectDrawEnumerateW"},
	{Burger::Globals::DDRAW_DLL,"DirectDrawEnumerateExA"},
	{Burger::Globals::DDRAW_DLL,"DirectDrawEnumerateExW"},
	{Burger::Globals::D3D9_DLL,"Direct3DCreate9"},
	{Burger::Globals::DSOUND_DLL,"DirectSoundCreate"},
	{Burger::Globals::DSOUND_DLL,"DirectSoundCreate8"},
	{Burger::Globals::RPCRT4_DLL,"UuidCreateSequential"},
	{Burger::Globals::WINMM_DLL,"timeGetTime"},
	{Burger::Globals::SHLWAPI_DLL,"PathSearchAndQualifyA"},
	{Burger::Globals::SHLWAPI_DLL,"PathSearchAndQualifyW"},
	{Burger::Globals::VERSION_DLL,"VerQueryValueA"},
	{Burger::Globals::VERSION_DLL,"VerQueryValueW"},
	{Burger::Globals::VERSION_DLL,"GetFileVersionInfoA"},
	{Burger::Globals::VERSION_DLL,"GetFileVersionInfoW"},
	{Burger::Globals::VERSION_DLL,"GetFileVersionInfoSizeA"},
	{Burger::Globals::VERSION_DLL,"GetFileVersionInfoSizeW"},
	{Burger::Globals::HID_DLL,"HidD_GetHidGuid"},
	{Burger::Globals::SETUPAPI_DLL,"SetupDiGetClassDevsA"},
	{Burger::Globals::SETUPAPI_DLL,"SetupDiGetClassDevsW"},
	{Burger::Globals::SETUPAPI_DLL,"SetupDiGetDeviceInterfaceDetailA"},
	{Burger::Globals::SETUPAPI_DLL,"SetupDiGetDeviceInterfaceDetailW"},
	{Burger::Globals::SETUPAPI_DLL,"SetupDiEnumDeviceInterfaces"},
	{Burger::Globals::SETUPAPI_DLL,"SetupDiDestroyDeviceInfoList"}
};

#endif		// Allow doxygen

/***************************************

	\brief Windows specific clean up code
	
	Some functions perform actions that reserve resources. Release them
	on application shutdown
	
***************************************/

Burger::Globals::~Globals()
{
	// Was DirectInput8 instantiated?
	if (m_pDirectInput8W) {
		m_pDirectInput8W->Release();
		m_pDirectInput8W = NULL;
	}
	
	// Was DirectInput instantiated?
	if (m_pDirectInputW) {
		m_pDirectInputW->Release();
		m_pDirectInputW = NULL;
	}

	// Dispose of all resolved calls to Windows
	MemoryClear(m_pWindowsCalls,sizeof(m_pWindowsCalls));

	// Finally, release all of the allocated DLLs
	WordPtr i = 0;
	do {
		if (m_hInstances[i]) {
			FreeLibrary(m_hInstances[i]);
			m_hInstances[i] = NULL;
			m_bInstancesTested[i] = FALSE;
		}
	} while (++i<DLL_COUNT);
}

/*! ************************************

	\fn Burger::Globals::GetInstance()
	\brief Get the application instance

	\note This function is only available on Windows

	\return Instance set by Globals::SetInstance()

	\sa Globals::SetInstance()
	
***************************************/

/*! ************************************

	\fn Burger::Globals::SetInstance(HINSTANCE__ *)
	\brief Set the application instance

	Upon application startup, an instance is assigned, use
	this function to allow Burgerlib to use this instance in
	other parts of the library.

	\note This function is only available on Windows

	\param pInput Instance of the application

	\sa Globals::GetInstance()
	
***************************************/

/*! ************************************

	\fn Burger::Globals::GetWindow()
	\brief Get the application window
	
	\note This function is only available on Windows

	\return Window set by Globals::SetWindow()

	\sa Globals::SetWindow()
	
***************************************/

/*! ************************************

	\fn Burger::Globals::SetWindow(HWND__ *)
	\brief Set the application window

	A window is created on application startup, it's pointer
	is stored via this call so other parts of Burgerlib
	can use this window for other systems.

	\note This function is only available on Windows

	\param pInput Instance of the application

	\sa Globals::GetWindow()
	
***************************************/

/*! ************************************

	\brief Detect and load DirectInput functions

	\note This function is only available on Windows

	\return \ref TRUE if DirectInput is present on the system, \ref FALSE if not
	
***************************************/

Word BURGER_API Burger::Globals::IsDirectInputPresent(void)
{
	Word uResult = FALSE;
	if (Globals::LoadLibraryIndex(DINPUT_DLL)) {
		uResult = TRUE;
	}
	return uResult;
}

/*! ************************************

	\brief Detect and load DirectInput8 functions

	\note This function is only available on Windows

	\return \ref TRUE if DirectInput8 is present on the system, \ref FALSE if not
	
***************************************/

Word BURGER_API Burger::Globals::IsDirectInput8Present(void)
{
	Word uResult = FALSE;
	if (Globals::LoadLibraryIndex(DINPUT8_DLL)) {
		uResult = TRUE;		// The code is good!
	}
	return uResult;			// TRUE if ok!
}

/*! ************************************

	\brief Detect and load DirectDraw functions

	\note This function is only available on Windows

	\return \ref TRUE if DirectDraw is present on the system, \ref FALSE if not
	
***************************************/

Word BURGER_API Burger::Globals::IsDirectDrawPresent(void)
{
	Word uResult = FALSE;
	if (Globals::LoadLibraryIndex(DDRAW_DLL)) {
		uResult = TRUE;		// The code is good!
	}
	return uResult;			// TRUE if ok!
}

/*! ************************************

	\brief Detect and load D3D9 functions

	\note This function is only available on Windows

	\return \ref TRUE if D3D9 is present on the system, \ref FALSE if not
	
***************************************/

Word BURGER_API Burger::Globals::IsD3D9Present(void)
{
	Word uResult = FALSE;
	if (Globals::LoadLibraryIndex(D3D9_DLL)) {
		uResult = TRUE;		// The code is good!
	}
	return uResult;			// TRUE if ok!
}

/*! ************************************

	\brief Detect and load DirectSound functions

	\note This function is only available on Windows

	\return \ref TRUE if DirectSound is present on the system, \ref FALSE if not
	
***************************************/

Word BURGER_API Burger::Globals::IsDirectSoundPresent(void)
{
	Word uResult = FALSE;
	if (Globals::LoadLibraryIndex(DSOUND_DLL)) {
		uResult = TRUE;		// The code is good!
	}
	return uResult;			// TRUE if ok!
}

/*! ************************************

	\brief Detect if running on an ancient version of windows

	Test if the system is a pre-NT Windows operating system.
	If it returns \ref FALSE, it's an NT kernel (XP, Vista, 7, 8 ...)

	\note This function is only available on Windows

	\return Returns \ref TRUE if Windows 3.1, 95, or 98. 
	
***************************************/

Word BURGER_API Burger::Globals::IsWin95orWin98(void)
{
	Globals *pThis = &g_Globals;		// Get the pointer to the singleton
	Word bResult = pThis->m_bWindow95;	// Get the value
	// Was it already tested?
	if (!(bResult&0x80)) {
		OSVERSIONINFOW Version;
		MemoryClear(&Version,sizeof(Version));
		Version.dwOSVersionInfoSize = sizeof(Version);
		if (GetVersionExW(&Version) && Version.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS) {
			bResult = 0x80|TRUE;
		} else {
			bResult = 0x80|FALSE;
		}
		pThis->m_bWindow95 = static_cast<Word8>(bResult);
	}
	// Return the value minus the other flags
	return bResult&1U;
}

/*! ************************************

	\brief Load in dinput.dll and call DirectInputCreateW

	To allow maximum compatibility, this function will manually load
	dinput.dll and then invoke DirectInputCreateW if present.

	\note This function is only available on Windows

	\param pOutput Pointer to the location where the valid IDirectInputW pointer
		will be stored. If the function fails, this value is guaranteed to be \ref NULL.
	\return Zero if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectInputCreateW(IDirectInputW **pOutput)
{
	// Was there already a DirectInput instance?
	IDirectInputW *pDirectInputW = g_Globals.m_pDirectInputW;
	Word uResult = 0;
	if (!pDirectInputW) {
		void *pDirectInputCreateW = LoadFunctionIndex(CALL_DirectInputCreateW);
		uResult = static_cast<Word>(DIERR_NOTFOUND);
		if (pDirectInputCreateW) {
			HRESULT hResult = static_cast<HRESULT (WINAPI *)(HINSTANCE,DWORD,IDirectInputW **,LPUNKNOWN)>(pDirectInputCreateW)(GetModuleHandle(NULL),0x700,&g_Globals.m_pDirectInputW,NULL);
			if (hResult<0) {
				g_Globals.m_pDirectInputW = NULL;
				uResult = static_cast<Word>(hResult);
			} else {
				pDirectInputW = g_Globals.m_pDirectInputW;
				uResult = 0;
			}
		}
	}
	pOutput[0] = pDirectInputW;
	return uResult;
}

/*! ************************************

	\brief Load in dinput8.dll and call DirectInput8Create

	To allow maximum compatibility, this function will manually load
	dinput8.dll and then invoke DirectInput8Create if present.

	\note This function is only available on Windows

	\param pOutput Pointer to the location where the valid IDirectInput8W pointer
		will be stored. If the function fails, this value is guaranteed to be \ref NULL.
	\return Zero if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectInput8Create(IDirectInput8W **pOutput)
{
	// Was there already a DirectInput8 instance?
	IDirectInput8W *pDirectInput8W = g_Globals.m_pDirectInput8W;
	Word uResult = 0;
	if (!pDirectInput8W) {
		void *pDirectInput8Create = LoadFunctionIndex(CALL_DirectInput8Create);
		uResult = static_cast<Word>(DIERR_NOTFOUND);
		if (pDirectInput8Create) {
			HRESULT hResult = static_cast<HRESULT(WINAPI *)(HINSTANCE,DWORD,REFIID,LPVOID *,LPUNKNOWN)>(pDirectInput8Create)(GetModuleHandle(NULL),0x800,IID_IDirectInput8W,reinterpret_cast<void **>(&g_Globals.m_pDirectInput8W),NULL);
			if (hResult<0) {
				g_Globals.m_pDirectInput8W = NULL;
				uResult = static_cast<Word>(hResult);
			} else {
				pDirectInput8W = g_Globals.m_pDirectInput8W;
				uResult = 0;
			}
		}
	}
	pOutput[0] = pDirectInput8W;
	return uResult;
}

/*! ************************************

	\brief Load in ddraw.dll and call DirectDrawCreateEx

	To allow maximum compatibility, this function will manually load
	ddraw.dll and then invoke DirectDrawCreateEx if present.

	http://msdn.microsoft.com/en-us/library/windows/desktop/gg426118(v=vs.85).aspx

	\note This function is only available on Windows

	\param pGuid Guid of the version of DirectDraw requested
	\param pOutput Pointer to the location where the valid IDirectDraw7 pointer
		will be stored. If the function fails, this value is guaranteed to be \ref NULL.
	\return Zero if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectDrawCreateEx(const GUID *pGuid,IDirectDraw7 **pOutput)
{
	// Was there already a DirectDraw instance?
	IDirectDraw7 *pDirectDraw7 = NULL;
	void *pDirectDrawCreateEx = LoadFunctionIndex(CALL_DirectDrawCreateEx);
	Word uResult = static_cast<Word>(DDERR_NOTFOUND);
	if (pDirectDrawCreateEx) {
		HRESULT hResult = static_cast<HRESULT(WINAPI *)(GUID *,LPVOID *,REFIID,IUnknown *)>(pDirectDrawCreateEx)(const_cast<GUID *>(pGuid),reinterpret_cast<void **>(&pDirectDraw7),IID_IDirectDraw7,NULL);
		uResult = 0;
		if (hResult<0) {
			uResult = static_cast<Word>(hResult);
		}
	}
	pOutput[0] = pDirectDraw7;
	return uResult;
}

/*! ************************************

	\brief Load in ddraw.dll and call DirectDrawEnumerateExA

	To allow maximum compatibility, this function will manually load
	ddraw.dll and then invoke DirectDrawEnumerateExA if present.

	http://msdn.microsoft.com/en-us/library/windows/desktop/gg426120(v=vs.85).aspx

	\note This function is only available on Windows

	\param lpCallback Address of a DDENUMCALLBACKEXA function to be called with a description of each enumerated DirectDraw-enabled hardware abstraction layer (HAL).
	\param lpContext Address of an application-defined value to be passed to the enumeration callback function each time that it is called.
	\param dwFlags Flags that specify the enumeration scope. This parameter can be 0 or a combination of the following flags. If the value is 0, the function enumerates only the primary display device.
	\return DD_OK if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectDrawEnumerateExA(void *lpCallback,void *lpContext,Word32 dwFlags)
{
	// Was there already a DirectDraw instance?
	void *pDirectDrawEnumerateExA = LoadFunctionIndex(CALL_DirectDrawEnumerateExA);
	Word uResult = static_cast<Word>(DDERR_NOTFOUND);
	if (pDirectDrawEnumerateExA) {
		HRESULT hResult = static_cast<HRESULT(WINAPI *)(LPDDENUMCALLBACKEXA,LPVOID,DWORD)>(pDirectDrawEnumerateExA)(static_cast<LPDDENUMCALLBACKEXA>(lpCallback),lpContext,dwFlags);
		uResult = 0;
		if (hResult<0) {
			uResult = static_cast<Word>(hResult);
		}
	}
	return uResult;
}

/*! ************************************

	\brief Load in ddraw.dll and call DirectDrawEnumerateExW

	To allow maximum compatibility, this function will manually load
	ddraw.dll and then invoke DirectDrawEnumerateExA if present.

	http://msdn.microsoft.com/en-us/library/windows/desktop/gg426120(v=vs.85).aspx

	\note Some video cards do not support this function call. Notably the nVidia GT 545

	\note This function is only available on Windows

	\param lpCallback Address of a DDENUMCALLBACKEXW function to be called with a description of each enumerated DirectDraw-enabled hardware abstraction layer (HAL).
	\param lpContext Address of an application-defined value to be passed to the enumeration callback function each time that it is called.
	\param dwFlags Flags that specify the enumeration scope. This parameter can be 0 or a combination of the following flags. If the value is 0, the function enumerates only the primary display device.
	\return DD_OK if no error. Any other value means an error occurred

***************************************/

Word BURGER_API Burger::Globals::DirectDrawEnumerateExW(void *lpCallback,void *lpContext,Word32 dwFlags)
{
	// Was there already a DirectDraw instance?
	void *pDirectDrawEnumerateExW = LoadFunctionIndex(CALL_DirectDrawEnumerateExW);
	Word uResult = static_cast<Word>(DDERR_NOTFOUND);
	if (pDirectDrawEnumerateExW) {
		HRESULT hResult = static_cast<HRESULT(WINAPI *)(LPDDENUMCALLBACKEXW,LPVOID,DWORD)>(pDirectDrawEnumerateExW)(static_cast<LPDDENUMCALLBACKEXW>(lpCallback),lpContext,dwFlags);
		uResult = 0;
		if (hResult<0) {
			uResult = static_cast<Word>(hResult);
		}
	}
	return uResult;
}

/*! ************************************

	\brief Load in d3d9.dll and call Direct3DCreate9

	To allow maximum compatibility, this function will manually load
	d3d9.dll and then invoke Direct3DCreate9 if present.

	\note This function is only available on Windows

	\param uSDKVersion Requested version of Direct3D 9
	\return \ref NULL if DirectX 9 is not present. A valid Direct3D9 pointer otherwise
	
***************************************/

IDirect3D9 *BURGER_API Burger::Globals::Direct3DCreate9(Word uSDKVersion)
{
	// Was there already a DirectDraw instance?
	IDirect3D9 *pDirect3D9 = NULL;
	void *pDirect3DCreate9 = LoadFunctionIndex(CALL_Direct3DCreate9);
	if (pDirect3DCreate9) {
		pDirect3D9 = static_cast<IDirect3D9 *(WINAPI *)(UINT)>(pDirect3DCreate9)(uSDKVersion);
	}
	return pDirect3D9;
}


/*! ************************************

	\brief Load in dsound.dll and call DirectSoundCreate

	To allow maximum compatibility, this function will manually load
	dsound.dll and then invoke DirectSoundCreate if present.

	\note This function is only available on Windows

	\param pGuidDevice Requested audio device
	\param pOutput Pointer to receive the pointer to the new IDirectSound instance
	\return DD_OK if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectSoundCreate(const GUID *pGuidDevice,IDirectSound **pOutput)
{
	// Was there already a DirectSound instance?
	IDirectSound *pDirectSound = NULL;
	Word uResult = static_cast<Word>(DSERR_INVALIDCALL);
	void *pDirectSoundCreate = LoadFunctionIndex(CALL_DirectSoundCreate);
	if (pDirectSoundCreate) {
		HRESULT hResult = static_cast<HRESULT(WINAPI *)(LPCGUID,IDirectSound **,LPUNKNOWN)>(pDirectSoundCreate)(pGuidDevice,&pDirectSound,NULL);
		uResult = 0;
		if (hResult<0) {
			uResult = static_cast<Word>(hResult);
		}
	}
	pOutput[0] = pDirectSound;
	return uResult;
}

/*! ************************************

	\brief Load in dsound.dll and call DirectSoundCreate8

	To allow maximum compatibility, this function will manually load
	dsound.dll and then invoke DirectSoundCreate8 if present.

	\note This function is only available on Windows

	\param pGuidDevice Requested audio device
	\param pOutput Pointer to receive the pointer to the new IDirectSound8 instance
	\return DD_OK if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectSoundCreate8(const GUID *pGuidDevice,IDirectSound8 **pOutput)
{
	// Was there already a DirectSound8 instance?
	IDirectSound8 *pDirectSound = NULL;
	Word uResult = static_cast<Word>(DSERR_INVALIDCALL);
	void *pDirectSoundCreate8 = LoadFunctionIndex(CALL_DirectSoundCreate8);
	if (pDirectSoundCreate8) {
		HRESULT hResult = static_cast<HRESULT (WINAPI *)(LPCGUID,IDirectSound8 **,LPUNKNOWN)>(pDirectSoundCreate8)(pGuidDevice,&pDirectSound,NULL);
		uResult = 0;
		if (hResult<0) {
			uResult = static_cast<Word>(hResult);
		}
	}
	pOutput[0] = pDirectSound;
	return uResult;
}

/*! ************************************

	\brief Call timeGetTime(void)
	
	Manually load winmm.dll if needed and
	call the Windows function timeGetTime()

	http://msdn.microsoft.com/en-us/library/dd757629(v=vs.85).aspx
	
	\return Time in milliseconds
		
***************************************/

Word BURGER_API Burger::Globals::timeGetTime(void)
{
	void *timeGetTime = LoadFunctionIndex(Globals::CALL_timeGetTime);
	Word uResult = 0;
	if (timeGetTime) {
		uResult = static_cast<DWORD (WINAPI *)(void)>(timeGetTime)();
	}
	return uResult;
}

/*! ************************************

	\brief Call PathSearchAndQualifyA(void)
	
	Manually load shlwapi.dll if needed and
	call the Windows function PathSearchAndQualifyA()

	http://msdn.microsoft.com/en-us/library/windows/desktop/bb773751(v=vs.85).aspx
	
	\note This function is only available on Windows

	\param pszPath A pointer to a null-terminated string of maximum length MAX_PATH that contains the path to search.
	\param pszBuf A pointer to a null-terminated string of length MAX_PATH that contains the path to be referenced.
	\param cchBuf The size of the buffer pointed to by pszBuf, in characters.
	\return Returns \ref TRUE if the path is qualified, or \ref FALSE otherwise.
		
***************************************/

Word BURGER_API Burger::Globals::PathSearchAndQualifyA(const char *pszPath,char *pszBuf,WordPtr cchBuf)
{
	void *pPathSearchAndQualifyA = LoadFunctionIndex(Globals::CALL_PathSearchAndQualifyA);
	Word uResult = FALSE;
	if (pPathSearchAndQualifyA) {
		uResult = static_cast<Word>(static_cast<BOOL (WINAPI *)(LPCSTR,LPSTR,UINT)>(pPathSearchAndQualifyA)(pszPath,pszBuf,static_cast<UINT>(cchBuf)));
	}
	return uResult;
}

/*! ************************************

	\brief Call PathSearchAndQualifyW(void)
	
	Manually load shlwapi.dll if needed and
	call the Windows function PathSearchAndQualifyW()

	http://msdn.microsoft.com/en-us/library/windows/desktop/bb773751(v=vs.85).aspx
	
	\note This function is only available on Windows

	\param pszPath A pointer to a null-terminated string of maximum length MAX_PATH that contains the path to search.
	\param pszBuf A pointer to a null-terminated string of length MAX_PATH that contains the path to be referenced.
	\param cchBuf The size of the buffer pointed to by pszBuf, in characters.
	\return Returns \ref TRUE if the path is qualified, or \ref FALSE otherwise.
		
***************************************/

Word BURGER_API Burger::Globals::PathSearchAndQualifyW(const Word16 *pszPath,Word16 *pszBuf,WordPtr cchBuf)
{
	void *pPathSearchAndQualifyW = LoadFunctionIndex(Globals::CALL_PathSearchAndQualifyW);
	Word uResult = FALSE;
	if (pPathSearchAndQualifyW) {
		uResult = static_cast<Word>(static_cast<BOOL (WINAPI *)(LPCWSTR,LPWSTR,UINT)>(pPathSearchAndQualifyW)(reinterpret_cast<LPCWSTR>(pszPath),reinterpret_cast<LPWSTR>(pszBuf),static_cast<UINT>(cchBuf)));
	}
	return uResult;
}

/*! ************************************

	\brief Call UuidCreateSequential()
	
	Manually load rpcrt4.dll if needed and
	call the Windows function UuidCreateSequential()

	http://msdn.microsoft.com/en-us/library/windows/desktop/aa379322(v=vs.85).aspx
	
	\note This function is only available on Windows

	\param pOutput A pointer to an uninitialized GUID.
	\return Zero for success or List of error codes http://msdn.microsoft.com/en-us/library/windows/desktop/aa378645(v=vs.85).aspx
		
***************************************/

Word BURGER_API Burger::Globals::UuidCreateSequential(GUID *pOutput)
{
	void *pUuidCreateSequential = LoadFunctionIndex(Globals::CALL_UuidCreateSequential);
	Word uResult = RPC_S_CALL_FAILED;
	if (pUuidCreateSequential) {
		uResult = static_cast<Word>(static_cast<RPC_STATUS (WINAPI *)(GUID *)>(pUuidCreateSequential)(pOutput));
	}
	return uResult;
}

/*! ************************************

	\brief Call VerQueryValueA(void)
	
	Manually load version.dll if needed and
	call the Windows function VerQueryValueA()

	http://msdn.microsoft.com/en-us/library/windows/desktop/ms647464(v=vs.85).aspx
	
	\note This function is only available on Windows

	\param pBlock The version-information resource returned by the GetFileVersionInfo function.
	\param lpSubBlock A pointer to the version-information value to be retrieved. 
	\param lplpBuffer A pointer that contains the address of a pointer to the requested version information in the buffer pointed to by pBlock.
	\param puLen The size of the buffer pointed to by lplpBuffer, in bytes.
	\return Returns \ref TRUE if successful, or \ref FALSE otherwise.
		
***************************************/

Word BURGER_API Burger::Globals::VerQueryValueA(const void *pBlock,const char *lpSubBlock,void **lplpBuffer,Word *puLen)
{
	void *pVerQueryValueA = LoadFunctionIndex(Globals::CALL_VerQueryValueA);
	Word uResult = FALSE;		// Failure
	if (pVerQueryValueA) {
		uResult = static_cast<Word>(static_cast<BOOL (WINAPI *)(LPCVOID,LPCSTR,LPVOID *,PUINT)>(pVerQueryValueA)(pBlock,lpSubBlock,lplpBuffer,puLen));
	}
	return uResult;
}

/*! ************************************

	\brief Call VerQueryValueW(void)
	
	Manually load version.dll if needed and
	call the Windows function VerQueryValueW()

	http://msdn.microsoft.com/en-us/library/windows/desktop/ms647464(v=vs.85).aspx
	
	\note This function is only available on Windows

	\param pBlock The version-information resource returned by the GetFileVersionInfo function.
	\param lpSubBlock A pointer to the version-information value to be retrieved. 
	\param lplpBuffer A pointer that contains the address of a pointer to the requested version information in the buffer pointed to by pBlock.
	\param puLen The size of the buffer pointed to by lplpBuffer, in bytes.
	\return Returns \ref TRUE if successful, or \ref FALSE otherwise.
		
***************************************/

Word BURGER_API Burger::Globals::VerQueryValueW(const void *pBlock,const Word16 *lpSubBlock,void **lplpBuffer,Word *puLen)
{
	void *pVerQueryValueW = LoadFunctionIndex(Globals::CALL_VerQueryValueW);
	Word uResult = FALSE;		// Failure
	if (pVerQueryValueW) {
		uResult = static_cast<Word>(static_cast<BOOL (WINAPI *)(LPCVOID,LPCWSTR,LPVOID *,PUINT)>(pVerQueryValueW)(pBlock,reinterpret_cast<LPCWSTR>(lpSubBlock),lplpBuffer,puLen));
	}
	return uResult;
}

/*! ************************************

	\brief Call GetFileVersionInfoA(void)
	
	Manually load version.dll if needed and
	call the Windows function GetFileVersionInfoA()

	http://msdn.microsoft.com/en-us/library/windows/desktop/ms647003(v=vs.85).aspx
	
	\note This function is only available on Windows

	\param lptstrFilename The name of the file
	\param dwHandle This parameter is ignored.
	\param dwLen The size, in bytes, of the buffer pointed to by the lpData parameter.
	\param lpData Pointer to a buffer that receives the file-version information.
	\return Returns \ref TRUE if successful, or \ref FALSE otherwise.
		
***************************************/

Word BURGER_API Burger::Globals::GetFileVersionInfoA(const char *lptstrFilename,Word32 dwHandle,Word32 dwLen,void *lpData)
{
	void *pGetFileVersionInfoA = LoadFunctionIndex(Globals::CALL_GetFileVersionInfoA);
	Word uResult = FALSE;		// Failure
	if (pGetFileVersionInfoA) {
		uResult = static_cast<Word>(static_cast<BOOL (WINAPI *)(LPCSTR,DWORD,DWORD,LPVOID)>(pGetFileVersionInfoA)(lptstrFilename,dwHandle,dwLen,lpData));
	}
	return uResult;
}

/*! ************************************

	\brief Call GetFileVersionInfoW(void)
	
	Manually load version.dll if needed and
	call the Windows function GetFileVersionInfoW()

	http://msdn.microsoft.com/en-us/library/windows/desktop/ms647003(v=vs.85).aspx
	
	\note This function is only available on Windows

	\param lptstrFilename The name of the file
	\param dwHandle This parameter is ignored.
	\param dwLen The size, in bytes, of the buffer pointed to by the lpData parameter.
	\param lpData Pointer to a buffer that receives the file-version information.
	\return Returns \ref TRUE if successful, or \ref FALSE otherwise.
		
***************************************/

Word BURGER_API Burger::Globals::GetFileVersionInfoW(const Word16 *lptstrFilename,Word32 dwHandle,Word32 dwLen,void *lpData)
{
	void *pGetFileVersionInfoW = LoadFunctionIndex(Globals::CALL_GetFileVersionInfoW);
	Word uResult = FALSE;		// Failure
	if (pGetFileVersionInfoW) {
		uResult = static_cast<Word>(static_cast<BOOL (WINAPI *)(LPCWSTR,DWORD,DWORD,LPVOID)>(pGetFileVersionInfoW)(reinterpret_cast<LPCWSTR>(lptstrFilename),dwHandle,dwLen,lpData));
	}
	return uResult;
}

/*! ************************************

	\brief Call GetFileVersionInfoA(void)
	
	Manually load version.dll if needed and
	call the Windows function GetFileVersionInfoA()

	http://msdn.microsoft.com/en-us/library/windows/desktop/ms647005(v=vs.85).aspx
	
	\note This function is only available on Windows

	\param lptstrFilename The name of the file of interest.
	\param lpdwHandle A pointer to a variable that the function sets to zero.
	\return Returns the number of bytes if successful, or zero otherwise.
		
***************************************/

Word32 BURGER_API Burger::Globals::GetFileVersionInfoSizeA(const char *lptstrFilename,unsigned long *lpdwHandle)
{
	void *pGetFileVersionInfoSizeA = LoadFunctionIndex(Globals::CALL_GetFileVersionInfoSizeA);
	Word uResult = 0;		// Failure
	if (pGetFileVersionInfoSizeA) {
		uResult = static_cast<Word>(static_cast<BOOL (WINAPI *)(LPCSTR,LPDWORD)>(pGetFileVersionInfoSizeA)(lptstrFilename,lpdwHandle));
	}
	return uResult;
}

/*! ************************************

	\brief Call GetFileVersionInfoSizeW(void)
	
	Manually load version.dll if needed and
	call the Windows function GetFileVersionInfoSizeW()

	http://msdn.microsoft.com/en-us/library/windows/desktop/ms647005(v=vs.85).aspx
	
	\note This function is only available on Windows

	\param lptstrFilename The name of the file of interest.
	\param lpdwHandle A pointer to a variable that the function sets to zero.
	\return Returns the number of bytes if successful, or zero otherwise.
		
***************************************/

Word32 BURGER_API Burger::Globals::GetFileVersionInfoSizeW(const Word16 *lptstrFilename,unsigned long *lpdwHandle)
{
	void *pGetFileVersionInfoSizeW = LoadFunctionIndex(Globals::CALL_GetFileVersionInfoSizeW);
	Word uResult = 0;		// Failure
	if (pGetFileVersionInfoSizeW) {
		uResult = static_cast<Word>(static_cast<BOOL (WINAPI *)(LPCWSTR,LPDWORD)>(pGetFileVersionInfoSizeW)(reinterpret_cast<LPCWSTR>(lptstrFilename),lpdwHandle));
	}
	return uResult;
}

/*! ************************************

	\brief Call HidD_GetHidGuid(GUID *)
	
	Manually load hid.dll if needed and
	call the Windows function HidD_GetHidGuid()

	http://msdn.microsoft.com/en-us/library/windows/hardware/ff538924(v=vs.85).aspx
	
	\note This function is only available on Windows

	\param HidGuid Pointer to a caller-allocated GUID buffer that the routine uses to return the device interface GUID for HIDClass devices.
		
***************************************/

void BURGER_API Burger::Globals::HidD_GetHidGuid(GUID *HidGuid)
{
	void *pHidD_GetHidGuid = LoadFunctionIndex(Globals::CALL_HidD_GetHidGuid);
	if (!pHidD_GetHidGuid) {
		MemoryClear(HidGuid,sizeof(GUID));
	} else {
		static_cast<void (WINAPI *)(GUID *)>(pHidD_GetHidGuid)(HidGuid);
	}
}

/*! ************************************

	\brief Call SetupDiGetClassDevsA(const GUID *ClassGuid,PCSTR Enumerator,HWND hwndParent,DWORD Flags)
	
	Manually load Setupapi.dll if needed and
	call the Windows function SetupDiGetClassDevsA()

	http://msdn.microsoft.com/en-us/library/windows/hardware/ff551069(v=vs.85).aspx
	
	\note This function is only available on Windows

	\param ClassGuid A pointer to the GUID for a device setup class or a device interface class. 
	\param Enumerator A pointer to a NULL-terminated string that specifies a PnP enumerator or instance ID
	\param hwndParent A handle to the top-level window to be used for a user interface that is associated with installing a device instance in the device information set.
	\param Flags A variable of type DWORD that specifies control options that filter the device information elements that are added to the device information set.
	\return If the operation succeeds, SetupDiGetClassDevs returns a handle to a device information set that contains all installed devices that matched the supplied parameters.

***************************************/

void * BURGER_API Burger::Globals::SetupDiGetClassDevsA(const GUID *ClassGuid,const char *Enumerator,HWND__ *hwndParent,Word32 Flags)
{
	void *pSetupDiGetClassDevsA = LoadFunctionIndex(Globals::CALL_SetupDiGetClassDevsA);
	void *pResult = INVALID_HANDLE_VALUE;		// Failure
	if (pSetupDiGetClassDevsA) {
		pResult = static_cast<void *(WINAPI *)(const GUID *,PCSTR,HWND,DWORD)>(pSetupDiGetClassDevsA)(ClassGuid,Enumerator,hwndParent,Flags);
	}
	return pResult;
}

/*! ************************************

	\brief Call SetupDiGetClassDevsW(const GUID *ClassGuid,PCWSTR Enumerator,HWND hwndParent,DWORD Flags)
	
	Manually load Setupapi.dll if needed and
	call the Windows function SetupDiGetClassDevsW()

	http://msdn.microsoft.com/en-us/library/windows/hardware/ff551069(v=vs.85).aspx
	
	\note This function is only available on Windows

	\param ClassGuid A pointer to the GUID for a device setup class or a device interface class. 
	\param Enumerator A pointer to a NULL-terminated string that specifies a PnP enumerator or instance ID
	\param hwndParent A handle to the top-level window to be used for a user interface that is associated with installing a device instance in the device information set.
	\param Flags A variable of type DWORD that specifies control options that filter the device information elements that are added to the device information set.
	\return If the operation succeeds, SetupDiGetClassDevs returns a handle to a device information set that contains all installed devices that matched the supplied parameters.

***************************************/

void * BURGER_API Burger::Globals::SetupDiGetClassDevsW(const GUID *ClassGuid,const Word16 *Enumerator,HWND__ *hwndParent,Word32 Flags)
{
	void *pSetupDiGetClassDevsW = LoadFunctionIndex(Globals::CALL_SetupDiGetClassDevsW);
	void *pResult = INVALID_HANDLE_VALUE;		// Failure
	if (pSetupDiGetClassDevsW) {
		pResult = static_cast<void *(WINAPI *)(const GUID *,PCWSTR,HWND,DWORD)>(pSetupDiGetClassDevsW)(ClassGuid,reinterpret_cast<PCWSTR>(Enumerator),hwndParent,Flags);
	}
	return pResult;
}

/*! ************************************

	\brief Call SetupDiGetDeviceInterfaceDetailA(HDEVINFO DeviceInfoSet,PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData,PSP_DEVICE_INTERFACE_DETAIL_DATA_A DeviceInterfaceDetailData,DWORD DeviceInterfaceDetailDataSize,PDWORD RequiredSize,PSP_DEVINFO_DATA DeviceInfoData)
	
	Manually load Setupapi.dll if needed and
	call the Windows function SetupDiGetDeviceInterfaceDetailA()

	http://msdn.microsoft.com/en-us/library/windows/hardware/ff551120(v=vs.85).aspx
	
	\note This function is only available on Windows

	\param DeviceInfoSet A pointer to the device information set that contains the interface for which to retrieve details.
	\param DeviceInterfaceData A pointer to an SP_DEVICE_INTERFACE_DATA structure that specifies the interface in DeviceInfoSet for which to retrieve details.
	\param DeviceInterfaceDetailData A pointer to an SP_DEVICE_INTERFACE_DETAIL_DATA structure to receive information about the specified interface. 
	\param DeviceInterfaceDetailDataSize The size of the DeviceInterfaceDetailData buffer. 
	\param RequiredSize A pointer to a variable of type Word32 that receives the required size of the DeviceInterfaceDetailData buffer.
	\param DeviceInfoData A pointer to a buffer that receives information about the device that supports the requested interface. 
	\return \ref TRUE if the function completed without error.

***************************************/

Word BURGER_API Burger::Globals::SetupDiGetDeviceInterfaceDetailA(void *DeviceInfoSet,_SP_DEVICE_INTERFACE_DATA *DeviceInterfaceData,_SP_DEVICE_INTERFACE_DETAIL_DATA_A *DeviceInterfaceDetailData,Word32 DeviceInterfaceDetailDataSize,Word32 *RequiredSize,_SP_DEVINFO_DATA *DeviceInfoData)
{
	void *pSetupDiGetDeviceInterfaceDetailA = LoadFunctionIndex(Globals::CALL_SetupDiGetDeviceInterfaceDetailA);
	Word uResult = 0;		// Failure
	if (pSetupDiGetDeviceInterfaceDetailA) {
		uResult = static_cast<Word>(static_cast<BOOL (WINAPI *)(HDEVINFO,PSP_DEVICE_INTERFACE_DATA,PSP_DEVICE_INTERFACE_DETAIL_DATA_A,DWORD,PDWORD,PSP_DEVINFO_DATA)>(pSetupDiGetDeviceInterfaceDetailA)(DeviceInfoSet,DeviceInterfaceData,DeviceInterfaceDetailData,DeviceInterfaceDetailDataSize,reinterpret_cast<DWORD *>(RequiredSize),DeviceInfoData));
	}
	return uResult;
}

/*! ************************************

	\brief Call SetupDiGetDeviceInterfaceDetailW(HDEVINFO DeviceInfoSet,PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData,PSP_DEVICE_INTERFACE_DETAIL_DATA_W DeviceInterfaceDetailData,DWORD DeviceInterfaceDetailDataSize,PDWORD RequiredSize,PSP_DEVINFO_DATA DeviceInfoData)
	
	Manually load Setupapi.dll if needed and
	call the Windows function SetupDiGetDeviceInterfaceDetailW()

	http://msdn.microsoft.com/en-us/library/windows/hardware/ff551120(v=vs.85).aspx
	
	\note This function is only available on Windows

	\param DeviceInfoSet A pointer to the device information set that contains the interface for which to retrieve details.
	\param DeviceInterfaceData A pointer to an SP_DEVICE_INTERFACE_DATA structure that specifies the interface in DeviceInfoSet for which to retrieve details.
	\param DeviceInterfaceDetailData A pointer to an SP_DEVICE_INTERFACE_DETAIL_DATA structure to receive information about the specified interface. 
	\param DeviceInterfaceDetailDataSize The size of the DeviceInterfaceDetailData buffer. 
	\param RequiredSize A pointer to a variable of type Word32 that receives the required size of the DeviceInterfaceDetailData buffer.
	\param DeviceInfoData A pointer to a buffer that receives information about the device that supports the requested interface. 
	\return \ref TRUE if the function completed without error.

***************************************/

Word BURGER_API Burger::Globals::SetupDiGetDeviceInterfaceDetailW(void *DeviceInfoSet,_SP_DEVICE_INTERFACE_DATA *DeviceInterfaceData,_SP_DEVICE_INTERFACE_DETAIL_DATA_W *DeviceInterfaceDetailData,Word32 DeviceInterfaceDetailDataSize,Word32 *RequiredSize,_SP_DEVINFO_DATA *DeviceInfoData)
{
	void *pSetupDiGetDeviceInterfaceDetailW = LoadFunctionIndex(Globals::CALL_SetupDiGetDeviceInterfaceDetailW);
	Word uResult = 0;		// Failure
	if (pSetupDiGetDeviceInterfaceDetailW) {
		uResult = static_cast<Word>(static_cast<BOOL (WINAPI *)(HDEVINFO,PSP_DEVICE_INTERFACE_DATA,PSP_DEVICE_INTERFACE_DETAIL_DATA_W,DWORD,PDWORD,PSP_DEVINFO_DATA)>(pSetupDiGetDeviceInterfaceDetailW)(DeviceInfoSet,DeviceInterfaceData,DeviceInterfaceDetailData,DeviceInterfaceDetailDataSize,reinterpret_cast<DWORD *>(RequiredSize),DeviceInfoData));
	}
	return uResult;
}

/*! ************************************

	\brief Call SetupDiEnumDeviceInterfaces(HDEVINFO DeviceInfoSet,PSP_DEVINFO_DATA DeviceInfoData,const GUID *InterfaceClassGuid,DWORD MemberIndex,PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData)

	Manually load Setupapi.dll if needed and
	call the Windows function SetupDiEnumDeviceInterfaces()

	http://msdn.microsoft.com/en-us/library/windows/hardware/ff551015(v=vs.85).aspx
	
	\note This function is only available on Windows

	\param DeviceInfoSet A pointer to a device information set that contains the device interfaces for which to return information. 
	\param DeviceInfoData A pointer to an SP_DEVINFO_DATA structure that specifies a device information element in DeviceInfoSet.
	\param InterfaceClassGuid A pointer to a GUID that specifies the device interface class for the requested interface.
	\param MemberIndex A zero-based index into the list of interfaces in the device information set. 
	\param DeviceInterfaceData A pointer to a caller-allocated buffer that contains, on successful return, a completed SP_DEVICE_INTERFACE_DATA structure that identifies an interface that meets the search parameters.
	\return \ref TRUE if the function completed without error.

***************************************/

Word BURGER_API Burger::Globals::SetupDiEnumDeviceInterfaces(void *DeviceInfoSet,_SP_DEVINFO_DATA *DeviceInfoData,const GUID *InterfaceClassGuid,Word32 MemberIndex,_SP_DEVICE_INTERFACE_DATA *DeviceInterfaceData)
{
	void *pSetupDiEnumDeviceInterfaces = LoadFunctionIndex(Globals::CALL_SetupDiEnumDeviceInterfaces);
	Word uResult = 0;		// Failure
	if (pSetupDiEnumDeviceInterfaces) {
		uResult = static_cast<Word>(static_cast<BOOL (WINAPI *)(HDEVINFO,PSP_DEVINFO_DATA,const GUID *,DWORD,PSP_DEVICE_INTERFACE_DATA)>(pSetupDiEnumDeviceInterfaces)(DeviceInfoSet,DeviceInfoData,InterfaceClassGuid,MemberIndex,DeviceInterfaceData));
	}
	return uResult;
}

/*! ************************************

	\brief Call SetupDiDestroyDeviceInfoList(HDEVINFO DeviceInfoSet)

	Manually load Setupapi.dll if needed and
	call the Windows function SetupDiDestroyDeviceInfoList()

	http://msdn.microsoft.com/en-us/library/windows/hardware/ff550996(v=vs.85).aspx
	
	\note This function is only available on Windows

	\param DeviceInfoSet A handle to the device information set to delete.
	\return \ref TRUE if the function completed without error.

***************************************/

Word BURGER_API Burger::Globals::SetupDiDestroyDeviceInfoList(void *DeviceInfoSet)
{
	void *pSetupDiDestroyDeviceInfoList = LoadFunctionIndex(Globals::CALL_SetupDiDestroyDeviceInfoList);
	Word uResult = 0;		// Failure
	if (pSetupDiDestroyDeviceInfoList) {
		uResult = static_cast<Word>(static_cast<BOOL (WINAPI *)(HDEVINFO)>(pSetupDiDestroyDeviceInfoList)(DeviceInfoSet));
	}
	return uResult;
}

/*! ************************************

	\brief Return the version of QuickTime.
	
	Detect if QuickTime is available, and if so, query
	it for the version present. If QuickTime is not available,
	the version returned is zero.
	
	This function is written so it only asks for the version
	once from QuickTime. It will cache the version and
	return the cached value on subsequent calls.
	
	By invoking DEEP magic, I will divine the version
	of QuickTimeX that is present. It will do a manual
	check of the system folder for either QTIM32.dll (Old)
	or Quicktime.qts (Current) and pull the version resource
	from the file.
	
	\return Version in the format of 0x0102 -> 1.2, 0x773 = 7.7.3
		
***************************************/

Word BURGER_API Burger::Globals::GetQuickTimeVersion(void)
{
	char PathName[MAX_PATH+32];

	Globals *pThis = &g_Globals;		// Get the pointer to the singleton
	if (!pThis->m_bQuickTimeVersionValid) {
		pThis->m_bQuickTimeVersionValid = TRUE;	// I got the version
		Word uResult = 0;						// I assume version 0!
		// Get the system directory for Quicktime
		WordPtr uPathLength = GetSystemDirectoryA(PathName,MAX_PATH);
		if (uPathLength) {

			// Get the Quicktime DLL using the old name for 2.0 or 3.0
			StringCopy(PathName+uPathLength,sizeof(PathName)-uPathLength,"\\QTIM32.DLL");
			DWORD uZeroLong = 0;
			DWORD uFileInfoSize = Globals::GetFileVersionInfoSizeA(PathName,&uZeroLong);
			const char *pQuery = "\\StringFileInfo\\040904E4\\ProductVersion";
			// Any data?
			if (!uFileInfoSize) {
				pQuery = "\\StringFileInfo\\040904B0\\FileVersion";
				// Try Quicktime 4.0
				uZeroLong = 0;
				StringCopy(PathName+uPathLength,sizeof(PathName)-uPathLength,"\\QuickTime.qts");
				uFileInfoSize = Globals::GetFileVersionInfoSizeA(PathName,&uZeroLong);
				if (!uFileInfoSize) {
					uPathLength = GetEnvironmentVariableA("ProgramFiles(x86)",PathName,MAX_PATH);
					if (!uPathLength) {
						uPathLength = GetEnvironmentVariableA("ProgramFiles",PathName,MAX_PATH);
					}
					if (uPathLength) {
						StringCopy(PathName+uPathLength,sizeof(PathName)-uPathLength,"\\QuickTime\\QTSystem\\QuickTime.qts");
						uFileInfoSize = Globals::GetFileVersionInfoSizeA(PathName,&uZeroLong);
					}
				}
			}
			if (uFileInfoSize) {
				// Get the data buffer
				char *pData = static_cast<char *>(Alloc(uFileInfoSize));
				if (pData) {
					void *pVersionData;			// Main data pointer to start parsing from
					if (Globals::GetFileVersionInfoA(PathName,0,uFileInfoSize,pData)) {
						UINT ZeroWord = 0;
						if (Globals::VerQueryValueA(pData,const_cast<char *>(pQuery),&pVersionData,&ZeroWord)) {
							// Running ascii pointer
							const char *pWorkPtr = static_cast<const char *>(pVersionData);
							uResult = AsciiToInteger(pWorkPtr,&pWorkPtr)<<8;
							if (pWorkPtr[0]=='.') {
								uFileInfoSize = AsciiToInteger(pWorkPtr+1,&pWorkPtr);
								if (uFileInfoSize>=16) {
									uFileInfoSize = 15;
								}
								uResult |= (uFileInfoSize<<4);
								if (pWorkPtr[0]=='.') {
									uFileInfoSize = AsciiToInteger(pWorkPtr+1);
									if (uFileInfoSize>=16) {
										uFileInfoSize = 15;
									}
									uResult |= uFileInfoSize;
								}
							}
						}
					}
					// Release the info pointer
					Free(pData);
				}
			}
		}
		pThis->m_uQuickTimeVersion = uResult;
	}
	// Return the QuickTime version
	return pThis->m_uQuickTimeVersion;
}

/***************************************

	Returns ULARGE_INTEGER with a file version of a file, or a failure code.

***************************************/

static Word64 BURGER_API GetFileVersion(const Word16* pFilename)
{
	Word64 uResult = 0;
	if (pFilename) {
		DWORD uNotUsed;
		UINT uBufferSize = Burger::Globals::GetFileVersionInfoSizeW(pFilename,&uNotUsed);
		if (uBufferSize) {
			BYTE* pFileVersionBuffer = static_cast<BYTE *>(Burger::Alloc(uBufferSize));
			if (pFileVersionBuffer) {
				if (Burger::Globals::GetFileVersionInfoW(pFilename,0,uBufferSize,pFileVersionBuffer)) {
					VS_FIXEDFILEINFO* pVersion = NULL;
					if (Burger::Globals::VerQueryValueW(pFileVersionBuffer,reinterpret_cast<const Word16 *>(L"\\"),(VOID**)&pVersion,&uBufferSize)) {
						if (pVersion != NULL) {
							uResult = (static_cast<Word64>(pVersion->dwFileVersionMS)<<32U)+pVersion->dwFileVersionLS;
						}
					}
				}
				Burger::Free(pFileVersionBuffer);
			}
		}
	}
	return uResult;
}

/***************************************

	Tries to get the DirectX version by looking at DirectX file versions
	Uses http://en.wikipedia.org/wiki/DirectX as a reference for file version lookups

***************************************/

static Word BURGER_API GetDirectXVersionViaFileVersions(void)
{
	Word16 szPath[512];
	Word uResult = 0;
	if (GetSystemDirectoryW(reinterpret_cast<LPWSTR>(szPath),MAX_PATH) != 0) {
		szPath[MAX_PATH-1] = 0;
		// Switch off the ddraw version
		size_t uLen = Burger::StringLength(szPath);
		Word16 *pDest = szPath+uLen;
		size_t uRemaining = sizeof(szPath)-(uLen*sizeof(Word16));
		Burger::StringCopy(pDest,uRemaining,reinterpret_cast<const Word16 *>(L"\\ddraw.dll"));
		Word64 uVersionDDraw = GetFileVersion(szPath);
		if (uVersionDDraw>=0x000400020000005FULL) {	// Win9x version 
			// file is >= DX1.0 version, so we must be at least DX1.0
			uResult = 0x0100;
		}
		if (uVersionDDraw>=0x0004000300000448ULL) {	// Win9x version
			// file is is >= DX2.0 version, so we must DX2.0 or DX2.0a (no redist change)
			uResult = 0x0200;
		}
		if (uVersionDDraw>=0x0004000400000044ULL) {	// Win9x version
			// file is is >= DX3.0 version, so we must be at least DX3.0
			uResult = 0x0300;
		}

		// Switch off the d3drg8x.dll version
		Burger::StringCopy(pDest,uRemaining,reinterpret_cast<const Word16 *>(L"\\d3drg8x.dll"));
		Word64 uVersionD3Drg8x = GetFileVersion(szPath);
		if (uVersionD3Drg8x>=0x0004000400000046ULL) {	// Win9x version
			// d3drg8x.dll is the DX3.0a version, so we must be DX3.0a or DX3.0b (no redist change)
			uResult = 0x301;
		}

		// No DirectX 4

		// Switch off the ddraw version
		if (uVersionDDraw>=0x000400050000009BULL) {	// Win9x version
			// ddraw.dll is the DX5.0 version, so we must be DX5.0 or DX5.2 (no redist change)
			uResult = 0x0500;
		}

		if (uVersionDDraw>=0x000400060000013EULL) {		// Win9x version
			// ddraw.dll is the DX6.0 version, so we must be at least DX6.0
			uResult = 0x0600;
		}

		if (uVersionDDraw>=0x00040006000001B4) {		// Win9x version
			// ddraw.dll is the DX6.1 version, so we must be at least DX6.1
			uResult = 0x0610;
		}


		// Switch off the dplayx.dll version
		Burger::StringCopy(pDest,uRemaining,reinterpret_cast<const Word16 *>(L"\\dplayx.dll"));
		Word64 uVersionDPlayx = GetFileVersion(szPath);
		if (uVersionDPlayx>=0x0004000600030206ULL) {	// Win9x version
			// ddraw.dll is the DX6.1 version, so we must be at least DX6.1a
			uResult = 0x0611;
		}

		// Switch off the ddraw version
		if (uVersionDDraw>=0x00040007000002BC) {	// Win9x version
			// TODO: find win2k version
			// ddraw.dll is the DX7.0 version, so we must be at least DX7.0
			uResult = 0x0700;
		}

		// Switch off the dinput version
		Burger::StringCopy(pDest,uRemaining,reinterpret_cast<const Word16 *>(L"\\dinput.dll"));
		Word64 uVersionDInput = GetFileVersion(szPath);
		if (uVersionDInput>=0x00040007000002CC) {	// Win9x version
			// ddraw.dll is the DX7.0 version, so we must be at least DX7.0a
			uResult = 0x0701;
		}

		// Switch off the ddraw version
		if ((((uVersionDDraw&0xFFFF000000000000ULL)==0x0004000000000000ULL) && (uVersionDDraw>=0x0004000800000190ULL)) || // Win9x version
			(((uVersionDDraw&0xFFFF000000000000ULL)==0x0005000000000000ULL) && (uVersionDDraw>=0x0005000108D20190ULL))) { // Win2k/WinXP version
			// ddraw.dll is the DX8.0 version, so we must be at least DX8.0 or DX8.0a (no redist change)
			uResult = 0x0800;
		}

		// Switch off the d3d8 version
		Burger::StringCopy(pDest,uRemaining,reinterpret_cast<const Word16 *>(L"\\d3d8.dll"));
		Word64 uVersionD3D8 = GetFileVersion(szPath);
		if ((((uVersionD3D8&0xFFFF000000000000ULL)==0x0004000000000000ULL) && (uVersionD3D8>=0x0004000800010371ULL)) || // Win9x version
			(((uVersionD3D8&0xFFFF000000000000ULL)==0x0005000000000000ULL) && (uVersionD3D8>=0x000500010A280371ULL))) { // Win2k/WinXP version
			// d3d8.dll is the DX8.1 version, so we must be at least DX8.1
			uResult = 0x0810;
		}

		if ((((uVersionD3D8&0xFFFF000000000000ULL)==0x0004000000000000ULL) && (uVersionD3D8>=0x0004000800010385ULL)) || // Win9x version
			(((uVersionD3D8&0xFFFF000000000000ULL)==0x0005000000000000ULL) && (uVersionD3D8>=0x000500010A280385ULL))) { // Win2k/WinXP version
			// d3d8.dll is the DX8.1a version, so we must be at least DX8.1a
			uResult = 0x0811;
		}

		// Switch off the Mpg2splt version
		Burger::StringCopy(pDest,uRemaining,reinterpret_cast<const Word16 *>(L"\\mpg2splt.ax"));
		Word64 uVersionMPG2Splt = GetFileVersion(szPath);
		if (uVersionMPG2Splt>=0x0006000300010375ULL) { // Win9x/Win2k/WinXP version
			// quartz.dll is the DX8.1b version, so we must be at least DX8.1b
			uResult = 0x0812;
		}

		// Switch off the dpnet version
		Burger::StringCopy(pDest,uRemaining,reinterpret_cast<const Word16 *>(L"\\dpnet.dll"));
		Word64 uVersionDPNet = GetFileVersion(szPath);
		if ((((uVersionDPNet&0xFFFF000000000000ULL)==0x0004000000000000ULL) && (uVersionDPNet>=0x0004000900000086ULL)) || // Win9x version
			(((uVersionDPNet&0xFFFF000000000000ULL)==0x0005000000000000ULL) && (uVersionDPNet>=0x000500020E5D0086ULL))) { // Win2k/WinXP version
			// dpnet.dll is the DX8.2 version, so we must be at least DX8.2
			uResult = 0x0820;
		}

		// Switch off the d3d9 version
		Burger::StringCopy(pDest,uRemaining,reinterpret_cast<const Word16 *>(L"\\d3d9.dll"));
		Word64 uVersionD3D9 = GetFileVersion(szPath);
		if (uVersionD3D9) {
			// File exists, but be at least DX9
			uResult = 0x0900;
		}
		if (uVersionD3D9>=0x0004000900000385ULL) {
			// 9.0a
			uResult = 0x0901;
		}
		if (uVersionD3D9>=0x0004000900000386ULL) {
			// 9.0b
			uResult = 0x0902;
		}
		if (uVersionD3D9>=0x0004000900000387ULL) {
			// 9.0c
			uResult = 0x0903;
		}
		if (uVersionD3D9>=0x0006000017704002ULL) {
			// 10.0
			uResult = 0x0A00;
		}
		if (uVersionD3D9>=0x0006000017714650ULL) {
			// 10.1
			uResult = 0x0A10;
		}
		if (uVersionD3D9>=0x00060000177246BBULL) {
			// 11.0
			uResult = 0x0B00;
		}
		if (uVersionD3D9>=0x0006000223F04000ULL) {
			// 11.1
			uResult = 0x0B10;
		}

	}
	return uResult;
}


/*! ************************************

	\brief Return the version of DirectX.
	
	Detect if DirectX is available, and if so, query
	it for the version present. If DirectX is not available,
	the version returned is zero.
	
	This function is written so it only asks for the version
	once from DirectX. It will cache the version and
	return the cached value on subsequent calls.
	
	By invoking DEEP magic, I will divine the version
	of DirectX that is present. It will do a manual
	check of the system folder for the DLLs
	and pull the version resource
	from them.

	\note This function is only available on Windows

	\return Version in the format of 0x0902 -> 9.2, 0xB01 = 11.0.1

***************************************/

Word BURGER_API Burger::Globals::GetDirectXVersion(void)
{
	Globals *pThis = &g_Globals;				// Get the pointer to the singleton
	if (!pThis->m_bDirectXVersionValid) {
		pThis->m_bDirectXVersionValid = TRUE;	// I got the version
		Word uResult = GetDirectXVersionViaFileVersions();
		pThis->m_uDirectXVersion = uResult;
	}
	// Return the DirectX version
	return pThis->m_uDirectXVersion;
}

/***************************************

	This callback is used to find a specific GUID for
	an enumerated device

***************************************/

#if !defined(DOXYGEN)
struct DeviceGuid_t {
	Word m_uDevNum;		// Count down
	GUID *m_pGUID;		// Buffer to store the located GUID
};
#endif

static int CALLBACK FindDeviceCallback(GUID *pGUID,LPSTR /* pName */,LPSTR /* pDeviceName */,void *pThis,void * /* Monitor */)
{
	DeviceGuid_t *pRef = static_cast<DeviceGuid_t *>(pThis);		// Deref the pointer
	if (!--pRef->m_uDevNum) {				// Found the device yet?
		if (pGUID) {						// Specific device?
			Burger::MemoryCopy(pRef->m_pGUID,pGUID,sizeof(pRef->m_pGUID[0]));	// Copy the GUID
		} else {
			Burger::MemoryClear(pRef->m_pGUID,sizeof(pRef->m_pGUID[0]));
		}
		return DDENUMRET_CANCEL;		// Stop now
	}
	return DDENUMRET_OK;				// Keep going
}

/*! ************************************

	\brief Given a specific device number, return the DirectX GUID

	Scan the device list for the GUID of the requested device.
	Device #0 returns the global display device (All screens)

	\note This function is only available on Windows

	\param pOutput Pointer to a buffer to accept the returned GUID. Cannot be \ref NULL
	\param uDevNum 0 for the master global device, 1-??? for the enumerated displays
	\return Zero if no error, non-zero if an error has occurred

***************************************/

Word BURGER_API Burger::Globals::GetVideoGUID(GUID *pOutput,Word uDevNum)
{
	Word uError = static_cast<Word>(E_FAIL);
	if (pOutput) {
		DeviceGuid_t Ref;
		Ref.m_uDevNum = ++uDevNum;		// Scan for this device
		Ref.m_pGUID = pOutput;
		uError = DirectDrawEnumerateExW(FindDeviceCallback,&Ref,DDENUM_ATTACHEDSECONDARYDEVICES|
			DDENUM_DETACHEDSECONDARYDEVICES|DDENUM_NONDISPLAYDEVICES);
		// The nVidia GT 545 fails on this call, so call using the 8 Bit Ascii version instead
		if (uError == E_NOTIMPL) {
			Ref.m_uDevNum = uDevNum;	// Scan for this device
			uError = DirectDrawEnumerateExA(FindDeviceCallback,&Ref,DDENUM_ATTACHEDSECONDARYDEVICES|
				DDENUM_DETACHEDSECONDARYDEVICES|DDENUM_NONDISPLAYDEVICES);
		}
		if (uError==DD_OK) {
			if (Ref.m_uDevNum) {		// Got it?
				uError = static_cast<Word>(E_FAIL);
			}
		}
	}
	return uError;
}

/*! ************************************

	\brief Call LoadLibraryA() without file error boxes

	When LoadLibraryA() is called in windows, it's possible that
	if the file is not found, windows will display an error message box
	mentioning that a DLL is missing. This function will prohibit
	this behavior by setting the ErrorMode to SEM_NOOPENFILEERRORBOX
	before the call to LoadLibraryA() and restoring the
	flag to the previous setting before function exit.

	\note This function is only available on Windows

	\param pInput ASCII pathname of the DLL file to load.
	\return \ref NULL if the DLL was not loaded, a valid HINSTANCE on success
	\sa LoadLibraryW()

***************************************/

HINSTANCE BURGER_API Burger::Globals::LoadLibraryA(const char *pInput)
{
	UINT uOldMode = SetErrorMode(SEM_NOOPENFILEERRORBOX);
	HINSTANCE hResult = ::LoadLibraryA(pInput);
	SetErrorMode(uOldMode);
	return hResult;
}

/*! ************************************

	\brief Call LoadLibraryW() without file error boxes

	When LoadLibraryW() is called in windows, it's possible that
	if the file is not found, windows will display an error message box
	mentioning that a DLL is missing. This function will prohibit
	this behavior by setting the ErrorMode to SEM_NOOPENFILEERRORBOX
	before the call to LoadLibraryW() and restoring the
	flag to the previous setting before function exit.

	\note This function is only available on Windows

	\param pInput UTF16 pathname of the DLL file to load. 
	\return \ref NULL if the DLL was not loaded, a valid HINSTANCE on success
	\sa LoadLibraryA()

***************************************/

HINSTANCE BURGER_API Burger::Globals::LoadLibraryW(const Word16 *pInput)
{
	UINT uOldMode = SetErrorMode(SEM_NOOPENFILEERRORBOX);
	HINSTANCE hResult = ::LoadLibraryW(reinterpret_cast<LPCWSTR>(pInput));
	SetErrorMode(uOldMode);
	return hResult;
}

/*! ************************************

	\brief Load a library if needed

	Given a DLL index, detect if the library has already been
	loaded and if so, return the existing HINSTANCE, otherwise,
	load the DLL and if successful, return the HINSTANCE. If
	the load failed, return \ref NULL.

	\note This function is only available on Windows

	\param eIndex \ref eWindowsDLLIndex index to a DLL Burgerlib is tracking.
	\return \ref NULL if the DLL was not loaded, a valid HINSTANCE on success
	\sa LoadFunctionIndex()
	
***************************************/

HINSTANCE BURGER_API Burger::Globals::LoadLibraryIndex(eWindowsDLLIndex eIndex)
{
	HINSTANCE hResult = NULL;
	// Valid index?
	if (eIndex<DLL_COUNT) {
		// Has it been loaded?
		hResult = g_Globals.m_hInstances[eIndex];
		// If not already tested and not loaded?
		if (!hResult && !g_Globals.m_bInstancesTested[eIndex]) {
			// Mark as tested
			g_Globals.m_bInstancesTested[eIndex] = TRUE;
			// Load the DLL
			hResult = Globals::LoadLibraryA(s_LibaryNames[eIndex]);
			if (hResult) {
				// If it loaded fine, save the result
				g_Globals.m_hInstances[eIndex] = hResult;
			}
		}
	}
	return hResult;
}

/*! ************************************

	\brief Load a function from a DLL library if needed

	Given a function index, detect if the library has already been
	loaded and if not. load it. If loaded, look up the function
	and return the pointer to the function or \ref NULL if
	not found.

	\note This function is only available on Windows

	\param eIndex \ref eWindowsCallIndex index to a Windows function Burgerlib is tracking.
	\return \ref NULL if the DLL was not loaded or the function didn't exist, a valid function pointer on success
	\sa LoadLibraryIndex()
	
***************************************/

void * BURGER_API Burger::Globals::LoadFunctionIndex(eWindowsCallIndex eIndex)
{
	void *pResult = NULL;
	// Valid index?
	if (eIndex<CALL_COUNT) {
		// Has the function been loaded?
		pResult = g_Globals.m_pWindowsCalls[eIndex];
		if (!pResult) {
			// Not found, try to load the DLL (Fast, because it will only test once)
			HINSTANCE hResult = LoadLibraryIndex(g_CallNames[eIndex].eDLL);
			if (hResult) {
				// Get the function from the DLL
				pResult = GetProcAddress(hResult,g_CallNames[eIndex].m_pName);
				g_Globals.m_pWindowsCalls[eIndex] = pResult;
			}
		}
	}
	return pResult;
}

/*! ************************************

	\brief Adds a directory to the start folder.

	When installing an application, it may be desirable to create an entry in
	the start menu to show an application folder and links for files/applications
	of interest. This function locates the users start menu folder and
	ensures that the folder is created and Windows Explorer is notified
	of the change

	\param pGroupName UTF8 "C" string of the folder name for the start menu

	\note This function only exists on the Windows platform

	\return Returns TRUE if an error occurs

***************************************/

Word BURGER_API Burger::Globals::AddGroupToProgramMenu(const char *pGroupName)
{
	// Get the pidl for the start menu
	// this will be used to initialize the folder browser

	Word uResult = 10;
	ITEMIDLIST *pIDListStartMenu;		// Item list for the start menu
	if (SHGetSpecialFolderLocation(Globals::GetWindow(),CSIDL_PROGRAMS,&pIDListStartMenu)==NOERROR) {
		Word16 WorkPath[MAX_PATH*2];
		if (SHGetPathFromIDListW(pIDListStartMenu,reinterpret_cast<LPWSTR>(WorkPath))) {
			// Append a directory divider
			WordPtr uLength = StringLength(WorkPath);
			WorkPath[uLength] = '\\';
			WorkPath[uLength+1] = 0;

			// Append the new folder name
			String16 GroupName(pGroupName);
			StringConcatenate(WorkPath,sizeof(WorkPath),GroupName.GetPtr());
			// Convert to UTF8 for Burgerlib
			String UTF8(WorkPath);
			Filename TempPath;
			TempPath.SetFromNative(UTF8.GetPtr());
			if (!FileManager::CreateDirectoryPath(&TempPath)) {		// Create the directory
				// Notify the shell that this folder was updated
				// Use SHCNF_PATHW since WorkPath is UTF16
				SHChangeNotify(SHCNE_MKDIR,SHCNF_PATHW,WorkPath,NULL);
			}
			uResult = 0;	// Success!
		}
	}
	return uResult;
}

/*! ************************************

	\brief Set a user registry key with a string

	Strings are all UTF-8. This function will perform
	conversion to UTF-16 for Windows

	\note This function only exists on the Windows platform

	\param pKey Key found in HKEY_CURRENT_USER
	\param pSubKey Name of the sub-key of interest, can be \ref NULL
	\param pData String to store in the registry
	\return Zero if successful, non-zero is the Windows error code.
	\sa AssociateFileExtensionToExe(const char *,const char *,const char *)

***************************************/

int BURGER_API Burger::Globals::CreateUserRegistryKey(const char *pKey,const char *pSubKey,const char *pData)
{
	// Convert from UTF8 to UTF16 for Windows
	String16 KeyUTF16(pKey);
	HKEY hKey = NULL;
	// Create the registry key
	LONG lStatus = RegCreateKeyExW(HKEY_CURRENT_USER,reinterpret_cast<LPWSTR>(KeyUTF16.GetPtr()),
		0,0,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
	if (lStatus == ERROR_SUCCESS) {
		const wchar_t *pSub16 = NULL;
		// SubKeyUTF16 has to remain in scope
		String16 SubKeyUTF16(pSubKey);
		if (SubKeyUTF16.GetLength()) {
			pSub16 = reinterpret_cast<const wchar_t *>(SubKeyUTF16.GetPtr());
		}
		String16 DataUTF16(pData);
		lStatus = RegSetValueExW(hKey,reinterpret_cast<LPCWSTR>(pSub16),0,REG_SZ,reinterpret_cast<const BYTE *>(DataUTF16.GetPtr()),(static_cast<DWORD>(DataUTF16.GetLength())+1)*2);
		RegCloseKey(hKey);
	}
	return lStatus;
}

/*! ************************************

	\brief Associate a data file to the application.
	
	Set the user registry to associate a data file type with the
	currently running executable.
	
	\note This function only exists on the Windows platform

	\code
	// Tell Windows Explorer to launch .datafile files with the currently running app by double-clicking
	AssociateFileExtensionToExe(".datafile","Data for the Fubar application","com.oldskuul.fubar");
	\endcode

	\param pFileExtension ".foo" Pointer to a "C" string with the file extension
	\param pDescription "Foo file" Description of this file type to be visible to Explorer
	\param pProgramID "com.company.application" program ID
	\sa CreateUserRegistryKey(const char *,const char *,const char *)

***************************************/

void BURGER_API Burger::Globals::AssociateFileExtensionToExe(const char *pFileExtension,const char *pDescription,const char *pProgramID)
{
	// Create the keys for the file extension and the description to show in Explorer
	{
		// Create the key for the file extension itself. .foo -> Unique program ID
		String ClassExtension(g_SoftwareClasses,pFileExtension);
		CreateUserRegistryKey(ClassExtension.GetPtr(),NULL,pProgramID);
	}
	{
		// Create the key for the unique program ID, with the file's description
		String ClassExtension(g_SoftwareClasses,pProgramID);
		CreateUserRegistryKey(ClassExtension.GetPtr(),NULL,pDescription);
	}

	// With the program ID already requested, generate the app's location for the ID
	// and the sample command line to use if you "drag and drop" a file on the exe.
	Word16 TempBuffer[MAX_PATH];
	// Get the pathname to the currently running application
	if (GetModuleFileNameW(NULL,reinterpret_cast<LPWSTR>(TempBuffer),MAX_PATH)<MAX_PATH) {
		String ExePath(TempBuffer);
		{
			// Create the key and command to launch on double click
			String ClassShellOpen(g_SoftwareClasses,pProgramID,"\\shell\\open\\command");
			String Command("\"",ExePath.GetPtr(),"\" \"%1\"");
			CreateUserRegistryKey(ClassShellOpen.GetPtr(),NULL,Command.GetPtr());
		}
		{
			// Create the key and reference to the icon for the data file
			String ClassIcon(g_SoftwareClasses,pProgramID,"\\DefaultIcon");
			String Command2("\"",ExePath.GetPtr(),"\",1");
			CreateUserRegistryKey(ClassIcon.GetPtr(),NULL,Command2.GetPtr());
		}
	}
}

/***************************************

	Load and launch a web page from an address string

***************************************/

Word BURGER_API Burger::Globals::LaunchURL(const char *pURL)
{
	String16 URL16(pURL);
	if (ShellExecuteW(GetDesktopWindow(),reinterpret_cast<LPCWSTR>(L"open"),reinterpret_cast<LPCWSTR>(URL16.GetPtr()),NULL,NULL,SW_SHOW)==0) {
		return TRUE;	/* I died */
	}
	return FALSE;		/* I launched */
}

/***************************************

	Execute a tool and capture the text output

***************************************/

int BURGER_API Burger::Globals::ExecuteTool(const char *pFilename,const char *pParameters,OutputMemoryStream *pOutput)
{
	// Get the parameter list
	Filename AppName(pFilename);
	String Full("\"",AppName.GetNative(),"\" ",pParameters);
	// Create the full Unicode command string
	String16 Unicode(Full);

	// Prepare the process information
	PROCESS_INFORMATION ProcessInfo;
	MemoryClear(&ProcessInfo,sizeof(ProcessInfo));
	STARTUPINFOW StartupInfo;
	MemoryClear(&StartupInfo,sizeof(StartupInfo));
	StartupInfo.cb = sizeof(StartupInfo);

	// Assume no text capturing

	HANDLE hCaptureIn = NULL;
	HANDLE hCaptureOut = NULL;

	// Is capturing requested?
	if (pOutput) {
		// Create a pipe for STDOUT
		SECURITY_ATTRIBUTES SecurityAttributes;
		MemoryClear(&SecurityAttributes,sizeof(SecurityAttributes));
		SecurityAttributes.nLength = sizeof(SecurityAttributes);
		SecurityAttributes.bInheritHandle = TRUE;
		SecurityAttributes.lpSecurityDescriptor = NULL;
		// Create them and allow the capture pipe to inherit permissions
		if (CreatePipe(&hCaptureIn,&hCaptureOut,&SecurityAttributes,0)) {
			if (SetHandleInformation(hCaptureIn,HANDLE_FLAG_INHERIT,0)) {
				// It's good, capture the output
				StartupInfo.hStdError = hCaptureOut;
				StartupInfo.hStdOutput = hCaptureOut;
				StartupInfo.dwFlags = STARTF_USESTDHANDLES;
			}
		}
	}

	// Assume failure
	DWORD nExitCode=10;

	// Call the tool
	BOOL bResult = CreateProcessW(NULL,reinterpret_cast<LPWSTR>(Unicode.GetPtr()),
		NULL,NULL,pOutput!=NULL,0,NULL,NULL,&StartupInfo,&ProcessInfo);

	// Did it even launch?
	if (bResult) {
		// Wait for the tool to finish executing
		if (WaitForSingleObject(ProcessInfo.hProcess,INFINITE)==WAIT_OBJECT_0) {
			// Get the exit code from the tool
			if (!GetExitCodeProcess(ProcessInfo.hProcess,&nExitCode)) {
				// Failure! Assume an error code of 10
				nExitCode = 10;
			}
		}
		// Release the handles (Since launch was successful)
		CloseHandle(ProcessInfo.hProcess);
		CloseHandle(ProcessInfo.hThread);
	}

	// Release the capture pipe (So the input pipe is finite)
	if (hCaptureOut) {
		CloseHandle(hCaptureOut);
	}

	// Only capture if needed
	if (bResult && pOutput) {
		DWORD uBytesRead; 
		Word8 Buffer[1024]; 
		for (;;) {
			// Read from the finite pipe
			BOOL bSuccess = ReadFile(hCaptureIn,Buffer,sizeof(Buffer),&uBytesRead,NULL);
			// Error or all done?
			if ((!bSuccess) || (!uBytesRead)) {
				break; 
			}
			pOutput->Append(Buffer,uBytesRead); 
		} 
	}
	// Clean up the last handle
	if (hCaptureIn) {
		CloseHandle(hCaptureIn);
	}
	// Exit with the tool's error code
	return static_cast<int>(nExitCode);
}

/***************************************

	Read an environment variable as UTF8

***************************************/

const char *BURGER_API Burger::Globals::GetEnvironmentString(const char *pKey)
{
	// Convert the key to UTF16
	String16 Key16(pKey);
	// How long is the key?
	DWORD uLength = GetEnvironmentVariableW(reinterpret_cast<LPCWSTR>(Key16.GetPtr()),NULL,0);
	char *pValue = NULL;
	// Any key?
	if (uLength) {
		// Set the buffer to accept the value
		String16 Output;
		Output.SetBufferSize(uLength);
		// Read in the environment variable as UTF16
		GetEnvironmentVariableW(reinterpret_cast<LPCWSTR>(Key16.GetPtr()),reinterpret_cast<LPWSTR>(Output.GetPtr()),uLength+1);
		// Convert to UTF8
		String Final(Output.GetPtr());
		// Return a copy
		pValue = StringDuplicate(Final.GetPtr());
	}
	return pValue;
}

/***************************************

	Set an environment variable with a UTF8 string

***************************************/

Word BURGER_API Burger::Globals::SetEnvironmentString(const char *pKey,const char *pInput)
{
	// Convert the key to UTF16
	String16 Key16(pKey);
	// Convert the input to UTF16
	String16 Input16(pInput);
	// If the input is an empty string or null, get rid of the string
	LPWSTR pInput16 = NULL;
	if (pInput && pInput[0]) {
		// Set to the new value
		pInput16 = reinterpret_cast<LPWSTR>(Input16.GetPtr());
	}
	// Set the variable!
	Word uResult = 0;
	if (!SetEnvironmentVariableW(reinterpret_cast<LPCWSTR>(Key16.GetPtr()),pInput16)) {
		uResult = static_cast<Word>(GetLastError());
	}
	return uResult;
}

#endif
