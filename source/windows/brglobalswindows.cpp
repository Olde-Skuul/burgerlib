/***************************************

	Global variable manager

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

//
// Handle some annoying defines that some windows SDKs may or may not
// have
//

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#if !defined(DIRECTINPUT_VERSION)
#define DIRECTINPUT_VERSION 0x800
#endif

#if !defined(DIRECT3D_VERSION)
#define DIRECT3D_VERSION 0x900
#endif

#if !defined(DIRECTDRAW_VERSION)
#define DIRECTDRAW_VERSION 0x700
#endif

#if defined(_DEBUG) && !defined(DOXYGEN)
#define D3D_DEBUG_INFO
#endif

#if !defined(BUILD_WINDOWS)
#define BUILD_WINDOWS
#endif

#include <Windows.h>
#include <MMReg.h>
#include <dinput.h>
#include <d3d.h>
#include <ddraw.h>
#include <dsound.h>
#include <shellapi.h>
#include <shlobj.h>
#include <SetupAPI.h>

#if !defined(STATUS_ENTRYPOINT_NOT_FOUND)
#define STATUS_ENTRYPOINT_NOT_FOUND ((DWORD)0xC0000139L)
#endif

static const char g_SoftwareClasses[] = "Software\\Classes\\";

//
// Globals
//

HINSTANCE__ *Burger::Globals::g_hInstance;
HWND__ *Burger::Globals::g_hWindow;
Word32 Burger::Globals::g_uQuickTimeVersion;
Word8 Burger::Globals::g_bQuickTimeVersionValid;
Word32 Burger::Globals::g_uDirectXVersion;
Word8 Burger::Globals::g_bDirectXVersionValid;
Word8 Burger::Globals::g_bWindows95;
Word8 Burger::Globals::g_bWindowsXP;
Word8 Burger::Globals::g_bWindowsVista;
#if defined(BURGER_WIN32) || defined(DOXYGEN)
Word8 Burger::Globals::g_bIsWindows64Bit;
#endif

/***************************************

	DLLs that can be dynamically loaded at runtime (So the 
	application can launch if they are missing or missing functions)
	
***************************************/

#if !defined(DOXYGEN)

//
// These filenames MUST match eWindowsDLLIndex
// Note: d3d9d.dll doesn't load properly in windows 10. Code was added
// to try to load d3d9.dll instead in the case of load failure of d3d9d.dll
//
// <sarcasm> Thank you, Microsoft. </sarcasm>
//

static const char *s_LibaryNames[Burger::Globals::DLL_COUNT] = {
	"ddraw.dll",
	"dinput.dll",
	"dinput8.dll",
#if defined(_DEBUG)
	"d3d9d.dll",
#else
	"d3d9.dll",
#endif
	"d3dx9_43.dll",
	"dsound.dll",
	"rpcrt4.dll",
	"winmm.dll",
	"shlwapi.dll",
	"version.dll",
	"hid.dll",
	"setupapi.dll",
	"user32.dll",
	"kernel32.dll"
};

//
// Internal structure for DLL function lookup
//

struct CallNames_t {
	Burger::Globals::eWindowsDLLIndex eDLL;		// Enumeration for the DLL
	const char *m_pName;						// Function name
};

static const CallNames_t g_CallNames[Burger::Globals::CALL_COUNT] = {
	{Burger::Globals::DINPUT_DLL,"DirectInputCreateA"},
	{Burger::Globals::DINPUT_DLL,"DirectInputCreateW"},

	{Burger::Globals::DINPUT8_DLL,"DirectInput8Create"},
	
	{Burger::Globals::DDRAW_DLL,"DirectDrawCreate"},
	{Burger::Globals::DDRAW_DLL,"DirectDrawCreateEx"},
	{Burger::Globals::DDRAW_DLL,"DirectDrawCreateClipper"},
	{Burger::Globals::DDRAW_DLL,"DirectDrawEnumerateA"},
	{Burger::Globals::DDRAW_DLL,"DirectDrawEnumerateW"},
	{Burger::Globals::DDRAW_DLL,"DirectDrawEnumerateExA"},
	{Burger::Globals::DDRAW_DLL,"DirectDrawEnumerateExW"},

	{Burger::Globals::D3D9_DLL,"Direct3DCreate9"},
	
	{Burger::Globals::D3DX9_43_DLL,"D3DXCreateMatrixStack"},

	{Burger::Globals::DSOUND_DLL,"DirectSoundCreate"},
	{Burger::Globals::DSOUND_DLL,"DirectSoundEnumerateA"},
	{Burger::Globals::DSOUND_DLL,"DirectSoundEnumerateW"},
	{Burger::Globals::DSOUND_DLL,"DirectSoundCaptureCreate"},
	{Burger::Globals::DSOUND_DLL,"DirectSoundCaptureEnumerateA"},
	{Burger::Globals::DSOUND_DLL,"DirectSoundCaptureEnumerateW"},
	{Burger::Globals::DSOUND_DLL,"DirectSoundCreate8"},
	{Burger::Globals::DSOUND_DLL,"DirectSoundCaptureCreate8"},
	{Burger::Globals::DSOUND_DLL,"DirectSoundFullDuplexCreate"},
	{Burger::Globals::DSOUND_DLL,"GetDeviceID"},
	
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
	{Burger::Globals::SETUPAPI_DLL,"SetupDiDestroyDeviceInfoList"},

	{Burger::Globals::USER32_DLL,"TrackMouseEvent"},

	{Burger::Globals::KERNEL32_DLL,"GetSystemWow64DirectoryA"},
	{Burger::Globals::KERNEL32_DLL,"GetSystemWow64DirectoryW"}
};

#endif

/***************************************

	Self destructing timer that tracks the mouse and will
	send a WM_MOUSELEAVE event to the application if the mouse left the
	application window. This is necessary because if the mouse
	leaves the window, no mouse events are passed to the window, so
	this timer will monitor the mouse and if detected it's off
	the window, it will fire the event and end.
	
	This code is used for Win95/XP and NT to support the feature
	that appeared in XP Service Pack 1

***************************************/

static VOID CALLBACK TrackMouseTimerProc(HWND pWindow,UINT /* uMsg */,UINT_PTR uEventID, DWORD /* dwTime */)
{
	// Get the rect of the tracked window
	RECT TheClientRect;
	GetClientRect(pWindow,&TheClientRect);

	// Remap it to the desktop (2 points make a rect)
	MapWindowPoints(pWindow,HWND_DESKTOP,static_cast<POINT *>(static_cast<void*>(&TheClientRect)),2);

	// Where is the cursor?
	POINT TheCursorLocation;
	GetCursorPos(&TheCursorLocation);

	// Check if outside
	if (!PtInRect(&TheClientRect,TheCursorLocation) || (WindowFromPoint(TheCursorLocation) != pWindow) ) {

		// My work is done, commit suicide
		KillTimer(pWindow,uEventID);
		// Post the message to the app, to alert it that the mouse has
		// gone AWOL
		PostMessage(pWindow,WM_MOUSELEAVE,0,0);
	}
}

/***************************************

	Compatibility function for old versions of windows that do not
	implement TrackMouseEvent()

	\note This is not a full implementation, only the functionality
	that is needed for Burgerlib
	
***************************************/

static BOOL WINAPI Win95TrackMouseEvent(TRACKMOUSEEVENT *pEventTrack)
{
	BOOL uResult = FALSE;
	if (pEventTrack->dwFlags == TME_LEAVE) {
		// Install the timer
		uResult = SetTimer(pEventTrack->hwndTrack,pEventTrack->dwFlags,100,TrackMouseTimerProc) != 0;
	}
	return uResult;
}

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
	MemoryClear(m_bFunctionsTested,sizeof(m_bFunctionsTested));

	// Finally, release all of the allocated DLLs
	WordPtr i = 0;
	do {
		if (m_hInstances[i]) {
			FreeLibrary(m_hInstances[i]);
			m_hInstances[i] = NULL;
		}
		m_bInstancesTested[i] = FALSE;
	} while (++i<DLL_COUNT);
}

/*! ************************************

	\fn Burger::Globals::GetInstance()
	\brief Get the application instance

	\windowsonly

	\return Instance set by Globals::SetInstance()

	\sa Globals::SetInstance()
	
***************************************/

/*! ************************************

	\fn Burger::Globals::SetInstance(HINSTANCE__ *)
	\brief Set the application instance

	Upon application startup, an instance is assigned, use
	this function to allow Burgerlib to use this instance in
	other parts of the library.

	\windowsonly

	\param pInput Instance of the application

	\sa Globals::GetInstance()
	
***************************************/

/*! ************************************

	\fn Burger::Globals::GetWindow()
	\brief Get the application window
	
	\windowsonly

	\return Window set by Globals::SetWindow()

	\sa Globals::SetWindow()
	
***************************************/

/*! ************************************

	\fn Burger::Globals::SetWindow(HWND__ *)
	\brief Set the application window

	A window is created on application startup, it's pointer
	is stored via this call so other parts of Burgerlib
	can use this window for other systems.

	\windowsonly
	\param pInput Instance of the application

	\sa Globals::GetWindow()
	
***************************************/

/*! ************************************

	\brief Detect and load DirectInput functions

	\windowsonly
	\return \ref TRUE if DirectInput is present on the system, \ref FALSE if not
	\sa IsDirectInput8Present(void)

***************************************/

Word BURGER_API Burger::Globals::IsDirectInputPresent(void)
{
	Word uResult = FALSE;
	if (LoadLibraryIndex(DINPUT_DLL)) {
		uResult = TRUE;
	}
	return uResult;
}

/*! ************************************

	\brief Detect and load DirectInput8 functions

	\windowsonly
	\return \ref TRUE if DirectInput8 is present on the system, \ref FALSE if not
	\sa IsDirectInputPresent(void)

***************************************/

Word BURGER_API Burger::Globals::IsDirectInput8Present(void)
{
	Word uResult = FALSE;
	if (LoadLibraryIndex(DINPUT8_DLL)) {
		uResult = TRUE;		// The code is good!
	}
	return uResult;			// TRUE if ok!
}

/*! ************************************

	\brief Detect and load DirectDraw functions

	\windowsonly
	\return \ref TRUE if DirectDraw is present on the system, \ref FALSE if not
	\sa IsD3D9Present(void)

***************************************/

