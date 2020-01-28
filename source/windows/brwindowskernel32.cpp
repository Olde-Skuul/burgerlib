/***************************************

	Shims for kernel32.dll

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

typedef UINT(WINAPI* GetSystemWow64DirectoryAPtr)(LPSTR lpBuffer, UINT uSize);
typedef UINT(WINAPI* GetSystemWow64DirectoryWPtr)(LPWSTR lpBuffer, UINT uSize);
typedef BOOL(WINAPI* IsDebuggerPresentPtr)(VOID);

// Unit tests for pointers

// GetSystemWow64DirectoryAPtr gGetSystemWow64DirectoryA =
//	::GetSystemWow64DirectoryA;
// GetSystemWow64DirectoryWPtr gGetSystemWow64DirectoryW =
//	::GetSystemWow64DirectoryW;
// IsDebuggerPresentPtr gIsDebuggerPresent = ::IsDebuggerPresent;

#endif

//
// kernel32.dll
//

/*! ************************************

	\brief Load in kernel32.dll and call GetSystemWow64DirectoryA

	Manually load kernel32.dll if needed and call the Windows function
	GetSystemWow64DirectoryA()

	https://msdn.microsoft.com/en-us/library/windows/desktop/ms724405(v=vs.85).aspx

	\windowsonly

	\param pBuffer A pointer to the buffer to receive the path
	\param uSize The maximum size of the buffer, in bytes

	\return If the function succeeds, the return value is the length, in bytes,
		of the string copied to the buffer

***************************************/

Word BURGER_API Burger::Windows::GetSystemWow64DirectoryA(
	char* pBuffer, Word32 uSize)
{
	void* pGetSystemWow64DirectoryA =
		LoadFunctionIndex(CALL_GetSystemWow64DirectoryA);
	Word uResult = 0; // Failure
	if (!pGetSystemWow64DirectoryA) {
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	} else {
		uResult = static_cast<GetSystemWow64DirectoryAPtr>(
			pGetSystemWow64DirectoryA)(pBuffer, uSize);
	}
	return uResult;
}

/*! ************************************

	\brief Load in kernel32.dll and call GetSystemWow64DirectoryW

	Manually load kernel32.dll if needed and call the Windows function
	GetSystemWow64DirectoryW()

	https://msdn.microsoft.com/en-us/library/windows/desktop/ms724405(v=vs.85).aspx

	\windowsonly

	\param pBuffer A pointer to the buffer to receive the path
	\param uSize The maximum size of the buffer, in Word16 entries

	\return If the function succeeds, the return value is the length, in Word16
		entries, of the string copied to the buffer

***************************************/

Word BURGER_API Burger::Windows::GetSystemWow64DirectoryW(
	Word16* pBuffer, Word32 uSize)
{
	void* pGetSystemWow64DirectoryW =
		LoadFunctionIndex(CALL_GetSystemWow64DirectoryW);
	Word uResult = 0; // Failure
	if (!pGetSystemWow64DirectoryW) {
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	} else {
		uResult =
			static_cast<GetSystemWow64DirectoryWPtr>(pGetSystemWow64DirectoryW)(
				reinterpret_cast<LPWSTR>(pBuffer), uSize);
	}
	return uResult;
}

/*! ************************************

	\brief Load in kernel32.dll and call IsDebuggerPresent

	Manually load kernel32.dll if needed and call the Windows function
	IsDebuggerPresent()

	https://msdn.microsoft.com/en-us/library/windows/desktop/ms680345(v=vs.85).aspx

	\windowsonly

	\return Nonzero if a debugger is present, Zero if none is found

***************************************/

Word BURGER_API Burger::Windows::IsDebuggerPresent(void)
{
	void* pIsDebuggerPresent = LoadFunctionIndex(CALL_IsDebuggerPresent);
	BOOL uResult = 0; // None
	if (pIsDebuggerPresent) {
		uResult = static_cast<IsDebuggerPresentPtr>(pIsDebuggerPresent)();
	}
	return static_cast<Word>(uResult);
}

#endif
