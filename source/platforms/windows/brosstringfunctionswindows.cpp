/***************************************

	Operating system string functions
	Windows version

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brosstringfunctions.h"

#if defined(BURGER_WINDOWS) && !defined(DOXYGEN)

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0501 // Windows XP
#endif

#if !defined(SECURITY_WIN32)
#define SECURITY_WIN32
#endif

#include "brstring.h"
#include "brstring16.h"

#include <windows.h>

#include <lm.h>
#include <security.h>

/***************************************

	\brief Retrieves the login name of the user associated with the current
		thread.

	On systems that use user logins, return the login name of the account
	associated with the current thread. If the platform doesn't support multiple
	user accounts, it will return "User" and the error code \ref
	kErrorNotSupportedOnThisPlatform.

	\param pOutput Pointer to a \ref String to receive the name in UTF-8
		encoding
	 \return Zero on no error, or non zero on failure.

	 \note On platforms where networking or user level access isn't available,
		it will return \ref kErrorNotSupportedOnThisPlatform as an error code.

	\sa GetUserRealName(String *) or GetMachineName(String *)

***************************************/

Burger::eError BURGER_API Burger::GetUserLoginName(
	String* pOutput) BURGER_NOEXCEPT
{

#if defined(BURGER_WATCOM)
#pragma library("Advapi32.lib")
#else
#pragma comment(lib, "Advapi32.lib")
#endif

	eError uResult = kErrorItemNotFound;
	DWORD uBufferSize = 0;
	GetUserNameW(nullptr, &uBufferSize);
	if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
		WCHAR* pBuffer =
			static_cast<WCHAR*>(alloc_clear(sizeof(WCHAR) * (uBufferSize + 1)));
		if (pBuffer) {
			if (GetUserNameW(pBuffer, &uBufferSize)) {
				uResult = pOutput->assign(static_cast<const uint16_t*>(
					static_cast<const void*>(pBuffer)));
			}
			Free(pBuffer);
		}
	}

	if (uResult) {
		pOutput->assign("User");
	}

	return uResult;
}

/***************************************

	\brief Get the real name of the current user.

	When someone has logged onto a computer, that person can associate a real
	name to the login user account. This routine will retrieve real name of the
	user. If for some reason a user name can't be found or the operating system
	doesn't support user logins, the name "User" will be returned.

	\param pOutput Pointer to a \ref String to receive the real name in UTF-8
		encoding
	\return Zero on no error, or non zero on failure.

	 \note On platforms where networking or user level access isn't available,
		it will always return \ref kErrorNotSupportedOnThisPlatform as an error
		code.

	\sa GetUserLoginName(String *) or GetMachineName(String *)

***************************************/

Burger::eError BURGER_API Burger::GetUserRealName(
	String* pOutput) BURGER_NOEXCEPT
{

#if defined(BURGER_WATCOM)
#pragma library("Secur32.lib")
#else
#pragma comment(lib, "Secur32.lib")
#endif

	eError uResult = kErrorItemNotFound;

	// Get the length of the user name
	DWORD uBufferSize = 0;
	GetUserNameExW(NameDisplay, nullptr, &uBufferSize);
	if (GetLastError() == ERROR_MORE_DATA) {
		String16 Temp;
		uResult = Temp.SetBufferSize(uBufferSize);
		if (!uResult) {
			// Try getting the real user's name
			if (GetUserNameExW(NameDisplay,
					reinterpret_cast<LPWSTR>(Temp.c_str()), &uBufferSize)) {
				uResult = pOutput->assign(Temp.c_str());
			}
		}
	}

	// No name found yet?
	if (uResult) {
		uResult = GetUserLoginName(pOutput);
	}
	return uResult;
}

/***************************************

	\brief Get the name the user has called the computer.

	Some computer owners have the option to give their computer a whimsical
	name. This routine will retrieve that name. If for some reason a name can't
	be found or the operating system doesn't support naming, the name of
	"Computer" will be returned.

	\param pOutput Pointer to a \ref String to receive the name in UTF-8
		encoding
	\return Zero on no error, or non zero on failure.

	\note On platforms where networking or user level access isn't available, it
		will return \ref kErrorNotSupportedOnThisPlatform as an error code.

	\note On MacOS 9, the machine name is found in the OS string number -16413
		from the system resource file.

	\sa GetUserLoginName(String *) or NetworkManager::GetHostName()

***************************************/