Word BURGER_API Burger::Globals::IsDirectDrawPresent(void)
{
	Word uResult = FALSE;
	if (LoadLibraryIndex(DDRAW_DLL)) {
		uResult = TRUE;		// The code is good!
	}
	return uResult;			// TRUE if ok!
}

/*! ************************************

	\brief Detect and load D3D9 functions

	\windowsonly
	\return \ref TRUE if D3D9 is present on the system, \ref FALSE if not
	\sa IsDirectDrawPresent(void)

***************************************/

Word BURGER_API Burger::Globals::IsD3D9Present(void)
{
	Word uResult = FALSE;
	if (LoadLibraryIndex(D3D9_DLL)) {
		uResult = TRUE;		// The code is good!
	}
	return uResult;			// TRUE if ok!
}

/*! ************************************

	\brief Detect and load DirectSound functions

	\windowsonly
	\return \ref TRUE if DirectSound is present on the system, \ref FALSE if not
	
***************************************/

Word BURGER_API Burger::Globals::IsDirectSoundPresent(void)
{
	Word uResult = FALSE;
	if (LoadLibraryIndex(DSOUND_DLL)) {
		uResult = TRUE;		// The code is good!
	}
	return uResult;			// TRUE if ok!
}

/*! ************************************

	\brief Detect if running on an ancient version of windows

	Test if the system is a pre-NT Windows operating system.
	If it returns \ref FALSE, it's an NT kernel (XP, Vista, 7, 8 ...)

	\windowsonly
	\return Returns \ref TRUE if Windows 3.1, 95, or 98. 
	\sa IsWinXPOrGreater(void) or IsVistaOrGreater(void)

***************************************/

Word BURGER_API Burger::Globals::IsWin95orWin98(void)
{
	Word bResult = g_bWindows95;	// Get the value
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
		g_bWindows95 = static_cast<Word8>(bResult);
	}
	// Return the value minus the other flags
	return bResult&1U;
}

/*! ************************************

	\brief Detect if running Windows XP or higher

	Test if the system is a Windows XP operating system or greater
	If it returns \ref FALSE, it's before Windows XP

	\windowsonly
	\return Returns \ref TRUE if Windows XP or greater
	\sa IsWin95orWin98(void) or IsVistaOrGreater(void)

***************************************/

Word BURGER_API Burger::Globals::IsWinXPOrGreater(void)
{
	Word bResult = g_bWindowsXP;	// Get the tested value
	// Was it already tested?
	if (!(bResult&0x80)) {
		OSVERSIONINFOW Version;
		MemoryClear(&Version,sizeof(Version));
		Version.dwOSVersionInfoSize = sizeof(Version);
		if (GetVersionExW(&Version) &&
			(Version.dwPlatformId==VER_PLATFORM_WIN32_NT) &&
			(Version.dwMajorVersion >= 5) && 
			(Version.dwMinorVersion >= 1)) {
			bResult = 0x80|TRUE;
		} else {
			bResult = 0x80|FALSE;
		}
		g_bWindowsXP = static_cast<Word8>(bResult);
	}
	// Return the value minus the other flags
	return bResult&1U;
}

/*! ************************************

	\brief Detect if running Windows Vista or higher

	Test if the system is a Windows Vista operating system or greater
	If it returns \ref FALSE, it's before Windows Vista (Usually XP)

	\windowsonly
	\return Returns \ref TRUE if Windows Vista or greater
	\sa IsWin95orWin98(void) or IsWinXPOrGreater(void)

***************************************/

Word BURGER_API Burger::Globals::IsVistaOrGreater(void)
{
	Word bResult = g_bWindowsVista;	// Get the tested value
	// Was it already tested?
	if (!(bResult&0x80)) {
		OSVERSIONINFOW Version;
		MemoryClear(&Version,sizeof(Version));
		Version.dwOSVersionInfoSize = sizeof(Version);
		if (GetVersionExW(&Version) &&
			(Version.dwPlatformId==VER_PLATFORM_WIN32_NT) &&
			(Version.dwMajorVersion >= 6)) {
			bResult = 0x80|TRUE;
		} else {
			bResult = 0x80|FALSE;
		}
		g_bWindowsVista = static_cast<Word8>(bResult);
	}
	// Return the value minus the other flags
	return bResult&1U;
}

#if defined(BURGER_WIN32) || defined(DOXYGEN)

/*! ************************************

	\brief Detect if a 32 bit windows app is running under 64 bit windows

	If an application is 32 bits, the operating system is tested if it's
	actually 64 bits. If so, the function will return \ref TRUE. If
	the application is 64 bits, or if the operating system is 32 bit
	then this function will return \ref FALSE.

	\windowsonly
	\return Returns \ref TRUE if the 32 bit application was running in 64 bit Windows
	\sa GetSystemWow64DirectoryW(Word16 *,Word)

***************************************/

Word BURGER_API Burger::Globals::IsWindows64Bit(void)
{
	Word bResult = g_bIsWindows64Bit;
	// Was it already tested?
	if (!(bResult&0x80)) {
		Word16 Temp[MAX_PATH];
		if ((GetSystemWow64DirectoryW(Temp,BURGER_ARRAYSIZE(Temp)) == 0) &&
			(GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)) {
			bResult = 0x80|FALSE;		// Not present? This is a 32 bit version of Windows
		} else {
			bResult = 0x80|TRUE;		// The 32 bit app is running in a 64 bit version of Windows
		}
		g_bIsWindows64Bit = static_cast<Word8>(bResult);
	}
	// Return the value minus the other flags
	return bResult&1U;
}

#endif

/*! ************************************

	\brief Return the singleton IDirectInputW pointer used by Burgerlib

	To allow multiple classes to use IDirectInputW without having to
	check if it was already created, use this function to create a
	global instance of IDirectInputW that will be released only
	on program exit.

	If DirectInput8 is required, use GetDirectInput8Singleton() instead.

	\windowsonly
	\param uVersion Version of DirectInput requested. Usually this is 0x700 (Highest number before DirectInput8 is invoked)
	\return Pointer to the global IDirectInputW pointer or \ref NULL if DirectInput couldn't be started.
	
***************************************/

IDirectInputW * BURGER_API Burger::Globals::GetDirectInputSingleton(Word uVersion)
{
	// Was there already a DirectInput instance?
	IDirectInputW *pDirectInputW = g_Globals.m_pDirectInputW;
	if (!pDirectInputW) {
		// Get DirectInput
		DirectInputCreateW(g_hInstance,uVersion,&g_Globals.m_pDirectInputW);
		// Get the updated pointer (Or NULL)
		pDirectInputW = g_Globals.m_pDirectInputW;
	}
	return pDirectInputW;
}

/*! ************************************

	\brief Return the singleton IDirectInput8W pointer used by Burgerlib

	To allow multiple classes to use IDirectInput8W without having to
	check if it was already created, use this function to create a
	global instance of IDirectInput8W that will be released only
	on program exit.

	If DirectInput is required, use GetDirectInputSingleton() instead.

	\windowsonly
	\return Pointer to the global IDirectInput8W pointer or \ref NULL if DirectInput8 couldn't be started.
	
***************************************/

IDirectInput8W * BURGER_API Burger::Globals::GetDirectInput8Singleton(void)
{
	// Was there already a DirectInput instance?
	IDirectInput8W *pDirectInput8W = g_Globals.m_pDirectInput8W;
	if (!pDirectInput8W) {
		// Get DirectInput
		DirectInput8Create(g_hInstance,0x800,IID_IDirectInput8W,reinterpret_cast<void **>(&g_Globals.m_pDirectInput8W));
		// Get the updated pointer (Or NULL)
		pDirectInput8W = g_Globals.m_pDirectInput8W;
	}
	return pDirectInput8W;
}





/***************************************

	The following functions are shim functions that
	will manually load in their appropriate DLL and then
	execute the function. This way, some DLLs that may
	or may not be present won't cause the application
	from failing to load due to a missing DLL
	
***************************************/

//
// dinput.dll
//

