/***************************************

    Shims for shlwapi.dll

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
#define _WIN32_WINNT 0x0501 // Windows XP
#endif

#include <Windows.h>

#include <Shlwapi.h>

typedef BOOL (STDAPICALLTYPE * PathSearchAndQualifyAPtr)(LPCSTR pszPath,LPSTR pszBuf,UINT cchBuf);
typedef BOOL (STDAPICALLTYPE * PathSearchAndQualifyWPtr)(LPCWSTR pszPath,LPWSTR pszBuf,UINT cchBuf);

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

Word BURGER_API Burger::Windows::PathSearchAndQualifyA(
	const char* pszPath,char* pszBuf,Word32 cchBuf)
{
	void* pPathSearchAndQualifyA = LoadFunctionIndex(CALL_PathSearchAndQualifyA);
	BOOL uResult = FALSE;
	if (pPathSearchAndQualifyA) {
		uResult = static_cast<PathSearchAndQualifyAPtr>(pPathSearchAndQualifyA)(pszPath,pszBuf,cchBuf);
	}
	return static_cast<Word>(uResult);
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

Word BURGER_API Burger::Windows::PathSearchAndQualifyW(
	const Word16* pszPath,Word16* pszBuf,Word32 cchBuf)
{
	void* pPathSearchAndQualifyW = LoadFunctionIndex(CALL_PathSearchAndQualifyW);
	BOOL uResult = FALSE;
	if (pPathSearchAndQualifyW) {
		uResult = static_cast<PathSearchAndQualifyWPtr>(pPathSearchAndQualifyW)(reinterpret_cast<LPCWSTR>(pszPath),reinterpret_cast<LPWSTR>(pszBuf),cchBuf);
	}
	return static_cast<Word>(uResult);
}

#endif
