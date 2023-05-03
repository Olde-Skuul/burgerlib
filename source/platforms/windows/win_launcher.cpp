/***************************************

	Application launchers for Windows

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "win_launcher.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brfilemanager.h"
#include "brglobals.h"
#include "brstring16.h"

#if !defined(DOXYGEN)

//
// Handle some annoying defines that some windows SDKs may or may not
// have
//

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0501 // Windows XP
#endif

#include <Windows.h>

#include <shellapi.h>

#endif

/*! ************************************

	\brief Call ShellExecuteW() with a UTF8 string

	Convert the input string from UTF-8 encoding and call
	ShellExecuteW(NULL,"open",pFileToOpen,nullptr,nullptr,SW_SHOWNORMAL)

	This function will return the result code without modification, a value of
	33 or higher means the function executed successfully.

	\windowsonly

	\param pFileToOpen UTF-8 encoded string to convert to use as input for
		ShellExecuteW()

	\return Returned value from the call to ShellExecuteW(), cast as a
		uintptr_t

***************************************/

uintptr_t BURGER_API Burger::Win32::ShellExecuteOpen(
	const char* pFileToOpen) BURGER_NOEXCEPT
{
	String16 Data16(pFileToOpen);
	HINSTANCE uResult =
		ShellExecuteW(nullptr, reinterpret_cast<LPCWSTR>(L"open"),
			reinterpret_cast<LPCWSTR>(Data16.GetPtr()), nullptr, nullptr,
			SW_SHOWNORMAL);

	// Note, even though it's cast as an HINSTANCE, it's really a uintptr_t
	return reinterpret_cast<uintptr_t>(uResult);
}

/*! ************************************

	\brief Launch the Media Center

	Locate the exe file ehshell.exe in the windows folder
	and execute it.

	\note As of Windows 10, this function is obsolete. Please do
	not expect this function to successfully execute on Windows 10
	platforms.

	\windowsonly
	\return Zero if media center was successfully launched, non-zero on error.

***************************************/

uint_t BURGER_API Burger::Win32::LaunchMediaCenter(void) BURGER_NOEXCEPT
{
	const char* pString = GetEnvironmentString("SystemRoot");

	// Assume error
	uint_t uResult = 10;
	if (pString) {
		Filename MediaCenterName;
		MediaCenterName.set_native(pString);

		// Release the environment string
		Free(pString);

		// Append the filename of the media center
		MediaCenterName.join("ehome:ehshell.exe");

		// See if the file exists
		if (FileManager::does_file_exist(&MediaCenterName)) {

			// If the returned value is higher then 32, it was successful
			if (ShellExecuteOpen(MediaCenterName.get_native()) > 32U) {
				uResult = 0;
			}
		}
	}
	return uResult;
}

#endif
