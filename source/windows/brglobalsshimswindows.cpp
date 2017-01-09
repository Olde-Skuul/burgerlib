/***************************************

	Windows Shim functions

	Functions that will allow calling Windows Vista/7/8/10 functions
	from a program that can be loaded on Windows XP.

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
#include "brwindowstypes.h"

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

#if !defined(BUILD_WINDOWS)
#define BUILD_WINDOWS
#endif

#include <Windows.h>
#include <MMReg.h>
#include <dinput.h>
#include <d3d.h>
#include <ddraw.h>
#include <dsound.h>
#include <dplay.h>
#include <dplay8.h>
#include <dplobby.h>
#include <dplobby8.h>
#include <shellapi.h>
#include <shlobj.h>
#include <SetupAPI.h>
#include <d3dcommon.h>
#include <Xinput.h>
#include <xaudio2.h>
#include <io.h>

//
// These defines may not be defined in earlier Windows SDKs
// To allow the use of these advanced features, they are manually
// defined here if they hadn't already been done so.
//

#if !defined(STATUS_ENTRYPOINT_NOT_FOUND)
#define STATUS_ENTRYPOINT_NOT_FOUND ((DWORD)0xC0000139L)
#endif

#if !defined(LOAD_LIBRARY_SEARCH_SYSTEM32)
#define LOAD_LIBRARY_SEARCH_SYSTEM32 0x800
#endif

/***************************************

	DLLs that can be dynamically loaded at runtime (So the 
	application can launch if they are missing or missing functions)
	
***************************************/

//
// These filenames MUST match eWindowsDLLIndex
//

static const char *s_LibaryNames[Burger::Globals::DLL_COUNT] = {
	"ddraw.dll",
	"dinput.dll",
	"dinput8.dll",
	"xinput1_4.dll",
	"xinput1_3.dll",
	"d3d9.dll",
	"d3dx9_43.dll",
	"d3d11.dll",
	"dxgi.dll",
	"dsound.dll",
	"dplayx.dll",
	"dplay.dll",
	"rpcrt4.dll",
	"winmm.dll",
	"shlwapi.dll",
	"version.dll",
	"hid.dll",
	"setupapi.dll",
	"user32.dll",
	"kernel32.dll",
	"shell32.dll"
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
	
	{Burger::Globals::XINPUT1_4_DLL,"XInputGetState"},
	{Burger::Globals::XINPUT1_4_DLL,"XInputSetState"},
	{Burger::Globals::XINPUT1_4_DLL,"XInputGetCapabilities"},
	{Burger::Globals::XINPUT1_4_DLL,"XInputGetDSoundAudioDeviceGuids"},
	{Burger::Globals::XINPUT1_4_DLL,"XInputEnable"},
	{Burger::Globals::XINPUT1_4_DLL,"XInputGetAudioDeviceIds"},
	{Burger::Globals::XINPUT1_4_DLL,"XInputGetBatteryInformation"},
	{Burger::Globals::XINPUT1_4_DLL,"XInputGetKeystroke"},

	{Burger::Globals::DDRAW_DLL,"DirectDrawCreate"},
	{Burger::Globals::DDRAW_DLL,"DirectDrawCreateEx"},
	{Burger::Globals::DDRAW_DLL,"DirectDrawCreateClipper"},
	{Burger::Globals::DDRAW_DLL,"DirectDrawEnumerateA"},
	{Burger::Globals::DDRAW_DLL,"DirectDrawEnumerateW"},
	{Burger::Globals::DDRAW_DLL,"DirectDrawEnumerateExA"},
	{Burger::Globals::DDRAW_DLL,"DirectDrawEnumerateExW"},

	{Burger::Globals::D3D9_DLL,"Direct3DCreate9"},
	{Burger::Globals::D3D9_DLL,"D3DPERF_BeginEvent"},
	{Burger::Globals::D3D9_DLL,"D3DPERF_EndEvent"},
	{Burger::Globals::D3D9_DLL,"D3DPERF_SetMarker"},
	{Burger::Globals::D3D9_DLL,"D3DPERF_SetRegion"},
	{Burger::Globals::D3D9_DLL,"D3DPERF_QueryRepeatFrame"},
	{Burger::Globals::D3D9_DLL,"D3DPERF_SetOptions"},
	{Burger::Globals::D3D9_DLL,"D3DPERF_GetStatus"},

	{Burger::Globals::D3DX9_43_DLL,"D3DXCreateMatrixStack"},

	{Burger::Globals::D3D11_DLL,"D3D11CreateDevice"},

	{Burger::Globals::DXGI_DLL,"CreateDXGIFactory"},
	{Burger::Globals::DXGI_DLL,"CreateDXGIFactory1"},
	{Burger::Globals::DXGI_DLL,"CreateDXGIFactory2"},

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
	
	{Burger::Globals::DPLAYX_DLL,"DirectPlayCreate"},
	{Burger::Globals::DPLAYX_DLL,"DirectPlayEnumerate"},
	{Burger::Globals::DPLAYX_DLL,"DirectPlayEnumerateA"},
	{Burger::Globals::DPLAYX_DLL,"DirectPlayEnumerateW"},
	{Burger::Globals::DPLAYX_DLL,"DirectPlayLobbyCreateA"},
	{Burger::Globals::DPLAYX_DLL,"DirectPlayLobbyCreateW"},

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
	{Burger::Globals::USER32_DLL,"GetMonitorInfoA"},
	{Burger::Globals::USER32_DLL,"GetMonitorInfoW"},
	{Burger::Globals::USER32_DLL,"MonitorFromWindow"},
	{Burger::Globals::USER32_DLL,"MonitorFromRect"},

	{Burger::Globals::KERNEL32_DLL,"GetSystemWow64DirectoryA"},
	{Burger::Globals::KERNEL32_DLL,"GetSystemWow64DirectoryW"},

	{Burger::Globals::SHELL32_DLL,"SHGetKnownFolderPath"}
};


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

	// Note: The only way the call to GetClientRect() to fail
	// is in the rare case where the application is in shutdown 
	// while this time function was active (I.E. The Window was
	// released before this timer was externally killed). To
	// handle that 1 in million case, test for GetClientRect()'s
	// success

	RECT TheClientRect;
	if (GetClientRect(pWindow,&TheClientRect)) {

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



/*! ************************************

	\brief Load a library if needed

	Given a DLL index, detect if the library has already been
	loaded and if so, return the existing HINSTANCE, otherwise,
	load the DLL and if successful, return the HINSTANCE. If
	the load failed, return \ref NULL.

	\note Since this function only loads in official Microsoft DLLs, it will
	under Vista or higher, use the function LoadLibraryExA(LOAD_LIBRARY_SEARCH_SYSTEM32)
	to ensure that only official DLL directories are scanned to find the files. 
	This was done to prevent Man-In-The-Middle attacks used by users who want to
	inject code to cheat. Windows versions before Vista (XP for example) doesn't
	support this flag, so the feature isn't used on those platforms.

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

			// For security reasons, force searching only in the official
			// windows folder to prevent a man-in-the-middle attack
			// Supported on Windows Vista or later. If running on XP, you're
			// out of luck.

			Word32 uFlags = 0;		// XP Flags
			if (IsVistaOrGreater()) {
				uFlags = LOAD_LIBRARY_SEARCH_SYSTEM32;
			}

			// Load the most recent version of the DLL
			hResult = LoadLibraryExA(s_LibaryNames[eIndex],NULL,uFlags);

			// Handle the special cases where if the most recent is not available, try
			// an older version of the dll
			if (!hResult) {
	
				eWindowsDLLIndex uAlternate = DLL_COUNT;

				// Try XInput 9.1.0
				if (eIndex==XINPUT1_4_DLL) {
					uAlternate = XINPUT1_3_DLL;

				// Try DirectPlay 1.0 (XP, and REALLY old)
				} else if (eIndex==DPLAYX_DLL) {
					uAlternate = DPLAY_DLL;
				}

				// Was an alternate chosen?
				if (uAlternate!=DLL_COUNT) {
					hResult = LoadLibraryExA(s_LibaryNames[uAlternate],NULL,uFlags);
				}

				// Here is where code should be added for future special casing of DLL searching.
			}

			// If it loaded fine, save the result
			if (hResult) {
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

	\windowsonly
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

	\fn Word Burger::Globals::IsDirectInputPresent(void)
	\brief Detect and load DirectInput functions

	\windowsonly
	\return \ref TRUE if DirectInput is present on the system, \ref FALSE if not
	\sa IsDirectInput8Present(void) or IsXInputPresent(void)

***************************************/

/*! ************************************

	\fn Word Burger::Globals::IsDirectInput8Present(void)
	\brief Detect and load DirectInput8 functions

	\windowsonly
	\return \ref TRUE if DirectInput8 is present on the system, \ref FALSE if not
	\sa IsDirectInputPresent(void) or IsXInputPresent(void)

***************************************/

/*! ************************************

	\fn Word Burger::Globals::IsXInputPresent(void)
	\brief Detect and load XInput functions

	\windowsonly
	\return \ref TRUE if XInput is present on the system, \ref FALSE if not
	\sa IsDirectInputPresent(void) or IsDirectInput8Present(void)

***************************************/

/*! ************************************

	\fn Word Burger::Globals::IsDirectDrawPresent(void)
	\brief Detect and load DirectDraw functions

	\windowsonly
	\return \ref TRUE if DirectDraw is present on the system, \ref FALSE if not
	\sa IsD3D9Present(void)

***************************************/

/*! ************************************

	\fn Word Burger::Globals::IsD3D9Present(void)
	\brief Detect and load D3D9 functions

	\windowsonly
	\return \ref TRUE if D3D9 is present on the system, \ref FALSE if not
	\sa IsDirectDrawPresent(void)

***************************************/

/*! ************************************

	\fn Word Burger::Globals::IsDirectSoundPresent(void)
	\brief Detect and load DirectSound functions

	\windowsonly
	\return \ref TRUE if DirectSound is present on the system, \ref FALSE if not
	
***************************************/

/*! ************************************

	\fn Word Burger::Globals::IsDirectPlayPresent(void)
	\brief Detect and load DirectPlay functions

	\windowsonly
	\return \ref TRUE if DirectPlay is present on the system, \ref FALSE if not

***************************************/



/*! ************************************

	\brief Find a Quicktime folder by reading the registry

	This function is a functional equivalent to the Quicktime for
	Windows function GetQTFolderFromRegistry(). It will
	query a registry value from the HKEY_LOCAL_MACHINE root
	and will return the value. 

	On success, the pathname will be in the buffer with a
	'\' or a '/' ending the string. Otherwise, the string
	will be empty on failure.

	\note Quicktime for Windows is a 32 bit API. Don't use it
	for 64 bit applications.

	\windowsonly
	\param pSubKey Pointer to registry sub key
	\param pValueName Pointer to the registry value name to a string
	\param pBuffer Pointer to a buffer to receive the string
	\param uSize Number of bytes in size of the string buffer

***************************************/

void BURGER_API Burger::Globals::GetQTFolderFromRegistry(const char *pSubKey,const char *pValueName,char *pBuffer,Word32 uSize)
{
	if (uSize) {
		pBuffer[0] = 0;
		if (uSize>=2) {
			// Clear out the output values
			HKEY pKey = NULL;

			// Open the key
			if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,pSubKey,0,KEY_QUERY_VALUE,&pKey)==ERROR_SUCCESS) {
				DWORD uLength = uSize-1;	// Space for the forced '\'

				// Read in the directory name and continue if there was data
				if ((RegQueryValueExA(pKey,pValueName,NULL,NULL,static_cast<BYTE *>(static_cast<void *>(pBuffer)),&uLength)==ERROR_SUCCESS) &&
					pBuffer[0]) {
					// Force the last character to a '\'
					EndWithWindowsSlashes(pBuffer);
				}
			}
			// Release the key, if loaded
			if (pKey) {
				RegCloseKey(pKey);
			}
		}
	}
}

