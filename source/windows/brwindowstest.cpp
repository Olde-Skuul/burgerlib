/***************************************

	Code to test windows version

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brwindowstypes.h"
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brmemoryfunctions.h"

// Windows defines
#if !defined(DOXYGEN)

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#if !defined(DIRECTINPUT_VERSION)
#define DIRECTINPUT_VERSION 0x800
#endif

#endif

#include <windows.h>
#include <dinput.h>
#include <xaudio2.h>

//
// These defines may not be defined in earlier Windows SDKs
// To allow the use of these advanced features, they are manually
// defined here if they hadn't already been done so.
//

#if !defined(DOXYGEN)

#if !defined(LOAD_LIBRARY_SEARCH_SYSTEM32)
#define LOAD_LIBRARY_SEARCH_SYSTEM32 0x800
#endif

#endif

/// Singleton instance of the windows global variables
Burger::Windows Burger::Windows::g_Globals;

/***************************************

	DLLs that can be dynamically loaded at runtime (So the application can
	launch if they are missing or missing functions)

***************************************/

//
// These filenames MUST match eDLLIndex
//

static const char* s_LibaryNames[Burger::Windows::DLL_COUNT] = {"ddraw.dll",
	"dinput.dll", "dinput8.dll", "xinput1_4.dll", "xinput1_3.dll", "d3d9.dll",
	"d3dx9_43.dll", "d3d11.dll", "dxgi.dll", "dsound.dll", "dplayx.dll",
	"dplay.dll", "rpcrt4.dll", "winmm.dll", "shlwapi.dll", "version.dll",
	"hid.dll", "setupapi.dll", "user32.dll", "kernel32.dll", "shell32.dll"};

//
// Internal structure for DLL function lookup
//

struct CallNames_t {
	Burger::Windows::eDLLIndex eDLL; // Enumeration for the DLL
	const char* m_pName;			 // Function name
};