/*! ************************************

	\brief Load in dinput.dll and call DirectInputCreateA

	To allow maximum compatibility, this function will manually load
	dinput.dll and then invoke DirectInputCreateA if present.

	\windowsonly
	\param hInst Instance of the application
	\param uVersion Requested version of DirectInput
	\param ppOutput Pointer to the location where the valid IDirectInputA pointer
		will be stored. If the function fails, this value is guaranteed to be \ref NULL.
	\param pOuter Pointer to the IUnknown passed to DirectInputCreateA()
	\return Zero if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectInputCreateA(HINSTANCE__ *hInst,Word32 uVersion,IDirectInputA **ppOutput,IUnknown *pOuter)
{
	if (ppOutput) {
		ppOutput[0] = NULL;		// Clear in case of error
	}
	// Get the function pointer
	void *pDirectInputCreateA = LoadFunctionIndex(CALL_DirectInputCreateA);
	HRESULT uResult = DIERR_NOTFOUND;
	if (pDirectInputCreateA) {
		uResult = static_cast<HRESULT (WINAPI *)(HINSTANCE,DWORD,LPDIRECTINPUTA *,LPUNKNOWN)>(pDirectInputCreateA)(hInst,uVersion,ppOutput,pOuter);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in dinput.dll and call DirectInputCreateW

	To allow maximum compatibility, this function will manually load
	dinput.dll and then invoke DirectInputCreateA if present.

	\windowsonly
	\param hInst Instance of the application
	\param uVersion Requested version of DirectInput
	\param ppOutput Pointer to the location where the valid IDirectInputA pointer
		will be stored. If the function fails, this value is guaranteed to be \ref NULL.
	\param pOuter Pointer to the IUnknown passed to DirectInputCreateW()
	\return Zero if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectInputCreateW(HINSTANCE__ *hInst,Word32 uVersion,IDirectInputW **ppOutput,IUnknown *pOuter)
{
	if (ppOutput) {
		ppOutput[0] = NULL;		// Clear in case of error
	}
	// Get the function pointer
	void *pDirectInputCreateW = LoadFunctionIndex(CALL_DirectInputCreateW);
	HRESULT uResult = DIERR_NOTFOUND;
	if (pDirectInputCreateW) {
		uResult = static_cast<HRESULT (WINAPI *)(HINSTANCE,DWORD,LPDIRECTINPUTW *,LPUNKNOWN)>(pDirectInputCreateW)(hInst,uVersion,ppOutput,pOuter);
	}
	return static_cast<Word>(uResult);
}


//
// dinput8.dll
//

/*! ************************************

	\brief Load in dinput8.dll and call DirectInput8Create

	To allow maximum compatibility, this function will manually load
	dinput8.dll and then invoke DirectInput8Create if present.

	\windowsonly
	\param hInst Instance of the application
	\param uVersion Requested version of DirectInput8
	\param rGUID Reference to the GUID of the DirectInput8 object to create
	\param ppOutput Pointer to the location where the valid IDirectInput8 pointer
		will be stored. If the function fails, this value is guaranteed to be \ref NULL.
	\param pOuter Pointer to the IUnknown passed to DirectInput8Create()

	\return Zero if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectInput8Create(HINSTANCE__ *hInst,Word32 uVersion,const GUID &rGUID,void **ppOutput,IUnknown *pOuter)
{
	if (ppOutput) {
		ppOutput[0] = NULL;		// Clear in case of error
	}
	// Get the function pointer
	void *pDirectInput8Create = LoadFunctionIndex(CALL_DirectInput8Create);
	HRESULT uResult = DIERR_NOTFOUND;
	if (pDirectInput8Create) {
		uResult = static_cast<HRESULT(WINAPI *)(HINSTANCE,DWORD,REFIID,LPVOID *,LPUNKNOWN)>(pDirectInput8Create)(hInst,uVersion,rGUID,ppOutput,pOuter);
	}
	return static_cast<Word>(uResult);
}





//
// ddraw.dll
//

/*! ************************************

	\brief Load in ddraw.dll and call DirectDrawCreate

	To allow maximum compatibility, this function will manually load
	ddraw.dll and then invoke DirectDrawCreate if present.

	https://msdn.microsoft.com/en-us/library/windows/desktop/gg426116(v=vs.85).aspx

	\windowsonly
	\param pGuid Guid of the version of DirectDraw requested
	\param ppOutput Pointer to the location where the valid IDirectDraw pointer
		will be stored. If the function fails, this value is guaranteed to be \ref NULL.
	\param pOuter Pointer to the IUnknown passed to DirectDrawCreate()
	\return Zero if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectDrawCreate(const GUID *pGuid,IDirectDraw **ppOutput,IUnknown *pOuter)
{
	if (ppOutput) {
		ppOutput[0] = NULL;		// Clear in case of error
	}
	// Get the function pointer
	void *pDirectDrawCreate = LoadFunctionIndex(CALL_DirectDrawCreate);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pDirectDrawCreate) {
		uResult = static_cast<HRESULT(WINAPI *)(GUID *,LPDIRECTDRAW *,IUnknown *)>(pDirectDrawCreate)(const_cast<GUID *>(pGuid),ppOutput,pOuter);
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
		will be stored. If the function fails, this value is guaranteed to be \ref NULL.
	\param rGUID Reference to the GUID of the DirectInput8 object to create
	\param pOuter Pointer to the IUnknown passed to DirectDrawCreateEx()

	\return Zero if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectDrawCreateEx(const GUID *pGuid,void **ppOutput,const GUID &rGUID,IUnknown *pOuter)
{
	if (ppOutput) {
		ppOutput[0] = NULL;		// Clear in case of error
	}
	// Get the function pointer
	void *pDirectDrawCreateEx = LoadFunctionIndex(CALL_DirectDrawCreateEx);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pDirectDrawCreateEx) {
		uResult = static_cast<HRESULT(WINAPI *)(GUID *,LPVOID *,REFIID,IUnknown *)>(pDirectDrawCreateEx)(const_cast<GUID *>(pGuid),ppOutput,rGUID,pOuter);
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
	\param ppOutput Pointer to the location where the valid IDirectDrawClipper pointer
		will be stored. If the function fails, this value is guaranteed to be \ref NULL.
	\param pOuter Pointer to the IUnknown passed to DirectDrawCreateClipper()

	\return Zero if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectDrawCreateClipper(Word32 uFlags,IDirectDrawClipper **ppOutput,IUnknown *pOuter)
{
	if (ppOutput) {
		ppOutput[0] = NULL;		// Clear in case of error
	}
	// Get the function pointer
	void *pDirectDrawCreateClipper = LoadFunctionIndex(CALL_DirectDrawCreateClipper);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pDirectDrawCreateClipper) {
		uResult = static_cast<HRESULT(WINAPI *)(DWORD,LPDIRECTDRAWCLIPPER *,IUnknown *)>(pDirectDrawCreateClipper)(uFlags,ppOutput,pOuter);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in ddraw.dll and call DirectDrawEnumerateA

	To allow maximum compatibility, this function will manually load
	ddraw.dll and then invoke DirectDrawEnumerateA if present.

	https://msdn.microsoft.com/en-us/library/windows/desktop/gg426119(v=vs.85).aspx

	\windowsonly
	\param pCallback Address of a DDENUMCALLBACKA function to be called with a description of each enumerated DirectDraw-enabled hardware abstraction layer (HAL).
	\param pContext Address of an application-defined value to be passed to the enumeration callback function each time that it is called.
	\return DD_OK if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectDrawEnumerateA(void *pCallback,void *pContext)
{
	// Get the function pointer
	void *pDirectDrawEnumerateA = LoadFunctionIndex(CALL_DirectDrawEnumerateA);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pDirectDrawEnumerateA) {
		uResult = static_cast<HRESULT(WINAPI *)(LPDDENUMCALLBACKA,LPVOID)>(pDirectDrawEnumerateA)(static_cast<LPDDENUMCALLBACKA>(pCallback),pContext);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in ddraw.dll and call DirectDrawEnumerateW

	To allow maximum compatibility, this function will manually load
	ddraw.dll and then invoke DirectDrawEnumerateW if present.

	https://msdn.microsoft.com/en-us/library/windows/desktop/gg426119(v=vs.85).aspx

	\windowsonly
	\param pCallback Address of a DDENUMCALLBACKW function to be called with a description of each enumerated DirectDraw-enabled hardware abstraction layer (HAL).
	\param pContext Address of an application-defined value to be passed to the enumeration callback function each time that it is called.
	\return DD_OK if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectDrawEnumerateW(void *pCallback,void *pContext)
{
	// Get the function pointer
	void *pDirectDrawEnumerateW = LoadFunctionIndex(CALL_DirectDrawEnumerateW);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pDirectDrawEnumerateW) {
		uResult = static_cast<HRESULT(WINAPI *)(LPDDENUMCALLBACKW,LPVOID)>(pDirectDrawEnumerateW)(static_cast<LPDDENUMCALLBACKW>(pCallback),pContext);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in ddraw.dll and call DirectDrawEnumerateExA

	To allow maximum compatibility, this function will manually load
	ddraw.dll and then invoke DirectDrawEnumerateExA if present.

	http://msdn.microsoft.com/en-us/library/windows/desktop/gg426120(v=vs.85).aspx

	\windowsonly
	\param pCallback Address of a DDENUMCALLBACKEXA function to be called with a description of each enumerated DirectDraw-enabled hardware abstraction layer (HAL).
	\param pContext Address of an application-defined value to be passed to the enumeration callback function each time that it is called.
	\param uFlags Flags that specify the enumeration scope. This parameter can be 0 or a combination of the following flags. If the value is 0, the function enumerates only the primary display device.
	\return DD_OK if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectDrawEnumerateExA(void *pCallback,void *pContext,Word32 uFlags)
{
	// Get the function pointer
	void *pDirectDrawEnumerateExA = LoadFunctionIndex(CALL_DirectDrawEnumerateExA);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pDirectDrawEnumerateExA) {
		uResult = static_cast<HRESULT(WINAPI *)(LPDDENUMCALLBACKEXA,LPVOID,DWORD)>(pDirectDrawEnumerateExA)(static_cast<LPDDENUMCALLBACKEXA>(pCallback),pContext,uFlags);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in ddraw.dll and call DirectDrawEnumerateExW

	To allow maximum compatibility, this function will manually load
	ddraw.dll and then invoke DirectDrawEnumerateExA if present.

	http://msdn.microsoft.com/en-us/library/windows/desktop/gg426120(v=vs.85).aspx

	\note Some video cards do not support this function call. Notably the nVidia GT 545

	\windowsonly
	\param pCallback Address of a DDENUMCALLBACKEXW function to be called with a description of each enumerated DirectDraw-enabled hardware abstraction layer (HAL).
	\param pContext Address of an application-defined value to be passed to the enumeration callback function each time that it is called.
	\param uFlags Flags that specify the enumeration scope. This parameter can be 0 or a combination of the following flags. If the value is 0, the function enumerates only the primary display device.
	\return DD_OK if no error. Any other value means an error occurred

***************************************/

Word BURGER_API Burger::Globals::DirectDrawEnumerateExW(void *pCallback,void *pContext,Word32 uFlags)
{
	// Get the function pointer
	void *pDirectDrawEnumerateExW = LoadFunctionIndex(CALL_DirectDrawEnumerateExW);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pDirectDrawEnumerateExW) {
		uResult = static_cast<HRESULT(WINAPI *)(LPDDENUMCALLBACKEXW,LPVOID,DWORD)>(pDirectDrawEnumerateExW)(static_cast<LPDDENUMCALLBACKEXW>(pCallback),pContext,uFlags);
	}
	return static_cast<Word>(uResult);
}




//
// d3d9.dll
//

/*! ************************************

	\brief Load in d3d9.dll and call Direct3DCreate9

	To allow maximum compatibility, this function will manually load
	d3d9.dll and then invoke Direct3DCreate9 if present.

	\windowsonly
	\param uSDKVersion Requested version of Direct3D 9
	\return \ref NULL if DirectX 9 is not present. A valid Direct3D9 pointer otherwise
	
***************************************/