/*! ************************************

	\brief Find the Quicktime folder

	This function is a functional equivalent to the Quicktime for
	Windows function GetPathToQuickTimeFolder(). It will
	query a registry values from the HKEY_LOCAL_MACHINE root
	and if found, it will return the path. Otherwise, it will check
	the system folder for the file Quicktime.qts.

	On success, the pathname will be in the buffer with a
	'\' or a '/' ending the string. Otherwise, the string
	will be empty on failure.

	\note Quicktime for Windows is a 32 bit API. Don't use it
	for 64 bit applications.

	\windowsonly
	\param pBuffer Pointer to a buffer to receive the string
	\param uSize Number of bytes in size of the string buffer
	\param pReserved Reserved, pass \ref NULL
	\return \ref TRUE if successful, \ref FALSE if not

***************************************/

Word BURGER_API Burger::Globals::GetPathToQuickTimeFolder(char *pBuffer,Word32 uSize,Word32 *pReserved)
{
	char Temp[1024];
	Word bResult = FALSE;
	// Only execute if there's a valid output buffer
	if (pBuffer && uSize) {

		// Erase the output buffer
		pBuffer[0] = 0;

		// Set the reserved value if there was a passed pointer
		if (pReserved) {
			pReserved[0] = 0;
		}

		if (uSize>=2) {

			// See if the module is already loaded
			HMODULE hQuickTime = GetModuleHandleA("QuickTime.qts");
			if (hQuickTime) {

				// Get the path from the loaded module
				if (GetModuleFileNameA(hQuickTime,Temp,BURGER_ARRAYSIZE(Temp))) {

					// Remove the string "Quicktime.qts"
					WordPtr uStrLength = StringLength(Temp);
					if (uStrLength>13) {
						// Copy up the string minus the ending "Quicktime.qts"
						// Note: This will end the string with a '\'
						StringCopy(pBuffer,uSize,Temp,uStrLength-13);
					}
				}

				// hQuicktime doesn't need to be released.
			}

			// No path yet?
			if (!pBuffer[0]) {

				// Try grabbing from the registry
				GetQTFolderFromRegistry("Software\\Apple Computer, Inc.\\QuickTime","QTSysDir",pBuffer,uSize);

				// Detect if the directory exists
				if (pBuffer[0] && 
					_access(pBuffer,0)) {
					// Failed
					pBuffer[0] = 0;
				}
			}

			// No path yet?
			if (!pBuffer[0]) {

				// Try again, but using another key
				GetQTFolderFromRegistry("Software\\Apple Computer, Inc.\\QuickTime","QuickTime.qts folder",pBuffer,uSize);

				// Detect if the directory exists
				if (pBuffer[0] && 
					_access(pBuffer,0)) {
					// Failed
					pBuffer[0] = 0;
				}
			}


			// No path yet?
			if (!pBuffer[0]) {

				// Try the windows directory
				if (!GetSystemDirectoryA(pBuffer,uSize) ||
					_access(pBuffer,0)) {
					pBuffer[0] = 0;
				}
			}

			// If something was found, ensure it ends with a slash

			if (pBuffer[0]) {
				EndWithWindowsSlashes(pBuffer);
			}
		}
		// Return TRUE if there was something in the buffer
		bResult = (pBuffer[0]!=0);
	}
	return bResult;
}