static const CallNames_t g_CallNames[Burger::Windows::CALL_COUNT] = {
	{Burger::Windows::DINPUT_DLL, "DirectInputCreateA"},
	{Burger::Windows::DINPUT_DLL, "DirectInputCreateW"},

	{Burger::Windows::DINPUT8_DLL, "DirectInput8Create"},

	{Burger::Windows::XINPUT1_4_DLL, "XInputGetState"},
	{Burger::Windows::XINPUT1_4_DLL, "XInputSetState"},
	{Burger::Windows::XINPUT1_4_DLL, "XInputGetCapabilities"},
	{Burger::Windows::XINPUT1_4_DLL, "XInputGetDSoundAudioDeviceGuids"},
	{Burger::Windows::XINPUT1_4_DLL, "XInputEnable"},
	{Burger::Windows::XINPUT1_4_DLL, "XInputGetAudioDeviceIds"},
	{Burger::Windows::XINPUT1_4_DLL, "XInputGetBatteryInformation"},
	{Burger::Windows::XINPUT1_4_DLL, "XInputGetKeystroke"},

	{Burger::Windows::DDRAW_DLL, "DirectDrawCreate"},
	{Burger::Windows::DDRAW_DLL, "DirectDrawCreateEx"},
	{Burger::Windows::DDRAW_DLL, "DirectDrawCreateClipper"},
	{Burger::Windows::DDRAW_DLL, "DirectDrawEnumerateA"},
	{Burger::Windows::DDRAW_DLL, "DirectDrawEnumerateW"},
	{Burger::Windows::DDRAW_DLL, "DirectDrawEnumerateExA"},
	{Burger::Windows::DDRAW_DLL, "DirectDrawEnumerateExW"},

	{Burger::Windows::D3D9_DLL, "Direct3DCreate9"},
	{Burger::Windows::D3D9_DLL, "D3DPERF_BeginEvent"},
	{Burger::Windows::D3D9_DLL, "D3DPERF_EndEvent"},
	{Burger::Windows::D3D9_DLL, "D3DPERF_SetMarker"},
	{Burger::Windows::D3D9_DLL, "D3DPERF_SetRegion"},
	{Burger::Windows::D3D9_DLL, "D3DPERF_QueryRepeatFrame"},
	{Burger::Windows::D3D9_DLL, "D3DPERF_SetOptions"},
	{Burger::Windows::D3D9_DLL, "D3DPERF_GetStatus"},

	{Burger::Windows::D3DX9_43_DLL, "D3DXCreateMatrixStack"},

	{Burger::Windows::D3D11_DLL, "D3D11CreateDevice"},

	{Burger::Windows::DXGI_DLL, "CreateDXGIFactory"},
	{Burger::Windows::DXGI_DLL, "CreateDXGIFactory1"},
	{Burger::Windows::DXGI_DLL, "CreateDXGIFactory2"},

	{Burger::Windows::DSOUND_DLL, "DirectSoundCreate"},
	{Burger::Windows::DSOUND_DLL, "DirectSoundEnumerateA"},
	{Burger::Windows::DSOUND_DLL, "DirectSoundEnumerateW"},
	{Burger::Windows::DSOUND_DLL, "DirectSoundCaptureCreate"},
	{Burger::Windows::DSOUND_DLL, "DirectSoundCaptureEnumerateA"},
	{Burger::Windows::DSOUND_DLL, "DirectSoundCaptureEnumerateW"},
	{Burger::Windows::DSOUND_DLL, "DirectSoundCreate8"},
	{Burger::Windows::DSOUND_DLL, "DirectSoundCaptureCreate8"},
	{Burger::Windows::DSOUND_DLL, "DirectSoundFullDuplexCreate"},
	{Burger::Windows::DSOUND_DLL, "GetDeviceID"},

	{Burger::Windows::DPLAYX_DLL, "DirectPlayCreate"},
	{Burger::Windows::DPLAYX_DLL, "DirectPlayEnumerate"},
	{Burger::Windows::DPLAYX_DLL, "DirectPlayEnumerateA"},
	{Burger::Windows::DPLAYX_DLL, "DirectPlayEnumerateW"},
	{Burger::Windows::DPLAYX_DLL, "DirectPlayLobbyCreateA"},
	{Burger::Windows::DPLAYX_DLL, "DirectPlayLobbyCreateW"},

	{Burger::Windows::RPCRT4_DLL, "UuidCreateSequential"},

	{Burger::Windows::WINMM_DLL, "timeGetTime"},

	{Burger::Windows::SHLWAPI_DLL, "PathSearchAndQualifyA"},
	{Burger::Windows::SHLWAPI_DLL, "PathSearchAndQualifyW"},

	{Burger::Windows::VERSION_DLL, "VerQueryValueA"},
	{Burger::Windows::VERSION_DLL, "VerQueryValueW"},
	{Burger::Windows::VERSION_DLL, "GetFileVersionInfoA"},
	{Burger::Windows::VERSION_DLL, "GetFileVersionInfoW"},
	{Burger::Windows::VERSION_DLL, "GetFileVersionInfoSizeA"},
	{Burger::Windows::VERSION_DLL, "GetFileVersionInfoSizeW"},

	{Burger::Windows::HID_DLL, "HidD_GetHidGuid"},

	{Burger::Windows::SETUPAPI_DLL, "SetupDiGetClassDevsA"},
	{Burger::Windows::SETUPAPI_DLL, "SetupDiGetClassDevsW"},
	{Burger::Windows::SETUPAPI_DLL, "SetupDiGetDeviceInterfaceDetailA"},
	{Burger::Windows::SETUPAPI_DLL, "SetupDiGetDeviceInterfaceDetailW"},
	{Burger::Windows::SETUPAPI_DLL, "SetupDiEnumDeviceInterfaces"},
	{Burger::Windows::SETUPAPI_DLL, "SetupDiDestroyDeviceInfoList"},

	{Burger::Windows::USER32_DLL, "TrackMouseEvent"},
	{Burger::Windows::USER32_DLL, "GetMonitorInfoA"},
	{Burger::Windows::USER32_DLL, "GetMonitorInfoW"},
	{Burger::Windows::USER32_DLL, "MonitorFromWindow"},
	{Burger::Windows::USER32_DLL, "MonitorFromRect"},

	{Burger::Windows::KERNEL32_DLL, "GetSystemWow64DirectoryA"},
	{Burger::Windows::KERNEL32_DLL, "GetSystemWow64DirectoryW"},
	{Burger::Windows::KERNEL32_DLL, "IsDebuggerPresent"},

	{Burger::Windows::SHELL32_DLL, "SHGetKnownFolderPath"}};

