/***************************************

	Function to test what version of Windows is present

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "win_version.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brmemoryfunctions.h"
#include "brstringfunctions.h"
#include "win_platformshims.h"

#if !defined(DOXYGEN)

// Win32::eVersion as set by get_version_flags().
static uint32_t s_uVersionFlags;

/// DirectX version 0x0900 (9.0) format
static uint32_t s_uDirectXVersion;
// \ref TRUE if DirectX's version is valid
static uint8_t s_bDirectXVersionValid;

#endif

#include "win_windows.h"

/*! ************************************

	\namespace Burger::Win32
	\brief Namespace for Windows specific functions and classes

	For functions and classes that only exist on the Windows platform, they are
	grouped in this namespace.

	\windowsonly

	\sa \ref PS4

***************************************/

/*! ************************************

	\enum Burger::Win32::eVersion
	\brief Flags set by get_version_flags()

	\windowsonly

	\sa get_version_flags()

***************************************/

/*! ************************************

	\brief Test all versions of windows

	Test for which version of windows the application is running under and set
	the flags accordingly. The value is cached, so all future calls are near
	instant.

	The flags are declared in \ref eVersion

	\note If the program doesn't have a manifest and it's running on Windows 10
		or higher, a test of the registry is performed to verify that the
		operating system is 10 or higher.

	\windowsonly

	\return Returns the flags with the versions of Windows found

	\sa is_XP3_or_higher(void), is_XP_or_higher(void) or
		is_10_or_higher(void)

***************************************/

uint32_t BURGER_API Burger::Win32::get_version_flags(void) BURGER_NOEXCEPT
{
	// Get the value
	uint32_t uResult = s_uVersionFlags;

	// Was it already tested?
	if (!(uResult & kVersionValid)) {

		// Create the mask to compare the major, minon and service pack versions
		const DWORDLONG uConditionMask = VerSetConditionMask(
			VerSetConditionMask(
				VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL),
				VER_MINORVERSION, VER_GREATER_EQUAL),
			VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

		// Initialize the version structure
		// Note: This structure hasn't changed since Visual Studio 2003, so the
		// size isn't an issue
		OSVERSIONINFOEXW OSVersionInfo;
		MemoryClear(&OSVersionInfo, sizeof(OSVersionInfo));
		OSVersionInfo.dwOSVersionInfoSize = sizeof(OSVersionInfo);

		// XP no service pack
		OSVersionInfo.dwMajorVersion = 5;
		OSVersionInfo.dwMinorVersion = 1;
		OSVersionInfo.wServicePackMajor = 0;
		uResult = kVersionValid;
		if (VerifyVersionInfoW(&OSVersionInfo,
				VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR,
				uConditionMask)) {
			uResult |= kVersionXPOrGreater;

			// XP Service pack 3
			OSVersionInfo.dwMajorVersion = 5;
			OSVersionInfo.dwMinorVersion = 1;
			OSVersionInfo.wServicePackMajor = 3;
			if (VerifyVersionInfoW(&OSVersionInfo,
					VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR,
					uConditionMask)) {
				uResult |= kVersionXP3OrGreater;

				// Fix Vista
				OSVersionInfo.dwMajorVersion = 6;
				OSVersionInfo.dwMinorVersion = 0;
				OSVersionInfo.wServicePackMajor = 0;
				if (VerifyVersionInfoW(&OSVersionInfo,
						VER_MAJORVERSION | VER_MINORVERSION |
							VER_SERVICEPACKMAJOR,
						uConditionMask)) {
					uResult |= kVersionVistaOrGreater;

					// Windows 7?

					OSVersionInfo.dwMajorVersion = 6;
					OSVersionInfo.dwMinorVersion = 1;
					OSVersionInfo.wServicePackMajor = 0;
					if (VerifyVersionInfoW(&OSVersionInfo,
							VER_MAJORVERSION | VER_MINORVERSION |
								VER_SERVICEPACKMAJOR,
							uConditionMask)) {
						uResult |= kVersion7OrGreater;

						// Let's try Windows 8

						OSVersionInfo.dwMajorVersion = 6;
						OSVersionInfo.dwMinorVersion = 2;
						OSVersionInfo.wServicePackMajor = 0;
						if (VerifyVersionInfoW(&OSVersionInfo,
								VER_MAJORVERSION | VER_MINORVERSION |
									VER_SERVICEPACKMAJOR,
								uConditionMask)) {
							uResult |= kVersion8OrGreater;

							// Go for broke! Windows 10?
							OSVersionInfo.dwMajorVersion = 10;
							OSVersionInfo.dwMinorVersion = 0;
							OSVersionInfo.wServicePackMajor = 0;
							if (VerifyVersionInfoW(&OSVersionInfo,
									VER_MAJORVERSION | VER_MINORVERSION |
										VER_SERVICEPACKMAJOR,
									uConditionMask)) {
								uResult |= kVersion10OrGreater;
							} else {

								// Microsoft. You suck.

								// The test above only returns 10 if the exe is
								// manifested to be compatible for Windows 10 or
								// higher, otherwise it will return 8 (6.2)
								// because they suck

								// Hack to bypass this feature, query the
								// registry key that has the REAL version of the
								// operating system

								HKEY hKey = nullptr;

								// Open the pre-existing registry key
								LONG lStatus = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
									"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
									0, KEY_QUERY_VALUE, &hKey);

								if (lStatus == ERROR_SUCCESS) {

									DWORD uMajorEx = 0;
									DWORD uLength = sizeof(DWORD);
									lStatus = RegQueryValueExA(hKey,
										"CurrentMajorVersionNumber", nullptr,
										nullptr,
										static_cast<BYTE*>(
											static_cast<void*>(&uMajorEx)),
										&uLength);

									RegCloseKey(hKey);

									// Test for Windows 10 or higher
									// For real, this time
									if (uMajorEx >= 10) {
										uResult |= kVersion10OrGreater;
									}
								}
							}
						}
					}
				}
			}
		}

		// Store the final result
		s_uVersionFlags = uResult;
	}
	// Return the value as is
	return uResult;
}

