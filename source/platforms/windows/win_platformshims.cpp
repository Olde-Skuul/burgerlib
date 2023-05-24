/***************************************

	System function loader specific to Windows

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "win_platformshims.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brmemoryfunctions.h"
#include "win_directxdata.h"
#include "win_kernel32.h"
#include "win_version.h"

// Windows defines for the windows headers
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

#endif

#include <stdlib.h>

#include "win_windows.h"

// Must be included AFTER windows.h
#include <xaudio2.h>

// A fix for atexit() callback warning
#if defined(BURGER_WATCOM)
// converted function type has different #pragma from original function type
#pragma warning 601 9
#endif

/***************************************

	DLLs that can be dynamically loaded at runtime (So the application can
	launch if they are missing or missing functions)

***************************************/

#if !defined(DOXYGEN)
//
// These filenames MUST match Win32::eDLLIndex
//

static const char* s_LibraryNames[Burger::Win32::kDLLIndexCount] = {"ddraw.dll",
	"dinput.dll", "dinput8.dll", "xinput1_4.dll", "xinput1_3.dll", "d3d9.dll",
	"d3dx9_43.dll", "d3d11.dll", "dxgi.dll", "dsound.dll", "dplayx.dll",
	"dplay.dll", "rpcrt4.dll", "winmm.dll", "shlwapi.dll", "version.dll",
	"hid.dll", "setupapi.dll", "user32.dll", "kernel32.dll", "shell32.dll"};

// Instances of dynamically loaded system DLLs
static HINSTANCE s_hInstances[Burger::Win32::kDLLIndexCount];

// Flags to determine if a DLL was tested for loading
// This is needed because testing for nullptr isn't good enough since if the DLL
// is missing, it must be nullptr. Therefore use this flag to determine if the
// test was performed
static uint8_t s_bInstancesTested[Burger::Win32::kDLLIndexCount];

// Set to true if the atexit() function was added
static uint8_t s_bAtExit;

#if defined(BURGER_WIN32)
// Non-zero if tested, low bit has \ref TRUE or \ref FALSE if the 32 bit app was
// running under 64 bit windows
static uint8_t s_bIsWindows64Bit;
#endif

//
// Internal structure for DLL function lookup
//

struct CallNames_t {
	Burger::Win32::eDLLIndex m_uDLL; // Enumeration for the DLL
	const char* m_pName;             // Function name
};