/*! ************************************

	\brief Find the Quicktime folder

	This function is a functional equivalent to the Quicktime for
	Windows function GetQTSystemDirectoryA(). It will
	query a registry values from the HKEY_LOCAL_MACHINE root
	and if found, it will return the path. Otherwise, it will check
	the system folder for the file Quicktime.qts.

	On success, the pathname will be in the buffer with a
	'\' or a '/' ending the string. Otherwise, the string
	will be empty on failure.

	\note Quicktime for Windows is a 32 bit API. Don't use it
	for 64 bit applications.

	\windowsonly
	\param pBuffer Pointer to a buffer to receive the string
	\param uSize Number of bytes in size of the string buffer
	\return Length of the returned string in chars.

***************************************/

Word32 BURGER_API Burger::Globals::GetQTSystemDirectoryA(char *pBuffer,Word32 uSize)
{
	GetPathToQuickTimeFolder(pBuffer,uSize,NULL);
	return static_cast<Word32>(StringLength(pBuffer));
}

/*! ************************************

	\brief Find the Quicktime application folder

	This function is a functional equivalent to the Quicktime for
	Windows function GetQTApplicationDirectoryA(). It will
	query a registry value from the HKEY_LOCAL_MACHINE root
	and if found, it will return the path.

	On success, the pathname will be in the buffer with a
	'\' or a '/' ending the string. Otherwise, the string
	will be empty on failure.

	\note Quicktime for Windows is a 32 bit API. Don't use it
	for 64 bit applications.

	\windowsonly
	\param pBuffer Pointer to a buffer to receive the string
	\param uSize Number of bytes in size of the string buffer
	\return Length of the returned string in chars.

***************************************/

Word32 BURGER_API Burger::Globals::GetQTApplicationDirectoryA(char *pBuffer,Word32 uSize)
{
	GetQTFolderFromRegistry("Software\\Apple Computer, Inc.\\QuickTime","InstallDir",pBuffer,uSize);
	return static_cast<Word32>(StringLength(pBuffer));
}

/*! ************************************

	\brief Find the Quicktime extensions folder

	This function is a functional equivalent to the Quicktime for
	Windows function GetQTExtensionDirectoryA(). It will
	query a registry value from the HKEY_LOCAL_MACHINE root
	and if found, it will return the path.

	On success, the pathname will be in the buffer with a
	'\' or a '/' ending the string. Otherwise, the string
	will be empty on failure.

	\note Quicktime for Windows is a 32 bit API. Don't use it
	for 64 bit applications.

	\windowsonly
	\param pBuffer Pointer to a buffer to receive the string
	\param uSize Number of bytes in size of the string buffer
	\return Length of the returned string in chars.

***************************************/

Word32 BURGER_API Burger::Globals::GetQTExtensionDirectoryA(char *pBuffer,Word32 uSize)
{
	char SystemDirName[256];
	char QTFolderName[256];

	// Try getting it from the registry
	GetQTFolderFromRegistry("Software\\Apple Computer, Inc.\\QuickTime","QTExtDir",pBuffer,uSize);
	if (!pBuffer[0] && (uSize>=11)) {

		// Didn't find it?
		uSize -= 10;			// Make space for "Quicktime\"

		// Find where Quicktime should be
		GetPathToQuickTimeFolder(QTFolderName,uSize,NULL);

		if (QTFolderName[0]) {

			// Copy up the Quicktime folder name
			StringCopy(pBuffer,uSize,QTFolderName);

			// Read in the system directory name
			GetSystemDirectoryA(SystemDirName,uSize);
			EndWithWindowsSlashes(SystemDirName);

			// Was the folder found the Windows system folder?
			if (!StringCompare(SystemDirName,QTFolderName)) {
				// Since it's the Windows folder that's found, append the
				// string QuickTime\ to the end of the pathname
				// to denote the old location of where Quicktime
				// extensions were stored.
				StringConcatenate(pBuffer,"QuickTime\\");
			}
		}
	}
	return static_cast<Word32>(StringLength(pBuffer));
}

/*! ************************************

	\brief Find the Quicktime components folder

	This function is a functional equivalent to the Quicktime for
	Windows function GetQTComponentDirectoryA(). It will
	query a registry value from the HKEY_LOCAL_MACHINE root
	and if found, it will return the path.

	On success, the pathname will be in the buffer with a
	'\' or a '/' ending the string. Otherwise, the string
	will be empty on failure.

	\note Quicktime for Windows is a 32 bit API. Don't use it
	for 64 bit applications.

	\windowsonly
	\param pBuffer Pointer to a buffer to receive the string
	\param uSize Number of bytes in size of the string buffer
	\return Length of the returned string in chars.

***************************************/

Word32 BURGER_API Burger::Globals::GetQTComponentDirectoryA(char *pBuffer,Word32 uSize)
{
	GetQTFolderFromRegistry("Software\\Apple Computer, Inc.\\QuickTime","QTComponentsDir",pBuffer,uSize);
	if (!pBuffer[0]) {
		GetSystemDirectoryA(pBuffer,uSize);
		if (pBuffer[0]) {
			EndWithWindowsSlashes(pBuffer);
			// Since it's the Windows folder that's found, append the
			// string QuickTime\ to the end of the pathname
			// to denote the old location of where Quicktime
			// extensions were stored.
			StringConcatenate(pBuffer,"QuickTime\\");
		}
	}
	return static_cast<Word32>(StringLength(pBuffer));
}

/*! ************************************

	\brief Locate and load a Quicktime DLL

	This function is a functional equivalent to the Quicktime for
	Windows function QTLoadLibrary().

	\note Do not pass in full pathnames. Only pass in the name of the DLL and
	nothing more. If a full pathname is available, use a direct call to
	LoadLibraryA(const char *) instead.

	\note Quicktime for Windows is a 32 bit API. Don't use it
	for 64 bit applications.

	\windowsonly
	\param pDLLName Pointer to the DLL requested
	\return HINSTANCE of the DLL or \ref NULL on failure

***************************************/

