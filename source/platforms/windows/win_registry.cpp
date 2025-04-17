/***************************************

	Registry functions for Windows

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "win_registry.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brstring.h"
#include "brstring16.h"
#include "brstringfunctions.h"
#include "win_loadlibrary.h"

#include <io.h>

#include "win_windows.h"

#if !defined(DOXYGEN)
// String for registry
static const char s_SoftwareClasses[] = "Software\\Classes\\";
#endif

/*! ************************************

	\brief Set a user registry key with a string

	Strings are all UTF-8. This function will perform conversion to UTF-16 for
	Windows

	\windowsonly

	\param pKey Key found in HKEY_CURRENT_USER
	\param pSubKey Name of the sub-key of interest, can be \ref nullptr
	\param pData String to store in the registry
	\return Zero if successful, non-zero is the Windows error code.
	\sa AssociateFileExtensionToExe(const char *,const char *,const char *)

***************************************/

LONG BURGER_API Burger::Win32::create_user_registry_key(
	const char* pKey, const char* pSubKey, const char* pData) BURGER_NOEXCEPT
{
	// Convert from UTF8 to UTF16 for Windows
	String16 KeyUTF16(pKey);
	HKEY hKey = nullptr;

	// Create the registry key
	LONG lStatus = RegCreateKeyExW(HKEY_CURRENT_USER,
		reinterpret_cast<LPWSTR>(KeyUTF16.c_str()), 0, 0,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &hKey, nullptr);

	if (lStatus == ERROR_SUCCESS) {

		const wchar_t* pSub16 = nullptr;
		// SubKeyUTF16 has to remain in scope
		String16 SubKeyUTF16(pSubKey);
		if (SubKeyUTF16.length()) {
			pSub16 = reinterpret_cast<const wchar_t*>(SubKeyUTF16.c_str());
		}

		String16 DataUTF16(pData);
		lStatus = RegSetValueExW(hKey, reinterpret_cast<LPCWSTR>(pSub16), 0,
			REG_SZ, reinterpret_cast<const BYTE*>(DataUTF16.c_str()),
			(static_cast<DWORD>(DataUTF16.length()) + 1) * 2);
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
	// Tell Windows Explorer to launch .datafile files with the currently
	// running app by double-clicking

	associate_file_extension_to_exe(".datafile",
		"Data for the Fubar application","com.oldskuul.fubar");

	\endcode

	\param pFileExtension ".foo" Pointer to a "C" string with the file extension
	\param pDescription "Foo file" Description of this file type to be visible
		to Explorer
	\param pProgramID "com.company.application" program ID

	\sa create_user_registry_key(const char *,const char *,const char *)

***************************************/

void BURGER_API Burger::Win32::associate_file_extension_to_exe(
	const char* pFileExtension, const char* pDescription,
	const char* pProgramID) BURGER_NOEXCEPT
{
	// Create the keys for the file extension and the description to show in
	// Explorer
	{
		// Create the key for the file extension itself. .foo -> Unique program
		// ID
		String ClassExtension(s_SoftwareClasses, pFileExtension);
		create_user_registry_key(ClassExtension.c_str(), nullptr, pProgramID);
	}

	{
		// Create the key for the unique program ID, with the file's description
		String ClassExtension(s_SoftwareClasses, pProgramID);
		create_user_registry_key(ClassExtension.c_str(), nullptr, pDescription);
	}

	// With the program ID already requested, generate the app's location for
	// the ID and the sample command line to use if you "drag and drop" a file
	// on the exe.
	uint16_t TempBuffer[MAX_PATH];

	// Get the pathname to the currently running application
	if (GetModuleFileNameW(nullptr, reinterpret_cast<LPWSTR>(TempBuffer),
			MAX_PATH) < MAX_PATH) {
		String ExePath(TempBuffer);
		{
			// Create the key and command to launch on double click
			String ClassShellOpen(
				s_SoftwareClasses, pProgramID, "\\shell\\open\\command");
			String Command("\"", ExePath.c_str(), "\" \"%1\"");
			create_user_registry_key(
				ClassShellOpen.c_str(), nullptr, Command.c_str());
		}
		{
			// Create the key and reference to the icon for the data file
			String ClassIcon(s_SoftwareClasses, pProgramID, "\\DefaultIcon");
			String Command2("\"", ExePath.c_str(), "\",1");
			create_user_registry_key(
				ClassIcon.c_str(), nullptr, Command2.c_str());
		}
	}
}

/*! ************************************

	\brief Find a Quicktime folder by reading the registry

	This function is a functional equivalent to the Quicktime for
	Windows internal function GetQTFolderFromRegistry(). It will
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

Burger::eError BURGER_API Burger::Win32::GetQTFolderFromRegistry(
	const char* pSubKey, const char* pValueName, char* pBuffer,
	uint32_t uSize) BURGER_NOEXCEPT
{
	eError uResult = kErrorBufferTooSmall;
	if (uSize) {
		pBuffer[0] = 0;
		if (uSize >= 2) {

			// Clear out the output values
			HKEY pKey = nullptr;

			uResult = kErrorPathNotFound;
			// Open the key
			if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, pSubKey, 0, KEY_QUERY_VALUE,
					&pKey) == ERROR_SUCCESS) {

				// Space for the forced '\'
				DWORD uLength = uSize - 1;

				uResult = kErrorBufferTooSmall;
				// Read in the directory name and continue if there was data
				if ((RegQueryValueExA(pKey, pValueName, nullptr, nullptr,
						 static_cast<BYTE*>(static_cast<void*>(pBuffer)),
						 &uLength) == ERROR_SUCCESS) &&
					pBuffer[0]) {

					uResult = kErrorNone;
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
	return uResult;
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
	\param pReserved Reserved, pass \ref nullptr

	\return \ref TRUE if successful, \ref FALSE if not

***************************************/

uint_t BURGER_API Burger::Win32::GetPathToQuickTimeFolder(
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
					uintptr_t uStrLength = string_length(Temp);
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
				if (!GetSystemDirectoryA(pBuffer, uSize) ||
					_access(pBuffer, 0)) {
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

uint32_t BURGER_API Burger::Win32::GetQTSystemDirectoryA(
	char* pBuffer, uint32_t uSize) BURGER_NOEXCEPT
{
	GetPathToQuickTimeFolder(pBuffer, uSize, nullptr);
	return static_cast<uint32_t>(string_length(pBuffer));
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

uint32_t BURGER_API Burger::Win32::GetQTApplicationDirectoryA(
	char* pBuffer, uint32_t uSize) BURGER_NOEXCEPT
{
	GetQTFolderFromRegistry("Software\\Apple Computer, Inc.\\QuickTime",
		"InstallDir", pBuffer, uSize);
	return static_cast<uint32_t>(string_length(pBuffer));
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

uint32_t BURGER_API Burger::Win32::GetQTExtensionDirectoryA(
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
		GetPathToQuickTimeFolder(QTFolderName, uSize, nullptr);

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
	return static_cast<uint32_t>(string_length(pBuffer));
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

uint32_t BURGER_API Burger::Win32::GetQTComponentDirectoryA(
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
	return static_cast<uint32_t>(string_length(pBuffer));
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

	\return HINSTANCE of the DLL or \ref nullptr on failure

***************************************/

HINSTANCE BURGER_API Burger::Win32::QTLoadLibrary(
	const char* pDLLName) BURGER_NOEXCEPT
{
	char FinalPathname[1024];

	// Assume no DLL
	HINSTANCE pResult = nullptr;
	const uintptr_t uDLLNameLength = string_length(pDLLName);

	// Get the path for Quicktime
	if (GetPathToQuickTimeFolder(FinalPathname,
			static_cast<uint32_t>(
				BURGER_ARRAYSIZE(FinalPathname) - uDLLNameLength),
			nullptr)) {

		StringConcatenate(FinalPathname, pDLLName);
		pResult = Win32::LoadLibraryA(FinalPathname);
		if (!pResult) {
			GetLastError();
			if (GetQTExtensionDirectoryA(FinalPathname,
					static_cast<uint32_t>(
						BURGER_ARRAYSIZE(FinalPathname) - uDLLNameLength))) {
				StringConcatenate(FinalPathname, pDLLName);
				pResult = Win32::LoadLibraryA(FinalPathname);
			}
		}
	}
	return pResult;
}

#endif