static const CallNames_t s_CallNames[Burger::Win32::kCallIndexCount] = {
	{Burger::Win32::kDLLIndex_DINPUT, "DirectInputCreateA"},
	{Burger::Win32::kDLLIndex_DINPUT, "DirectInputCreateW"},

	{Burger::Win32::kDLLIndex_DINPUT8, "DirectInput8Create"},

	{Burger::Win32::kDLLIndex_XINPUT1_4, "XInputGetState"},
	{Burger::Win32::kDLLIndex_XINPUT1_4, "XInputSetState"},
	{Burger::Win32::kDLLIndex_XINPUT1_4, "XInputGetCapabilities"},
	{Burger::Win32::kDLLIndex_XINPUT1_4, "XInputGetDSoundAudioDeviceGuids"},
	{Burger::Win32::kDLLIndex_XINPUT1_4, "XInputEnable"},
	{Burger::Win32::kDLLIndex_XINPUT1_4, "XInputGetAudioDeviceIds"},
	{Burger::Win32::kDLLIndex_XINPUT1_4, "XInputGetBatteryInformation"},
	{Burger::Win32::kDLLIndex_XINPUT1_4, "XInputGetKeystroke"},

	{Burger::Win32::kDLLIndex_DDRAW, "DirectDrawCreate"},
	{Burger::Win32::kDLLIndex_DDRAW, "DirectDrawCreateEx"},
	{Burger::Win32::kDLLIndex_DDRAW, "DirectDrawCreateClipper"},
	{Burger::Win32::kDLLIndex_DDRAW, "DirectDrawEnumerateA"},
	{Burger::Win32::kDLLIndex_DDRAW, "DirectDrawEnumerateW"},
	{Burger::Win32::kDLLIndex_DDRAW, "DirectDrawEnumerateExA"},
	{Burger::Win32::kDLLIndex_DDRAW, "DirectDrawEnumerateExW"},

	{Burger::Win32::kDLLIndex_D3D9, "Direct3DCreate9"},
	{Burger::Win32::kDLLIndex_D3D9, "D3DPERF_BeginEvent"},
	{Burger::Win32::kDLLIndex_D3D9, "D3DPERF_EndEvent"},
	{Burger::Win32::kDLLIndex_D3D9, "D3DPERF_SetMarker"},
	{Burger::Win32::kDLLIndex_D3D9, "D3DPERF_SetRegion"},
	{Burger::Win32::kDLLIndex_D3D9, "D3DPERF_QueryRepeatFrame"},
	{Burger::Win32::kDLLIndex_D3D9, "D3DPERF_SetOptions"},
	{Burger::Win32::kDLLIndex_D3D9, "D3DPERF_GetStatus"},

	{Burger::Win32::kDLLIndex_D3DX9_43, "D3DXCreateMatrixStack"},

	{Burger::Win32::kDLLIndex_D3D11, "D3D11CreateDevice"},

	{Burger::Win32::kDLLIndex_DXGI, "CreateDXGIFactory"},
	{Burger::Win32::kDLLIndex_DXGI, "CreateDXGIFactory1"},
	{Burger::Win32::kDLLIndex_DXGI, "CreateDXGIFactory2"},

	{Burger::Win32::kDLLIndex_DSOUND, "DirectSoundCreate"},
	{Burger::Win32::kDLLIndex_DSOUND, "DirectSoundEnumerateA"},
	{Burger::Win32::kDLLIndex_DSOUND, "DirectSoundEnumerateW"},
	{Burger::Win32::kDLLIndex_DSOUND, "DirectSoundCaptureCreate"},
	{Burger::Win32::kDLLIndex_DSOUND, "DirectSoundCaptureEnumerateA"},
	{Burger::Win32::kDLLIndex_DSOUND, "DirectSoundCaptureEnumerateW"},
	{Burger::Win32::kDLLIndex_DSOUND, "DirectSoundCreate8"},
	{Burger::Win32::kDLLIndex_DSOUND, "DirectSoundCaptureCreate8"},
	{Burger::Win32::kDLLIndex_DSOUND, "DirectSoundFullDuplexCreate"},
	{Burger::Win32::kDLLIndex_DSOUND, "GetDeviceID"},

	{Burger::Win32::kDLLIndex_DPLAYX, "DirectPlayCreate"},
	{Burger::Win32::kDLLIndex_DPLAYX, "DirectPlayEnumerate"},
	{Burger::Win32::kDLLIndex_DPLAYX, "DirectPlayEnumerateA"},
	{Burger::Win32::kDLLIndex_DPLAYX, "DirectPlayEnumerateW"},
	{Burger::Win32::kDLLIndex_DPLAYX, "DirectPlayLobbyCreateA"},
	{Burger::Win32::kDLLIndex_DPLAYX, "DirectPlayLobbyCreateW"},

	{Burger::Win32::kDLLIndex_RPCRT4, "UuidCreateSequential"},

	{Burger::Win32::kDLLIndex_WINMM, "timeGetTime"},

	{Burger::Win32::kDLLIndex_SHLWAPI, "PathSearchAndQualifyA"},
	{Burger::Win32::kDLLIndex_SHLWAPI, "PathSearchAndQualifyW"},

	{Burger::Win32::kDLLIndex_VERSION, "VerQueryValueA"},
	{Burger::Win32::kDLLIndex_VERSION, "VerQueryValueW"},
	{Burger::Win32::kDLLIndex_VERSION, "GetFileVersionInfoA"},
	{Burger::Win32::kDLLIndex_VERSION, "GetFileVersionInfoW"},
	{Burger::Win32::kDLLIndex_VERSION, "GetFileVersionInfoSizeA"},
	{Burger::Win32::kDLLIndex_VERSION, "GetFileVersionInfoSizeW"},

	{Burger::Win32::kDLLIndex_HID, "HidD_GetHidGuid"},

	{Burger::Win32::kDLLIndex_SETUPAPI, "SetupDiGetClassDevsA"},
	{Burger::Win32::kDLLIndex_SETUPAPI, "SetupDiGetClassDevsW"},
	{Burger::Win32::kDLLIndex_SETUPAPI, "SetupDiGetDeviceInterfaceDetailA"},
	{Burger::Win32::kDLLIndex_SETUPAPI, "SetupDiGetDeviceInterfaceDetailW"},
	{Burger::Win32::kDLLIndex_SETUPAPI, "SetupDiEnumDeviceInterfaces"},
	{Burger::Win32::kDLLIndex_SETUPAPI, "SetupDiDestroyDeviceInfoList"},

	{Burger::Win32::kDLLIndex_USER32, "TrackMouseEvent"},
	{Burger::Win32::kDLLIndex_USER32, "GetMonitorInfoA"},
	{Burger::Win32::kDLLIndex_USER32, "GetMonitorInfoW"},
	{Burger::Win32::kDLLIndex_USER32, "MonitorFromWindow"},
	{Burger::Win32::kDLLIndex_USER32, "MonitorFromRect"},

	{Burger::Win32::kDLLIndex_KERNEL32, "GetSystemWow64DirectoryA"},
	{Burger::Win32::kDLLIndex_KERNEL32, "GetSystemWow64DirectoryW"},
	{Burger::Win32::kDLLIndex_KERNEL32, "IsDebuggerPresent"},
	{Burger::Win32::kDLLIndex_KERNEL32, "SetThreadDescription"},

	{Burger::Win32::kDLLIndex_SHELL32, "SHGetKnownFolderPath"}};

