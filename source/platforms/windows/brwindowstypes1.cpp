/***************************************

    Windows helper functions

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brwindowstypes.h"
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brcommandparameterstring.h"
#include "brfilemanager.h"
#include "brfilename.h"
#include "brglobals.h"
#include "brstring.h"
#include "brstring16.h"

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

#include <Windows.h>

#include <ShellAPI.h>

#endif

HINSTANCE Burger::Windows::g_hInstance;

/*! ************************************

	\fn Burger::Windows::GetInstance()
	\brief Get the application instance

	\windowsonly

	\return Instance set by SetInstance()

	\sa SetInstance()

***************************************/

/*! ************************************

	\fn Burger::Windows::SetInstance(HINSTANCE__ *)
	\brief Set the application instance

	Upon application startup, an instance is assigned, use this function to
	allow Burgerlib to use this instance in other parts of the library.

	\windowsonly

	\param pInput Instance of the application

	\sa GetInstance()

***************************************/

/*! ************************************

	\brief Call LoadLibraryA() without file error boxes

	When LoadLibraryA() is called in windows, it's possible that if the file is
	not found, windows will display an error message box mentioning that a DLL
	is missing. This function will prohibit this behavior by setting the
	ErrorMode to SEM_NOOPENFILEERRORBOX before the call to LoadLibraryA() and
	restoring the flag to the previous setting before function exit.

	https://msdn.microsoft.com/en-us/library/windows/desktop/ms684175(v=vs.85).aspx

	\windowsonly

	\param pInput ASCII pathname of the DLL file to load.

	\return \ref NULL if the DLL was not loaded, a valid HINSTANCE on success

	\sa LoadLibraryExA() or LoadLibraryW()

***************************************/

HINSTANCE BURGER_API Burger::Windows::LoadLibraryA(const char* pInput)
{
	// Disable user interactive dialogs
	UINT uOldMode =
		SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);

	HINSTANCE hResult = ::LoadLibraryA(pInput);

	// Restore the dialog state
	SetErrorMode(uOldMode);
	return hResult;
}

/*! ************************************

	\brief Call LoadLibraryW() without file error boxes

	When LoadLibraryW() is called in windows, it's possible that if the file is
	not found, windows will display an error message box mentioning that a DLL
	is missing. This function will prohibit this behavior by setting the
	ErrorMode to SEM_NOOPENFILEERRORBOX before the call to LoadLibraryW() and
	restoring the flag to the previous setting before function exit.

	https://msdn.microsoft.com/en-us/library/windows/desktop/ms684175(v=vs.85).aspx

	\windowsonly

	\param pInput UTF16 pathname of the DLL file to load.

	\return \ref NULL if the DLL was not loaded, a valid HINSTANCE on success

	\sa LoadLibraryExW() or LoadLibraryA()

***************************************/

HINSTANCE BURGER_API Burger::Windows::LoadLibraryW(const uint16_t* pInput)
{
	// Disable user interactive dialogs
	UINT uOldMode =
		SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);

	HINSTANCE hResult = ::LoadLibraryW(reinterpret_cast<LPCWSTR>(pInput));

	// Restore the dialog state
	SetErrorMode(uOldMode);
	return hResult;
}

/*! ************************************

    \brief Call LoadLibraryExA() without file error boxes

    When LoadLibraryExA() is called in windows, it's possible that if the file
    is not found, windows will display an error message box mentioning that a
    DLL is missing. This function will prohibit this behavior by setting the
    ErrorMode to SEM_NOOPENFILEERRORBOX before the call to LoadLibraryExA() and
    restoring the flag to the previous setting before function exit.

    https://msdn.microsoft.com/en-us/library/windows/desktop/ms684179(v=vs.85).aspx

    \windowsonly

    \param pInput ASCII pathname of the DLL file to load.
    \param hFile This parameter is reserved for future use. It must be \ref
        nullptr.
    \param uFlags The action to be taken when loading the module.

    \return \ref NULL if the DLL was not loaded, a valid HINSTANCE on success

    \sa LoadLibraryA() or LoadLibraryExW()

***************************************/

HINSTANCE BURGER_API Burger::Windows::LoadLibraryExA(
	const char* pInput, void* hFile, uint32_t uFlags)
{
	// Disable user interactive dialogs
	UINT uOldMode =
		SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);

	HINSTANCE hResult = ::LoadLibraryExA(pInput, hFile, uFlags);

	// Restore the dialog state
	SetErrorMode(uOldMode);
	return hResult;
}