IDirect3D9 *BURGER_API Burger::Globals::Direct3DCreate9(Word uSDKVersion)
{
	// Get the function pointer
	void *pDirect3DCreate9 = LoadFunctionIndex(CALL_Direct3DCreate9);
	IDirect3D9 *pDirect3D9 = NULL;
	if (pDirect3DCreate9) {
		pDirect3D9 = static_cast<IDirect3D9 *(WINAPI *)(UINT)>(pDirect3DCreate9)(uSDKVersion);
	}
	return pDirect3D9;
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

Word BURGER_API Burger::Globals::D3DXCreateMatrixStack(Word uFlags,ID3DXMatrixStack **ppStack)
{
	// Get the function pointer
	void *pD3DXCreateMatrixStack = LoadFunctionIndex(CALL_D3DXCreateMatrixStack);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pD3DXCreateMatrixStack) {
		uResult = static_cast<HRESULT(WINAPI *)(DWORD,ID3DXMatrixStack**)>(pD3DXCreateMatrixStack)(uFlags,ppStack);
	}
	return static_cast<Word>(uResult);
}




//
// dsound.dll
//

/*! ************************************

	\brief Load in dsound.dll and call DirectSoundCreate

	To allow maximum compatibility, this function will manually load
	dsound.dll and then invoke DirectSoundCreate if present.

	\windowsonly
	\param pGuidDevice Requested audio device
	\param ppOutput Pointer to receive the pointer to the new IDirectSound instance
	\param pOuter Pointer to the IUnknown passed to DirectSoundCreate()
	\return DD_OK if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectSoundCreate(const GUID *pGuidDevice,IDirectSound **ppOutput,IUnknown *pOuter)
{
	if (ppOutput) {
		ppOutput[0] = NULL;		// Clear in case of error
	}
	// Get the function pointer
	HRESULT uResult = DSERR_INVALIDCALL;
	void *pDirectSoundCreate = LoadFunctionIndex(CALL_DirectSoundCreate);
	if (pDirectSoundCreate) {
		uResult = static_cast<HRESULT(WINAPI *)(LPCGUID,LPDIRECTSOUND *,LPUNKNOWN)>(pDirectSoundCreate)(pGuidDevice,ppOutput,pOuter);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in dsound.dll and call DirectSoundEnumerateA

	To allow maximum compatibility, this function will manually load
	dsound.dll and then invoke DirectSoundEnumerateA if present.

	https://msdn.microsoft.com/en-us/library/microsoft.directx_sdk.reference.directsoundenumerate(v=vs.85).aspx

	\windowsonly
	\param pDSEnumCallback Address of a DSENUMCALLBACKA function to be called with a description of each enumerated DirectSound-enabled hardware abstraction layer (HAL).
	\param pContext Address of an application-defined value to be passed to the enumeration callback function each time that it is called.
	\return DS_OK if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectSoundEnumerateA(void *pDSEnumCallback,void *pContext)
{
	// Get the function pointer
	void *pDirectSoundEnumerateA = LoadFunctionIndex(CALL_DirectSoundEnumerateA);
	HRESULT uResult = DSERR_NOINTERFACE;
	if (pDirectSoundEnumerateA) {
		uResult = static_cast<HRESULT(WINAPI *)(LPDSENUMCALLBACKA,LPVOID)>(pDirectSoundEnumerateA)(static_cast<LPDSENUMCALLBACKA>(pDSEnumCallback),pContext);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in dsound.dll and call DirectSoundEnumerateW

	To allow maximum compatibility, this function will manually load
	dsound.dll and then invoke DirectSoundEnumerateW if present.

	https://msdn.microsoft.com/en-us/library/microsoft.directx_sdk.reference.directsoundenumerate(v=vs.85).aspx

	\windowsonly
	\param pDSEnumCallback Address of a DSENUMCALLBACKW function to be called with a description of each enumerated DirectSound-enabled hardware abstraction layer (HAL).
	\param pContext Address of an application-defined value to be passed to the enumeration callback function each time that it is called.
	\return DS_OK if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectSoundEnumerateW(void *pDSEnumCallback,void *pContext)
{
	// Get the function pointer
	void *pDirectSoundEnumerateW = LoadFunctionIndex(CALL_DirectSoundEnumerateW);
	HRESULT uResult = DSERR_NOINTERFACE;
	if (pDirectSoundEnumerateW) {
		uResult = static_cast<HRESULT(WINAPI *)(LPDSENUMCALLBACKW,LPVOID)>(pDirectSoundEnumerateW)(static_cast<LPDSENUMCALLBACKW>(pDSEnumCallback),pContext);
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
	\param ppOutput Pointer to receive the pointer to the new IDirectSoundCapture instance
	\param pOuter Pointer to the IUnknown passed to DirectSoundCaptureCreate()
	\return DD_OK if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectSoundCaptureCreate(const GUID *pGuidDevice,IDirectSoundCapture **ppOutput,IUnknown *pOuter)
{
	if (ppOutput) {
		ppOutput[0] = NULL;		// Clear in case of error
	}
	// Get the function pointer
	HRESULT uResult = DSERR_INVALIDCALL;
	void *pDirectSoundCaptureCreate = LoadFunctionIndex(CALL_DirectSoundCaptureCreate);
	if (pDirectSoundCaptureCreate) {
		uResult = static_cast<HRESULT(WINAPI *)(LPCGUID,LPDIRECTSOUNDCAPTURE *,LPUNKNOWN)>(pDirectSoundCaptureCreate)(pGuidDevice,ppOutput,pOuter);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in dsound.dll and call DirectSoundCaptureEnumerateA

	To allow maximum compatibility, this function will manually load
	dsound.dll and then invoke DirectSoundCaptureEnumerateA if present.

	https://msdn.microsoft.com/en-us/library/vs/alm/microsoft.directx_sdk.reference.directsoundcaptureenumerate(v=vs.85).aspx

	\windowsonly
	\param pDSEnumCallback Address of a DSENUMCALLBACKA function to be called with a description of each enumerated DirectSound-enabled hardware abstraction layer (HAL).
	\param pContext Address of an application-defined value to be passed to the enumeration callback function each time that it is called.
	\return DS_OK if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectSoundCaptureEnumerateA(void *pDSEnumCallback,void *pContext)
{
	// Get the function pointer
	void *pDirectSoundCaptureEnumerateA = LoadFunctionIndex(CALL_DirectSoundCaptureEnumerateA);
	HRESULT uResult = DSERR_NOINTERFACE;
	if (pDirectSoundCaptureEnumerateA) {
		uResult = static_cast<HRESULT(WINAPI *)(LPDSENUMCALLBACKA,LPVOID)>(pDirectSoundCaptureEnumerateA)(static_cast<LPDSENUMCALLBACKA>(pDSEnumCallback),pContext);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in dsound.dll and call DirectSoundCaptureEnumerateW

	To allow maximum compatibility, this function will manually load
	dsound.dll and then invoke DirectSoundCaptureEnumerateW if present.

	https://msdn.microsoft.com/en-us/library/vs/alm/microsoft.directx_sdk.reference.directsoundcaptureenumerate(v=vs.85).aspx

	\windowsonly
	\param pDSEnumCallback Address of a DSENUMCALLBACKW function to be called with a description of each enumerated DirectSound-enabled hardware abstraction layer (HAL).
	\param pContext Address of an application-defined value to be passed to the enumeration callback function each time that it is called.
	\return DS_OK if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectSoundCaptureEnumerateW(void *pDSEnumCallback,void *pContext)
{
	// Get the function pointer
	void *pDirectSoundCaptureEnumerateW = LoadFunctionIndex(CALL_DirectSoundCaptureEnumerateW);
	HRESULT uResult = DSERR_NOINTERFACE;
	if (pDirectSoundCaptureEnumerateW) {
		uResult = static_cast<HRESULT(WINAPI *)(LPDSENUMCALLBACKW,LPVOID)>(pDirectSoundCaptureEnumerateW)(static_cast<LPDSENUMCALLBACKW>(pDSEnumCallback),pContext);
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
	\param ppOutput Pointer to receive the pointer to the new IDirectSound8 instance
	\param pOuter Pointer to the IUnknown passed to DirectSoundCreate8()
	\return DD_OK if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectSoundCreate8(const GUID *pGuidDevice,IDirectSound8 **ppOutput,IUnknown *pOuter)
{
	if (ppOutput) {
		ppOutput[0] = NULL;		// Clear in case of error
	}
	// Get the function pointer
	void *pDirectSoundCreate8 = LoadFunctionIndex(CALL_DirectSoundCreate8);
	HRESULT uResult = DSERR_INVALIDCALL;
	if (pDirectSoundCreate8) {
		uResult = static_cast<HRESULT (WINAPI *)(LPCGUID,IDirectSound8 **,LPUNKNOWN)>(pDirectSoundCreate8)(pGuidDevice,ppOutput,pOuter);
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
	\param ppOutput Pointer to receive the pointer to the new IDirectSoundCapture8 instance
	\param pOuter Pointer to the IUnknown passed to DirectSoundCaptureCreate8()
	\return DD_OK if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectSoundCaptureCreate8(const GUID *pGuidDevice,IDirectSoundCapture **ppOutput,IUnknown *pOuter)
{
	if (ppOutput) {
		ppOutput[0] = NULL;		// Clear in case of error
	}
	// Get the function pointer
	void *pDirectSoundCaptureCreate8 = LoadFunctionIndex(CALL_DirectSoundCaptureCreate8);
	HRESULT uResult = DSERR_INVALIDCALL;
	if (pDirectSoundCaptureCreate8) {
		uResult = static_cast<HRESULT (WINAPI *)(LPCGUID,LPDIRECTSOUNDCAPTURE8 *,LPUNKNOWN)>(pDirectSoundCaptureCreate8)(pGuidDevice,ppOutput,pOuter);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in dsound.dll and call DirectSoundFullDuplexCreate

	To allow maximum compatibility, this function will manually load
	dsound.dll and then invoke DirectSoundFullDuplexCreate if present.
	
	https://msdn.microsoft.com/en-us/library/vs/alm/microsoft.directx_sdk.reference.directsoundfullduplexcreate8(v=vs.85).aspx

	\windowsonly
	\param pGuidCaptureDevice Address of the GUID that identifies the sound capture device
	\param pGuidRenderDevice Address of the GUID that identifies the sound render device for full-duplex output
	\param pDSCBufferDesc Address of a DSCBUFFERDESC structure that specifies the characteristics of the capture buffer
	\param pDSBufferDesc Address of a DSCBUFFERDESC structure that specifies the characteristics of the render buffer
	\param hWnd Handle to the application window
	\param uLevel Cooperative level for the device
	\param ppDSFD Pointer to receive the pointer to the new IDirectSoundFullDuplex instance
	\param ppDSCBuffer8 Pointer to receive the pointer to the new IDirectSoundCaptureBuffer8 instance
	\param ppDSBuffer8 Pointer to receive the pointer to the new IDirectSoundBuffer8 instance
	\param pOuter Pointer to the IUnknown passed to DirectSoundFullDuplexCreate()
	
	\return DD_OK if no error. Any other value means an error occurred

***************************************/