/*! ************************************

	\struct Burger::Windows
	\brief Functions exclusive to Microsoft Windows

	\windowsonly

	\sa Burger::Globals

***************************************/

/*! ************************************

	\enum Burger::Windows::eVersion
	\brief Flags set by \ref TestVersion()

	\windowsonly

	\sa TestVersion()

***************************************/

/*! ************************************

	\enum Burger::Windows::eDLLIndex
	\brief DLL enumeration for loading in system DLLs

	\windowsonly

	\sa LoadLibraryIndex()

***************************************/

/*! ************************************

	\enum Burger::Windows::eCallIndex
	\brief Function enumeration for loading in system calls

	\windowsonly

	\sa LoadFunctionIndex()

***************************************/

/*! ************************************

	\brief Clean up globals

	If any global resources were allocated at runtime, this function will
	release all resources before application shutdown

	\note This should not be called by an application directly.

***************************************/

Burger::Windows::~Windows()
{
	// Was DirectInput8 instantiated?
	if (m_pDirectInput8W) {
		m_pDirectInput8W->Release();
		m_pDirectInput8W = nullptr;
	}

	// Was DirectInput instantiated?
	if (m_pDirectInputW) {
		m_pDirectInputW->Release();
		m_pDirectInputW = nullptr;
	}

	// Dispose of all resolved calls to Windows
	MemoryClear(m_pWindowsCalls, sizeof(m_pWindowsCalls));
	MemoryClear(m_bFunctionsTested, sizeof(m_bFunctionsTested));

	// Finally, release all of the allocated DLLs
	WordPtr i = 0;
	do {
		if (m_hInstances[i]) {
			FreeLibrary(m_hInstances[i]);
			m_hInstances[i] = nullptr;
		}
		m_bInstancesTested[i] = FALSE;
	} while (++i < DLL_COUNT);
}

/*! ************************************

	\brief Test all versions of windows

	Test for which version of windows the application is running under and set
	the flags accordingly. This will set the global \ref
	Windows::m_bVersionFlags.

	\note This function currently returns the windows version of 8 when running
	under Windows 10 if the application doesn't have a Windows 10 manifest.

	\windowsonly

	\return Returns the value of \ref Windows::m_bVersionFlags

	\sa IsWin95orWin98(void), IsWinXPOrGreater(void) or IsVistaOrGreater(void)

***************************************/