/*! ************************************

	\brief Detect if running Windows XP or higher

	Test if the system is a Windows XP operating system or greater. If it
	returns \ref FALSE, it's before Windows XP

	\windowsonly

	\return Returns \ref TRUE if Windows XP or greater

	\sa get_version_flags(void), is_XP3_or_higher(void) or
		is_vista_or_higher(void)

***************************************/

uint_t BURGER_API Burger::Win32::is_XP_or_higher(void) BURGER_NOEXCEPT
{
	return (get_version_flags() & kVersionXPOrGreater) != 0;
}

/*! ************************************

	\brief Detect if running Windows XP SP3 or higher

	Test if the system is a Windows XP operating system with service pack 3 or
	greater. If it returns \ref FALSE, it doesn't have service pack 3.

	\windowsonly

	\return Returns \ref TRUE if Windows XP SP3 or greater

	\sa get_version_flags(void), is_XP_or_higher(void) or
		is_vista_or_higher(void)

***************************************/

uint_t BURGER_API Burger::Win32::is_XP3_or_higher(void) BURGER_NOEXCEPT
{
	return (get_version_flags() & kVersionXP3OrGreater) != 0;
}

/*! ************************************

	\brief Detect if running Windows Vista or higher

	Test if the system is a Windows Vista operating system or greater. If it
	returns \ref FALSE, it's before Windows Vista (Usually XP)

	\windowsonly

	\return Returns \ref TRUE if Windows Vista or greater

	\sa get_version_flags(void), is_XP3_or_higher(void) or
		is_XP_or_higher(void)

***************************************/

uint_t BURGER_API Burger::Win32::is_vista_or_higher(void) BURGER_NOEXCEPT
{
	return (get_version_flags() & kVersionVistaOrGreater) != 0;
}

/*! ************************************

	\brief Detect if running Windows 7 or higher

	Test if the system is a Windows 7 operating system or greater. If it returns
	\ref FALSE, it's before Windows 7

	\windowsonly

	\return Returns \ref TRUE if Windows 7 or greater

	\sa get_version_flags(void), is_vista_or_higher(void) or
		is_XP_or_higher(void)

***************************************/

uint_t BURGER_API Burger::Win32::is_7_or_higher(void) BURGER_NOEXCEPT
{
	return (get_version_flags() & kVersion7OrGreater) != 0;
}

/*! ************************************

	\brief Detect if running Windows 8 or higher

	Test if the system is a Windows 8 operating system or greater. If it returns
	\ref FALSE, it's before Windows 8

	\windowsonly

	\return Returns \ref TRUE if Windows 8 or greater

	\sa get_version_flags(void), is_7_or_higher(void),
		is_vista_or_higher(void) or is_XP_or_higher(void)

***************************************/