Word BURGER_API Burger::Globals::DirectSoundFullDuplexCreate(const GUID *pGuidCaptureDevice,const GUID *pGuidRenderDevice,
	const _DSCBUFFERDESC *pDSCBufferDesc,const _DSBUFFERDESC *pDSBufferDesc,HWND__ *hWnd,Word32 uLevel,
	IDirectSoundFullDuplex **ppDSFD,IDirectSoundCaptureBuffer8 **ppDSCBuffer8,IDirectSoundBuffer8 **ppDSBuffer8,IUnknown *pOuter)
{
	if (ppDSFD) {
		ppDSFD[0] = NULL;		// Clear in case of error
	}
	if (ppDSCBuffer8) {
		ppDSCBuffer8[0] = NULL;
	}
	if (ppDSBuffer8) {
		ppDSBuffer8[0] = NULL;
	}
	// Get the function pointer
	void *pDirectSoundFullDuplexCreate = LoadFunctionIndex(CALL_DirectSoundFullDuplexCreate);
	HRESULT uResult = DSERR_INVALIDCALL;
	if (pDirectSoundFullDuplexCreate) {
		uResult = static_cast<HRESULT (WINAPI *)(LPCGUID,LPCGUID,LPCDSCBUFFERDESC,LPCDSBUFFERDESC,HWND,DWORD,
			LPDIRECTSOUNDFULLDUPLEX*,LPDIRECTSOUNDCAPTUREBUFFER8 *,LPDIRECTSOUNDBUFFER8 *,LPUNKNOWN)>(pDirectSoundFullDuplexCreate)(pGuidCaptureDevice,pGuidRenderDevice,
			pDSCBufferDesc,pDSBufferDesc,hWnd,uLevel,ppDSFD,ppDSCBuffer8,ppDSBuffer8,pOuter);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in dsound.dll and call GetDeviceID

	To allow maximum compatibility, this function will manually load
	dsound.dll and then invoke GetDeviceID if present.

	https://msdn.microsoft.com/en-us/library/vs/alm/microsoft.directx_sdk.reference.getdeviceid(v=vs.85).aspx

	\windowsonly
	\param pGuidSrc Address of a variable that specifies a valid device identifier
	\param pGuidDest Address of a variable that receives the unique identifier of the device
	\return DD_OK if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::GetDeviceID(const GUID *pGuidSrc,GUID *pGuidDest)
{
	// Get the function pointer
	void *pGetDeviceID = LoadFunctionIndex(CALL_GetDeviceID);
	HRESULT uResult = DSERR_INVALIDCALL;
	if (pGetDeviceID) {
		uResult = static_cast<HRESULT (WINAPI *)(LPCGUID,LPGUID)>(pGetDeviceID)(pGuidSrc,pGuidDest);
	}
	return static_cast<Word>(uResult);
}




//
// rpcrt4.dll
//

/*! ************************************

	\brief Load in rpcrt4.dll and call UuidCreateSequential
	
	Manually load rpcrt4.dll if needed and
	call the Windows function UuidCreateSequential()

	http://msdn.microsoft.com/en-us/library/windows/desktop/aa379322(v=vs.85).aspx
	
	\windowsonly
	\param pOutput A pointer to an uninitialized GUID.
	\return Zero for success or List of error codes http://msdn.microsoft.com/en-us/library/windows/desktop/aa378645(v=vs.85).aspx
		
***************************************/

Word BURGER_API Burger::Globals::UuidCreateSequential(GUID *pOutput)
{
	void *pUuidCreateSequential = LoadFunctionIndex(CALL_UuidCreateSequential);
	RPC_STATUS uResult = RPC_S_CALL_FAILED;
	if (pUuidCreateSequential) {
		uResult = static_cast<RPC_STATUS (RPC_ENTRY *)(GUID *)>(pUuidCreateSequential)(pOutput);
	}
	return static_cast<Word>(uResult);
}



//
// winmm.dll
//

/*! ************************************

	\brief Load in winmm.dll and call timeGetTime
	
	Manually load winmm.dll if needed and
	call the Windows function timeGetTime()

	http://msdn.microsoft.com/en-us/library/dd757629(v=vs.85).aspx
	
	\return Time in milliseconds
		
***************************************/

Word32 BURGER_API Burger::Globals::timeGetTime(void)
{
	void *ptimeGetTime = LoadFunctionIndex(CALL_timeGetTime);
	Word uResult = 0;
	if (ptimeGetTime) {
		uResult = static_cast<DWORD (WINAPI *)(void)>(ptimeGetTime)();
	}
	return uResult;
}





//
// shlwapi.dll
//

/*! ************************************

	\brief Load in shlwapi.dll and call PathSearchAndQualifyA
	
	Manually load shlwapi.dll if needed and
	call the Windows function PathSearchAndQualifyA()

	http://msdn.microsoft.com/en-us/library/windows/desktop/bb773751(v=vs.85).aspx
	
	\windowsonly
	\param pszPath A pointer to a null-terminated string of maximum length MAX_PATH that contains the path to search.
	\param pszBuf A pointer to a null-terminated string of length MAX_PATH that contains the path to be referenced.
	\param cchBuf The size of the buffer pointed to by pszBuf, in characters.
	\return Returns \ref TRUE if the path is qualified, or \ref FALSE otherwise.
		
***************************************/

Word BURGER_API Burger::Globals::PathSearchAndQualifyA(const char *pszPath,char *pszBuf,Word32 cchBuf)
{
	void *pPathSearchAndQualifyA = LoadFunctionIndex(CALL_PathSearchAndQualifyA);
	BOOL uResult = FALSE;
	if (pPathSearchAndQualifyA) {
		uResult = static_cast<BOOL (WINAPI *)(LPCSTR,LPSTR,UINT)>(pPathSearchAndQualifyA)(pszPath,pszBuf,cchBuf);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in shlwapi.dll and call PathSearchAndQualifyW
	
	Manually load shlwapi.dll if needed and
	call the Windows function PathSearchAndQualifyW()

	http://msdn.microsoft.com/en-us/library/windows/desktop/bb773751(v=vs.85).aspx
	
	\windowsonly
	\param pszPath A pointer to a null-terminated string of maximum length MAX_PATH that contains the path to search.
	\param pszBuf A pointer to a null-terminated string of length MAX_PATH that contains the path to be referenced.
	\param cchBuf The size of the buffer pointed to by pszBuf, in characters.
	\return Returns \ref TRUE if the path is qualified, or \ref FALSE otherwise.
		
***************************************/

Word BURGER_API Burger::Globals::PathSearchAndQualifyW(const Word16 *pszPath,Word16 *pszBuf,Word32 cchBuf)
{
	void *pPathSearchAndQualifyW = LoadFunctionIndex(CALL_PathSearchAndQualifyW);
	BOOL uResult = FALSE;
	if (pPathSearchAndQualifyW) {
		uResult = static_cast<BOOL (WINAPI *)(LPCWSTR,LPWSTR,UINT)>(pPathSearchAndQualifyW)(reinterpret_cast<LPCWSTR>(pszPath),reinterpret_cast<LPWSTR>(pszBuf),cchBuf);
	}
	return static_cast<Word>(uResult);
}


//
// version.dll
//

/*! ************************************

	\brief Load in version.dll and call VerQueryValueA
	
	Manually load version.dll if needed and
	call the Windows function VerQueryValueA()

	http://msdn.microsoft.com/en-us/library/windows/desktop/ms647464(v=vs.85).aspx
	
	\windowsonly
	\param pBlock The version-information resource returned by the GetFileVersionInfo function.
	\param pSubBlock A pointer to the version-information value to be retrieved. 
	\param ppBuffer A pointer that contains the address of a pointer to the requested version information in the buffer pointed to by pBlock.
	\param pLen The size of the buffer pointed to by lplpBuffer, in bytes.
	\return Returns \ref TRUE if successful, or \ref FALSE otherwise.
		
***************************************/

Word BURGER_API Burger::Globals::VerQueryValueA(const void *pBlock,const char *pSubBlock,void **ppBuffer,Word *pLen)
{
	void *pVerQueryValueA = LoadFunctionIndex(CALL_VerQueryValueA);
	BOOL uResult = FALSE;		// Failure
	if (pVerQueryValueA) {
		uResult = static_cast<BOOL (WINAPI *)(LPCVOID,LPCSTR,LPVOID *,PUINT)>(pVerQueryValueA)(pBlock,pSubBlock,ppBuffer,pLen);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in version.dll and call VerQueryValueW
	
	Manually load version.dll if needed and
	call the Windows function VerQueryValueW()

	http://msdn.microsoft.com/en-us/library/windows/desktop/ms647464(v=vs.85).aspx
	
	\windowsonly
	\param pBlock The version-information resource returned by the GetFileVersionInfo function.
	\param pSubBlock A pointer to the version-information value to be retrieved. 
	\param ppBuffer A pointer that contains the address of a pointer to the requested version information in the buffer pointed to by pBlock.
	\param pLen The size of the buffer pointed to by lplpBuffer, in bytes.
	\return Returns \ref TRUE if successful, or \ref FALSE otherwise.
		
***************************************/

Word BURGER_API Burger::Globals::VerQueryValueW(const void *pBlock,const Word16 *pSubBlock,void **ppBuffer,Word *pLen)
{
	void *pVerQueryValueW = LoadFunctionIndex(CALL_VerQueryValueW);
	BOOL uResult = FALSE;		// Failure
	if (pVerQueryValueW) {
		uResult = static_cast<BOOL (WINAPI *)(LPCVOID,LPCWSTR,LPVOID *,PUINT)>(pVerQueryValueW)(pBlock,reinterpret_cast<LPCWSTR>(pSubBlock),ppBuffer,pLen);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in version.dll and call GetFileVersionInfoA
	
	Manually load version.dll if needed and
	call the Windows function GetFileVersionInfoA()

	http://msdn.microsoft.com/en-us/library/windows/desktop/ms647003(v=vs.85).aspx
	
	\windowsonly
	\param ptstrFilename The name of the file
	\param dwHandle This parameter is ignored.
	\param dwLen The size, in bytes, of the buffer pointed to by the lpData parameter.
	\param pData Pointer to a buffer that receives the file-version information.
	\return Returns \ref TRUE if successful, or \ref FALSE otherwise.
		
***************************************/

Word BURGER_API Burger::Globals::GetFileVersionInfoA(const char *ptstrFilename,Word32 dwHandle,Word32 dwLen,void *pData)
{
	void *pGetFileVersionInfoA = LoadFunctionIndex(CALL_GetFileVersionInfoA);
	BOOL uResult = FALSE;		// Failure
	if (pGetFileVersionInfoA) {
		uResult = static_cast<BOOL (WINAPI *)(LPCSTR,DWORD,DWORD,LPVOID)>(pGetFileVersionInfoA)(ptstrFilename,dwHandle,dwLen,pData);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in version.dll and call GetFileVersionInfoW
	
	Manually load version.dll if needed and
	call the Windows function GetFileVersionInfoW()

	http://msdn.microsoft.com/en-us/library/windows/desktop/ms647003(v=vs.85).aspx
	
	\windowsonly
	\param ptstrFilename The name of the file
	\param dwHandle This parameter is ignored.
	\param dwLen The size, in bytes, of the buffer pointed to by the lpData parameter.
	\param pData Pointer to a buffer that receives the file-version information.
	\return Returns \ref TRUE if successful, or \ref FALSE otherwise.
		
***************************************/

Word BURGER_API Burger::Globals::GetFileVersionInfoW(const Word16 *ptstrFilename,Word32 dwHandle,Word32 dwLen,void *pData)
{
	void *pGetFileVersionInfoW = LoadFunctionIndex(CALL_GetFileVersionInfoW);
	BOOL uResult = FALSE;		// Failure
	if (pGetFileVersionInfoW) {
		uResult = static_cast<BOOL (WINAPI *)(LPCWSTR,DWORD,DWORD,LPVOID)>(pGetFileVersionInfoW)(reinterpret_cast<LPCWSTR>(ptstrFilename),dwHandle,dwLen,pData);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in version.dll and call GetFileVersionInfoA
	
	Manually load version.dll if needed and
	call the Windows function GetFileVersionInfoA()

	http://msdn.microsoft.com/en-us/library/windows/desktop/ms647005(v=vs.85).aspx
	
	\windowsonly
	\param ptstrFilename The name of the file of interest.
	\param pdwHandle A pointer to a variable that the function sets to zero.
	\return Returns the number of bytes if successful, or zero otherwise.
		
***************************************/

Word32 BURGER_API Burger::Globals::GetFileVersionInfoSizeA(const char *ptstrFilename,unsigned long *pdwHandle)
{
	void *pGetFileVersionInfoSizeA = LoadFunctionIndex(CALL_GetFileVersionInfoSizeA);
	Word uResult = 0;		// Failure
	if (pGetFileVersionInfoSizeA) {
		uResult = static_cast<DWORD (WINAPI *)(LPCSTR,LPDWORD)>(pGetFileVersionInfoSizeA)(ptstrFilename,pdwHandle);
	}
	return uResult;
}

/*! ************************************

	\brief Load in version.dll and call GetFileVersionInfoSizeW
	
	Manually load version.dll if needed and
	call the Windows function GetFileVersionInfoSizeW()

	http://msdn.microsoft.com/en-us/library/windows/desktop/ms647005(v=vs.85).aspx
	
	\windowsonly
	\param ptstrFilename The name of the file of interest.
	\param pdwHandle A pointer to a variable that the function sets to zero.
	\return Returns the number of bytes if successful, or zero otherwise.
		
***************************************/

Word32 BURGER_API Burger::Globals::GetFileVersionInfoSizeW(const Word16 *ptstrFilename,unsigned long *pdwHandle)
{
	void *pGetFileVersionInfoSizeW = LoadFunctionIndex(CALL_GetFileVersionInfoSizeW);
	Word uResult = 0;		// Failure
	if (pGetFileVersionInfoSizeW) {
		uResult = static_cast<DWORD (WINAPI *)(LPCWSTR,LPDWORD)>(pGetFileVersionInfoSizeW)(reinterpret_cast<LPCWSTR>(ptstrFilename),pdwHandle);
	}
	return uResult;
}




//
// hid.dll
//

/*! ************************************

	\brief Load in hid.dll and call HidD_GetHidGuid
	
	Manually load hid.dll if needed and
	call the Windows function HidD_GetHidGuid()

	http://msdn.microsoft.com/en-us/library/windows/hardware/ff538924(v=vs.85).aspx
	
	\windowsonly
	\param pHidGuid Pointer to a caller-allocated GUID buffer that the routine uses to return the device interface GUID for HIDClass devices.
		
***************************************/

void BURGER_API Burger::Globals::HidD_GetHidGuid(GUID *pHidGuid)
{
	void *pHidD_GetHidGuid = LoadFunctionIndex(CALL_HidD_GetHidGuid);
	if (!pHidD_GetHidGuid) {
		MemoryClear(pHidGuid,sizeof(GUID));
	} else {
		static_cast<void (WINAPI *)(GUID *)>(pHidD_GetHidGuid)(pHidGuid);
	}
}



//
// setupapi.dll
//

/*! ************************************

	\brief Load in setupapi.dll and call SetupDiGetClassDevsA
	
	Manually load Setupapi.dll if needed and
	call the Windows function SetupDiGetClassDevsA()

	http://msdn.microsoft.com/en-us/library/windows/hardware/ff551069(v=vs.85).aspx
	
	\windowsonly
	\param ClassGuid A pointer to the GUID for a device setup class or a device interface class. 
	\param Enumerator A pointer to a NULL-terminated string that specifies a PnP enumerator or instance ID
	\param hwndParent A handle to the top-level window to be used for a user interface that is associated with installing a device instance in the device information set.
	\param Flags A variable of type DWORD that specifies control options that filter the device information elements that are added to the device information set.
	\return If the operation succeeds, SetupDiGetClassDevs returns a handle to a device information set that contains all installed devices that matched the supplied parameters.

***************************************/

void * BURGER_API Burger::Globals::SetupDiGetClassDevsA(const GUID *ClassGuid,const char *Enumerator,HWND__ *hwndParent,Word32 Flags)
{
	void *pSetupDiGetClassDevsA = LoadFunctionIndex(CALL_SetupDiGetClassDevsA);
	void *pResult = INVALID_HANDLE_VALUE;		// Failure
	if (pSetupDiGetClassDevsA) {
		pResult = static_cast<HDEVINFO (WINAPI *)(const GUID *,PCSTR,HWND,DWORD)>(pSetupDiGetClassDevsA)(ClassGuid,Enumerator,hwndParent,Flags);
	}
	return pResult;
}

/*! ************************************

	\brief Load in setupapi.dll and call SetupDiGetClassDevsW
	
	Manually load Setupapi.dll if needed and
	call the Windows function SetupDiGetClassDevsW()

	http://msdn.microsoft.com/en-us/library/windows/hardware/ff551069(v=vs.85).aspx
	
	\windowsonly
	\param ClassGuid A pointer to the GUID for a device setup class or a device interface class. 
	\param Enumerator A pointer to a NULL-terminated string that specifies a PnP enumerator or instance ID
	\param hwndParent A handle to the top-level window to be used for a user interface that is associated with installing a device instance in the device information set.
	\param Flags A variable of type DWORD that specifies control options that filter the device information elements that are added to the device information set.
	\return If the operation succeeds, SetupDiGetClassDevs returns a handle to a device information set that contains all installed devices that matched the supplied parameters.

***************************************/

void * BURGER_API Burger::Globals::SetupDiGetClassDevsW(const GUID *ClassGuid,const Word16 *Enumerator,HWND__ *hwndParent,Word32 Flags)
{
	void *pSetupDiGetClassDevsW = LoadFunctionIndex(CALL_SetupDiGetClassDevsW);
	void *pResult = INVALID_HANDLE_VALUE;		// Failure
	if (pSetupDiGetClassDevsW) {
		pResult = static_cast<HDEVINFO (WINAPI *)(const GUID *,PCWSTR,HWND,DWORD)>(pSetupDiGetClassDevsW)(ClassGuid,reinterpret_cast<PCWSTR>(Enumerator),hwndParent,Flags);
	}
	return pResult;
}

/*! ************************************

	\brief Load in setupapi.dll and call SetupDiGetDeviceInterfaceDetailA
	
	Manually load Setupapi.dll if needed and
	call the Windows function SetupDiGetDeviceInterfaceDetailA()

	http://msdn.microsoft.com/en-us/library/windows/hardware/ff551120(v=vs.85).aspx
	
	\windowsonly
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
	void *pSetupDiGetDeviceInterfaceDetailA = LoadFunctionIndex(CALL_SetupDiGetDeviceInterfaceDetailA);
	BOOL uResult = FALSE;		// Failure
	if (pSetupDiGetDeviceInterfaceDetailA) {
		uResult = static_cast<BOOL (WINAPI *)(HDEVINFO,PSP_DEVICE_INTERFACE_DATA,PSP_DEVICE_INTERFACE_DETAIL_DATA_A,DWORD,PDWORD,PSP_DEVINFO_DATA)>(pSetupDiGetDeviceInterfaceDetailA)(DeviceInfoSet,DeviceInterfaceData,DeviceInterfaceDetailData,DeviceInterfaceDetailDataSize,reinterpret_cast<DWORD *>(RequiredSize),DeviceInfoData);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in setupapi.dll and call SetupDiGetDeviceInterfaceDetailW
	
	Manually load Setupapi.dll if needed and
	call the Windows function SetupDiGetDeviceInterfaceDetailW()

	http://msdn.microsoft.com/en-us/library/windows/hardware/ff551120(v=vs.85).aspx
	
	\windowsonly
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
	void *pSetupDiGetDeviceInterfaceDetailW = LoadFunctionIndex(CALL_SetupDiGetDeviceInterfaceDetailW);
	BOOL uResult = FALSE;		// Failure
	if (pSetupDiGetDeviceInterfaceDetailW) {
		uResult = static_cast<BOOL (WINAPI *)(HDEVINFO,PSP_DEVICE_INTERFACE_DATA,PSP_DEVICE_INTERFACE_DETAIL_DATA_W,DWORD,PDWORD,PSP_DEVINFO_DATA)>(pSetupDiGetDeviceInterfaceDetailW)(DeviceInfoSet,DeviceInterfaceData,DeviceInterfaceDetailData,DeviceInterfaceDetailDataSize,reinterpret_cast<DWORD *>(RequiredSize),DeviceInfoData);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in setupapi.dll and call SetupDiEnumDeviceInterfaces

	Manually load Setupapi.dll if needed and
	call the Windows function SetupDiEnumDeviceInterfaces()

	http://msdn.microsoft.com/en-us/library/windows/hardware/ff551015(v=vs.85).aspx
	
	\windowsonly
	\param DeviceInfoSet A pointer to a device information set that contains the device interfaces for which to return information. 
	\param DeviceInfoData A pointer to an SP_DEVINFO_DATA structure that specifies a device information element in DeviceInfoSet.
	\param InterfaceClassGuid A pointer to a GUID that specifies the device interface class for the requested interface.
	\param MemberIndex A zero-based index into the list of interfaces in the device information set. 
	\param DeviceInterfaceData A pointer to a caller-allocated buffer that contains, on successful return, a completed SP_DEVICE_INTERFACE_DATA structure that identifies an interface that meets the search parameters.
	\return \ref TRUE if the function completed without error.

***************************************/

Word BURGER_API Burger::Globals::SetupDiEnumDeviceInterfaces(void *DeviceInfoSet,_SP_DEVINFO_DATA *DeviceInfoData,const GUID *InterfaceClassGuid,Word32 MemberIndex,_SP_DEVICE_INTERFACE_DATA *DeviceInterfaceData)
{
	void *pSetupDiEnumDeviceInterfaces = LoadFunctionIndex(CALL_SetupDiEnumDeviceInterfaces);
	BOOL uResult = FALSE;		// Failure
	if (pSetupDiEnumDeviceInterfaces) {
		uResult = static_cast<BOOL (WINAPI *)(HDEVINFO,PSP_DEVINFO_DATA,const GUID *,DWORD,PSP_DEVICE_INTERFACE_DATA)>(pSetupDiEnumDeviceInterfaces)(DeviceInfoSet,DeviceInfoData,InterfaceClassGuid,MemberIndex,DeviceInterfaceData);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in setupapi.dll and call SetupDiDestroyDeviceInfoList

	Manually load Setupapi.dll if needed and
	call the Windows function SetupDiDestroyDeviceInfoList()

	http://msdn.microsoft.com/en-us/library/windows/hardware/ff550996(v=vs.85).aspx
	
	\windowsonly
	\param DeviceInfoSet A handle to the device information set to delete.
	\return \ref TRUE if the function completed without error.

***************************************/

Word BURGER_API Burger::Globals::SetupDiDestroyDeviceInfoList(void *DeviceInfoSet)
{
	void *pSetupDiDestroyDeviceInfoList = LoadFunctionIndex(CALL_SetupDiDestroyDeviceInfoList);
	BOOL uResult = FALSE;		// Failure
	if (pSetupDiDestroyDeviceInfoList) {
		uResult = static_cast<BOOL (WINAPI *)(HDEVINFO)>(pSetupDiDestroyDeviceInfoList)(DeviceInfoSet);
	}
	return static_cast<Word>(uResult);
}





//
// user32.dll
//

/*! ************************************

	\brief Load in user32.dll and call TrackMouseEvent

	Manually load user32.dll if needed and
	call the Windows function TrackMouseEvent()

	On versions of windows that do not have TrackMouseEvent(), use a
	compatibility function that performs the same task

	https://msdn.microsoft.com/en-us/library/windows/desktop/ms646265(v=vs.85).aspx
	
	\windowsonly
	\param pEventTrack A pointer to a TRACKMOUSEEVENT structure that contains tracking information.
	\return If the function succeeds, the return value is nonzero.

***************************************/

Word BURGER_API Burger::Globals::TrackMouseEvent(::tagTRACKMOUSEEVENT *pEventTrack)
{
	void *pTrackMouseEvent = LoadFunctionIndex(CALL_TrackMouseEvent);
	// No?
	if (!pTrackMouseEvent) {
		// Use a compatibility function
		pTrackMouseEvent = Win95TrackMouseEvent;
		g_Globals.m_pWindowsCalls[CALL_TrackMouseEvent] = pTrackMouseEvent;
	}
	return static_cast<Word>(static_cast<BOOL (WINAPI *)(LPTRACKMOUSEEVENT)>(pTrackMouseEvent)(pEventTrack));
}




//
// kernel32.dll
//

/*! ************************************

	\brief Load in kernel32.dll and call GetSystemWow64DirectoryA

	Manually load kernel32.dll if needed and
	call the Windows function GetSystemWow64DirectoryA()

	https://msdn.microsoft.com/en-us/library/windows/desktop/ms724405(v=vs.85).aspx
	
	\windowsonly
	\param pBuffer A pointer to the buffer to receive the path
	\param uSize The maximum size of the buffer, in bytes
	\return If the function succeeds, the return value is the length, in bytes, of the string copied to the buffer

***************************************/

Word BURGER_API Burger::Globals::GetSystemWow64DirectoryA(char *pBuffer,Word32 uSize)
{
	void *pGetSystemWow64DirectoryA = LoadFunctionIndex(CALL_GetSystemWow64DirectoryA);
	Word uResult = 0;		// Failure
	if (!pGetSystemWow64DirectoryA) {
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	} else {
		uResult = static_cast<UINT (WINAPI *)(LPSTR,UINT)>(pGetSystemWow64DirectoryA)(pBuffer,uSize);
	}
	return uResult;
}

/*! ************************************

	\brief Load in kernel32.dll and call GetSystemWow64DirectoryW

	Manually load kernel32.dll if needed and
	call the Windows function GetSystemWow64DirectoryW()

	https://msdn.microsoft.com/en-us/library/windows/desktop/ms724405(v=vs.85).aspx
	
	\windowsonly
	\param pBuffer A pointer to the buffer to receive the path
	\param uSize The maximum size of the buffer, in Word16 entries
	\return If the function succeeds, the return value is the length, in Word16 entries, of the string copied to the buffer

***************************************/

Word BURGER_API Burger::Globals::GetSystemWow64DirectoryW(Word16 *pBuffer,Word32 uSize)
{
	void *pGetSystemWow64DirectoryW = LoadFunctionIndex(CALL_GetSystemWow64DirectoryW);
	Word uResult = 0;		// Failure
	if (!pGetSystemWow64DirectoryW) {
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	} else {
		uResult = static_cast<UINT (WINAPI *)(LPWSTR,UINT)>(pGetSystemWow64DirectoryW)(reinterpret_cast<LPWSTR>(pBuffer),uSize);
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

	if (!g_bQuickTimeVersionValid) {
		g_bQuickTimeVersionValid = TRUE;		// I got the version
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
				// Try the location of Quicktime 4.0 and 5.0
				pQuery = "\\StringFileInfo\\040904B0\\FileVersion";
				// Try Quicktime 4.0
				uZeroLong = 0;
				StringCopy(PathName+uPathLength,sizeof(PathName)-uPathLength,"\\QuickTime.qts");
				uFileInfoSize = GetFileVersionInfoSizeA(PathName,&uZeroLong);
				if (!uFileInfoSize) {
					// Try the location of Quicktime 6.0 and later
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
		g_uQuickTimeVersion = uResult;
	}
	// Return the QuickTime version
	return g_uQuickTimeVersion;
}

/*! ************************************

	\brief Returns a 64 bit version of a file.
	
	Given a filename in Windows Unicode format, open the file and return
	the 64 bit Windows extended version number from the dwFileVersionMS
	and dwFileVersionLS entries in the VS_FIXEDFILEINFO structure.
	
	\return Version in 64 bit Windows format or 0 on error
		
***************************************/

Word64 BURGER_API Burger::Globals::GetFileVersion64(const Word16* pWindowsFilename)
{
	Word64 uResult = 0;
	if (pWindowsFilename) {
		DWORD uNotUsed;
		// Get the size of the data
		UINT uBufferSize = GetFileVersionInfoSizeW(pWindowsFilename,&uNotUsed);
		if (uBufferSize) {
			BYTE* pFileVersionBuffer = static_cast<BYTE *>(Alloc(uBufferSize));
			if (pFileVersionBuffer) {
				// Load the data
				if (GetFileVersionInfoW(pWindowsFilename,0,uBufferSize,pFileVersionBuffer)) {
					VS_FIXEDFILEINFO* pVersion = NULL;
					// Extract the version value
					if (VerQueryValueW(pFileVersionBuffer,reinterpret_cast<const Word16 *>(L"\\"),(VOID**)&pVersion,&uBufferSize)) {
						if (pVersion != NULL) {
							uResult = (static_cast<Word64>(pVersion->dwFileVersionMS)<<32U)+pVersion->dwFileVersionLS;
						}
					}
				}
				// Release the buffer
				Free(pFileVersionBuffer);
			}
		}
	}
	return uResult;
}

/*! ************************************

	\brief Return the version of DirectX by scanning file versions

	Tries to get the DirectX version by looking at DirectX file versions
	Uses http://en.wikipedia.org/wiki/DirectX as a reference for file version lookups
	\return Version number of DirectX in 16 bit format 0x900 is 9.0, 0x903 = 9.0c

***************************************/

Word BURGER_API Burger::Globals::GetDirectXVersionViaFileVersions(void)
{
	Word16 szPath[MAX_PATH*2];

	Word uResult = 0;
	if (GetSystemDirectoryW(reinterpret_cast<LPWSTR>(szPath),MAX_PATH) != 0) {
	
		szPath[MAX_PATH-1] = 0;		// Failsafe

		// Switch off the ddraw version
		WordPtr uLen = StringLength(szPath);
		Word16 *pDest = szPath+uLen;
		WordPtr uRemaining = sizeof(szPath)-(uLen*sizeof(Word16));

		StringCopy(pDest,uRemaining,reinterpret_cast<const Word16 *>(L"\\ddraw.dll"));

		Word64 uVersionDDraw = GetFileVersion64(szPath);
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
		StringCopy(pDest,uRemaining,reinterpret_cast<const Word16 *>(L"\\d3drg8x.dll"));
		Word64 uVersionD3Drg8x = GetFileVersion64(szPath);
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
		StringCopy(pDest,uRemaining,reinterpret_cast<const Word16 *>(L"\\dplayx.dll"));
		Word64 uVersionDPlayx = GetFileVersion64(szPath);
		if (uVersionDPlayx>=0x0004000600030206ULL) {	// Win9x version
			// ddraw.dll is the DX6.1 version, so we must be at least DX6.1a
			uResult = 0x0611;
		}

		// Switch off the ddraw version
		if (uVersionDDraw>=0x00040007000002BC) {	// Win9x version
			// ddraw.dll is the DX7.0 version, so we must be at least DX7.0
			uResult = 0x0700;
		}

		// Switch off the dinput version
		StringCopy(pDest,uRemaining,reinterpret_cast<const Word16 *>(L"\\dinput.dll"));
		Word64 uVersionDInput = GetFileVersion64(szPath);
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
		StringCopy(pDest,uRemaining,reinterpret_cast<const Word16 *>(L"\\d3d8.dll"));
		Word64 uVersionD3D8 = GetFileVersion64(szPath);
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
		StringCopy(pDest,uRemaining,reinterpret_cast<const Word16 *>(L"\\mpg2splt.ax"));
		Word64 uVersionMPG2Splt = GetFileVersion64(szPath);
		if (uVersionMPG2Splt>=0x0006000300010375ULL) { // Win9x/Win2k/WinXP version
			// quartz.dll is the DX8.1b version, so we must be at least DX8.1b
			uResult = 0x0812;
		}

		// Switch off the dpnet version
		StringCopy(pDest,uRemaining,reinterpret_cast<const Word16 *>(L"\\dpnet.dll"));
		Word64 uVersionDPNet = GetFileVersion64(szPath);
		if ((((uVersionDPNet&0xFFFF000000000000ULL)==0x0004000000000000ULL) && (uVersionDPNet>=0x0004000900000086ULL)) || // Win9x version
			(((uVersionDPNet&0xFFFF000000000000ULL)==0x0005000000000000ULL) && (uVersionDPNet>=0x000500020E5D0086ULL))) { // Win2k/WinXP version
			// dpnet.dll is the DX8.2 version, so we must be at least DX8.2
			uResult = 0x0820;
		}

		// Switch off the d3d9 version
		StringCopy(pDest,uRemaining,reinterpret_cast<const Word16 *>(L"\\d3d9.dll"));
		Word64 uVersionD3D9 = GetFileVersion64(szPath);
		if (uVersionD3D9) {
			// File exists, but be at least DX9
			uResult = 0x0900;		// 9.0
		}
		if (uVersionD3D9>=0x0004000900000385ULL) {
			uResult = 0x0901;		// 9.0a
		}
		if (uVersionD3D9>=0x0004000900000386ULL) {
			uResult = 0x0902;		// 9.0b
		}
		if (uVersionD3D9>=0x0004000900000387ULL) {
			uResult = 0x0903;		// 9.0c
		}
		if (uVersionD3D9>=0x0006000017704002ULL) {		// 6.00.6000.16386
			uResult = 0x0A00;		// 10.0
		}
		if (uVersionD3D9>=0x0006000017714650ULL) {		// 6.00.6001.18000
			uResult = 0x0A10;		// 10.1
		}
		if (uVersionD3D9>=0x00060000177246BBULL) {		// 6.00.6002.18107
			uResult = 0x0B00;		// 11.0
		}
		if (uVersionD3D9>=0x0006000223F04000ULL) {		// 6.02.9200.16384
			uResult = 0x0B10;		// 11.1
		}
		if (uVersionD3D9>=0x0006000225804000ULL) {		// 6.02.9600.16384
			uResult = 0x0B20;		// 11.2
		}
//		if (uVersionD3D9>=0x000A000028004000ULL) {		// 10.00.10240.16384 (Not found yet)
//			uResult = 0x0C00;		// 12.0
//		}
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

	\windowsonly
	\return Version in the format of 0x0902 -> 9.2, 0xB01 = 11.0.1

***************************************/

Word BURGER_API Burger::Globals::GetDirectXVersion(void)
{
	if (!g_bDirectXVersionValid) {
		g_bDirectXVersionValid = TRUE;	// I got the version
		Word uResult = GetDirectXVersionViaFileVersions();
		g_uDirectXVersion = uResult;
	}
	// Return the DirectX version
	return g_uDirectXVersion;
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

static int CALLBACK FindDeviceCallback(GUID *pGUID,LPSTR /* pName */,LPSTR /* pDeviceName */,void *pThis,void * /* pMonitor */)
{
	DeviceGuid_t *pRef = static_cast<DeviceGuid_t *>(pThis);		// Deref the pointer
	int iResult = DDENUMRET_OK;				// Keep going
	if (!--pRef->m_uDevNum) {				// Found the device yet?
		if (pGUID) {						// Specific device?
			Burger::MemoryCopy(pRef->m_pGUID,pGUID,sizeof(GUID));	// Copy the GUID
		} else {
			Burger::MemoryClear(pRef->m_pGUID,sizeof(GUID));
		}
		iResult = DDENUMRET_CANCEL;		// Stop now
	}
	return iResult;
}

/*! ************************************

	\brief Given a specific device number, return the DirectX GUID

	Scan the device list for the GUID of the requested device.
	Device #0 returns the global display device (All screens)

	\windowsonly
	\param pOutput Pointer to a buffer to accept the returned GUID. Cannot be \ref NULL
	\param uDevNum 0 for the master global device, 1-??? for the enumerated displays
	\return Zero if no error, non-zero if an error has occurred

***************************************/

Word BURGER_API Burger::Globals::GetVideoGUID(GUID *pOutput,Word uDevNum)
{
	HRESULT uError = E_FAIL;
	if (pOutput) {
		DeviceGuid_t Ref;
		Ref.m_uDevNum = ++uDevNum;		// Scan for this device
		Ref.m_pGUID = pOutput;
		uError = static_cast<HRESULT>(DirectDrawEnumerateExW(FindDeviceCallback,&Ref,DDENUM_ATTACHEDSECONDARYDEVICES|
			DDENUM_DETACHEDSECONDARYDEVICES|DDENUM_NONDISPLAYDEVICES));
		// The nVidia GT 545 fails on this call, so call using the 8 Bit Ascii version instead
		if (uError == E_NOTIMPL) {
			Ref.m_uDevNum = uDevNum;	// Scan for this device
			uError = static_cast<HRESULT>(DirectDrawEnumerateExA(FindDeviceCallback,&Ref,DDENUM_ATTACHEDSECONDARYDEVICES|
				DDENUM_DETACHEDSECONDARYDEVICES|DDENUM_NONDISPLAYDEVICES));
		}
		if (uError==DD_OK) {
			if (Ref.m_uDevNum) {		// Got it?
				uError = E_FAIL;		// Force an error
			}
		}
	}
	return static_cast<Word>(uError);
}

/*! ************************************

	\brief Call LoadLibraryA() without file error boxes

	When LoadLibraryA() is called in windows, it's possible that
	if the file is not found, windows will display an error message box
	mentioning that a DLL is missing. This function will prohibit
	this behavior by setting the ErrorMode to SEM_NOOPENFILEERRORBOX
	before the call to LoadLibraryA() and restoring the
	flag to the previous setting before function exit.

	\windowsonly
	\param pInput ASCII pathname of the DLL file to load.
	\return \ref NULL if the DLL was not loaded, a valid HINSTANCE on success
	\sa LoadLibraryW()

***************************************/

HINSTANCE BURGER_API Burger::Globals::LoadLibraryA(const char *pInput)
{
	// Disable user interactive dialogs
	UINT uOldMode = SetErrorMode(SEM_NOOPENFILEERRORBOX|SEM_FAILCRITICALERRORS);
	HINSTANCE hResult = ::LoadLibraryA(pInput);
	// Restore the dialog state
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

	\windowsonly
	\param pInput UTF16 pathname of the DLL file to load. 
	\return \ref NULL if the DLL was not loaded, a valid HINSTANCE on success
	\sa LoadLibraryA()

***************************************/

HINSTANCE BURGER_API Burger::Globals::LoadLibraryW(const Word16 *pInput)
{
	UINT uOldMode = SetErrorMode(SEM_NOOPENFILEERRORBOX|SEM_FAILCRITICALERRORS);
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

	\windowsonly
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
			hResult = LoadLibraryA(s_LibaryNames[eIndex]);
			if (hResult) {
				// If it loaded fine, save the result
				g_Globals.m_hInstances[eIndex] = hResult;
			}
#if defined(_DEBUG)

			// Direct3D version 9, is a special case, if the debug
			// dll is not present, load the release version as a failsafe
			else {
				if (eIndex==D3D9_DLL) {
					hResult = LoadLibraryA("d3d9.dll");
					if (hResult) {
						// If it loaded fine, save the result
						g_Globals.m_hInstances[eIndex] = hResult;
					}

				}
			}
#endif
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

	\windowsonly
	\param eIndex \ref eWindowsCallIndex index to a Windows function Burgerlib is tracking.
	\return \ref NULL if the DLL was not loaded or the function didn't exist, a valid function pointer on success
	\sa LoadLibraryIndex()
	
***************************************/

void * BURGER_API Burger::Globals::LoadFunctionIndex(eWindowsCallIndex eIndex)
{
	void *pResult;
	// Valid index?
	if (eIndex>=CALL_COUNT) {
		pResult = NULL;
	} else {
		// Has the function been loaded?
		pResult = g_Globals.m_pWindowsCalls[eIndex];
		// Speed up in the form of disabling multiple tests
		if (!pResult && !g_Globals.m_bFunctionsTested[eIndex]) {
			// Mark as tested
			g_Globals.m_bFunctionsTested[eIndex] = TRUE;
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

	\windowsonly
	\return Returns TRUE if an error occurs

***************************************/

Word BURGER_API Burger::Globals::AddGroupToProgramMenu(const char *pGroupName)
{
	// Get the pidl for the start menu
	// this will be used to initialize the folder browser

	Word uResult = 10;
	ITEMIDLIST *pIDListStartMenu;		// Item list for the start menu
	if (SHGetSpecialFolderLocation(GetWindow(),CSIDL_PROGRAMS,&pIDListStartMenu)==NOERROR) {
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

	\windowsonly
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
	
	\windowsonly

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
