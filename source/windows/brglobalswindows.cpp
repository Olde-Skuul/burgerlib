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

#if !defined(WIN32_LEAN_AND_MEAN) && !defined(DOXYGEN)
#define WIN32_LEAN_AND_MEAN
#endif

#if !defined(DIRECTINPUT_VERSION) && !defined(DOXYGEN)
#define DIRECTINPUT_VERSION 0x800
#endif

#if !defined(DIRECT3D_VERSION) && !defined(DOXYGEN)
#define DIRECT3D_VERSION 0x700
#endif

#if !defined(DIRECTDRAW_VERSION) && !defined(DOXYGEN)
#define DIRECTDRAW_VERSION 0x700
#endif

#include <Windows.h>
#include <MMReg.h>
#include <dinput.h>
#include <d3d.h>
#include <dsound.h>
#include <shellapi.h>
#include <shlobj.h>

// Used by the Quicktime and DirectX version functions
#if defined(BURGER_WATCOM)
#pragma library ("version.lib");
#pragma library ("shell32.lib");
#pragma library ("advapi32.lib");
#else
#pragma comment(lib,"version.lib")
#pragma comment(lib,"shell32.lib")
#pragma comment(lib,"advapi32.lib")
#endif

static const char g_SoftwareClasses[] = "Software\\Classes\\";

/***************************************

	\brief Windows specific clean up code
	
	Some functions perform actions that reserve resources. Release them
	on application shutdown
	
***************************************/