// Pointers to resolved windows function calls
static void* s_pWindowsCalls[Burger::Win32::kCallIndexCount];

// Flags to determine if a function was tested for loading
static uint8_t s_bFunctionsTested[Burger::Win32::kCallIndexCount];

/*! ************************************

	\brief Release loaded DLLs

	If any DLLs were dynamically loaded by load_library(), then this
	function on application exit, will release all DLLs with a calls to
	FreeLibrary()

	\note Only called via atexit()

	\windowsonly

	\sa load_library()

***************************************/

static void BURGER_ANSIAPI release_libraries(void)
{
	// Finally, release all of the allocated DLLs
	uintptr_t i = 0;
	do {
		if (s_hInstances[i]) {
			FreeLibrary(s_hInstances[i]);
			s_hInstances[i] = nullptr;
		}
		s_bInstancesTested[i] = FALSE;
	} while (++i < BURGER_ARRAYSIZE(s_hInstances));

	// Dispose of all resolved calls to Windows
	Burger::MemoryClear(s_pWindowsCalls, sizeof(s_pWindowsCalls));
	Burger::MemoryClear(s_bFunctionsTested, sizeof(s_bFunctionsTested));
}

#endif

/*! ************************************

	\enum Burger::Win32::eDLLIndex
	\brief DLL enumeration for loading in system DLLs

	\windowsonly

	\sa load_library()

***************************************/

/*! ************************************

	\enum Burger::Win32::eCallIndex
	\brief Function enumeration for loading in system calls

	\windowsonly

	\sa load_function()

***************************************/

/*! ************************************

	\brief Load a Windows library if needed

	Given a DLL index, detect if the library has already been loaded and if so,
	return the existing HINSTANCE, otherwise, load the DLL and if successful,
	return the HINSTANCE. If the load failed, return \ref nullptr.

	\note Since this function only loads in official Microsoft DLLs, it will
	under Vista or higher, use the function
	LoadLibraryExA(LOAD_LIBRARY_SEARCH_SYSTEM32) to ensure that only official
	DLL directories are scanned to find the files. This was done to prevent
	Man-In-The-Middle attacks used by users who want to inject code to cheat.
	Windows versions before Vista (XP for example) doesn't support this flag, so
	the feature isn't used on those platforms.

	\windowsonly

	\param uIndex \ref eDLLIndex index to a DLL Burgerlib is tracking.

	\return \ref nullptr if the DLL was not loaded, a valid HINSTANCE on success

	\sa load_function()

***************************************/

