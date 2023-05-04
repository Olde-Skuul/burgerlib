/***************************************

	Detect the X11 sandbox

	Copyright (c) 2022-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __LINUX_SANDBOX_H__
#define __LINUX_SANDBOX_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger { namespace Linux {
enum eX11Sandbox {
	/** No sandbox was found */
	kX11SandboxNone,
	/** Flatpak sandbox found */
	kX11SandboxFlatpak,
	/** Snap sandbox found */
	kX11SandboxSnap,
	/** Sandbox found, but unknown type */
	kX11SandboxUnknown
};

extern eX11Sandbox BURGER_API detect_x11_sandbox(void) BURGER_NOEXCEPT;

}}
/* END */

#endif
