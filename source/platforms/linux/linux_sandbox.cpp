/***************************************

	Detect the X11 sandbox

	Copyright (c) 2022-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "linux_sandbox.h"

#if defined(BURGER_LINUX) || defined(DOXYGEN)
#include <unistd.h>
#include <stdlib.h>

/*! ************************************

	\enum Burger::Linux::eX11Sandbox
	\brief Enumeration of X11 sandbox

	Returned from detect_x11_sandbox() when and if a sandbox was detected.

	\linuxonly

	\sa detect_x11_sandbox()

***************************************/

/*! ************************************

	\brief Detect the X11 sandbox

	Check the system to see of Flatpak, Snap or generic sandboxs are active on
	Linux

	\linuxonly

	\return A eX11Sandbox enumerator with the result of the tests

***************************************/

Burger::Linux::eX11Sandbox BURGER_API Burger::Linux::detect_x11_sandbox(
	void) BURGER_NOEXCEPT
{
	// Is flatback running?
	if (!access("/.flatpak-info", F_OK)) {
		return kX11SandboxFlatpak;
	}

	// Detect Snap

	// Of course it's not as easy as flatpack. Ripped off how WebKitGTK checks
	if (getenv("SNAP") && getenv("SNAP_NAME") && getenv("SNAP_REVISION")) {
		return kX11SandboxSnap;
	}

	if (access("/run/host/container-manager", F_OK) == 0) {
		return kX11SandboxUnknown;
	}

	return kX11SandboxNone;
}
#endif
