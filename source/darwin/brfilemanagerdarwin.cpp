/***************************************

	FileManager Class: Darwin

	Copyright (c) 2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilemanager.h"

#if defined(BURGER_DARWIN) || defined(DOXYGEN)
#include "brglobalmemorymanager.h"
#include "brstringfunctions.h"

/***************************************

	\brief Handle platform specific startup code

	Calls system functions to determine the version, state and several platform
	specific variables to allow the FileManager to run better by pre-caching
	relevant data.

	For MS-DOS, it will determine the version and flavor of MS/DOS this
	application is running. It will also detect DosBox.

	For Linux, it will scan all the mounted volumes for quick access to shared
	volumes.

	For Android and consoles, it will check if there are mounted SD Cards or
	other external data storage devices so the application can be aware of them.

	\sa Burger::FileManager or BURGER_MSDOS

***************************************/

void BURGER_API Burger::FileManager::PlatformSetup(void) BURGER_NOEXCEPT
{
	Filename MyFilename;

	// Get the boot volume name
	eError uResult = GetVolumeName(&MyFilename, 0); 
	if (!uResult) {
		// Set the initial prefix
		const char* pBootName = MyFilename.c_str();
		m_uBootNameSize = StringLength(pBootName);
		m_pBootName = StringDuplicate(pBootName);
	}
}

/***************************************

	\brief Handle platform specific shutdown code

	Calls system functions to release cached values obtained from the platform's
	operating system.

	\sa PlatformSetup(), Burger::FileManager

***************************************/

void BURGER_API Burger::FileManager::PlatformShutdown(void) BURGER_NOEXCEPT
{
	Free(m_pBootName);
	m_pBootName = nullptr;
	m_uBootNameSize = 0;
}

#endif