Word BURGER_API Burger::Windows::TestVersion(void) BURGER_NOEXCEPT
{
	Word uResult = g_Globals.m_bVersionFlags; // Get the value
	// Was it already tested?
	if (!(uResult & VERSION_TESTED)) {
		uResult = VERSION_TESTED;

		// Initialize the version structure
		OSVERSIONINFOW OSVersionInfo;
		MemoryClear(&OSVersionInfo, sizeof(OSVersionInfo));
		OSVersionInfo.dwOSVersionInfoSize = sizeof(OSVersionInfo);

		// Read in the version information
		if (GetVersionExW(&OSVersionInfo)) {
			// Is this Windows 95/98?!?!?
			if (OSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
				// Holy cow! Set and end testing
				uResult = VERSION_TESTED | VERSION_9598;

				// NT tech (Which is pretty much everything since windows 98)

			} else if (OSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) {

				// Cache the versions
				Word uMajor = OSVersionInfo.dwMajorVersion;
				Word uMinor = OSVersionInfo.dwMinorVersion;

				// Test for XP
				if (uMajor >= 5) {
					if (uMinor >= 1) {
						// 5.1 = XP
						uResult |= VERSION_XPORGREATER;
					}

					// Try Vista
					if (uMajor >= 6) {
						// 6.0 = Vista
						uResult |= VERSION_VISTAORGREATER;

						// Try Windows 7
						if ((uMajor > 6) || (uMinor >= 1)) {
							// 6.1 = 7
							uResult |= VERSION_7ORGREATER;

							// Ooh... Windows 8?
							if ((uMajor > 6) || (uMinor >= 2)) {
								// 6.2 = 8
								uResult |= VERSION_8ORGREATER;

								// Microsoft. You suck.

								// uMinor only returns 10 if the exe is
								// manifested to be compatible for Windows 10 or
								// higher, otherwise it will return 8 (6.2)
								// because they suck

								// Hack to bypass this feature, query the
								// registry key that has the REAL version of the
								// operating system

								HKEY hKey = nullptr;
								// Open the pre-existing registry key
								LONG lStatus = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
									"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
									0, KEY_QUERY_VALUE, &hKey);

								if (lStatus == ERROR_SUCCESS) {

									DWORD uMajorEx = 0;
									DWORD uLength = sizeof(DWORD);
									lStatus = RegQueryValueExA(hKey,
										"CurrentMajorVersionNumber", nullptr,
										nullptr,
										static_cast<BYTE*>(
											static_cast<void*>(&uMajorEx)),
										&uLength);

									RegCloseKey(hKey);

									// Test for Windows 10 or higher
									if (uMajorEx >= 10) {
										uResult |= VERSION_10ORGREATER;
									}
								}
							}
						}
					}
				}
			}
		}

		// Store the final result
		g_Globals.m_bVersionFlags = static_cast<Word8>(uResult);
	}
	// Return the value as is
	return uResult;
}

/*! ************************************

	\fn Word Burger::Windows::IsWin95orWin98(void)
	\brief Detect if running on an ancient version of windows

	Test if the system is a pre-NT Windows operating system. If it returns \ref
	FALSE, it's an NT kernel (XP, Vista, 7, 8 ...)

	\windowsonly

	\return Returns \ref TRUE if Windows 3.1, 95, or 98.

	\sa TestVersion(void), IsWinXPOrGreater(void) or
		IsVistaOrGreater(void)

***************************************/

/*! ************************************

	\fn Word Burger::Windows::IsWinXPOrGreater(void)
	\brief Detect if running Windows XP or higher

	Test if the system is a Windows XP operating system or greater. If it
	returns \ref FALSE, it's before Windows XP

	\windowsonly

	\return Returns \ref TRUE if Windows XP or greater

	\sa TestVersion(void), IsWin95orWin98(void) or IsVistaOrGreater(void)

***************************************/

/*! ************************************

	\fn Word Burger::Windows::IsVistaOrGreater(void)
	\brief Detect if running Windows Vista or higher

	Test if the system is a Windows Vista operating system or greater. If it
	returns \ref FALSE, it's before Windows Vista (Usually XP)

	\windowsonly

	\return Returns \ref TRUE if Windows Vista or greater

	\sa TestVersion(void), IsWin95orWin98(void) or IsWinXPOrGreater(void)

***************************************/

/*! ************************************

	\fn Word Burger::Windows::IsWin7OrGreater(void)
	\brief Detect if running Windows 7 or higher

	Test if the system is a Windows 7 operating system or greater. If it returns
	\ref FALSE, it's before Windows 7

	\windowsonly

	\return Returns \ref TRUE if Windows 7 or greater

	\sa TestVersion(void), IsVistaOrGreater(void) or IsWinXPOrGreater(void)

***************************************/

/*! ************************************

	\fn Word Burger::Windows::IsWin8OrGreater(void)
	\brief Detect if running Windows 8 or higher

	Test if the system is a Windows 8 operating system or greater. If it returns
	\ref FALSE, it's before Windows 8

	\windowsonly

	\return Returns \ref TRUE if Windows 8 or greater

	\sa TestVersion(void), IsWin7OrGreater(void), IsVistaOrGreater(void) or
		IsWinXPOrGreater(void)

***************************************/