uint_t BURGER_API Burger::Win32::is_8_or_higher(void) BURGER_NOEXCEPT
{
	return (get_version_flags() & kVersion8OrGreater) != 0;
}

/*! ************************************

	\brief Detect if running Windows 10 or higher

	Test if the system is a Windows 10 operating system or greater. If it
	returns \ref FALSE, it's before Windows 10

	\note If the program doesn't have a manifest and it's running on Windows 10
		or higher, a test of the registry is performed to verify that the
		operating system is 10 or higher.

	\windowsonly

	\return Returns \ref TRUE if Windows 10 or greater

	\sa get_version_flags(void), is_8_or_higher(void),
		is_7_or_higher(void), is_vista_or_higher(void) or
		is_XP_or_higher(void)

***************************************/

uint_t BURGER_API Burger::Win32::is_10_or_higher(void) BURGER_NOEXCEPT
{
	return (get_version_flags() & kVersion10OrGreater) != 0;
}

#if !defined(DOXYGEN)
// Function prototypes, lovingly stolen from winver.h
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

BOOL BURGER_API Burger::Win32::VerQueryValueA(const void* pBlock,
	const char* pSubBlock, void** ppBuffer, uint_t* pLen) BURGER_NOEXCEPT
{
	void* pVerQueryValueA = load_function(kCall_VerQueryValueA);

	// Assume failure
	BOOL uResult = FALSE;
	if (pVerQueryValueA) {
		uResult = static_cast<VerQueryValueAPtr>(pVerQueryValueA)(
			pBlock, pSubBlock, ppBuffer, pLen);
	}
	return uResult;
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

BOOL BURGER_API Burger::Win32::VerQueryValueW(const void* pBlock,
	const uint16_t* pSubBlock, void** ppBuffer, uint_t* pLen) BURGER_NOEXCEPT
{
	void* pVerQueryValueW = load_function(kCall_VerQueryValueW);

	// Assume failure
	BOOL uResult = FALSE;
	if (pVerQueryValueW) {
		uResult = static_cast<VerQueryValueWPtr>(pVerQueryValueW)(
			pBlock, reinterpret_cast<LPCWSTR>(pSubBlock), ppBuffer, pLen);
	}
	return uResult;
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

BOOL BURGER_API Burger::Win32::GetFileVersionInfoA(const char* ptstrFilename,
	uint32_t dwHandle, uint32_t dwLen, void* pData) BURGER_NOEXCEPT
{
	void* pGetFileVersionInfoA = load_function(kCall_GetFileVersionInfoA);

	// Assume failure
	BOOL uResult = FALSE;
	if (pGetFileVersionInfoA) {
		uResult = static_cast<GetFileVersionInfoAPtr>(pGetFileVersionInfoA)(
			ptstrFilename, dwHandle, dwLen, pData);
	}
	return uResult;
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

BOOL BURGER_API Burger::Win32::GetFileVersionInfoW(
	const uint16_t* ptstrFilename, uint32_t dwHandle, uint32_t dwLen,
	void* pData) BURGER_NOEXCEPT
{
	void* pGetFileVersionInfoW = load_function(kCall_GetFileVersionInfoW);

	// Assume failure
	BOOL uResult = FALSE;
	if (pGetFileVersionInfoW) {
		uResult = static_cast<GetFileVersionInfoWPtr>(pGetFileVersionInfoW)(
			reinterpret_cast<LPCWSTR>(ptstrFilename), dwHandle, dwLen, pData);
	}
	return uResult;
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

uint32_t BURGER_API Burger::Win32::GetFileVersionInfoSizeA(
	const char* ptstrFilename, unsigned long* pdwHandle) BURGER_NOEXCEPT
{
	void* pGetFileVersionInfoSizeA =
		load_function(kCall_GetFileVersionInfoSizeA);

	// Assume failure
	uint32_t uResult = 0;
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

uint32_t BURGER_API Burger::Win32::GetFileVersionInfoSizeW(
	const uint16_t* ptstrFilename, unsigned long* pdwHandle) BURGER_NOEXCEPT
{
	void* pGetFileVersionInfoSizeW =
		load_function(kCall_GetFileVersionInfoSizeW);

	// Assume failure
	uint32_t uResult = 0;
	if (pGetFileVersionInfoSizeW) {
		uResult =
			static_cast<GetFileVersionInfoSizeWPtr>(pGetFileVersionInfoSizeW)(
				reinterpret_cast<LPCWSTR>(ptstrFilename), pdwHandle);
	}
	return uResult;
}

/*! ************************************

	\brief Returns a 64 bit version of a file.

	Given a filename in Windows Unicode format, open the file and return
	the 64 bit Windows extended version number from the dwFileVersionMS
	and dwFileVersionLS entries in the VS_FIXEDFILEINFO structure.

	\return Version in 64 bit Windows format or 0 on error

***************************************/

uint64_t BURGER_API Burger::Win32::get_file_version64(
	const uint16_t* pWindowsFilename) BURGER_NOEXCEPT
{
	// Assume failure
	uint64_t uResult = 0;

	if (pWindowsFilename) {
		DWORD uNotUsed;

		// Get the size of the data
		UINT uBufferSize =
			Win32::GetFileVersionInfoSizeW(pWindowsFilename, &uNotUsed);

		if (uBufferSize) {

			// Allocate Windows memory
			HANDLE hHeap = GetProcessHeap();

			BYTE* pFileVersionBuffer =
				static_cast<BYTE*>(HeapAlloc(hHeap, 0, uBufferSize));

			if (pFileVersionBuffer) {
				// Load the data
				if (Win32::GetFileVersionInfoW(
						pWindowsFilename, 0, uBufferSize, pFileVersionBuffer)) {
					VS_FIXEDFILEINFO* pVersion = nullptr;

					// Extract the version value
					if (Win32::VerQueryValueW(pFileVersionBuffer,
							reinterpret_cast<const uint16_t*>(L"\\"),
							(VOID**)&pVersion, &uBufferSize)) {
						if (pVersion != nullptr) {
							uResult = (static_cast<uint64_t>(
										   pVersion->dwFileVersionMS)
										  << 32U) +
								pVersion->dwFileVersionLS;
						}
					}
				}

				// Release the buffer
				HeapFree(hHeap, 0, pFileVersionBuffer);
			}
		}
	}
	return uResult;
}

/*! ************************************

	\brief Return the version of DirectX by scanning file versions

	Tries to get the DirectX version by looking at DirectX file versions
	Uses http://en.wikipedia.org/wiki/DirectX as a reference for file version
	lookups

	\return Version number of DirectX in 16 bit format 0x900 is 9.0, 0x903
		= 9.0c

***************************************/

uint32_t BURGER_API Burger::Win32::get_DirectX_version_via_file_versions(
	void) BURGER_NOEXCEPT
{
	uint16_t szPath[MAX_PATH * 2];

	uint32_t uResult = 0;
	if (GetSystemDirectoryW(reinterpret_cast<LPWSTR>(szPath), MAX_PATH) != 0) {

		// Failsafe
		szPath[MAX_PATH - 1] = 0;

		// Switch off the ddraw version
		uintptr_t uLen = string_length(szPath);
		uint16_t* pDest = szPath + uLen;
		uintptr_t uRemaining = sizeof(szPath) - (uLen * sizeof(uint16_t));

		StringCopy(pDest, uRemaining,
			reinterpret_cast<const uint16_t*>(L"\\ddraw.dll"));

		uint64_t uVersionDDraw = Win32::get_file_version64(szPath);
		if (uVersionDDraw >= 0x000400020000005FULL) { // Win9x version
			// file is >= DX1.0 version, so we must be at least DX1.0
			uResult = 0x0100;
		}
		if (uVersionDDraw >= 0x0004000300000448ULL) { // Win9x version
			// file is is >= DX2.0 version, so we must DX2.0 or DX2.0a (no
			// redist change)
			uResult = 0x0200;
		}
		if (uVersionDDraw >= 0x0004000400000044ULL) { // Win9x version
			// file is is >= DX3.0 version, so we must be at least DX3.0
			uResult = 0x0300;
		}

		// Switch off the d3drg8x.dll version
		StringCopy(pDest, uRemaining,
			reinterpret_cast<const uint16_t*>(L"\\d3drg8x.dll"));
		uint64_t uVersionD3Drg8x = Win32::get_file_version64(szPath);
		if (uVersionD3Drg8x >= 0x0004000400000046ULL) { // Win9x version
			// d3drg8x.dll is the DX3.0a version, so we must be DX3.0a or DX3.0b
			// (no redist change)
			uResult = 0x301;
		}

		// No DirectX 4

		// Switch off the ddraw version
		if (uVersionDDraw >= 0x000400050000009BULL) { // Win9x version
			// ddraw.dll is the DX5.0 version, so we must be DX5.0 or DX5.2 (no
			// redist change)
			uResult = 0x0500;
		}

		if (uVersionDDraw >= 0x000400060000013EULL) { // Win9x version
			// ddraw.dll is the DX6.0 version, so we must be at least DX6.0
			uResult = 0x0600;
		}

		if (uVersionDDraw >= 0x00040006000001B4) { // Win9x version
			// ddraw.dll is the DX6.1 version, so we must be at least DX6.1
			uResult = 0x0610;
		}

		// Switch off the dplayx.dll version
		StringCopy(pDest, uRemaining,
			reinterpret_cast<const uint16_t*>(L"\\dplayx.dll"));
		uint64_t uVersionDPlayx = Win32::get_file_version64(szPath);
		if (uVersionDPlayx >= 0x0004000600030206ULL) { // Win9x version
			// ddraw.dll is the DX6.1 version, so we must be at least DX6.1a
			uResult = 0x0611;
		}

		// Switch off the ddraw version
		if (uVersionDDraw >= 0x00040007000002BC) { // Win9x version
			// ddraw.dll is the DX7.0 version, so we must be at least DX7.0
			uResult = 0x0700;
		}

		// Switch off the dinput version
		StringCopy(pDest, uRemaining,
			reinterpret_cast<const uint16_t*>(L"\\dinput.dll"));
		uint64_t uVersionDInput = Win32::get_file_version64(szPath);
		if (uVersionDInput >= 0x00040007000002CC) { // Win9x version
			// ddraw.dll is the DX7.0 version, so we must be at least DX7.0a
			uResult = 0x0701;
		}

		// Switch off the ddraw version
		if ((((uVersionDDraw & 0xFFFF000000000000ULL) ==
				 0x0004000000000000ULL) &&
				(uVersionDDraw >= 0x0004000800000190ULL)) || // Win9x version
			(((uVersionDDraw & 0xFFFF000000000000ULL) ==
				 0x0005000000000000ULL) &&
				(uVersionDDraw >=
					0x0005000108D20190ULL))) { // Win2k/WinXP version
			// ddraw.dll is the DX8.0 version, so we must be at least DX8.0 or
			// DX8.0a (no redist change)
			uResult = 0x0800;
		}

		// Switch off the d3d8 version
		StringCopy(pDest, uRemaining,
			reinterpret_cast<const uint16_t*>(L"\\d3d8.dll"));
		uint64_t uVersionD3D8 = Win32::get_file_version64(szPath);
		if ((((uVersionD3D8 & 0xFFFF000000000000ULL) ==
				 0x0004000000000000ULL) &&
				(uVersionD3D8 >= 0x0004000800010371ULL)) || // Win9x version
			(((uVersionD3D8 & 0xFFFF000000000000ULL) ==
				 0x0005000000000000ULL) &&
				(uVersionD3D8 >=
					0x000500010A280371ULL))) { // Win2k/WinXP version
			// d3d8.dll is the DX8.1 version, so we must be at least DX8.1
			uResult = 0x0810;
		}

		if ((((uVersionD3D8 & 0xFFFF000000000000ULL) ==
				 0x0004000000000000ULL) &&
				(uVersionD3D8 >= 0x0004000800010385ULL)) || // Win9x version
			(((uVersionD3D8 & 0xFFFF000000000000ULL) ==
				 0x0005000000000000ULL) &&
				(uVersionD3D8 >=
					0x000500010A280385ULL))) { // Win2k/WinXP version
			// d3d8.dll is the DX8.1a version, so we must be at least DX8.1a
			uResult = 0x0811;
		}

		// Switch off the Mpg2splt version
		StringCopy(pDest, uRemaining,
			reinterpret_cast<const uint16_t*>(L"\\mpg2splt.ax"));
		uint64_t uVersionMPG2Splt = Win32::get_file_version64(szPath);
		if (uVersionMPG2Splt >=
			0x0006000300010375ULL) { // Win9x/Win2k/WinXP version
			// quartz.dll is the DX8.1b version, so we must be at least DX8.1b
			uResult = 0x0812;
		}

		// Switch off the dpnet version
		StringCopy(pDest, uRemaining,
			reinterpret_cast<const uint16_t*>(L"\\dpnet.dll"));
		uint64_t uVersionDPNet = Win32::get_file_version64(szPath);
		if ((((uVersionDPNet & 0xFFFF000000000000ULL) ==
				 0x0004000000000000ULL) &&
				(uVersionDPNet >= 0x0004000900000086ULL)) || // Win9x version
			(((uVersionDPNet & 0xFFFF000000000000ULL) ==
				 0x0005000000000000ULL) &&
				(uVersionDPNet >=
					0x000500020E5D0086ULL))) { // Win2k/WinXP version
			// dpnet.dll is the DX8.2 version, so we must be at least DX8.2
			uResult = 0x0820;
		}

		// Switch off the d3d9 version
		StringCopy(pDest, uRemaining,
			reinterpret_cast<const uint16_t*>(L"\\d3d9.dll"));
		uint64_t uVersionD3D9 = Win32::get_file_version64(szPath);

		if (uVersionD3D9) {
			// File exists, so it must be at least DX9
			uResult = 0x0900; // 9.0
		}

		if (uVersionD3D9 >= 0x0004000900000385ULL) { // 4.09.00.0901
			uResult = 0x0901;                        // 9.0a
		}

		if (uVersionD3D9 >= 0x0004000900000386ULL) { // 4.09.00.0902
			uResult = 0x0902;                        // 9.0b
		}

		if (uVersionD3D9 >= 0x0004000900000387ULL) { // 4.09.00.0903
			uResult = 0x0903;                        // 9.0c
		}

		// DirectX 10
		if (uVersionD3D9 >= 0x0006000017704002ULL) { // 6.00.6000.16386
			uResult = 0x0A00;                        // 10.0
		}
		if (uVersionD3D9 >= 0x0006000017714650ULL) { // 6.00.6001.18000
			uResult = 0x0A10;                        // 10.1
		}

		// DirectX 11
		if (uVersionD3D9 >= 0x00060000177246BBULL) { // 6.00.6002.18107
			uResult = 0x0B00;                        // 11.0
		}
		if (uVersionD3D9 >= 0x0006000223F04000ULL) { // 6.02.9200.16384
			uResult = 0x0B10;                        // 11.1
		}

		if (uVersionD3D9 >= 0x0006000225804000ULL) { // 6.02.9600.16384
			uResult = 0x0B20;                        // 11.2
		}

		if (uVersionD3D9 >= 0x00060002383901BFULL) { // 6.02.14393.447
			uResult = 0x0B30;                        // 11.3
		}

		//		if (uVersionD3D9>=0x000A000028004000ULL) {
		//// 10.00.10240.16384 (Not found yet) 			uResult = 0x0C00;
		//// 12.0
		//		}
	}
	return uResult;
}

/*! ************************************

	\brief Return the version of DirectX.

	Detect if DirectX is available, and if so, query it for the version present.
	If DirectX is not available, the version returned is zero.

	This function is written so it only asks for the version once from DirectX.
	It will cache the version and return the cached value on subsequent calls.

	By invoking DEEP magic, I will divine the version of DirectX that is
	present. It will do a manual check of the system folder for the DLLs and
	pull the version resource from them.

	\windowsonly
	\return Version in the format of 0x0902 -> 9.2, 0xB01 = 11.0.1

***************************************/

uint32_t BURGER_API Burger::Win32::get_DirectX_version(void) BURGER_NOEXCEPT
{
	// There is a version number in the registry, however, it is only valid for
	// DirectX versions 1 through 9.0c. The registry key is not valid for
	// DirectX 10 and higher

	// The same issue exists for obtaining the version of DirectX using the
	// "Dialog" string, it tops out at DirectX 9, and doesn't properly map to
	// versions of DirectX later than 11

	if (!s_bDirectXVersionValid) {

		// I got the version
		s_bDirectXVersionValid = TRUE;
		const uint_t uResult = Win32::get_DirectX_version_via_file_versions();
		s_uDirectXVersion = uResult;
	}
	// Return the DirectX version
	return s_uDirectXVersion;
}

#endif
