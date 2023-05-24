/***************************************

	Shims for shell32.dll

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "win_shell32.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brfilemanager.h"
#include "brstring16.h"
#include "win_platformshims.h"
#include "win_winutils.h"

#if defined(BURGER_WATCOM)
#pragma library("shfolder.lib")
#endif

#if !defined(DOXYGEN)

//
// Handle some annoying defines that some windows SDKs may or may not have
//

#include "win_windows.h"

#include <Shellapi.h>
#include <objbase.h>
#include <shlobj.h>

//
// GUIDs needed for locating folders in Vista or higher
//

#ifndef KF_FLAG_DONT_UNEXPAND
#define KF_FLAG_DONT_UNEXPAND 0x00002000
#endif

#ifndef KF_FLAG_DONT_VERIFY
#define KF_FLAG_DONT_VERIFY 0x00004000
#endif

// Function prototypes, lovingly stolen from shlobj.h
typedef HRESULT(WINAPI* SHGetKnownFolderPathPtr)(
	const GUID* rfid, DWORD dwFlags, HANDLE hToken, PWSTR* ppszPath);

// Unit tests for pointers

// SHGetKnownFolderPathPtr gSHGetKnownFolderPath = ::SHGetKnownFolderPath;

#endif

/*! ************************************

	\brief Load in shell32.dll and call SHGetKnownFolderPath

	Manually load shell32.dll if needed and call the Windows function
	SHGetKnownFolderPath()

	This function is present on Windows Vista or later.

	https://msdn.microsoft.com/en-us/library/windows/desktop/bb762188(v=vs.85).aspx

	\windowsonly

	\param pGuid A pointer to the KNOWNFOLDERID that identifies the folder.
	\param uFlags Flags that specify special retrieval options.
	\param hHandle An access token that represents a particular use
	\param ppResult When this method returns, contains the address of a pointer
		to a null-terminated Unicode string that specifies the path of the known
		folder

	\return Returns S_OK if successful, or an error value otherwise

***************************************/

HRESULT BURGER_API Burger::Win32::SHGetKnownFolderPath(const GUID* pGuid,
	uint32_t uFlags, void* hHandle, uint16_t** ppResult) BURGER_NOEXCEPT
{
	void* pSHGetKnownFolderPath = load_function(kCall_SHGetKnownFolderPath);

	// Failure
	HRESULT uResult = E_FAIL;
	if (pSHGetKnownFolderPath) {
		uResult = static_cast<SHGetKnownFolderPathPtr>(pSHGetKnownFolderPath)(
			pGuid, uFlags, hHandle, (PWSTR*)ppResult);
	}
	return uResult;
}

/*! ************************************

	\brief Adds a directory to the start folder.

	When installing an application, it may be desirable to create an entry in
	the start menu to show an application folder and links for
	files/applications of interest. This function locates the users start menu
	folder and ensures that the folder is created and Windows Explorer is
	notified of the change

	\param pGroupName UTF8 "C" string of the folder name for the start menu

	\windowsonly

	\return Returns non-zero if an error occurs

***************************************/

Burger::eError BURGER_API Burger::Win32::add_group_to_program_menu(
	const char* pGroupName) BURGER_NOEXCEPT
{
	// Get the pidl for the start menu
	// this will be used to initialize the folder browser

	eError uResult = kErrorFileNotFound;

	// Item list for the start menu
	// Use the XP call for backwards compatibilty
	ITEMIDLIST* pIDListStartMenu = nullptr;
	if (SHGetFolderLocation(get_window(), CSIDL_PROGRAMS, nullptr, 0,
			&pIDListStartMenu) == S_OK) {

		// Big buffer, because paranoia
		uint16_t WorkPath[MAX_PATH * 2];
		if (SHGetPathFromIDListW(
				pIDListStartMenu, reinterpret_cast<LPWSTR>(WorkPath))) {

			// Append a directory divider if needed
			StringEndsWith(WorkPath, '\\');

			// Append the new folder name
			String16 GroupName(pGroupName);
			StringConcatenate(WorkPath, sizeof(WorkPath), GroupName.c_str());

			// Convert to UTF8 for Burgerlib
			Filename TempPath;
			TempPath.set_native(WorkPath);

			// Create the directory
			uResult = FileManager::create_directory_path(&TempPath);
			if (!uResult) {
				// Notify the shell that this folder was updated
				// Use SHCNF_PATHW since WorkPath is UTF16
				SHChangeNotify(SHCNE_MKDIR, SHCNF_PATHW, WorkPath, nullptr);
			}
		}
	}

	// Release any allocated memory
	if (pIDListStartMenu) {
		ILFree(pIDListStartMenu);
	}
	return uResult;
}

/*! ************************************

	\brief Set the filename to the local machine preferences directory

	Given a Vista GUID and/or a Windows XP path ID for the calls
	SHGetKnownFolderPath() and SHGetFolderPathW() respectively, return the path
	found.

	The operating system if first checked if SHGetKnownFolderPath() is
	available, and if so, it will be called to get the path. Otherwise, if
	pVista is \ref nullptr or if the previous call failed, the XP version of the
	call is attempted to get the folder.

	If iXP is -1, then the XP path is never taken.

	\param pFilename Pointer to a Burgerlib Filename object for the output
	\param pVista Pointer to the GUID for SHGetKnownFolderPath()
	\param iXP CSIDL_* for SHGetFolderPathW()

	\return Zero on success, non-zero on error

***************************************/

Burger::eError BURGER_API Burger::Win32::get_known_folder_path(
	Filename* pFilename, const GUID* pVista, int iXP) BURGER_NOEXCEPT
{
	eError uResult = kErrorNotADirectory;

	// Try the code for Vista or higher if needed
	if (pVista) {

		// This function will return E_FAIL on Windows XP
		uint16_t* pResult = nullptr;
		const HRESULT hResult = Win32::SHGetKnownFolderPath(pVista,
			KF_FLAG_DONT_UNEXPAND | KF_FLAG_DONT_VERIFY, nullptr, &pResult);
		if (hResult == S_OK) {

			// All good! Use this pathname!
			uResult = pFilename->set_native(pResult);

			// Release the pointer
			CoTaskMemFree(pResult);
		}
	}

	// Failed or never attempted above, give XP a shot
	if (uResult && (iXP != -1)) {

		// Try it for Windows XP instead
		WCHAR NameBuffer[MAX_PATH];

		// Application system data folder (Local for Vista and Win7)
		const HRESULT hResult2 =
			SHGetFolderPathW(nullptr, iXP, nullptr, 0, NameBuffer);

		if ((hResult2 == S_OK) || (hResult2 == E_FAIL)) {

			// All good! Use this pathname!
			uResult = pFilename->set_native(
				reinterpret_cast<const uint16_t*>(NameBuffer));
		}
	}

	// Was there an error?
	if (uResult) {

		// Zap the output
		pFilename->clear();
	}
	return uResult;
}

#endif
