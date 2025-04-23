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
#include "win_loadlibrary.h"
#include "win_registry.h"
#include "win_version.h"


#if !defined(DOXYGEN)

//
// Handle some annoying defines that some windows SDKs may or may not
// have
//

#if !defined(BUILD_WINDOWS)
#define BUILD_WINDOWS
#endif

#include "win_windows.h"

#if defined(BURGER_METROWERKS)
#pragma warn_emptydecl off
#endif

#if defined(BURGER_WATCOM)
// Disable nested comment found in comment (Direct X headers trigger this)
#pragma disable_message(15)
// Disable redefinition of the typedef name (DInput.h headers trigger this)
#pragma disable_message(583)
#endif

#include <MMSystem.h>

#include <SetupAPI.h>
#include <Xinput.h>
#include <d3d9.h>
#include <d3dcommon.h>
#include <ddraw.h>
#include <dinput.h>
#include <dsound.h>
#include <io.h>
#include <shellapi.h>
#include <shlobj.h>

#if defined(BURGER_METROWERKS)
#pragma warn_emptydecl reset
#endif

#if defined(BURGER_WATCOM)
// Disable nested comment found in comment (Direct X headers trigger this)
#pragma enable_message(15)
// Disable redefinition of the typedef name (DInput.h headers trigger this)
#pragma enable_message(583)
#endif

#endif // Allow doxygen

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
			string_copy(PathName + uPathLength, sizeof(PathName) - uPathLength,
				"\\QTIM32.DLL");
			DWORD uZeroLong = 0;
			DWORD uFileInfoSize =
				Win32::GetFileVersionInfoSizeA(PathName, &uZeroLong);
			const char* pQueryString =
				"\\StringFileInfo\\040904E4\\ProductVersion";
			// Any data?
			if (!uFileInfoSize) {
				// Try the location of Quicktime 4.0 and 5.0
				pQueryString = "\\StringFileInfo\\040904B0\\FileVersion";
				// Try Quicktime 4.0
				uZeroLong = 0;
				string_copy(PathName + uPathLength,
					sizeof(PathName) - uPathLength, "\\QuickTime.qts");
				uFileInfoSize =
					Win32::GetFileVersionInfoSizeA(PathName, &uZeroLong);
				if (!uFileInfoSize) {
					// Try the location of Quicktime 6.0 and later
					uPathLength = GetEnvironmentVariableA("ProgramFiles(x86)",
						PathName, BURGER_ARRAYSIZE(PathName));
					if (!uPathLength) {
						uPathLength = GetEnvironmentVariableA("ProgramFiles",
							PathName, BURGER_ARRAYSIZE(PathName));
					}
					if (uPathLength) {
						string_copy(PathName + uPathLength,
							sizeof(PathName) - uPathLength,
							"\\QuickTime\\QTSystem\\QuickTime.qts");
						uFileInfoSize = Win32::GetFileVersionInfoSizeA(
							PathName, &uZeroLong);
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
					if (Win32::GetFileVersionInfoA(
							PathName, 0, uFileInfoSize, pData)) {
						UINT ZeroWord = 0;
						if (Win32::VerQueryValueA(pData,
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