Burger::Globals::~Globals()
{
	// Was DirectInput8 functions loaded?
	if (m_pDirectInput8W) {
		m_pDirectInput8W->Release();
		m_pDirectInput8W = NULL;
	}
	
	if (m_hDInput8DLL) {
		FreeLibrary(m_hDInput8DLL);
		m_hDInput8DLL = NULL;
		m_pDirectInput8Create = NULL;
	}

	// Was DirectInput functions loaded?
	if (m_pDirectInputW) {
		m_pDirectInputW->Release();
		m_pDirectInputW = NULL;
	}
	// Release the loaded DirectInput DLL
	if (m_hDInputDLL) {
		FreeLibrary(m_hDInputDLL);
		m_hDInputDLL = NULL;
		m_pDirectInputCreateW = NULL;
		m_pDirectInputW = NULL;
	}

	// Was DirectDraw loaded?
	if (m_hDDraw) {
		FreeLibrary(m_hDDraw);
		m_hDDraw = NULL;
		m_pDirectDrawCreate = NULL;
		m_pDirectDrawCreateEx = NULL;
		m_pDirectDrawCreateClipper = NULL;
		m_pDirectDrawEnumerateA = NULL;
		m_pDirectDrawEnumerateW = NULL;
		m_pDirectDrawEnumerateExA = NULL;
		m_pDirectDrawEnumerateExW = NULL;
	}
	// Was D3D9 loaded?
	if (m_hD3D9) {
		FreeLibrary(m_hD3D9);
		m_hD3D9 = NULL;
		m_pDirect3DCreate9 = NULL;
	}

	// Was DirectSound loaded?
	if (m_hDirectSound) {
		FreeLibrary(m_hDirectSound);
		m_hDirectSound = NULL;
		m_pDirectSoundCreate = NULL;
		m_pDirectSoundCreate8 = NULL;
		m_pDirectSoundEnumerateA = NULL;
		m_pDirectSoundEnumerateW = NULL;
		m_pGetDeviceID = NULL;
		m_pDirectSoundFullDuplexCreate = NULL;
	}
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
	if (g_Globals.m_pDirectInputCreateW) {
		uResult = TRUE;
	} else {
		// Attempt to load the DLL
		HINSTANCE hLibrary = Globals::LoadLibraryA("dinput.dll");
		// Is the DLL present?
		if (hLibrary) {
			void *pFunction = GetProcAddress(hLibrary,"DirectInputCreateW");
			// Is the function present?
			if (!pFunction) {
				// Darn, release the DLL
				FreeLibrary(hLibrary);
			} else {
				// Store the function and the DLL handle
				g_Globals.m_pDirectInputCreateW = pFunction;
				g_Globals.m_hDInputDLL = hLibrary;
				uResult = TRUE;
			}
		}
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
	if (g_Globals.m_pDirectInput8Create) {
		uResult = TRUE;
	} else {
		HINSTANCE hLibrary = Globals::LoadLibraryA("dinput8.dll");
		if (hLibrary) {				// Good!
			void *pFunction = GetProcAddress(hLibrary,"DirectInput8Create");
			if (!pFunction) {
				FreeLibrary(hLibrary);
			} else {
				g_Globals.m_pDirectInput8Create = pFunction;
				g_Globals.m_hDInput8DLL = hLibrary;
				uResult = TRUE;		// The code is good!
			}
		}
	}
	return uResult;		// TRUE if ok!
}

/*! ************************************

	\brief Detect and load DirectDraw functions

	\note This function is only available on Windows

	\return \ref TRUE if DirectDraw is present on the system, \ref FALSE if not
	
***************************************/

Word BURGER_API Burger::Globals::IsDirectDrawPresent(void)
{
	Word uResult = FALSE;
	if (g_Globals.m_pDirectDrawCreate) {
		uResult = TRUE;
	} else {
		HINSTANCE hLibrary = Globals::LoadLibraryA("ddraw.dll");
		if (hLibrary) {				// Good!
			void *pFunction = GetProcAddress(hLibrary,"DirectDrawCreate");
			if (!pFunction) {
				FreeLibrary(hLibrary);
			} else {
				g_Globals.m_pDirectDrawCreate = pFunction;
				g_Globals.m_hDDraw = hLibrary;
				g_Globals.m_pDirectDrawCreateEx = GetProcAddress(hLibrary,"DirectDrawCreateEx");
				g_Globals.m_pDirectDrawCreateClipper = GetProcAddress(hLibrary,"DirectDrawCreateClipper");
				g_Globals.m_pDirectDrawEnumerateA = GetProcAddress(hLibrary,"DirectDrawEnumerateA");
				g_Globals.m_pDirectDrawEnumerateW = GetProcAddress(hLibrary,"DirectDrawEnumerateW");
				g_Globals.m_pDirectDrawEnumerateExA = GetProcAddress(hLibrary,"DirectDrawEnumerateExA");
				g_Globals.m_pDirectDrawEnumerateExW = GetProcAddress(hLibrary,"DirectDrawEnumerateExW");
				uResult = TRUE;		// The code is good!
			}
		}
	}
	return uResult;		// TRUE if ok!
}

/*! ************************************

	\brief Detect and load D3D9 functions

	\note This function is only available on Windows

	\return \ref TRUE if D3D9 is present on the system, \ref FALSE if not
	
***************************************/

Word BURGER_API Burger::Globals::IsD3D9Present(void)
{
	Word uResult = FALSE;
	if (g_Globals.m_pDirect3DCreate9) {
		uResult = TRUE;
	} else {
		HINSTANCE hLibrary = Globals::LoadLibraryA("d3d9.dll");
		if (hLibrary) {				// Good!
			void *pFunction = GetProcAddress(hLibrary,"Direct3DCreate9");
			if (!pFunction) {
				FreeLibrary(hLibrary);
			} else {
				g_Globals.m_pDirect3DCreate9 = pFunction;
				g_Globals.m_hD3D9 = hLibrary;
				uResult = TRUE;		// The code is good!
			}
		}
	}
	return uResult;		// TRUE if ok!
}

/*! ************************************

	\brief Detect and load DirectSound functions

	\note This function is only available on Windows

	\return \ref TRUE if DirectSound is present on the system, \ref FALSE if not
	
***************************************/

Word BURGER_API Burger::Globals::IsDirectSoundPresent(void)
{
	Word uResult = FALSE;
	if (g_Globals.m_pDirectSoundCreate) {
		uResult = TRUE;
	} else {
		HINSTANCE hLibrary = Globals::LoadLibraryA("dsound.dll");
		if (hLibrary) {				// Good!
			void *pFunction = GetProcAddress(hLibrary,"DirectSoundCreate");
			if (!pFunction) {
				FreeLibrary(hLibrary);
			} else {
				g_Globals.m_pDirectSoundCreate = pFunction;
				g_Globals.m_hDirectSound = hLibrary;
				g_Globals.m_pDirectSoundCreate8 = GetProcAddress(hLibrary,"DirectSoundCreate8");
				g_Globals.m_pDirectSoundEnumerateA = GetProcAddress(hLibrary,"DirectSoundEnumerateA");
				g_Globals.m_pDirectSoundEnumerateW = GetProcAddress(hLibrary,"DirectSoundEnumerateW");
				g_Globals.m_pGetDeviceID = GetProcAddress(hLibrary,"GetDeviceID");
				g_Globals.m_pDirectSoundFullDuplexCreate = GetProcAddress(hLibrary,"DirectSoundFullDuplexCreate");
				uResult = TRUE;		// The code is good!
			}
		}
	}
	return uResult;		// TRUE if ok!
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
	Word uResult;
	// Was there already a DirectInput instance?
	IDirectInputW *pDirectInputW = g_Globals.m_pDirectInputW;
	if (pDirectInputW) {
		// Return this instance
		uResult = 0;
	} else {
		if (!IsDirectInputPresent()) {
			uResult = 10;
		} else {
			typedef HRESULT (WINAPI *DirectInputCreateWProc)(HINSTANCE,DWORD,IDirectInputW **,LPUNKNOWN);
			DirectInputCreateWProc pDirectInputCreateWProc = static_cast<DirectInputCreateWProc>(g_Globals.m_pDirectInputCreateW);
			HRESULT hResult = pDirectInputCreateWProc(GetModuleHandle(NULL),0x700,&g_Globals.m_pDirectInputW,NULL);
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
	Word uResult;
	// Was there already a DirectInput8 instance?
	IDirectInput8W *pDirectInput8W = g_Globals.m_pDirectInput8W;
	if (pDirectInput8W) {
		// Return this instance
		uResult = 0;
	} else {
		if (!IsDirectInput8Present()) {
			uResult = static_cast<Word>(DIERR_NOTFOUND);
		} else {
			typedef HRESULT (WINAPI *DirectInput8CreateProc)(HINSTANCE,DWORD,REFIID,LPVOID *,LPUNKNOWN);
			DirectInput8CreateProc pDirectInput8CreateProc = static_cast<DirectInput8CreateProc>(g_Globals.m_pDirectInput8Create);
			HRESULT hResult = pDirectInput8CreateProc(GetModuleHandle(NULL),0x800,IID_IDirectInput8W,reinterpret_cast<void **>(&g_Globals.m_pDirectInput8W),NULL);
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

	\brief Load in ddraw.dll and call DirectDrawCreateExW

	To allow maximum compatibility, this function will manually load
	ddraw.dll and then invoke DirectDrawCreateExW if present.

    http://msdn.microsoft.com/en-us/library/windows/desktop/gg426118(v=vs.85).aspx

	\note This function is only available on Windows

	\param pGuid Guid of the version of DirectDraw requested
	\param pOutput Pointer to the location where the valid IDirectDraw7 pointer
		will be stored. If the function fails, this value is guaranteed to be \ref NULL.
	\return Zero if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectDrawCreateExW(const GUID *pGuid,IDirectDraw7 **pOutput)
{
	Word uResult;
	// Was there already a DirectDraw instance?
	IDirectDraw7 *pDirectDraw7 = NULL;
	if (!IsDirectDrawPresent()) {
		uResult = 10;
	} else {
		typedef HRESULT (WINAPI *DirectDrawCreateExProc)( GUID FAR * lpGuid, LPVOID  *lplpDD, REFIID  iid,IUnknown FAR *pUnkOuter );
		DirectDrawCreateExProc pDirectDrawCreateExProc = static_cast<DirectDrawCreateExProc>(g_Globals.m_pDirectDrawCreateEx);
		HRESULT hResult = pDirectDrawCreateExProc(const_cast<GUID *>(pGuid),reinterpret_cast<void **>(&pDirectDraw7),IID_IDirectDraw7,NULL);
		if (hResult<0) {
			uResult = static_cast<Word>(hResult);
		} else {
			uResult = 0;
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
	Word uResult;
	// Was there already a DirectDraw instance?
	if (!IsDirectDrawPresent()) {
		uResult = 10;
	} else {
		typedef HRESULT (WINAPI *DirectDrawEnumerateExAProc)(LPDDENUMCALLBACKEXA lpCallback,LPVOID lpContext,DWORD dwFlags);
		DirectDrawEnumerateExAProc pDirectDrawEnumerateExAProc = static_cast<DirectDrawEnumerateExAProc>(g_Globals.m_pDirectDrawEnumerateExA);
		HRESULT hResult = pDirectDrawEnumerateExAProc(static_cast<LPDDENUMCALLBACKEXA>(lpCallback),lpContext,dwFlags);
		if (hResult<0) {
			uResult = static_cast<Word>(hResult);
		} else {
			uResult = 0;
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
	Word uResult;
	// Was there already a DirectDraw instance?
	if (!IsDirectDrawPresent()) {
		uResult = 10;
	} else {
		typedef HRESULT (WINAPI *DirectDrawEnumerateExWProc)(LPDDENUMCALLBACKEXW lpCallback, LPVOID lpContext, DWORD dwFlags);
		DirectDrawEnumerateExWProc pDirectDrawEnumerateExWProc = static_cast<DirectDrawEnumerateExWProc>(g_Globals.m_pDirectDrawEnumerateExW);
		HRESULT hResult = pDirectDrawEnumerateExWProc(static_cast<LPDDENUMCALLBACKEXW>(lpCallback),lpContext,dwFlags);
		if (hResult<0) {
			uResult = static_cast<Word>(hResult);
		} else {
			uResult = 0;
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
	if (IsD3D9Present()) {
		typedef IDirect3D9 * (WINAPI *Direct3DCreate9Proc)(UINT SDKVersion);
		Direct3DCreate9Proc pDirect3DCreate9Proc = static_cast<Direct3DCreate9Proc>(g_Globals.m_pDirect3DCreate9);
		pDirect3D9 = pDirect3DCreate9Proc(uSDKVersion);
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
	\return DD_OK if no error. Any other value means an error occured
	
***************************************/

Word BURGER_API Burger::Globals::DirectSoundCreate(const GUID *pGuidDevice,IDirectSound **pOutput)
{
	Word uResult;
	// Was there already a DirectSound instance?
	IDirectSound *pDirectSound = NULL;
	if (!IsDirectSoundPresent()) {
		uResult = static_cast<Word>(DSERR_INVALIDCALL);
	} else {
		typedef HRESULT (WINAPI *DirectSoundCreateProc)(LPCGUID pcGuidDevice, IDirectSound **ppDS, LPUNKNOWN pUnkOuter);
		DirectSoundCreateProc pDirectSoundCreateProc = static_cast<DirectSoundCreateProc>(g_Globals.m_pDirectSoundCreate);
		HRESULT hResult = pDirectSoundCreateProc(pGuidDevice,&pDirectSound,NULL);
		if (hResult<0) {
			uResult = static_cast<Word>(hResult);
		} else {
			uResult = 0;
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
	\return DD_OK if no error. Any other value means an error occured
	
***************************************/

Word BURGER_API Burger::Globals::DirectSoundCreate8(const GUID *pGuidDevice,IDirectSound8 **pOutput)
{
	Word uResult;
	// Was there already a DirectSound8 instance?
	IDirectSound8 *pDirectSound = NULL;
	if (!IsDirectSoundPresent()) {
		uResult = static_cast<Word>(DSERR_INVALIDCALL);
	} else {
		typedef HRESULT (WINAPI *DirectSoundCreate8Proc)(LPCGUID pcGuidDevice, IDirectSound8 **ppDS, LPUNKNOWN pUnkOuter);
		DirectSoundCreate8Proc pDirectSoundCreate8Proc = static_cast<DirectSoundCreate8Proc>(g_Globals.m_pDirectSoundCreate8);
		// If NULL then DirectSound8 is not in dsound.dll
		if (!pDirectSoundCreate8Proc) {
			uResult = static_cast<Word>(DSERR_INVALIDCALL);
		} else {
			HRESULT hResult = pDirectSoundCreate8Proc(pGuidDevice,&pDirectSound,NULL);
			if (hResult<0) {
				uResult = static_cast<Word>(hResult);
			} else {
				uResult = 0;
			}
		}
	}
	pOutput[0] = pDirectSound;
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
	check of the system folder for eithr QTIM32.dll (Old)
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
			DWORD uFileInfoSize = GetFileVersionInfoSizeA(PathName,&uZeroLong);
			const char *pQuery = "\\StringFileInfo\\040904E4\\ProductVersion";
			// Any data?
			if (!uFileInfoSize) {
				pQuery = "\\StringFileInfo\\040904B0\\FileVersion";
				// Try Quicktime 4.0
				uZeroLong = 0;
				StringCopy(PathName+uPathLength,sizeof(PathName)-uPathLength,"\\QuickTime.qts");
				uFileInfoSize = GetFileVersionInfoSizeA(PathName,&uZeroLong);
				if (!uFileInfoSize) {
					uPathLength = GetEnvironmentVariableA("ProgramFiles(x86)",PathName,MAX_PATH);
					if (!uPathLength) {
						uPathLength = GetEnvironmentVariableA("ProgramFiles",PathName,MAX_PATH);
					}
					if (uPathLength) {
						StringCopy(PathName+uPathLength,sizeof(PathName)-uPathLength,"\\QuickTime\\QTSystem\\QuickTime.qts");
						uFileInfoSize = GetFileVersionInfoSizeA(PathName,&uZeroLong);
					}
				}
			}
			if (uFileInfoSize) {
				// Get the data buffer
				char *pData = static_cast<char *>(Alloc(uFileInfoSize));
				if (pData) {
					void *pVersionData;			// Main data pointer to start parsing from
					if (GetFileVersionInfoA(PathName,0,uFileInfoSize,pData)) {
						UINT ZeroWord = 0;
						if (VerQueryValueA(pData,const_cast<char *>(pQuery),&pVersionData,&ZeroWord)) {
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
		UINT uBufferSize = GetFileVersionInfoSizeW(reinterpret_cast<LPCWSTR>(pFilename),&uNotUsed);
		if (uBufferSize) {
			BYTE* pFileVersionBuffer = static_cast<BYTE *>(Burger::Alloc(uBufferSize));
			if (pFileVersionBuffer) {
				if (GetFileVersionInfoW(reinterpret_cast<LPCWSTR>(pFilename),0,uBufferSize,pFileVersionBuffer)) {
					VS_FIXEDFILEINFO* pVersion = NULL;
					if (VerQueryValueW(pFileVersionBuffer,(LPWSTR)L"\\",(VOID**)&pVersion,&uBufferSize)) {
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
			// d3drg8x.dll is the DX3.0a version, so we must be DX3.0a or DX3.0b  (no redist change)
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
	Word m_uDevNum;     // Count down
	GUID *m_pGUID;      // Buffer to store the located GUID
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

    \param pOutput Pointer to a buffer to accept the returned GUID. Cannot be \ref NULL
    \param uDevNum 0 for the master global device, 1-??? for the enumerated displays
    \return Zero if no error, non-zero if an error has occured

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

#endif
