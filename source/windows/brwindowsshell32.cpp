/***************************************

	Shims for shell32.dll

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brwindowstypes.h"
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)

#if !defined(DOXYGEN)

//
// Handle some annoying defines that some windows SDKs may or may not have
//

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0601 // Windows XP
#endif

#include <Windows.h>

#include <ShlObj.h>

typedef HRESULT(WINAPI* SHGetKnownFolderPathPtr)(
	const GUID *rfid, DWORD dwFlags, HANDLE hToken, PWSTR* ppszPath);

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

Word BURGER_API Burger::Windows::SHGetKnownFolderPath(
	const GUID* pGuid, Word32 uFlags, void* hHandle, Word16** ppResult)
{
	void* pSHGetKnownFolderPath = LoadFunctionIndex(CALL_SHGetKnownFolderPath);
	HRESULT uResult = E_FAIL; // Failure
	if (pSHGetKnownFolderPath) {
		uResult = static_cast<SHGetKnownFolderPathPtr>(pSHGetKnownFolderPath)(
			pGuid, uFlags, hHandle, (PWSTR *)ppResult);
	}
	return static_cast<Word>(uResult);
}

#endif
