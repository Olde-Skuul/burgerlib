/***************************************

    Class to add perforce integration to Windows tools

    Windows version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brperforce.h"

#if defined(BURGER_WINDOWS) && !defined(DOXYGEN)
// Include the perforce header
#include "brglobalmemorymanager.h"
#include "brfilename.h"
#include "brglobals.h"
#include "brfilemanager.h"
#include "brutf8.h"
#include "win_shlwapi.h"

#include "win_windows.h"

#include <shlwapi.h>

/***************************************

	Initialize the class

***************************************/

Burger::Perforce::Perforce() :
	m_PerforceFilename(),
	m_bFilenameInitialized(FALSE)
{
}

/***************************************

	Release the perforce tool and clean up

***************************************/

Burger::Perforce::~Perforce()
{
	Shutdown();
}

/***************************************

	Open a connection to perforce
	
***************************************/

uint_t BURGER_API Burger::Perforce::Init(void)
{
	uint_t uResult = 0;
	if (!m_bFilenameInitialized) {
		// Let's find the perforce EXE
		// Check for an environment variable with the directory
		const char *pAppdirectory = GetEnvironmentString("PERFORCE");
		uint_t bFilenameInitialized = FALSE;
		if (pAppdirectory) {
			m_PerforceFilename.set_native(pAppdirectory);
			free_memory(pAppdirectory);
			m_PerforceFilename.join("p4.exe");
			// Is there an exec here?
			if (FileManager::does_file_exist(&m_PerforceFilename)) {
				bFilenameInitialized = TRUE;
			}
		}

		// Try finding it in the usual installation folder

		if (!bFilenameInitialized) {
			pAppdirectory = GetEnvironmentString("ProgramFiles");
			if (pAppdirectory) {
				m_PerforceFilename.set_native(pAppdirectory);
				free_memory(pAppdirectory);
				m_PerforceFilename.join("Perforce:p4.exe");
				if (FileManager::does_file_exist(&m_PerforceFilename)) {
					bFilenameInitialized = TRUE;
				}
			}
		}

		// Fooey. Ask windows if it can find it

		if (!bFilenameInitialized) {
			uint16_t Output[2048];
			if (Win32::PathSearchAndQualifyW(reinterpret_cast<const uint16_t *>(L"p4.exe"),Output,BURGER_ARRAYSIZE(Output))) {
				m_PerforceFilename.set_native(Output);
				if (FileManager::does_file_exist(&m_PerforceFilename)) {
					bFilenameInitialized = TRUE;
				}
			}
		}
		m_bFilenameInitialized = bFilenameInitialized;
		if (!bFilenameInitialized) {
			uResult = 10;
		}
	}
	return uResult;
}

/***************************************

	Burger::Perforce::Shutdown(void)

***************************************/

uint_t BURGER_API Burger::Perforce::Shutdown(void)
{
	m_PerforceFilename.clear();
	m_bFilenameInitialized = FALSE;
	return 0;
}

/***************************************

	Issue an "edit" command to open a file

***************************************/

uint_t BURGER_API Burger::Perforce::Edit(const char *pFilename)
{
	uint_t uResult = Init();
	if (!uResult) {
		Filename Translate(pFilename);
		String Parameters("-s edit \"",Translate.get_native(),"\"");
		OutputMemoryStream Capture;
		// Issue the command to Perforce
		uResult = static_cast<uint_t>(Globals::ExecuteTool(m_PerforceFilename.c_str(),Parameters.c_str(),&Capture));
		if (!uResult) {
			// If the filename was not found (An error)
			// it only mentions it in the stderr text. Detect it
			Capture.Save(&Parameters);
			const char *pHit = StringString(Parameters.c_str(),"error:");
			if (pHit) {
				// An error had occurred!
				uResult = 10;
			}
		}
	}
	return uResult;
}

/***************************************

	Issue an "revert" command to revert a file if it hasn't changed

***************************************/

uint_t BURGER_API Burger::Perforce::RevertIfUnchanged(const char *pFilename)
{
	uint_t uResult = Init();
	if (!uResult) {
		Filename Translate(pFilename);
		String Parameters("-s revert -a \"",Translate.get_native(),"\"");
		OutputMemoryStream Capture;
		// Issue the command to Perforce
		uResult = static_cast<uint_t>(Globals::ExecuteTool(m_PerforceFilename.c_str(),Parameters.c_str(),&Capture));
		if (!uResult) {
			// If the filename was not found (An error)
			// it only mentions it in the stderr text. Detect it
			Capture.Save(&Parameters);
			const char *pHit = StringString(Parameters.c_str(),"error:");
			if (pHit) {
				// An error had occurred!
				uResult = 10;
			}
		}
	}
	return uResult;
}

#endif
