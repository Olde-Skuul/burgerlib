/***************************************

	Global variable manager, Linux version

	Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brglobals.h"
#if defined(BURGER_LINUX) || defined(DOXYGEN)
#include "brlinuxtypes.h"
#include "brmemoryfunctions.h"
#include "brstringfunctions.h"

#include <errno.h>
#include <sys/utsname.h>
#include <unistd.h>

/*! ************************************

	Return Linux uname data

	Calls Linux uname() and caches the result. All subsequent calls will read
	from the local cache.

	\return Pointer to a Globals::uname structure with all the strings read in
		from uname()

***************************************/

const Burger::Globals::Uname_t* Burger::Globals::GetUname(void) BURGER_NOEXCEPT
{
	Globals* pGlobals = &g_Globals;
	if (!pGlobals->m_bLinuxUnameTested) {

		// Load the data from uname
		struct utsname TempBuffer;
		errno = 0;
		if (uname(&TempBuffer) >= 0) {
			// Copy the data into the cache
			pGlobals->m_uname.m_sysnam = TempBuffer.sysname;
			pGlobals->m_uname.m_nodename = TempBuffer.nodename;
			pGlobals->m_uname.m_release = TempBuffer.release;
			pGlobals->m_uname.m_version = TempBuffer.version;
			pGlobals->m_uname.m_machine = TempBuffer.machine;

// Only set if this distro supports networking
#if _UTSNAME_DOMAIN_LENGTH - 0
// Because, reasons...
#ifdef __USE_GNU
			pGlobals->m_uname.m_domainname = TempBuffer.domainname;
#else
			pGlobals->m_uname.m_domainname = TempBuffer.__domainname;
#endif
#endif
		}
		pGlobals->m_bLinuxUnameTested = TRUE;
	}
	// Return the class
	return &pGlobals->m_uname;
}

/*! ************************************

	Return Linux flavor.

	Do some runtime checks to determine the distribution type of Linux. It will
	check for Cygwin, MSYS, Windows Subsystem for Linux and default to native
	Linux.

	\return Type of Linux runtime found.

***************************************/

Burger::Globals::eLinuxFlavor Burger::Globals::GetLinuxFlavor(
	void) BURGER_NOEXCEPT
{
	Globals* pGlobals = &g_Globals;
	eLinuxFlavor uFlavor = pGlobals->m_uLinuxFlavor;
	if (uFlavor == kLinuxUnknown) {
		const Uname_t* pUName = GetUname();

		// Check the usual suspects, Cygwin and MSYS2?
		if (!MemoryCaseCompare(pUName->m_sysnam.c_str(), "CYGWIN", 6)) {
			uFlavor = kLinuxCygwin;
		} else if (!MemoryCaseCompare(pUName->m_sysnam.c_str(), "MSYS", 4)) {
			uFlavor = kLinuxMSYS2;

			// Microsoft inserts their name in the string for WSL
		} else if (StringString(pUName->m_release.c_str(), "Microsoft")) {
			uFlavor = kLinuxWSL;
		} else {

			// It's a stock Linux distro
			uFlavor = kLinuxNative;
		}
		// Cache the result
		pGlobals->m_uLinuxFlavor = uFlavor;
	}
	return uFlavor;
}

/***************************************

	Read an environment variable as UTF8

***************************************/

const char* BURGER_API Burger::GetEnvironmentString(
	const char* pKey) BURGER_NOEXCEPT
{
	const char* pValue = getenv(pKey);
	if (pValue) {
		pValue = StringDuplicate(pValue);
	}
	return pValue;
}

/***************************************

	Set an environment variable with a UTF8 string

***************************************/

Burger::eError BURGER_API Burger::SetEnvironmentString(
	const char* pKey, const char* pInput) BURGER_NOEXCEPT
{
	// Pass to the operating system
	return static_cast<eError>(setenv(pKey, pInput, TRUE));
}

/***************************************

	Test if the application has elevated privileges

***************************************/

uint_t BURGER_API Burger::IsElevated(void) BURGER_NOEXCEPT
{
	// Assume not elevated
	uint_t bResult = FALSE;

	if (!getuid()) {
		bResult = TRUE;
	}
	return bResult;
}

#endif
