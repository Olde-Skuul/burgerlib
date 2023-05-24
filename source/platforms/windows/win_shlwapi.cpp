/***************************************

	Shims for shlwapi.dll

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "win_shlwapi.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "win_platformshims.h"

#if !defined(DOXYGEN)

//
// Handle some annoying defines that some windows SDKs may or may not have
//

#include "win_windows.h"

#include <Shlwapi.h>

// Function prototypes, lovingly stolen from shlwapi.h
typedef BOOL(STDAPICALLTYPE* PathSearchAndQualifyAPtr)(
	LPCSTR pszPath, LPSTR pszBuf, UINT cchBuf);
typedef BOOL(STDAPICALLTYPE* PathSearchAndQualifyWPtr)(
	LPCWSTR pszPath, LPWSTR pszBuf, UINT cchBuf);

// Unit tests for pointers
// PathSearchAndQualifyAPtr gPathSearchAndQualifyA = ::PathSearchAndQualifyA;
// PathSearchAndQualifyWPtr gPathSearchAndQualifyW = ::PathSearchAndQualifyW;

#endif

//
// shlwapi.dll
//

/*! ************************************

	\brief Load in shlwapi.dll and call PathSearchAndQualifyA

	Manually load shlwapi.dll if needed and	call the Windows function
	PathSearchAndQualifyA()

	http://msdn.microsoft.com/en-us/library/windows/desktop/bb773751(v=vs.85).aspx

	\windowsonly

	\param pszPath A pointer to a null-terminated string of maximum length
		MAX_PATH that contains the path to search.
	\param pszBuf A pointer to a null-terminated string of length MAX_PATH that
		contains the path to be referenced.
	\param cchBuf The size of the buffer pointed to by pszBuf, in characters.

	\return Returns \ref TRUE if the path is qualified, or \ref FALSE otherwise.

***************************************/

BOOL BURGER_API Burger::Win32::PathSearchAndQualifyA(
	const char* pszPath, char* pszBuf, uint32_t cchBuf) BURGER_NOEXCEPT
{
	void* pPathSearchAndQualifyA = load_function(kCall_PathSearchAndQualifyA);

	// Assume failure
	BOOL uResult = FALSE;
	if (pPathSearchAndQualifyA) {
		uResult = static_cast<PathSearchAndQualifyAPtr>(pPathSearchAndQualifyA)(
			pszPath, pszBuf, cchBuf);
	}
	return uResult;
}

/*! ************************************

	\brief Load in shlwapi.dll and call PathSearchAndQualifyW

	Manually load shlwapi.dll if needed and call the Windows function
	PathSearchAndQualifyW()

	http://msdn.microsoft.com/en-us/library/windows/desktop/bb773751(v=vs.85).aspx

	\windowsonly

	\param pszPath A pointer to a null-terminated string of maximum length
		MAX_PATH that contains the path to search.
	\param pszBuf A pointer to a null-terminated string of length MAX_PATH that
		contains the path to be referenced.
	\param cchBuf The size of the buffer pointed to by pszBuf, in characters.

	\return Returns \ref TRUE if the path is qualified, or \ref FALSE otherwise.

***************************************/

BOOL BURGER_API Burger::Win32::PathSearchAndQualifyW(
	const uint16_t* pszPath, uint16_t* pszBuf, uint32_t cchBuf) BURGER_NOEXCEPT
{
	void* pPathSearchAndQualifyW = load_function(kCall_PathSearchAndQualifyW);

	// Assume failure
	BOOL uResult = FALSE;
	if (pPathSearchAndQualifyW) {
		uResult = static_cast<PathSearchAndQualifyWPtr>(pPathSearchAndQualifyW)(
			reinterpret_cast<LPCWSTR>(pszPath),
			reinterpret_cast<LPWSTR>(pszBuf), cchBuf);
	}
	return uResult;
}

#endif
