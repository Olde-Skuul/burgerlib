/***************************************

	Detect multiple launches class

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brdetectmultilaunch.h"

#if defined(BURGER_WINDOWS)
#include "brstring16.h"

#include "win_windows.h"

/***************************************

	\brief Initialize the object to a power up state.

	Variables are initialized but no action is taken

***************************************/

Burger::DetectMultiLaunch::DetectMultiLaunch() BURGER_NOEXCEPT
	: m_hInstanceLock(INVALID_HANDLE_VALUE)
{
}

/***************************************

	\brief Tear down on exit

	If is_multi_launched() is called, a global object exists.
	Once the class is destroyed, so is the object.

***************************************/

Burger::DetectMultiLaunch::~DetectMultiLaunch()
{
	if (m_hInstanceLock != INVALID_HANDLE_VALUE) {
		CloseHandle(m_hInstanceLock);
		// Clean up
		m_hInstanceLock = INVALID_HANDLE_VALUE;
	}
}

/***************************************

	\fn Burger::DetectMultiLaunch::is_multi_launched(
		const char *pSignature)

	\brief Test if this is the only instance of this class systemwide.

	An object is created, for Windows, it's a FileMapping object of 32 bytes
	that stays in memory. If executed again, the recreation of the object will
	return a ERROR_ALREADY_EXISTS error that will tell the app there is already
	an instance running.

	For Darwin, a named sem_t is created and if it's already created, it's
	assumed another instance already exists.

	\param pSignature Pointer to a UTF8 "C" string that's unique to the
		application

	\return \ref TRUE if there is another instance of the application
		already running, \ref FALSE if this is the only instance.

***************************************/

uint_t Burger::DetectMultiLaunch::is_multi_launched(
	const char* pSignature) BURGER_NOEXCEPT
{
	uint_t uResult = FALSE;

	// Did I run already?
	if (m_hInstanceLock == INVALID_HANDLE_VALUE) {

		// Convert signature to UTF-16
		String16 NewName(pSignature);

		// Create a global instance of a file mapper that's 32 bytes in size
		HANDLE hLock = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr,
			PAGE_READONLY, 0, 32, reinterpret_cast<LPCWSTR>(NewName.c_str()));

		uResult = TRUE;
		if (hLock != nullptr) {

			// Got the mapping object?
			// Create and not already present?

			// Note: If already exists, don't dispose of it, because it would
			// cause the GLOBAL object to be released.
			if (GetLastError() != ERROR_ALREADY_EXISTS) {
				m_hInstanceLock = hLock;
				// No error
				uResult = FALSE;
			}
		}
	}

	// Error! Either the file couldn't be made or it already exists
	return uResult;
}

#endif
