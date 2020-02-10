/***************************************

    Shims for version.dll

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

#include <WinVer.h>

typedef BOOL(APIENTRY* VerQueryValueAPtr)(
	LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID* lplpBuffer, PUINT puLen);
typedef BOOL(APIENTRY* VerQueryValueWPtr)(
	LPCVOID pBlock, LPCWSTR lpSubBlock, LPVOID* lplpBuffer, PUINT puLen);
typedef BOOL(APIENTRY* GetFileVersionInfoAPtr)(
	LPCSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData);
typedef BOOL(APIENTRY* GetFileVersionInfoWPtr)(
	LPCWSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData);
typedef DWORD(APIENTRY* GetFileVersionInfoSizeAPtr)(
	LPCSTR lptstrFilename, LPDWORD lpdwHandle);
typedef DWORD(APIENTRY* GetFileVersionInfoSizeWPtr)(
	LPCWSTR lptstrFilename, LPDWORD lpdwHandle);

// Unit tests for pointers
// While these tests fail on Codewarrior, it's okay because it's only a
// difference in const.

// VerQueryValueAPtr gVerQueryValueA = ::VerQueryValueA;
// VerQueryValueWPtr gVerQueryValueW = ::VerQueryValueW;
// GetFileVersionInfoAPtr gGetFileVersionInfoA = ::GetFileVersionInfoA;
// GetFileVersionInfoWPtr gGetFileVersionInfoW = ::GetFileVersionInfoW;
// GetFileVersionInfoSizeAPtr gGetFileVersionInfoSizeA =
// ::GetFileVersionInfoSizeA;
// GetFileVersionInfoSizeWPtr gGetFileVersionInfoSizeW =
// ::GetFileVersionInfoSizeW;

#endif

//
// version.dll
//

/*! ************************************

	\brief Load in version.dll and call VerQueryValueA

	Manually load version.dll if needed and call the Windows function
	VerQueryValueA()

	http://msdn.microsoft.com/en-us/library/windows/desktop/ms647464(v=vs.85).aspx

	\windowsonly

	\param pBlock The version-information resource returned by the
		GetFileVersionInfo function. \param pSubBlock A pointer to the
		version-information value to be retrieved.
	\param ppBuffer A pointer that contains the address of a pointer to the
		requested version information in the buffer pointed to by pBlock.
	\param pLen The size of the buffer pointed to by lplpBuffer, in bytes.

	\return Returns \ref TRUE if successful, or \ref FALSE otherwise.

***************************************/