/*! ************************************

	\fn Word Burger::Windows::IsWin10OrGreater(void)
	\brief Detect if running Windows 10 or higher

	Test if the system is a Windows 10 operating system or greater. If it
	returns \ref FALSE, it's before Windows 10

	\note Windows 10 requires a manifest linked into the application that
	notifies Windows that the application is compatible with Windows 10. If
	there is no manifest, this function will always return \ref FALSE because
	Windows identifies itself as Windows 8.

	\windowsonly

	\return Returns \ref TRUE if Windows 10 or greater

	\sa TestVersion(void), IsWin8OrGreater(void), IsWin7OrGreater(void),
		IsVistaOrGreater(void) or IsWinXPOrGreater(void)

***************************************/

/*! ************************************

	\brief Load a library if needed

	Given a DLL index, detect if the library has already been loaded and if so,
	return the existing HINSTANCE, otherwise, load the DLL and if successful,
	return the HINSTANCE. If the load failed, return \ref NULL.

	\note Since this function only loads in official Microsoft DLLs, it will
	under Vista or higher, use the function
	LoadLibraryExA(LOAD_LIBRARY_SEARCH_SYSTEM32) to ensure that only official
	DLL directories are scanned to find the files. This was done to prevent
	Man-In-The-Middle attacks used by users who want to inject code to cheat.
	Windows versions before Vista (XP for example) doesn't support this flag, so
	the feature isn't used on those platforms.

	\windowsonly

	\param eIndex \ref eDLLIndex index to a DLL Burgerlib is tracking.

	\return \ref NULL if the DLL was not loaded, a valid HINSTANCE on success

	\sa LoadFunctionIndex()

***************************************/