Burger::eError BURGER_API Burger::GetMachineName(
	String* pOutput) BURGER_NOEXCEPT
{
#if defined(BURGER_WATCOM)
#pragma library("Netapi32.lib")
#else
#pragma comment(lib, "Netapi32.lib")
#endif

	// Assume error
	eError uResult = kErrorGeneric;

	// Pull the server information from local machine
	BYTE* pComment = nullptr;
	if (!NetServerGetInfo(nullptr, 101, &pComment)) {
		const SERVER_INFO_101* pInfo =
			reinterpret_cast<const SERVER_INFO_101*>(pComment);
		// Was there a comment?
		if (pInfo->sv101_comment && pInfo->sv101_comment[0]) {
			// Return the comment
			uResult = pOutput->assign(
				reinterpret_cast<const uint16_t*>(pInfo->sv101_comment));
		}
	}

	// If there was no comment, try the generic computer name
	if (uResult) {
		// Get the length of the computer name
		DWORD uBufferSize = 0;
		if (!GetComputerNameExW(
				ComputerNameDnsHostname, nullptr, &uBufferSize)) {
			WCHAR* pBuffer = static_cast<WCHAR*>(
				alloc_clear(sizeof(WCHAR) * (uBufferSize + 1)));
			if (GetComputerNameExW(
					ComputerNameDnsHostname, pBuffer, &uBufferSize)) {
				uResult = pOutput->assign(static_cast<const uint16_t*>(
					static_cast<const void*>(pBuffer)));
			}
			Free(pBuffer);
		}

		// If that didn't work, punt.
		if (uResult) {
			pOutput->assign("Computer");
		}
	}
	return uResult;
}

/***************************************

	\brief Get absolute path of a Windows path.

	Assuming the 8 bit string is in UTF-8 encoding, convert to UTF-16 and call
	the windows function GetFullPathNameW() to convert to a full pathname.
	Afterwards convert back to UTF-8 and store the result in the output string.

	\param pOutput Pointer to a \ref String to receive the name in UTF-8
		encoding
	\param pInput Pointer to a "C" string of a Windows native pathname.
	\return Zero on no error, or non zero on failure.

***************************************/

Burger::eError BURGER_API Burger::GetFullPathNameUTF8(
	String* pOutput, const char* pInput) BURGER_NOEXCEPT
{
	// First thing, convert it to UTF16
	String16 Temp16;

	eError uResult = Temp16.Set(pInput);
	if (!uResult) {
		// Now that it's UTF16, let's have Windows expand it
		WCHAR ExpandedPath[512];
		WCHAR* pExpanded;

		// Have windows expand it out
		uintptr_t uExpandedLength =
			GetFullPathNameW(reinterpret_cast<const WCHAR*>(Temp16.c_str()),
				BURGER_ARRAYSIZE(ExpandedPath), ExpandedPath, nullptr);

		if (uExpandedLength >= BURGER_ARRAYSIZE(ExpandedPath)) {
			pExpanded = static_cast<WCHAR*>(
				Alloc((uExpandedLength + 2) * sizeof(WCHAR)));
			if (pExpanded) {
				uExpandedLength = GetFullPathNameW(
					reinterpret_cast<const WCHAR*>(Temp16.c_str()),
					static_cast<DWORD>(uExpandedLength + 2), pExpanded,
					nullptr);
			} else {
				uResult = kErrorOutOfMemory;
			}
		} else {
			pExpanded = ExpandedPath;
		}

		// Valid input?
		if (pExpanded) {

			// Save out the expanded path
			uResult =
				pOutput->assign(reinterpret_cast<const uint16_t*>(pExpanded));

			// Delete the temp buffer
			if (pExpanded != ExpandedPath) {
				Free(pExpanded);
			}
		}
	}
	return uResult;
}

#endif
