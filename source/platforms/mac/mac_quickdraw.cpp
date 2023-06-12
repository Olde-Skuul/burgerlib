/***************************************

	MacOS specific quickdraw functions

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "mac_quickdraw.h"

#if defined(BURGER_MAC) || defined(DOXYGEN)
#include "brmactypes.h"

#include <Gestalt.h>
#include <Movies.h>
#include <Quickdraw.h>

/*! ************************************

	\brief Return the version of Quickdraw.

	Use Gestalt to test for what version of Quickdraw is available.

	\return Version in the format of 0x0102 -> 1.2

***************************************/

uint32_t BURGER_API Burger::MacOS::get_quickdraw_version(void) BURGER_NOEXCEPT
{
	// Discovered version of Quicdraw (Cached)
	static uint32_t s_uQuickdrawVersion = UINT32_MAX;

	uint_t uVersion = s_uQuickdrawVersion;
	if (uVersion == UINT32_MAX) {
		long MyAnswer;

		// Assume failure (Ancient Mac, like Mac 128K)
		uVersion = gestaltOriginalQD;

		// Get the version (Quickdraw always exists)
		if (!Gestalt(gestaltQuickdrawVersion, &MyAnswer)) {
			uVersion = static_cast<uint_t>(MyAnswer);
		}

		s_uQuickdrawVersion = uVersion;
	}
	return uVersion;
}

/*! ************************************

	\brief Detect if the PowerPC native version of Quicktime is present.

	Detect if the PowerPC native extension of Quicktime is loaded and available.
	This is only for Quicktime version 2.5 during the initial transition from
	680x0 to PowerPC. It's almost certainly going to return \ref TRUE on PowerPC
	systems. Only in very rare cases of Quicktime 2.5 where it's emulated in
	680x0 code (And therefore a performance bottleneck) in which this matters.

	This function is written so it only does the detection once and it caches
	the result. The cached value is returned on subsequent calls.

	\return \ref TRUE if Quicktime is running in PowerPC code, \ref FALSE if
		emulated 680x0.

	\note This code always returns \ref FALSE on non-PowerPC targets.

***************************************/

uint_t BURGER_API Burger::MacOS::has_quicktime_power_plug(void) BURGER_NOEXCEPT
{
#if defined(BURGER_POWERPC)
	static uint8_t s_bIsQuickTimePlugInTested = FALSE;

	uint_t bResult = s_bIsQuickTimePlugInTested;
	// Was it already tested?
	if (!(bResult & 0x80)) {
		long gestaltAnswer;
		if (!Gestalt(gestaltQuickTimeFeatures, &gestaltAnswer)) {
			if ((gestaltAnswer & (1 << gestaltPPCQuickTimeLibPresent)) &&
				(EnterMovies != nullptr)) {
				// Quicktime is in PowerPC
				bResult = 0x80 | TRUE;
			} else {
				bResult = 0x80 | FALSE;
			}
		}
		s_bIsQuickTimePlugInTested = static_cast<uint8_t>(bResult);
	}
	// Return the value minus the other flags
	return bResult & 1U;
#else
	// Emulated 68k if false
	return FALSE;
#endif
}

/*! ************************************

	\brief Test for color Quickdraw

	Check the version of quickdraw and if color is supported, return TRUE.

	\return TRUE if color is supported.

***************************************/

uint_t BURGER_API Burger::MacOS::has_color_quickdraw(void) BURGER_NOEXCEPT
{
	return get_quickdraw_version() > gestaltOriginalQD;
}

/*! ************************************

	\brief Test for color GrafPort

	Check the version of the GrafPort and if color is supported, return TRUE.

	\return TRUE if the GrafPort is really a CGrafPort.

***************************************/

uint_t BURGER_API Burger::MacOS::is_color_grafport(
	const GrafPort* pInput) BURGER_NOEXCEPT
{
#if defined(BURGER_MACCARBON)
	BURGER_UNUSED(pInput);
	return TRUE;
#else
	// Do a version check to detect for color
	return reinterpret_cast<const CGrafPort*>(pInput)->portVersion < 0;
#endif
}

#endif