HINSTANCE BURGER_API Burger::Win32::load_library(
	eDLLIndex uIndex) BURGER_NOEXCEPT
{
	HINSTANCE hResult = nullptr;
	// Valid index?
	if (static_cast<uint_t>(uIndex) < kDLLIndexCount) {

		// Has it been loaded?
		hResult = s_hInstances[uIndex];

		// If not already tested?
		if (!s_bInstancesTested[uIndex]) {

			// Mark as tested
			s_bInstancesTested[uIndex] = TRUE;

			// For security reasons, force searching only in the official
			// windows folder to prevent a man-in-the-middle attack. Supported
			// on Windows Vista or later. If running on XP, you're out of luck.

			// XP Flags
			uint32_t uFlags = 0;
			if (is_vista_or_higher()) {
				uFlags = LOAD_LIBRARY_SEARCH_SYSTEM32;
			}

			// Load the most recent version of the DLL
			hResult = LoadLibraryExA(s_LibraryNames[uIndex], nullptr, uFlags);

			// Handle the special cases where if the most recent is not
			// available, try an older version of the dll
			if (!hResult) {

				eDLLIndex uAlternate = kDLLIndexCount;

				// Try XInput 9.1.0
				if (uIndex == kDLLIndex_XINPUT1_4) {
					uAlternate = kDLLIndex_XINPUT1_3;

					// Try DirectPlay 1.0 (XP, and REALLY old)
				} else if (uIndex == kDLLIndex_DPLAYX) {
					uAlternate = kDLLIndex_DPLAY;
				}

				// Was an alternate chosen?
				if (uAlternate != kDLLIndexCount) {
					hResult = LoadLibraryExA(
						s_LibraryNames[uAlternate], nullptr, uFlags);
				}

				// Here is where code should be added for future special casing
				// of DLL searching.
			}

			// If it loaded fine, save the result
			if (hResult) {
				s_hInstances[uIndex] = hResult;

				// Since a library was loaded, add the function to release the
				// resources on application exit
				if (!s_bAtExit) {
					s_bAtExit = TRUE;
					atexit(release_libraries);
				}
			}
		}
	}
	return hResult;
}

/*! ************************************

	\brief Load a function from a DLL library if needed

	Given a function index, detect if the library has already been loaded and if
	not. load it. If loaded, look up the function and return the pointer to the
	function or \ref nullptr if not found.

	\windowsonly

	\param uIndex \ref eCallIndex index to a Windows function Burgerlib is
		tracking.

	\return \ref nullptr if the DLL was not loaded or the function didn't exist,
		a valid function pointer on success

	\sa load_library(), or override_function()

***************************************/

void* BURGER_API Burger::Win32::load_function(eCallIndex uIndex) BURGER_NOEXCEPT
{
	void* pResult = nullptr;
	// Valid index?
	if (static_cast<uint_t>(uIndex) < kCallIndexCount) {

		// Has the function been loaded?
		pResult = s_pWindowsCalls[uIndex];

		// Speed up in the form of disabling multiple tests
		if (!s_bFunctionsTested[uIndex]) {

			// Mark as tested
			s_bFunctionsTested[uIndex] = TRUE;

			// Get the library
			HINSTANCE hResult = load_library(s_CallNames[uIndex].m_uDLL);
			if (hResult) {

				// Get the function from the DLL
				pResult = GetProcAddress(hResult, s_CallNames[uIndex].m_pName);

				// Get the pointer, can be nullptr
				s_pWindowsCalls[uIndex] = pResult;
			}
		}
	}
	return pResult;
}

/*! ************************************

	\brief Override a Windows function shim

	In some cases, a function pointer is not available from windows because the
	call doesn't exist on the version of windows running. This call allows
	substitute code to be injected that would mimic the behavior of the function
	so high level code can still operate.

	\windowsonly

	\param uIndex \ref eCallIndex index to a Windows function Burgerlib is
		tracking.
	\param pFunction Pointer to the function to use as an override.

	\sa load_library(), or override_function()

***************************************/

void BURGER_API Burger::Win32::override_function(
	eCallIndex uIndex, void* pFunction) BURGER_NOEXCEPT
{
	// Save the function and mark as "active"
	s_pWindowsCalls[uIndex] = pFunction;
	s_bFunctionsTested[uIndex] = TRUE;
}

/*! ************************************

	\brief Detect if DirectInput exists

	\windowsonly

	\return \ref TRUE if DirectInput is present on the system, \ref FALSE if not

	\sa has_DirectInput8(void) or has_XInput(void)

***************************************/

uint_t BURGER_API Burger::Win32::has_DirectInput(void) BURGER_NOEXCEPT
{
	return load_library(kDLLIndex_DINPUT) != nullptr;
}

/*! ************************************

	\brief Detect if DirectInput8 exists

	\windowsonly

	\return \ref TRUE if DirectInput8 is present on the system, \ref FALSE if
		not

	\sa has_DirectInput(void) or has_XInput(void)

***************************************/

uint_t BURGER_API Burger::Win32::has_DirectInput8(void) BURGER_NOEXCEPT
{
	return load_library(kDLLIndex_DINPUT8) != nullptr;
}

/*! ************************************

	\brief Detect if XInput exists

	\windowsonly

	\return \ref TRUE if XInput is present on the system, \ref FALSE if not

	\sa has_DirectInput(void) or has_DirectInput8(void)

***************************************/