HINSTANCE BURGER_API Burger::Windows::LoadLibraryIndex(eDLLIndex eIndex)
{
	HINSTANCE hResult = nullptr;
	// Valid index?
	if (eIndex < DLL_COUNT) {

		// Has it been loaded?
		hResult = g_Globals.m_hInstances[eIndex];

		// If not already tested and not loaded?
		if (!hResult && !g_Globals.m_bInstancesTested[eIndex]) {

			// Mark as tested
			g_Globals.m_bInstancesTested[eIndex] = TRUE;

			// For security reasons, force searching only in the official
			// windows folder to prevent a man-in-the-middle attack. Supported
			// on Windows Vista or later. If running on XP, you're out of luck.

			Word32 uFlags = 0; // XP Flags
			if (IsVistaOrGreater()) {
				uFlags = LOAD_LIBRARY_SEARCH_SYSTEM32;
			}

			// Load the most recent version of the DLL
			hResult = LoadLibraryExA(s_LibaryNames[eIndex], nullptr, uFlags);

			// Handle the special cases where if the most recent is not
			// available, try an older version of the dll
			if (!hResult) {

				eDLLIndex uAlternate = DLL_COUNT;

				// Try XInput 9.1.0
				if (eIndex == XINPUT1_4_DLL) {
					uAlternate = XINPUT1_3_DLL;

					// Try DirectPlay 1.0 (XP, and REALLY old)
				} else if (eIndex == DPLAYX_DLL) {
					uAlternate = DPLAY_DLL;
				}

				// Was an alternate chosen?
				if (uAlternate != DLL_COUNT) {
					hResult = LoadLibraryExA(
						s_LibaryNames[uAlternate], nullptr, uFlags);
				}

				// Here is where code should be added for future special casing
				// of DLL searching.
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

	Given a function index, detect if the library has already been loaded and if
	not. load it. If loaded, look up the function and return the pointer to the
	function or \ref NULL if not found.

	\windowsonly
	\param eIndex \ref eCallIndex index to a Windows function Burgerlib is
		tracking.

	\return \ref NULL if the DLL was not loaded or the function didn't exist, a
		valid function pointer on success

	\sa LoadLibraryIndex()

***************************************/

void* BURGER_API Burger::Windows::LoadFunctionIndex(eCallIndex eIndex)
{
	void* pResult = nullptr;
	// Valid index?
	if (eIndex < CALL_COUNT) {

		// Has the function been loaded?
		pResult = g_Globals.m_pWindowsCalls[eIndex];

		// Speed up in the form of disabling multiple tests
		if (!pResult && !g_Globals.m_bFunctionsTested[eIndex]) {

			// Mark as tested
			g_Globals.m_bFunctionsTested[eIndex] = TRUE;
			HINSTANCE hResult = LoadLibraryIndex(g_CallNames[eIndex].eDLL);
			if (hResult) {

				// Get the function from the DLL
				pResult = GetProcAddress(hResult, g_CallNames[eIndex].m_pName);
				g_Globals.m_pWindowsCalls[eIndex] = pResult;
			}
		}
	}
	return pResult;
}

/*! ************************************

	\fn Word Burger::Windows::IsDirectInputPresent(void)
	\brief Detect and load DirectInput functions

	\windowsonly

	\return \ref TRUE if DirectInput is present on the system, \ref FALSE if not

	\sa IsDirectInput8Present(void) or IsXInputPresent(void)

***************************************/

/*! ************************************

	\fn Word Burger::Windows::IsDirectInput8Present(void)
	\brief Detect and load DirectInput8 functions

	\windowsonly

	\return \ref TRUE if DirectInput8 is present on the system, \ref FALSE if
		not

	\sa IsDirectInputPresent(void) or IsXInputPresent(void)

***************************************/

/*! ************************************

	\fn Word Burger::Windows::IsXInputPresent(void)
	\brief Detect and load XInput functions

	\windowsonly

	\return \ref TRUE if XInput is present on the system, \ref FALSE if not

	\sa IsDirectInputPresent(void) or IsDirectInput8Present(void)

***************************************/

/*! ************************************

	\fn Word Burger::Windows::IsDirectDrawPresent(void)
	\brief Detect and load DirectDraw functions

	\windowsonly

	\return \ref TRUE if DirectDraw is present on the system, \ref FALSE if not

	\sa IsD3D9Present(void)

***************************************/

/*! ************************************

	\fn Word Burger::Windows::IsD3D9Present(void)
	\brief Detect and load D3D9 functions

	\windowsonly

	\return \ref TRUE if D3D9 is present on the system, \ref FALSE if not

	\sa IsDirectDrawPresent(void)

***************************************/

/*! ************************************

	\fn Word Burger::Windows::IsDirectSoundPresent(void)
	\brief Detect and load DirectSound functions

	\windowsonly

	\return \ref TRUE if DirectSound is present on the system, \ref FALSE if not

***************************************/

/*! ************************************

	\fn Word Burger::Windows::IsDirectPlayPresent(void)
	\brief Detect and load DirectPlay functions

	\windowsonly

	\return \ref TRUE if DirectPlay is present on the system, \ref FALSE if not

***************************************/

/*! ************************************

	\brief Detect if XAudio2 2.7 or higher is installed

	Test if XAudio2 2.7 or higher is installed and return TRUE if found.

	\windowsonly

	\return Returns \ref TRUE if the XAudio2 is available

***************************************/

Word BURGER_API Burger::Windows::IsXAudio2Present(void)
{
	// Start up CoInitialize() to allow creating instances
	Word bCleanupCOM = (CoInitialize(nullptr) >= 0);

	IXAudio2* pXAudio2 = nullptr;
	HRESULT hr = CoCreateInstance(CLSID_XAudio2, nullptr, CLSCTX_INPROC_SERVER,
		IID_IXAudio2, reinterpret_cast<void**>(&pXAudio2));

	// Did the call succeed?
	Word bResult = FALSE;
	if (hr >= 0) {
		// Release the instance
		pXAudio2->Release();
		bResult = TRUE;
	}

	//
	// If CoInitialize() was successful, release it
	//
	if (bCleanupCOM) {
		CoUninitialize();
	}
	return bResult;
}

#endif