/*! ************************************

	\brief Call LoadLibraryExW() without file error boxes

	When LoadLibraryExW() is called in windows, it's possible that if the file
	is not found, windows will display an error message box mentioning that a
	DLL is missing. This function will prohibit this behavior by setting the
	ErrorMode to SEM_NOOPENFILEERRORBOX before the call to LoadLibraryExW() and
	restoring the flag to the previous setting before function exit.

	https://msdn.microsoft.com/en-us/library/windows/desktop/ms684179(v=vs.85).aspx

	\windowsonly

	\param pInput UTF16 pathname of the DLL file to load.
	\param hFile This parameter is reserved for future use. It must be \ref
		NULL.
	\param uFlags The action to be taken when loading the module.

	\return \ref NULL if the DLL was not loaded, a valid HINSTANCE on success

	\sa LoadLibraryExA() or LoadLibraryW()

***************************************/

HINSTANCE BURGER_API Burger::Windows::LoadLibraryExW(
	const uint16_t* pInput, void* hFile, uint32_t uFlags)
{
	// Disable user interactive dialogs
	UINT uOldMode =
		SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);

	HINSTANCE hResult =
		::LoadLibraryExW(reinterpret_cast<LPCWSTR>(pInput), hFile, uFlags);

	// Restore the dialog state
	SetErrorMode(uOldMode);
	return hResult;
}

#if 0
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


Burger::eError BURGER_API Burger::Windows::GetPathFromRegistry(
	HKEY hKey, const char* pSubKey, const char* pValueName, Filename* pOutput)
{
	// BECKY NOT FINISHED
	eError uError = kErrorInvalidParameter;
	if (pOutput) {
		pOutput->Clear();

		// Clear out the output values
		HKEY pKey = NULL;

		// Open the main key
		String16 ToWide(pSubKey);
		String foo;
		LSTATUS uStatus = RegOpenKeyExW(
			hKey, (LPCWSTR)ToWide.GetPtr(), 0, KEY_QUERY_VALUE, &pKey);
		if (uStatus == ERROR_SUCCESS) {

			// Read in the directory name and continue if there was data
			ToWide = pValueName;
			BYTE Temp[1024];
			DWORD uLength = sizeof(Temp);
			DWORD uType = 0;
			uStatus = RegQueryValueExW(pKey, (LPCWSTR)ToWide.GetPtr(), nullptr,
				&uType, Temp, &uLength);
			if ((uStatus == ERROR_SUCCESS) &&
				Temp[0]) {
				// Force the last character to a '\'
				EndWithWindowsSlashes((char *)Temp);
				pOutput->set_native((const uint16_t *)Temp);
			}
		}

		// Release the key, if loaded
		if (pKey) {
			RegCloseKey(pKey);
		}
	}
	return uError;
}
#endif

/*! ************************************

	\brief Change the style flags of a windows

	Set and clear the style and extended style flags. The flags to clear will be
	bit flipped before applying an AND operation on the bits.

	\windowsonly

	\param hWindow HWND of the window
	\param uAddStyle Bits to set in the GWL_STYLE entry
	\param uAddStyleEx Bits to set in the GWL_EXSTYLE entry
	\param uSubStyle Bits to clear in the GWL_STYLE entry
	\param uSubStyleEx Bits to clear in the GWL_EXSTYLE entry

***************************************/

void Burger::Windows::ChangeStyle(HWND hWindow, uint32_t uAddStyle,
	uint32_t uAddStyleEx, uint32_t uSubStyle, uint32_t uSubStyleEx)
{
	SetWindowLongW(hWindow, GWL_STYLE,
		static_cast<LONG>(
			(GetWindowLongW(hWindow, GWL_STYLE) | uAddStyle) & (~uSubStyle)));
	SetWindowLongW(hWindow, GWL_EXSTYLE,
		static_cast<LONG>((GetWindowLongW(hWindow, GWL_EXSTYLE) | uAddStyleEx) &
			(~uSubStyleEx)));
}

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

uintptr_t BURGER_API Burger::Windows::ShellExecuteOpen(const char* pFileToOpen)
{
	String16 Data16(pFileToOpen);
	HINSTANCE uResult = ShellExecuteW(NULL, reinterpret_cast<LPCWSTR>(L"open"),
		reinterpret_cast<LPCWSTR>(Data16.GetPtr()), nullptr, nullptr,
		SW_SHOWNORMAL);
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

uint_t BURGER_API Burger::Windows::LaunchMediaCenter(void)
{
	const char* pString = GetEnvironmentString("SystemRoot");
	uint_t uResult = 10; // Assume error
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
			if (ShellExecuteOpen(MediaCenterName.get_native()) > 32) {
				uResult = 0;
			}
		}
	}
	return uResult;
}

#endif
