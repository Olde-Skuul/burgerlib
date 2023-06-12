/***************************************

	MacOS specific version checking

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "mac_version.h"

#if defined(BURGER_MAC) || defined(DOXYGEN)
#include <Gestalt.h>
#include <Navigation.h>

#if !defined(DOXYGEN)

//
// Table to look up appleshare versions
//

static const uint16_t g_AppleShareVer[] = {0x000, 0x350, 0x360, 0x361, 0x362,
	0x363, 0x370, 0x372, 0x380, 0x381, 0x382, 0x383, 0x384};

#endif

/*! ************************************

	\brief Return the version of AppleShare library.

	Ask the AppleShare library what version it is and return that value. The
	function caches the value so subsequent calls do not ask AppleShare for the
	version again.

	\return Version in the format of 0x0102 -> 1.2

	\note If the AppleShare library is not installed, it will return 0.

***************************************/

uint32_t BURGER_API Burger::MacOS::get_appleshare_version(void) BURGER_NOEXCEPT
{
	// Discovered version of AppleShare cache
	static uint32_t s_uAppleShareVersion = UINT32_MAX;

	uint32_t uVersion = s_uAppleShareVersion;
	if (uVersion == UINT32_MAX) {
		long MyAnswer;

		// Assume failure
		uVersion = 0;

		// Detect Appleshare
		if (!Gestalt(gestaltAFPClient, &MyAnswer)) {
			uint32_t uIndex =
				static_cast<uint32_t>(MyAnswer & gestaltAFPClientVersionMask);
			if (uIndex > 12U) {
				uIndex = 12U;
			}
			// Look up the version via a table
			uVersion = g_AppleShareVer[uIndex];
		}
		s_uAppleShareVersion = uVersion;
	}
	return uVersion;
}

/*! ************************************

	\brief Get the version of NavServices

	Ask NavServices if it exists and return the version.

	\return 32 bit version number or 0 if NavServices is not available.

***************************************/

uint32_t BURGER_API Burger::MacOS::get_nav_services_version(
	void) BURGER_NOEXCEPT
{
	// Discovered version of NavServices (Cached)
	static uint32_t s_uNavServicesVersion = UINT32_MAX;

	uint32_t uVersion = s_uNavServicesVersion;
	if (uVersion == UINT32_MAX) {

		// Code even present?
		uVersion = 0;
		if (NavServicesAvailable()) {
			uVersion = NavLibraryVersion();
		}
		s_uNavServicesVersion = uVersion;
	}
	return uVersion;
}

/*! ************************************

	\brief Return the version of MacOS.

	Ask MacOS what version it is and return that value.

	\return Version in the format of 0x0102 -> 1.2

***************************************/

uint32_t BURGER_API Burger::MacOS::get_os_version(void) BURGER_NOEXCEPT
{
	// Discovered version of MacOS (Cached)
	static uint32_t s_uMacOSVersion = UINT32_MAX;

	uint32_t uVersion = s_uMacOSVersion;
	if (s_uMacOSVersion == UINT32_MAX) {

		long lAnswer;

		// Get the version with Gestalt
		if (Gestalt(gestaltSystemVersion, &lAnswer)) {

			// Should NEVER execute, failsafe
			lAnswer = 0;
		}

		// Get the version of the OS in 0x0102 (1.2) format
		uVersion = static_cast<uint32_t>(lAnswer & 0xFFFFU);
		s_uMacOSVersion = uVersion;
	}
	return uVersion;
}

/*! ************************************

	\brief Return the version of QuickTime.

	Detect if QuickTime is available, and if so, query it for the version
	present. If QuickTime is not available, the version returned is zero.

	This function is written so it only asks for the version once from
	QuickTime. It will cache the version and return the cached value on
	subsequent calls.

	\return Version in the format of 0x0102 -> 1.2

***************************************/

uint32_t BURGER_API Burger::MacOS::get_quicktime_version(void) BURGER_NOEXCEPT
{
	// Get the pointer to the singleton
	static uint32_t s_uQuickTimeVersion = UINT32_MAX;

	uint32_t uVersion = s_uQuickTimeVersion;
	if (s_uQuickTimeVersion == UINT32_MAX) {

		long gestaltAnswer;
		uVersion = 0;
		if (!Gestalt(gestaltQuickTimeVersion, &gestaltAnswer)) {

			// Major version
			uVersion = (gestaltAnswer >> 16U) & 0xFFFFU;
		}
		s_uQuickTimeVersion = uVersion;
	}

	// Return the quicktime version
	return uVersion;
}

/*! ************************************

	\brief Test if the application is running under MacOSX

	Ask MacOS what version it is and if it's 10.0 or higher, return \ref TRUE

	\return \ref TRUE if running under MacOSX, \ref FALSE if MacOS 9 or earlier

***************************************/

uint_t BURGER_API Burger::MacOS::is_running_osx(void) BURGER_NOEXCEPT
{
	return (get_os_version() >= 0x1000U);
}

/*! ************************************

	\brief Get the version of ControlStrip

	Ask ControlStrip if it exists and return the version.

	\return 32 bit version number or 0 if NavServices is not available.

***************************************/

uint32_t BURGER_API Burger::MacOS::get_control_strip_version(
	void) BURGER_NOEXCEPT
{
	// Discovered version of ControlStrip (Cached)
	static uint32_t s_uControlStripVersion = UINT32_MAX;

	uint32_t uVersion = s_uControlStripVersion;
	if (uVersion == UINT32_MAX) {
		// Code even present?
		uVersion = 0;
		
		long gestaltAnswer;
		if (!Gestalt(gestaltControlStripVersion, &gestaltAnswer)) {
			uVersion = static_cast<uint32_t>(gestaltAnswer);
		}
		s_uControlStripVersion = uVersion;
	}
	return uVersion;
}

#endif