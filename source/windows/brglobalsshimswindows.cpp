/***************************************

    Windows Shim functions

    Functions that will allow calling Windows Vista/7/8/10 functions
    from a program that can be loaded on Windows XP.

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brglobals.h"
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brfilemanager.h"
#include "brstring.h"
#include "brstring16.h"
#include "brstringfunctions.h"
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
#include <SetupAPI.h>
#include <Xinput.h>
#include <d3d.h>
#include <d3dcommon.h>
#include <ddraw.h>
#include <dinput.h>
#include <dplay.h>
#include <dplay8.h>
#include <dplobby.h>
#include <dplobby8.h>
#include <dsound.h>
#include <io.h>
#include <shellapi.h>
#include <shlobj.h>
#include <xaudio2.h>

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

#endif // Allow doxygen


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

void BURGER_API Burger::Globals::GetQTFolderFromRegistry(
	const char* pSubKey, const char* pValueName, char* pBuffer, uint32_t uSize)
{
	if (uSize) {
		pBuffer[0] = 0;
		if (uSize >= 2) {
			// Clear out the output values
			HKEY pKey = NULL;

			// Open the key
			if (RegOpenKeyExA(
					HKEY_LOCAL_MACHINE, pSubKey, 0, KEY_QUERY_VALUE, &pKey)
				== ERROR_SUCCESS) {
				DWORD uLength = uSize - 1; // Space for the forced '\'

				// Read in the directory name and continue if there was data
				if ((RegQueryValueExA(pKey, pValueName, NULL, NULL,
						 static_cast<BYTE*>(static_cast<void*>(pBuffer)),
						 &uLength)
						== ERROR_SUCCESS)
					&& pBuffer[0]) {
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

uint_t BURGER_API Burger::Globals::GetPathToQuickTimeFolder(
	char* pBuffer, uint32_t uSize, uint32_t* pReserved) BURGER_NOEXCEPT
{
	char Temp[1024];
	uint_t bResult = FALSE;
	// Only execute if there's a valid output buffer
	if (pBuffer && uSize) {

		// Erase the output buffer
		pBuffer[0] = 0;

		// Set the reserved value if there was a passed pointer
		if (pReserved) {
			pReserved[0] = 0;
		}

		if (uSize >= 2) {

			// See if the module is already loaded
			HMODULE hQuickTime = GetModuleHandleA("QuickTime.qts");
			if (hQuickTime) {

				// Get the path from the loaded module
				if (GetModuleFileNameA(
						hQuickTime, Temp, BURGER_ARRAYSIZE(Temp))) {

					// Remove the string "Quicktime.qts"
					uintptr_t uStrLength = StringLength(Temp);
					if (uStrLength > 13) {
						// Copy up the string minus the ending "Quicktime.qts"
						// Note: This will end the string with a '\'
						StringCopy(pBuffer, uSize, Temp, uStrLength - 13);
					}
				}

				// hQuicktime doesn't need to be released.
			}

			// No path yet?
			if (!pBuffer[0]) {

				// Try grabbing from the registry
				GetQTFolderFromRegistry(
					"Software\\Apple Computer, Inc.\\QuickTime", "QTSysDir",
					pBuffer, uSize);

				// Detect if the directory exists
				if (pBuffer[0] && _access(pBuffer, 0)) {
					// Failed
					pBuffer[0] = 0;
				}
			}

			// No path yet?
			if (!pBuffer[0]) {

				// Try again, but using another key
				GetQTFolderFromRegistry(
					"Software\\Apple Computer, Inc.\\QuickTime",
					"QuickTime.qts folder", pBuffer, uSize);

				// Detect if the directory exists
				if (pBuffer[0] && _access(pBuffer, 0)) {
					// Failed
					pBuffer[0] = 0;
				}
			}

			// No path yet?
			if (!pBuffer[0]) {

				// Try the windows directory
				if (!GetSystemDirectoryA(pBuffer, uSize)
					|| _access(pBuffer, 0)) {
					pBuffer[0] = 0;
				}
			}

			// If something was found, ensure it ends with a slash

			if (pBuffer[0]) {
				EndWithWindowsSlashes(pBuffer);
			}
		}
		// Return TRUE if there was something in the buffer
		bResult = (pBuffer[0] != 0);
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

uint32_t BURGER_API Burger::Globals::GetQTSystemDirectoryA(
	char* pBuffer, uint32_t uSize) BURGER_NOEXCEPT
{
	GetPathToQuickTimeFolder(pBuffer, uSize, nullptr);
	return static_cast<uint32_t>(StringLength(pBuffer));
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

uint32_t BURGER_API Burger::Globals::GetQTApplicationDirectoryA(
	char* pBuffer, uint32_t uSize) BURGER_NOEXCEPT
{
	GetQTFolderFromRegistry("Software\\Apple Computer, Inc.\\QuickTime",
		"InstallDir", pBuffer, uSize);
	return static_cast<uint32_t>(StringLength(pBuffer));
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

uint32_t BURGER_API Burger::Globals::GetQTExtensionDirectoryA(
	char* pBuffer, uint32_t uSize) BURGER_NOEXCEPT
{
	char SystemDirName[256];
	char QTFolderName[256];

	// Try getting it from the registry
	GetQTFolderFromRegistry("Software\\Apple Computer, Inc.\\QuickTime",
		"QTExtDir", pBuffer, uSize);
	if (!pBuffer[0] && (uSize >= 11)) {

		// Didn't find it?
		uSize -= 10; // Make space for "Quicktime\"

		// Find where Quicktime should be
		GetPathToQuickTimeFolder(QTFolderName, uSize, NULL);

		if (QTFolderName[0]) {

			// Copy up the Quicktime folder name
			StringCopy(pBuffer, uSize, QTFolderName);

			// Read in the system directory name
			GetSystemDirectoryA(SystemDirName, uSize);
			EndWithWindowsSlashes(SystemDirName);

			// Was the folder found the Windows system folder?
			if (!StringCompare(SystemDirName, QTFolderName)) {
				// Since it's the Windows folder that's found, append the
				// string QuickTime\ to the end of the pathname
				// to denote the old location of where Quicktime
				// extensions were stored.
				StringConcatenate(pBuffer, "QuickTime\\");
			}
		}
	}
	return static_cast<uint32_t>(StringLength(pBuffer));
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

uint32_t BURGER_API Burger::Globals::GetQTComponentDirectoryA(
	char* pBuffer, uint32_t uSize) BURGER_NOEXCEPT
{
	GetQTFolderFromRegistry("Software\\Apple Computer, Inc.\\QuickTime",
		"QTComponentsDir", pBuffer, uSize);
	if (!pBuffer[0]) {
		GetSystemDirectoryA(pBuffer, uSize);
		if (pBuffer[0]) {
			EndWithWindowsSlashes(pBuffer);
			// Since it's the Windows folder that's found, append the
			// string QuickTime\ to the end of the pathname
			// to denote the old location of where Quicktime
			// extensions were stored.
			StringConcatenate(pBuffer, "QuickTime\\");
		}
	}
	return static_cast<uint32_t>(StringLength(pBuffer));
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

HINSTANCE BURGER_API Burger::Globals::QTLoadLibrary(const char* pDLLName) BURGER_NOEXCEPT
{
	char FinalPathname[1024];

	// Assume no DLL
	HINSTANCE pResult = NULL;
	const uintptr_t uDLLNameLength = StringLength(pDLLName);

	// Get the path for Quicktime
	if (GetPathToQuickTimeFolder(FinalPathname,
			static_cast<uint32_t>(
				BURGER_ARRAYSIZE(FinalPathname) - uDLLNameLength),
			NULL)) {
		StringConcatenate(FinalPathname, pDLLName);
		pResult = Windows::LoadLibraryA(FinalPathname);
		if (!pResult) {
			GetLastError();
			if (GetQTExtensionDirectoryA(FinalPathname,
					static_cast<uint32_t>(
						BURGER_ARRAYSIZE(FinalPathname) - uDLLNameLength))) {
				StringConcatenate(FinalPathname, pDLLName);
				pResult = Windows::LoadLibraryA(FinalPathname);
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

uint_t BURGER_API Burger::Globals::GetQuickTimeVersion(void) BURGER_NOEXCEPT
{
	char PathName[1024 + 32];

	if (!g_bQuickTimeVersionValid) {
		g_bQuickTimeVersionValid = TRUE; // I got the version

		uint_t uResult = 0; // I assume version 0!
		// Get the system directory for Quicktime
		uintptr_t uPathLength =
			GetSystemDirectoryA(PathName, BURGER_ARRAYSIZE(PathName) - 32);
		if (uPathLength) {

			// Get the Quicktime DLL using the old name for 2.0 or 3.0
			StringCopy(PathName + uPathLength, sizeof(PathName) - uPathLength,
				"\\QTIM32.DLL");
			DWORD uZeroLong = 0;
			DWORD uFileInfoSize = Windows::GetFileVersionInfoSizeA(PathName, &uZeroLong);
			const char* pQueryString =
				"\\StringFileInfo\\040904E4\\ProductVersion";
			// Any data?
			if (!uFileInfoSize) {
				// Try the location of Quicktime 4.0 and 5.0
				pQueryString = "\\StringFileInfo\\040904B0\\FileVersion";
				// Try Quicktime 4.0
				uZeroLong = 0;
				StringCopy(PathName + uPathLength,
					sizeof(PathName) - uPathLength, "\\QuickTime.qts");
				uFileInfoSize = Windows::GetFileVersionInfoSizeA(PathName, &uZeroLong);
				if (!uFileInfoSize) {
					// Try the location of Quicktime 6.0 and later
					uPathLength = GetEnvironmentVariableA("ProgramFiles(x86)",
						PathName, BURGER_ARRAYSIZE(PathName));
					if (!uPathLength) {
						uPathLength = GetEnvironmentVariableA("ProgramFiles",
							PathName, BURGER_ARRAYSIZE(PathName));
					}
					if (uPathLength) {
						StringCopy(PathName + uPathLength,
							sizeof(PathName) - uPathLength,
							"\\QuickTime\\QTSystem\\QuickTime.qts");
						uFileInfoSize =
							Windows::GetFileVersionInfoSizeA(PathName, &uZeroLong);
					}
				}
			}
			if (uFileInfoSize) {

				// Use HeapAlloc() instead of Alloc to allow code to use this
				// function without starting Burgerlib Memory

				// Get the data buffer
				HANDLE hHeap = GetProcessHeap();
				char* pData =
					static_cast<char*>(HeapAlloc(hHeap, 0, uFileInfoSize));
				if (pData) {
					void*
						pVersionData; // Main data pointer to start parsing from
					if (Windows::GetFileVersionInfoA(
							PathName, 0, uFileInfoSize, pData)) {
						UINT ZeroWord = 0;
						if (Windows::VerQueryValueA(pData,
								const_cast<char*>(pQueryString), &pVersionData,
								&ZeroWord)) {
							// Running ASCII pointer
							const char* pWorkPtr =
								static_cast<const char*>(pVersionData);
							uResult = AsciiToInteger(pWorkPtr, &pWorkPtr) << 8;
							if (pWorkPtr[0] == '.') {
								uFileInfoSize =
									AsciiToInteger(pWorkPtr + 1, &pWorkPtr);
								if (uFileInfoSize >= 16) {
									uFileInfoSize = 15;
								}
								uResult |= (uFileInfoSize << 4);
								if (pWorkPtr[0] == '.') {
									uFileInfoSize =
										AsciiToInteger(pWorkPtr + 1);
									if (uFileInfoSize >= 16) {
										uFileInfoSize = 15;
									}
									uResult |= uFileInfoSize;
								}
							}
						}
					}
					// Release the info pointer
					HeapFree(hHeap, 0, pData);
				}
			}
		}
		g_uQuickTimeVersion = uResult;
	}
	// Return the QuickTime version
	return g_uQuickTimeVersion;
}

#endif