HINSTANCE BURGER_API Burger::Globals::QTLoadLibrary(const char *pDLLName)
{
	char FinalPathname[1024];

	// Assume no DLL
	HINSTANCE pResult = NULL;
	WordPtr uDLLNameLength = StringLength(pDLLName);

	// Get the path for Quicktime
	if (GetPathToQuickTimeFolder(FinalPathname,static_cast<Word32>(BURGER_ARRAYSIZE(FinalPathname)-uDLLNameLength),NULL)) {
		StringConcatenate(FinalPathname,pDLLName);
		pResult = LoadLibraryA(FinalPathname);
		if (!pResult) {
			GetLastError();
			if (GetQTExtensionDirectoryA(FinalPathname,static_cast<Word32>(BURGER_ARRAYSIZE(FinalPathname)-uDLLNameLength))) {
				StringConcatenate(FinalPathname,pDLLName);
				pResult = LoadLibraryA(FinalPathname);
			}
		}
	}
	return pResult;
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
	char PathName[1024+32];

	if (!g_bQuickTimeVersionValid) {
		g_bQuickTimeVersionValid = TRUE;		// I got the version

		Word uResult = 0;						// I assume version 0!
		// Get the system directory for Quicktime
		WordPtr uPathLength = GetSystemDirectoryA(PathName,BURGER_ARRAYSIZE(PathName)-32);
		if (uPathLength) {

			// Get the Quicktime DLL using the old name for 2.0 or 3.0
			StringCopy(PathName+uPathLength,sizeof(PathName)-uPathLength,"\\QTIM32.DLL");
			DWORD uZeroLong = 0;
			DWORD uFileInfoSize = GetFileVersionInfoSizeA(PathName,&uZeroLong);
			const char *pQueryString = "\\StringFileInfo\\040904E4\\ProductVersion";
			// Any data?
			if (!uFileInfoSize) {
				// Try the location of Quicktime 4.0 and 5.0
				pQueryString = "\\StringFileInfo\\040904B0\\FileVersion";
				// Try Quicktime 4.0
				uZeroLong = 0;
				StringCopy(PathName+uPathLength,sizeof(PathName)-uPathLength,"\\QuickTime.qts");
				uFileInfoSize = GetFileVersionInfoSizeA(PathName,&uZeroLong);
				if (!uFileInfoSize) {
					// Try the location of Quicktime 6.0 and later
					uPathLength = GetEnvironmentVariableA("ProgramFiles(x86)",PathName,BURGER_ARRAYSIZE(PathName));
					if (!uPathLength) {
						uPathLength = GetEnvironmentVariableA("ProgramFiles",PathName,BURGER_ARRAYSIZE(PathName));
					}
					if (uPathLength) {
						StringCopy(PathName+uPathLength,sizeof(PathName)-uPathLength,"\\QuickTime\\QTSystem\\QuickTime.qts");
						uFileInfoSize = GetFileVersionInfoSizeA(PathName,&uZeroLong);
					}
				}
			}
			if (uFileInfoSize) {

				// Use HeapAlloc() instead of Alloc to allow code to use this function
				// without starting Burgerlib Memory

				// Get the data buffer
				HANDLE hHeap = GetProcessHeap();
				char *pData = static_cast<char *>(HeapAlloc(hHeap,0,uFileInfoSize));
				if (pData) {
					void *pVersionData;			// Main data pointer to start parsing from
					if (GetFileVersionInfoA(PathName,0,uFileInfoSize,pData)) {
						UINT ZeroWord = 0;
						if (VerQueryValueA(pData,const_cast<char *>(pQueryString),&pVersionData,&ZeroWord)) {
							// Running ASCII pointer
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
					HeapFree(hHeap,0,pData);
				}
			}
		}
		g_uQuickTimeVersion = uResult;
	}
	// Return the QuickTime version
	return g_uQuickTimeVersion;
}




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
		// Get DirectInput 8
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

	Note: There are some Vista/7/10 functions included, this
	is to allow Windows XP applications to test for advanced
	features, or fall back to the "old way" of doing things.
	
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
// xinput1_4.dll or xinput1_3.dll
//

/*! ************************************

	\brief Load in xinput1_4.dll and call XInputGetState

	To allow maximum compatibility, this function will manually load
	xinput1_4.dll or xinput1_3.dll and then invoke XInputGetState if present.

	https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinputgetstate(v=vs.85).aspx

	\windowsonly
	\param dwUserIndex Index of the user's controller. Can be a value from 0 to 3.
	\param pState Pointer to an XINPUT_STATE structure that receives the current state of the controller.
	\return Zero if no error. Any other value means an error occurred, ERROR_CALL_NOT_IMPLEMENTED means the function was not found
	
***************************************/

Word32 BURGER_API Burger::Globals::XInputGetState(Word32 dwUserIndex,_XINPUT_STATE* pState)
{
	// Get the function pointer
	void *pXInputGetState = LoadFunctionIndex(CALL_XInputGetState);
	Word32 uResult = ERROR_CALL_NOT_IMPLEMENTED;
	if (pXInputGetState) {
		uResult = static_cast<DWORD(WINAPI *)(DWORD,XINPUT_STATE *)>(pXInputGetState)(dwUserIndex,pState);
	}
	return uResult;
}

/*! ************************************

	\brief Load in xinput1_4.dll and call XInputSetState

	To allow maximum compatibility, this function will manually load
	xinput1_4.dll or xinput1_3.dll and then invoke XInputSetState if present.

	https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinputsetstate(v=vs.85).aspx

	\windowsonly
	\param dwUserIndex Index of the user's controller. Can be a value from 0 to 3.
	\param pVibration Pointer to an XINPUT_VIBRATION structure containing the vibration information to send to the controller.
	\return Zero if no error. Any other value means an error occurred, ERROR_CALL_NOT_IMPLEMENTED means the function was not found
	
***************************************/

Word32 BURGER_API Burger::Globals::XInputSetState(Word32 dwUserIndex,_XINPUT_VIBRATION* pVibration)
{
	// Get the function pointer
	void *pXInputSetState = LoadFunctionIndex(CALL_XInputSetState);
	Word32 uResult = ERROR_CALL_NOT_IMPLEMENTED;
	if (pXInputSetState) {
		uResult = static_cast<DWORD(WINAPI *)(DWORD,XINPUT_VIBRATION *)>(pXInputSetState)(dwUserIndex,pVibration);
	}
	return uResult;
}

/*! ************************************

	\brief Load in xinput1_4.dll and call XInputGetCapabilities

	To allow maximum compatibility, this function will manually load
	xinput1_4.dll or xinput1_3.dll and then invoke XInputGetCapabilities if present.

	https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinputgetcapabilities(v=vs.85).aspx

	\windowsonly
	\param dwUserIndex Index of the user's controller. Can be a value from 0 to 3.
	\param dwFlags Input flags that identify the controller type. If this value is 0, then the capabilities of all controllers connected to the system are returned.
	\param pCapabilities Pointer to an XINPUT_CAPABILITIES structure that receives the controller capabilities.
	\return Zero if no error. Any other value means an error occurred, ERROR_CALL_NOT_IMPLEMENTED means the function was not found
	
***************************************/

Word32 BURGER_API Burger::Globals::XInputGetCapabilities(Word32 dwUserIndex,Word32 dwFlags,_XINPUT_CAPABILITIES* pCapabilities)
{
	// Get the function pointer
	void *pXInputGetCapabilities = LoadFunctionIndex(CALL_XInputGetCapabilities);
	Word32 uResult = ERROR_CALL_NOT_IMPLEMENTED;
	if (pXInputGetCapabilities) {
		uResult = static_cast<DWORD(WINAPI *)(DWORD,Word32,XINPUT_CAPABILITIES *)>(pXInputGetCapabilities)(dwUserIndex,dwFlags,pCapabilities);
	}
	return uResult;
}

/*! ************************************

	\brief Load in xinput1_4.dll and call XInputGetDSoundAudioDeviceGuids

	To allow maximum compatibility, this function will manually load
	xinput1_4.dll or xinput1_3.dll and then invoke XInputGetDSoundAudioDeviceGuids if present.

	https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinputgetdsoundaudiodeviceguids(v=vs.85).aspx

	\note This function is deprecated as of Windows 8 (DLL XInput 1.4 or later)

	\windowsonly
	\param dwUserIndex Index of the user's controller. Can be a value from 0 to 3.
	\param pDSoundRenderGuid Pointer that receives the GUID of the headset sound rendering device.
	\param pDSoundCaptureGuid Pointer that receives the GUID of the headset sound capture device.
	\return Zero if no error. Any other value means an error occurred, ERROR_CALL_NOT_IMPLEMENTED means the function was not found
	
***************************************/

Word32 BURGER_API Burger::Globals::XInputGetDSoundAudioDeviceGuids(Word32 dwUserIndex,GUID* pDSoundRenderGuid,GUID* pDSoundCaptureGuid)
{
	// Get the function pointer
	void *pXInputGetDSoundAudioDeviceGuids = LoadFunctionIndex(CALL_XInputGetDSoundAudioDeviceGuids);
	Word32 uResult = ERROR_CALL_NOT_IMPLEMENTED;
	if (pXInputGetDSoundAudioDeviceGuids) {
		uResult = static_cast<DWORD(WINAPI *)(DWORD,GUID *,GUID *)>(pXInputGetDSoundAudioDeviceGuids)(dwUserIndex,pDSoundRenderGuid,pDSoundCaptureGuid);
	}
	return uResult;
}

/*! ************************************

	\brief Load in xinput1_4.dll and call XInputEnable

	To allow maximum compatibility, this function will manually load
	xinput1_4.dll or xinput1_3.dll and then invoke XInputEnable if present.

	https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinputenable(v=vs.85).aspx

	\windowsonly
	\param bEnable If enable is \ref FALSE, XInput will only send neutral data in response to XInputGetState() (all buttons up, axes centered, and triggers at 0).
		Sending any value other than \ref FALSE will restore reading and writing functionality to normal.

***************************************/

void BURGER_API Burger::Globals::XInputEnable(Word bEnable)
{
	// Get the function pointer
	void *pXInputEnable = LoadFunctionIndex(CALL_XInputEnable);
	if (pXInputEnable) {
		static_cast<void(WINAPI *)(BOOL)>(pXInputEnable)(bEnable!=0);
	}
}

/*! ************************************

	\brief Load in xinput1_4.dll and call XInputGetAudioDeviceIds

	To allow maximum compatibility, this function will manually load
	xinput1_4.dll or xinput1_3.dll and then invoke XInputGetAudioDeviceIds if present.

	https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinputgetaudiodeviceids(v=vs.85).aspx

	\note This function is only available as of Windows 8 (DLL XInput 1.4 or later)

	\windowsonly
	\param dwUserIndex Index of the gamer associated with the device.
	\param pRenderDeviceId Pointer that receives Windows Core Audio device ID string for render (speakers).
	\param pRenderCount Pointer that receives the size, in wide-chars, of the render device ID string buffer.
	\param pCaptureDeviceId Pointer that receives Windows Core Audio device ID string for capture (microphone).
	\param pCaptureCount Pointer that receives the size, in wide-chars, of capture device ID string buffer.
	\return Zero if no error. Any other value means an error occurred, ERROR_CALL_NOT_IMPLEMENTED means the function was not found
	
***************************************/

Word32 BURGER_API Burger::Globals::XInputGetAudioDeviceIds(Word32 dwUserIndex,Word16 *pRenderDeviceId,Word *pRenderCount,Word16 *pCaptureDeviceId,Word*pCaptureCount)
{
	// Get the function pointer
	void *pXInputGetAudioDeviceIds = LoadFunctionIndex(CALL_XInputGetAudioDeviceIds);
	Word32 uResult = ERROR_CALL_NOT_IMPLEMENTED;
	if (pXInputGetAudioDeviceIds) {
		uResult = static_cast<DWORD(WINAPI *)(DWORD,Word16 *,UINT*,Word16 *,UINT*)>(pXInputGetAudioDeviceIds)(dwUserIndex,pRenderDeviceId,pRenderCount,pCaptureDeviceId,pCaptureCount);
	}
	return uResult;
}

/*! ************************************

	\brief Load in xinput1_4.dll and call XInputGetBatteryInformation

	To allow maximum compatibility, this function will manually load
	xinput1_4.dll or xinput1_3.dll and then invoke XInputGetBatteryInformation if present.

	https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinputgetbatteryinformation(v=vs.85).aspx

	\windowsonly
	\param dwUserIndex Index of the signed-in gamer associated with the device. Can be a value in the range 0-XUSER_MAX_COUNT - 1.
	\param devType Input Specifies which device associated with this user index should be queried. Must be BATTERY_DEVTYPE_GAMEPAD or BATTERY_DEVTYPE_HEADSET.
	\param pBatteryInformation Pointer to an XINPUT_BATTERY_INFORMATION structure that receives the battery information.
	\return Zero if no error. Any other value means an error occurred, ERROR_CALL_NOT_IMPLEMENTED means the function was not found

***************************************/

Word32 BURGER_API Burger::Globals::XInputGetBatteryInformation(Word32 dwUserIndex,Word devType,_XINPUT_BATTERY_INFORMATION* pBatteryInformation)
{
	// Get the function pointer
	void *pXInputGetBatteryInformation = LoadFunctionIndex(CALL_XInputGetBatteryInformation);
	Word32 uResult = ERROR_CALL_NOT_IMPLEMENTED;
	if (pXInputGetBatteryInformation) {
		uResult = static_cast<DWORD(WINAPI *)(DWORD,BYTE,XINPUT_BATTERY_INFORMATION *)>(pXInputGetBatteryInformation)(dwUserIndex,static_cast<BYTE>(devType),pBatteryInformation);
	}
	return uResult;
}

/*! ************************************

	\brief Load in xinput1_4.dll and call XInputGetKeystroke

	To allow maximum compatibility, this function will manually load
	xinput1_4.dll or xinput1_3.dll and then invoke XInputGetKeystroke if present.

	https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinputgetkeystroke(v=vs.85).aspx

	\windowsonly
	\param dwUserIndex Index of the signed-in gamer associated with the device. Can be a value in the range 0-XUSER_MAX_COUNT - 1 
		or XUSER_INDEX_ANY to fetch the next available input event from any user.
	\param dwReserved Set to zero.
	\param pKeystroke Pointer to an XINPUT_KEYSTROKE structure that receives an input event.
	\return Zero if no error. Any other value means an error occurred, ERROR_CALL_NOT_IMPLEMENTED means the function was not found

***************************************/

Word32 BURGER_API Burger::Globals::XInputGetKeystroke(Word32 dwUserIndex,Word32 dwReserved,_XINPUT_KEYSTROKE *pKeystroke)
{
	// Get the function pointer
	void *pXInputGetKeystroke = LoadFunctionIndex(CALL_XInputGetKeystroke);
	Word32 uResult = ERROR_CALL_NOT_IMPLEMENTED;
	if (pXInputGetKeystroke) {
		uResult = static_cast<DWORD(WINAPI *)(DWORD,DWORD,PXINPUT_KEYSTROKE)>(pXInputGetKeystroke)(dwUserIndex,dwReserved,pKeystroke);
	}
	return uResult;
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

/*! ************************************

	\brief Load in d3d9.dll and call D3DPERF_BeginEvent

	To allow maximum compatibility, this function will manually load
	d3d9.dll and then invoke D3DPERF_BeginEvent if present.

	\windowsonly
	\param col The color of the event
	\param wszName Pointer to UTF-16 string of the name of the event
	\return Is the level starting from 0 in the hierarchy to start this event. If an error occurs, the return value is negative.
	
***************************************/

int BURGER_API Burger::Globals::D3DPERF_BeginEvent(Word32 col,const Word16 *wszName)
{
	// Get the function pointer
	void *pD3DPERF_BeginEvent = LoadFunctionIndex(CALL_D3DPERF_BeginEvent);
	int iResult = -1;
	if (pD3DPERF_BeginEvent) {
		iResult = static_cast<int(WINAPI *)(D3DCOLOR,LPCWSTR)>(pD3DPERF_BeginEvent)(col,static_cast<LPCWSTR>(static_cast<const void *>(wszName)));
	}
	return iResult;
}

/*! ************************************

	\brief Load in d3d9.dll and call D3DPERF_EndEvent

	To allow maximum compatibility, this function will manually load
	d3d9.dll and then invoke D3DPERF_EndEvent if present.

	\windowsonly
	\return Is the level starting from 0 in the hierarchy to start this event. If an error occurs, the return value is negative.
	
***************************************/

int BURGER_API Burger::Globals::D3DPERF_EndEvent(void)
{
	// Get the function pointer
	void *pD3DPERF_EndEvent = LoadFunctionIndex(CALL_D3DPERF_EndEvent);
	int iResult = -1;
	if (pD3DPERF_EndEvent) {
		iResult = static_cast<int(WINAPI *)(void)>(pD3DPERF_EndEvent)();
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

void BURGER_API Burger::Globals::D3DPERF_SetMarker(Word32 col,const Word16 *wszName)
{
	// Get the function pointer
	void *pD3DPERF_SetMarker = LoadFunctionIndex(CALL_D3DPERF_SetMarker);
	if (pD3DPERF_SetMarker) {
		static_cast<void(WINAPI *)(D3DCOLOR,LPCWSTR)>(pD3DPERF_SetMarker)(col,static_cast<LPCWSTR>(static_cast<const void *>(wszName)));
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

void BURGER_API Burger::Globals::D3DPERF_SetRegion(Word32 col,const Word16 *wszName)
{
	// Get the function pointer
	void *pD3DPERF_SetRegion = LoadFunctionIndex(CALL_D3DPERF_SetRegion);
	if (pD3DPERF_SetRegion) {
		static_cast<void(WINAPI *)(D3DCOLOR,LPCWSTR)>(pD3DPERF_SetRegion)(col,static_cast<LPCWSTR>(static_cast<const void *>(wszName)));
	}
}

/*! ************************************

	\brief Load in d3d9.dll and call D3DPERF_QueryRepeatFrame

	To allow maximum compatibility, this function will manually load
	d3d9.dll and then invoke D3DPERF_QueryRepeatFrame if present.

	\windowsonly
	\return When the return value is \ref TRUE, the caller will need to repeat the same sequence of calls. If \ref FALSE, the caller needs to move forward.
	
***************************************/

int BURGER_API Burger::Globals::D3DPERF_QueryRepeatFrame(void)
{
	// Get the function pointer
	void *pD3DPERF_QueryRepeatFrame = LoadFunctionIndex(CALL_D3DPERF_QueryRepeatFrame);
	int iResult = FALSE;
	if (pD3DPERF_QueryRepeatFrame) {
		iResult = static_cast<int(WINAPI *)(void)>(pD3DPERF_QueryRepeatFrame)();
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

void BURGER_API Burger::Globals::D3DPERF_SetOptions(Word32 dwOptions)
{
	// Get the function pointer
	void *pD3DPERF_SetOptions = LoadFunctionIndex(CALL_D3DPERF_SetOptions);
	if (pD3DPERF_SetOptions) {
		static_cast<void(WINAPI *)(DWORD)>(pD3DPERF_SetOptions)(dwOptions);
	}
}

/*! ************************************

	\brief Load in d3d9.dll and call D3DPERF_GetStatus

	To allow maximum compatibility, this function will manually load
	d3d9.dll and then invoke D3DPERF_GetStatus if present.

	\windowsonly
	\return Non-zero if profiled by PIX. 0 if PIX is not present.
	
***************************************/

Word BURGER_API Burger::Globals::D3DPERF_GetStatus(void)
{
	// Get the function pointer
	void *pD3DPERF_GetStatus = LoadFunctionIndex(CALL_D3DPERF_GetStatus);
	Word uResult = 0;
	if (pD3DPERF_GetStatus) {
		uResult = static_cast<DWORD (WINAPI *)(void)>(pD3DPERF_GetStatus)();
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
// d3d11.dll
//

/*! ************************************

	\brief Load in d3d11.dll and call D3D11CreateDevice

	To allow maximum compatibility, this function will manually load
	d3d11.dll if needed and then invoke D3D11CreateDevice.

	\windowsonly

	\param pAdapter A pointer to the video adapter to use when creating a device. Pass NULL to use the default adapter
	\param DriverType The D3D_DRIVER_TYPE, which represents the driver type to create.
	\param Software A handle to a DLL that implements a software rasterizer
	\param Flags The runtime layers to enable (see D3D11_CREATE_DEVICE_FLAG); values can be bitwise OR'd together.
	\param pFeatureLevels A pointer to an array of D3D_FEATURE_LEVELs, which determine the order of feature levels to attempt to create.
	\param FeatureLevels The number of elements in pFeatureLevels.
	\param SDKVersion The SDK version; use D3D11_SDK_VERSION.
	\param ppDevice Returns the address of a pointer to an ID3D11Device object that represents the device created.
	\param pFeatureLevel If successful, returns the first D3D_FEATURE_LEVEL from the pFeatureLevels array which succeeded
	\param ppImmediateContext Returns the address of a pointer to an ID3D11DeviceContext object that represents the device context
	\return S_OK if the call succeeded. Windows error if otherwise
	
***************************************/

Word BURGER_API Burger::Globals::D3D11CreateDevice(IDXGIAdapter *pAdapter,Word DriverType,HINSTANCE__ *Software,
	Word Flags,const Word *pFeatureLevels,Word FeatureLevels,Word SDKVersion,ID3D11Device **ppDevice,
	Word *pFeatureLevel,ID3D11DeviceContext **ppImmediateContext)
{
	// Get the function pointer
	void *pD3D11CreateDevice = LoadFunctionIndex(CALL_D3D11CreateDevice);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pD3D11CreateDevice) {
		uResult = static_cast<HRESULT(WINAPI *)(IDXGIAdapter *,D3D_DRIVER_TYPE,HMODULE,
			UINT,const D3D_FEATURE_LEVEL *,UINT,UINT,ID3D11Device **,D3D_FEATURE_LEVEL *,
			ID3D11DeviceContext **)>
			(pD3D11CreateDevice)(pAdapter,static_cast<D3D_DRIVER_TYPE>(DriverType),Software,Flags,static_cast<const D3D_FEATURE_LEVEL *>(static_cast<const void *>(pFeatureLevels)),
				FeatureLevels,SDKVersion,ppDevice,static_cast<D3D_FEATURE_LEVEL *>(static_cast<void *>(pFeatureLevel)),ppImmediateContext);
	}
	return static_cast<Word>(uResult);
}





//
// dxgi.dll
//

/*! ************************************

	\brief Load in dxgi.dll and call CreateDXGIFactory

	To allow maximum compatibility, this function will manually load
	dxgi.dll if needed and then invoke CreateDXGIFactory.

	\windowsonly
	\param pGuidFactory The globally unique identifier (GUID) of the IDXGIFactory object referenced by the ppFactory parameter
	\param ppFactory Address of a pointer to an IDXGIFactory object.
	\return S_OK if the call succeeded. Windows error if otherwise
	
***************************************/

Word BURGER_API Burger::Globals::CreateDXGIFactory(const GUID *pGuidFactory,void **ppFactory)
{
	// Get the function pointer
	void *pCreateDXGIFactory = LoadFunctionIndex(CALL_CreateDXGIFactory);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pCreateDXGIFactory) {
		uResult = static_cast<HRESULT(WINAPI *)(REFIID,void**)>(pCreateDXGIFactory)(pGuidFactory[0],ppFactory);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in dxgi.dll and call CreateDXGIFactory1

	To allow maximum compatibility, this function will manually load
	dxgi.dll if needed and then invoke CreateDXGIFactory1.

	\windowsonly
	\param pGuidFactory The globally unique identifier (GUID) of the IDXGIFactory1 object referenced by the ppFactory parameter
	\param ppFactory Address of a pointer to an IDXGIFactory1 object.
	\return S_OK if the call succeeded. Windows error if otherwise
	
***************************************/

Word BURGER_API Burger::Globals::CreateDXGIFactory1(const GUID *pGuidFactory,void **ppFactory)
{
	// Get the function pointer
	void *pCreateDXGIFactory1 = LoadFunctionIndex(CALL_CreateDXGIFactory1);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pCreateDXGIFactory1) {
		uResult = static_cast<HRESULT(WINAPI *)(REFIID,void**)>(pCreateDXGIFactory1)(pGuidFactory[0],ppFactory);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in dxgi.dll and call CreateDXGIFactory2

	To allow maximum compatibility, this function will manually load
	dxgi.dll if needed and then invoke CreateDXGIFactory2.

	\windowsonly
	\param uFlags Valid values include the DXGI_CREATE_FACTORY_DEBUG (0x01) flag, and zero.
	\param pGuidFactory The globally unique identifier (GUID) of the IDXGIFactory2 object referenced by the ppFactory parameter
	\param ppFactory Address of a pointer to an IDXGIFactory2 object.
	\return S_OK if the call succeeded. Windows error if otherwise
	
***************************************/

Word BURGER_API Burger::Globals::CreateDXGIFactory2(Word uFlags,const GUID *pGuidFactory,void **ppFactory)
{
	// Get the function pointer
	void *pCreateDXGIFactory2 = LoadFunctionIndex(CALL_CreateDXGIFactory2);
	HRESULT uResult = DDERR_NOTFOUND;
	if (pCreateDXGIFactory2) {
		uResult = static_cast<HRESULT(WINAPI *)(UINT,REFIID,void**)>(pCreateDXGIFactory2)(uFlags,pGuidFactory[0],ppFactory);
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
// dplayx.dll
//


/*! ************************************

	\brief Load in dplayx.dll and call DirectPlayCreate

	To allow maximum compatibility, this function will manually load
	dplayx.dll or dplay.dll and then invoke DirectPlayCreate if present.

	\windowsonly
	\param pGuidSrc Address of a variable that specifies a valid device identifier
	\param ppOutput Address of a variable that receives the unique identifier of the device
	\param pOuter Address of IUnknown, set to \ref NULL
	\return DP_OK if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectPlayCreate(GUID *pGuidSrc,IDirectPlay **ppOutput,IUnknown *pOuter)
{
	// Get the function pointer
	void *pDirectPlayCreate = LoadFunctionIndex(CALL_DirectPlayCreate);
	HRESULT uResult = DSERR_INVALIDCALL;
	if (pDirectPlayCreate) {
		uResult = static_cast<HRESULT (WINAPI *)(GUID *,LPDIRECTPLAY *,IUnknown *)>(pDirectPlayCreate)(pGuidSrc,ppOutput,pOuter);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in dplayx.dll and call DirectPlayEnumerate

	This function is called CallDirectPlayEnumerate instead of DirectPlayEnumerate
	to avoid a conflict of a macro found in the dplay.h header.

	To allow maximum compatibility, this function will manually load
	dplayx.dll or dplay.dll and then invoke DirectPlayEnumerate if present.

	\note This function is OBSOLETE, call DirectPlayEnumerateA() instead

	\windowsonly
	\param pCallback Address of the callback function of type LPDPENUMDPCALLBACKA
	\param pContext Address of user data
	\return DP_OK if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::CallDirectPlayEnumerate(void *pCallback,void *pContext)
{
	// Get the function pointer
	void *pDirectPlayEnumerate = LoadFunctionIndex(CALL_DirectPlayEnumerate);
	HRESULT uResult = DSERR_INVALIDCALL;
	if (pDirectPlayEnumerate) {
		uResult = static_cast<HRESULT (WINAPI *)(LPDPENUMDPCALLBACKA,LPVOID)>(pDirectPlayEnumerate)(static_cast<LPDPENUMDPCALLBACKA>(pCallback),pContext);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in dplayx.dll and call DirectPlayEnumerateA

	To allow maximum compatibility, this function will manually load
	dplayx.dll and then invoke DirectPlayEnumerateA if present.

	\windowsonly
	\param pCallback Address of the callback function of type LPDPENUMDPCALLBACKA
	\param pContext Address of user data
	\return DP_OK if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectPlayEnumerateA(void *pCallback,void *pContext)
{
	// Get the function pointer
	void *pDirectPlayEnumerateA = LoadFunctionIndex(CALL_DirectPlayEnumerateA);
	HRESULT uResult = DSERR_INVALIDCALL;
	if (pDirectPlayEnumerateA) {
		uResult = static_cast<HRESULT (WINAPI *)(LPDPENUMDPCALLBACKA,LPVOID)>(pDirectPlayEnumerateA)(static_cast<LPDPENUMDPCALLBACKA>(pCallback),pContext);
	}
	return static_cast<Word>(uResult);
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

Word BURGER_API Burger::Globals::DirectPlayEnumerateW(void *pCallback,void *pContext)
{
	// Get the function pointer
	void *pDirectPlayEnumerateW = LoadFunctionIndex(CALL_DirectPlayEnumerateW);
	HRESULT uResult = DSERR_INVALIDCALL;
	if (pDirectPlayEnumerateW) {
		uResult = static_cast<HRESULT (WINAPI *)(LPDPENUMDPCALLBACK,LPVOID)>(pDirectPlayEnumerateW)(static_cast<LPDPENUMDPCALLBACK>(pCallback),pContext);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in dplayx.dll and call DirectPlayLobbyCreateA

	To allow maximum compatibility, this function will manually load
	dplayx.dll and then invoke DirectPlayLobbyCreateA if present.

	\windowsonly
	\param pGuidSrc Address of a variable that specifies a valid device identifier
	\param ppOutput Address of a variable that receives the pointer to the IDirectPlayLobby
	\param pOuter Address of IUnknown, set to \ref NULL
	\param pData Address of custom data needed for the lobby
	\param uDataSize Size in bytes of the custom data for the lobby
	\return DP_OK if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectPlayLobbyCreateA(GUID *pGuidSrc,IDirectPlayLobby **ppOutput,IUnknown *pOuter,void *pData,Word uDataSize)
{
	// Get the function pointer
	void *pDirectPlayLobbyCreateA = LoadFunctionIndex(CALL_DirectPlayLobbyCreateA);
	HRESULT uResult = DSERR_INVALIDCALL;
	if (pDirectPlayLobbyCreateA) {
		uResult = static_cast<HRESULT (WINAPI *)(LPGUID,LPDIRECTPLAYLOBBYA *,IUnknown *,LPVOID,DWORD)>(pDirectPlayLobbyCreateA)(pGuidSrc,ppOutput,pOuter,pData,uDataSize);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in dplayx.dll and call DirectPlayLobbyCreateW

	To allow maximum compatibility, this function will manually load
	dplayx.dll and then invoke DirectPlayLobbyCreateW if present.

	\windowsonly
	\param pGuidSrc Address of a variable that specifies a valid device identifier
	\param ppOutput Address of a variable that receives the pointer to the IDirectPlayLobby
	\param pOuter Address of IUnknown, set to \ref NULL
	\param pData Address of custom data needed for the lobby
	\param uDataSize Size in bytes of the custom data for the lobby

	\return DP_OK if no error. Any other value means an error occurred
	
***************************************/

Word BURGER_API Burger::Globals::DirectPlayLobbyCreateW(GUID *pGuidSrc,IDirectPlayLobby **ppOutput,IUnknown *pOuter,void *pData,Word uDataSize)
{
	// Get the function pointer
	void *pDirectPlayLobbyCreateW = LoadFunctionIndex(CALL_DirectPlayLobbyCreateW);
	HRESULT uResult = DSERR_INVALIDCALL;
	if (pDirectPlayLobbyCreateW) {
		uResult = static_cast<HRESULT (WINAPI *)(LPGUID,LPDIRECTPLAYLOBBY *,IUnknown *,LPVOID,DWORD)>(pDirectPlayLobbyCreateW)(pGuidSrc,ppOutput,pOuter,pData,uDataSize);
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

/*! ************************************

	\brief Load in user32.dll and call TrackMouseEvent

	Manually load user32.dll if needed and
	call the Windows function GetMonitorInfoA()

	On versions of windows that do not have GetMonitorInfoA(), use a
	compatibility function that performs the same task

	https://msdn.microsoft.com/en-us/library/windows/desktop/dd144901(v=vs.85).aspx
	
	\windowsonly
	\param hMonitor A handle to the display monitor of interest.
	\param pMonitorInfo A pointer to a MONITORINFO or MONITORINFOEX structure that receives information about the specified display monitor.
	\return If the function succeeds, the return value is nonzero.

***************************************/

Word BURGER_API Burger::Globals::GetMonitorInfoA(HMONITOR__ *hMonitor,tagMONITORINFO *pMonitorInfo)
{
	void *pGetMonitorInfoA = LoadFunctionIndex(CALL_GetMonitorInfoA);
	BOOL uResult = FALSE;		// Failure
	if (pGetMonitorInfoA) {
		uResult = static_cast<BOOL (WINAPI *)(HMONITOR,LPMONITORINFO)>(pGetMonitorInfoA)(hMonitor,pMonitorInfo);
	} else {
		RECT TempRect;
		if ((hMonitor == ((HMONITOR)0x12340042)) &&
			pMonitorInfo &&
			(pMonitorInfo->cbSize >= sizeof(MONITORINFO)) &&
			SystemParametersInfoA(SPI_GETWORKAREA,0,&TempRect,0)) {

			pMonitorInfo->rcMonitor.left = 0;
			pMonitorInfo->rcMonitor.top = 0;
			pMonitorInfo->rcMonitor.right = GetSystemMetrics(SM_CXSCREEN);
			pMonitorInfo->rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
			pMonitorInfo->rcWork = TempRect;
			pMonitorInfo->dwFlags = MONITORINFOF_PRIMARY;
			uResult = TRUE;
		}
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in user32.dll and call TrackMouseEvent

	Manually load user32.dll if needed and
	call the Windows function GetMonitorInfoW()

	On versions of windows that do not have GetMonitorInfoW(), use a
	compatibility function that performs the same task

	https://msdn.microsoft.com/en-us/library/windows/desktop/dd144901(v=vs.85).aspx
	
	\windowsonly
	\param hMonitor A handle to the display monitor of interest.
	\param pMonitorInfo A pointer to a MONITORINFO or MONITORINFOEX structure that receives information about the specified display monitor.
	\return If the function succeeds, the return value is nonzero.

***************************************/

Word BURGER_API Burger::Globals::GetMonitorInfoW(HMONITOR__ *hMonitor,tagMONITORINFO *pMonitorInfo)
{
	void *pGetMonitorInfoW = LoadFunctionIndex(CALL_GetMonitorInfoW);
	BOOL uResult = FALSE;		// Failure
	if (pGetMonitorInfoW) {
		uResult = static_cast<BOOL (WINAPI *)(HMONITOR,LPMONITORINFO)>(pGetMonitorInfoW)(hMonitor,pMonitorInfo);
	} else {
		RECT TempRect;
		if ((hMonitor == ((HMONITOR)0x12340042)) &&
			pMonitorInfo &&
			(pMonitorInfo->cbSize >= sizeof(MONITORINFO)) &&
			SystemParametersInfoA(SPI_GETWORKAREA,0,&TempRect,0)) {

			pMonitorInfo->rcMonitor.left = 0;
			pMonitorInfo->rcMonitor.top = 0;
			pMonitorInfo->rcMonitor.right = GetSystemMetrics(SM_CXSCREEN);
			pMonitorInfo->rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
			pMonitorInfo->rcWork = TempRect;
			pMonitorInfo->dwFlags = MONITORINFOF_PRIMARY;
			uResult = TRUE;
		}
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in user32.dll and call MonitorFromWindow

	Manually load user32.dll if needed and
	call the Windows function MonitorFromWindow()

	On versions of windows that do not have MonitorFromWindow(), use a
	compatibility function that performs the same task

	https://msdn.microsoft.com/en-us/library/windows/desktop/dd145064(v=vs.85).aspx
	
	\windowsonly
	\param pWindow A handle to the window of interest.
	\param uFlags Determines the function's return value if the window does not intersect any display monitor.
	\return \ref NULL on failure, HMONITOR on success.

***************************************/

HMONITOR__ * BURGER_API Burger::Globals::MonitorFromWindow(HWND__ *pWindow,Word uFlags)
{
	void *pMonitorFromWindow = LoadFunctionIndex(CALL_MonitorFromWindow);
	HMONITOR pResult = ((HMONITOR)0x12340042);		// Failure
	if (pMonitorFromWindow) {
		pResult = static_cast<HMONITOR (WINAPI *)(HWND,DWORD)>(pMonitorFromWindow)(pWindow,uFlags);
	}
	return pResult;
}

/*! ************************************

	\brief Load in user32.dll and call MonitorFromRect

	Manually load user32.dll if needed and
	call the Windows function MonitorFromRect()

	On versions of windows that do not have MonitorFromRect(), use a
	compatibility function that performs the same task

	https://msdn.microsoft.com/en-us/library/windows/desktop/dd145063(v=vs.85).aspx
	
	\windowsonly
	\param pRect A pointer to a RECT structure that specifies the rectangle of interest in virtual-screen coordinates.
	\param uFlags Determines the function's return value if the window does not intersect any display monitor.
	\return \ref NULL on failure, HMONITOR on success.

***************************************/

HMONITOR__ * BURGER_API Burger::Globals::MonitorFromRect(const tagRECT *pRect,Word uFlags)
{
	void *pMonitorFromRect = LoadFunctionIndex(CALL_MonitorFromRect);
	HMONITOR pResult = ((HMONITOR)0x12340042);		// Failure
	if (pMonitorFromRect) {
		pResult = static_cast<HMONITOR (WINAPI *)(LPCRECT,DWORD)>(pMonitorFromRect)(pRect,uFlags);
	}
	return pResult;
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

	\brief Load in shell32.dll and call SHGetKnownFolderPath

	Manually load shell32.dll if needed and
	call the Windows function SHGetKnownFolderPath()

	https://msdn.microsoft.com/en-us/library/windows/desktop/bb762188(v=vs.85).aspx
	
	\windowsonly
	\param pGuid A pointer to the KNOWNFOLDERID that identifies the folder.
	\param uFlags Flags that specify special retrieval options.
	\param hHandle An access token that represents a particular use
	\param ppResult When this method returns, contains the address of a pointer to a null-terminated Unicode string that specifies the path of the known folder
	\return Returns S_OK if successful, or an error value otherwise

***************************************/

Word BURGER_API Burger::Globals::SHGetKnownFolderPath(const GUID *pGuid,Word32 uFlags,void *hHandle,Word16 **ppResult)
{
	void *pSHGetKnownFolderPath = LoadFunctionIndex(CALL_SHGetKnownFolderPath);
	HRESULT uResult = E_FAIL;		// Failure
	if (pSHGetKnownFolderPath) {
		uResult = static_cast<HRESULT (WINAPI *)(const GUID *,DWORD,HANDLE,Word16 **)>(pSHGetKnownFolderPath)(pGuid,uFlags,hHandle,ppResult);
	}
	return static_cast<Word>(uResult);
}


#endif
