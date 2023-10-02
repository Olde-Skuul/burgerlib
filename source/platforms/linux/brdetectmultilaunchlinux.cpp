/***************************************

	Detect multiple launches class

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brdetectmultilaunch.h"

#if defined(BURGER_LINUX)
#include "brglobalmemorymanager.h"
#include "brstringfunctions.h"

#include <fcntl.h>
#include <semaphore.h>

/***************************************

	\brief Initialize the object to a power up state.

	Variables are initialized but no action is taken

***************************************/

Burger::DetectMultiLaunch::DetectMultiLaunch() BURGER_NOEXCEPT: m_pName(nullptr)
{
}

/***************************************

	\brief Teardown on exit

	If is_multi_launched() is called, a global object exists. Once the class is
	destroyed, so is the object.

***************************************/

Burger::DetectMultiLaunch::~DetectMultiLaunch()
{
	// Is there a name stored?
	if (m_pName) {

		// Release the global semaphore by name
		sem_unlink(m_pName);

		// Release the name
		Free(m_pName);
		m_pName = nullptr;
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
#include <stdio.h>
uint_t Burger::DetectMultiLaunch::is_multi_launched(
	const char* pSignature) BURGER_NOEXCEPT
{
	// Assume success
	uint_t uResult = FALSE;

	// If already connected, this is the primary process
	if (!m_pName) {

		// Assume this is not the primary process
		uResult = TRUE;

		// See if it already exists
		sem_t* pSemaphore = sem_open(pSignature, O_CREAT, S_IRWXU, 1);

		// Got the semaphore
		if (pSemaphore != SEM_FAILED) {

			// Try to lock it
			int iResult = sem_trywait(pSemaphore);
			if (!iResult) {
				// Got the lock! Keep it around
				m_pName = StringDuplicate(pSignature);
				uResult = FALSE;
			}
		}
	}

	// Error! Either the name couldn't be registered or it already exists
	return uResult;
}

#endif