uint_t BURGER_API Burger::Win32::has_XInput(void) BURGER_NOEXCEPT
{
	return load_library(kDLLIndex_XINPUT1_4) != nullptr;
}

/*! ************************************

	\brief Detect if DirectDraw exists

	\windowsonly

	\return \ref TRUE if DirectDraw is present on the system, \ref FALSE if not

	\sa has_D3D9(void)

***************************************/

uint_t BURGER_API Burger::Win32::has_DirectDraw(void) BURGER_NOEXCEPT
{
	return load_library(kDLLIndex_DDRAW) != nullptr;
}

/*! ************************************

	\brief Detect if Direct3D 9 exists

	\windowsonly

	\return \ref TRUE if D3D9 is present on the system, \ref FALSE if not

	\sa has_DirectDraw(void)

***************************************/

uint_t BURGER_API Burger::Win32::has_D3D9(void) BURGER_NOEXCEPT
{
	return load_library(kDLLIndex_D3D9) != nullptr;
}

/*! ************************************

	\brief Detect if DirectSound exists

	\windowsonly

	\return \ref TRUE if DirectSound is present on the system, \ref FALSE if not

***************************************/

uint_t BURGER_API Burger::Win32::has_DirectSound(void) BURGER_NOEXCEPT
{
	return load_library(kDLLIndex_DSOUND) != nullptr;
}

/*! ************************************

	\brief Detect if DirectPlay exists

	\windowsonly

	\return \ref TRUE if DirectPlay is present on the system, \ref FALSE if not

***************************************/

uint_t BURGER_API Burger::Win32::has_DirectPlay(void) BURGER_NOEXCEPT
{
	return load_library(kDLLIndex_DPLAYX) != nullptr;
}

/*! ************************************

	\brief Detect if XAudio2 2.7 or higher is installed

	Test if XAudio2 2.7 or higher is installed and return TRUE if found.

	\windowsonly

	\return Returns \ref TRUE if the XAudio2 is available

***************************************/

uint_t BURGER_API Burger::Win32::has_XAudio2(void) BURGER_NOEXCEPT
{
	// Start up CoInitializeEx() to allow creating instances
	const uint_t bCleanupCOM =
		(CoInitializeEx(nullptr, COINIT_MULTITHREADED) >= 0);

	IXAudio2* pXAudio2 = nullptr;
	const HRESULT hResult =
		CoCreateInstance(CLSID_XAudio2, nullptr, CLSCTX_INPROC_SERVER,
			IID_IXAudio2_2_7, reinterpret_cast<void**>(&pXAudio2));

	// Did the call succeed?
	uint_t bResult = FALSE;
	if (hResult >= 0) {
		// Release the instance
		pXAudio2->Release();
		bResult = TRUE;
	}

	//
	// If CoInitializeEx() was successful, release it
	//
	if (bCleanupCOM) {
		CoUninitialize();
	}
	return bResult;
}

/*! ************************************

	\brief Detect if a 32 bit windows app is running under 64 bit windows

	If an application is 32 bits, the operating system is tested if it's
	actually 64 bits. If so, the function will return \ref TRUE. If the
	application is 64 bits, or if the operating system is 32 bit then this
	function will return \ref FALSE.

	\windowsonly

	\return Returns \ref TRUE if the 32 bit application was running in 64 bit
		Windows

	\sa GetSystemWow64DirectoryW(uint16_t *,uint32_t)

***************************************/

uint_t BURGER_API Burger::Win32::is_windows64Bit(void) BURGER_NOEXCEPT
{
#if !defined(BURGER_WIN32)
	return TRUE;
#else
	uint_t bResult = s_bIsWindows64Bit;

	// Was it already tested?
	if (!(bResult & 0x80U)) {

		uint16_t Temp[MAX_PATH];
		if ((Win32::GetSystemWow64DirectoryW(Temp, BURGER_ARRAYSIZE(Temp)) ==
				0) &&
			(GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)) {

			// Not present? This is a 32 bit version of Windows
			bResult = 0x80 | FALSE;
		} else {
			// The 32 bit app is running in a 64 bit version of Windows
			bResult = 0x80 | TRUE;
		}
		s_bIsWindows64Bit = static_cast<uint8_t>(bResult);
	}
	// Return the value minus the other flags
	return bResult & 1U;
#endif
}

#endif