Word BURGER_API Burger::Windows::VerQueryValueA(
	const void* pBlock, const char* pSubBlock, void** ppBuffer, Word* pLen)
{
	void* pVerQueryValueA = LoadFunctionIndex(CALL_VerQueryValueA);
	BOOL uResult = FALSE; // Failure
	if (pVerQueryValueA) {
		uResult = static_cast<VerQueryValueAPtr>(pVerQueryValueA)(
			pBlock, pSubBlock, ppBuffer, pLen);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in version.dll and call VerQueryValueW

	Manually load version.dll if needed and call the Windows function
	VerQueryValueW()

	http://msdn.microsoft.com/en-us/library/windows/desktop/ms647464(v=vs.85).aspx

	\windowsonly

	\param pBlock The version-information resource returned by the
		GetFileVersionInfo function.
	\param pSubBlock A pointer to the version-information value to be retrieved.
	\param ppBuffer A pointer that contains the address of a pointer to the
		requested version information in the buffer pointed to by pBlock.
	\param pLen The size of the buffer pointed to by lplpBuffer, in bytes.

	\return Returns \ref TRUE if successful, or \ref FALSE otherwise.

***************************************/

Word BURGER_API Burger::Windows::VerQueryValueW(
	const void* pBlock, const Word16* pSubBlock, void** ppBuffer, Word* pLen)
{
	void* pVerQueryValueW = LoadFunctionIndex(CALL_VerQueryValueW);
	BOOL uResult = FALSE; // Failure
	if (pVerQueryValueW) {
		uResult = static_cast<VerQueryValueWPtr>(pVerQueryValueW)(
			pBlock, reinterpret_cast<LPCWSTR>(pSubBlock), ppBuffer, pLen);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in version.dll and call GetFileVersionInfoA

	Manually load version.dll if needed and call the Windows function
	GetFileVersionInfoA()

	http://msdn.microsoft.com/en-us/library/windows/desktop/ms647003(v=vs.85).aspx

	\windowsonly

	\param ptstrFilename The name of the file
	\param dwHandle This parameter is ignored.
	\param dwLen The size, in bytes, of the buffer pointed to by the lpData
		parameter.
	\param pData Pointer to a buffer that receives the file-version information.

	\return Returns \ref TRUE if successful, or \ref FALSE otherwise.

***************************************/

Word BURGER_API Burger::Windows::GetFileVersionInfoA(
	const char* ptstrFilename, Word32 dwHandle, Word32 dwLen, void* pData)
{
	void* pGetFileVersionInfoA = LoadFunctionIndex(CALL_GetFileVersionInfoA);
	BOOL uResult = FALSE; // Failure
	if (pGetFileVersionInfoA) {
		uResult = static_cast<GetFileVersionInfoAPtr>(pGetFileVersionInfoA)(
			ptstrFilename, dwHandle, dwLen, pData);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in version.dll and call GetFileVersionInfoW

	Manually load version.dll if needed and call the Windows function
	GetFileVersionInfoW()

	http://msdn.microsoft.com/en-us/library/windows/desktop/ms647003(v=vs.85).aspx

	\windowsonly

	\param ptstrFilename The name of the file
	\param dwHandle This parameter is ignored.
	\param dwLen The size, in bytes, of the buffer pointed to by the lpData
		parameter.
	\param pData Pointer to a buffer that receives the file-version information.

	\return Returns \ref TRUE if successful, or \ref FALSE otherwise.

***************************************/

Word BURGER_API Burger::Windows::GetFileVersionInfoW(
	const Word16* ptstrFilename, Word32 dwHandle, Word32 dwLen, void* pData)
{
	void* pGetFileVersionInfoW = LoadFunctionIndex(CALL_GetFileVersionInfoW);
	BOOL uResult = FALSE; // Failure
	if (pGetFileVersionInfoW) {
		uResult = static_cast<GetFileVersionInfoWPtr>(pGetFileVersionInfoW)(
			reinterpret_cast<LPCWSTR>(ptstrFilename), dwHandle, dwLen, pData);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in version.dll and call GetFileVersionInfoA

	Manually load version.dll if needed and call the Windows function
	GetFileVersionInfoA()

	http://msdn.microsoft.com/en-us/library/windows/desktop/ms647005(v=vs.85).aspx

	\windowsonly

	\param ptstrFilename The name of the file of interest.
	\param pdwHandle A pointer to a variable that the function sets to zero.

	\return Returns the number of bytes if successful, or zero otherwise.

***************************************/

Word32 BURGER_API Burger::Windows::GetFileVersionInfoSizeA(
	const char* ptstrFilename, unsigned long* pdwHandle)
{
	void* pGetFileVersionInfoSizeA =
		LoadFunctionIndex(CALL_GetFileVersionInfoSizeA);
	Word uResult = 0; // Failure
	if (pGetFileVersionInfoSizeA) {
		uResult = static_cast<GetFileVersionInfoSizeAPtr>(
			pGetFileVersionInfoSizeA)(ptstrFilename, pdwHandle);
	}
	return uResult;
}

/*! ************************************

	\brief Load in version.dll and call GetFileVersionInfoSizeW

	Manually load version.dll if needed and call the Windows function
	GetFileVersionInfoSizeW()

	http://msdn.microsoft.com/en-us/library/windows/desktop/ms647005(v=vs.85).aspx

	\windowsonly

	\param ptstrFilename The name of the file of interest.
	\param pdwHandle A pointer to a variable that the function sets to zero.

	\return Returns the number of bytes if successful, or zero otherwise.

***************************************/

Word32 BURGER_API Burger::Windows::GetFileVersionInfoSizeW(
	const Word16* ptstrFilename, unsigned long* pdwHandle)
{
	void* pGetFileVersionInfoSizeW =
		LoadFunctionIndex(CALL_GetFileVersionInfoSizeW);
	Word uResult = 0; // Failure
	if (pGetFileVersionInfoSizeW) {
		uResult =
			static_cast<GetFileVersionInfoSizeWPtr>(pGetFileVersionInfoSizeW)(
				reinterpret_cast<LPCWSTR>(ptstrFilename), pdwHandle);
	}
	return uResult;
}

#endif
