/***************************************

	Linux version

	Copyright (c) 2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilename.h"

#if defined(BURGER_LINUX) || defined(DOXYGEN)
#include "brglobals.h"

#include <errno.h>
#include <limits.h>
#include <sys/utsname.h>
#include <unistd.h>

/***************************************

	\brief Set the filename to the application's directory

	Determine the directory where the application resides and set the filename
	to that directory. The path is converted into UTF8 character encoding and
	stored in Burgerlib filename format.

	On platforms where a current working directory doesn't make sense, like an
	ROM based system, the filename is cleared out.

***************************************/

Burger::eError BURGER_API Burger::Filename::SetApplicationDirectory(void) BURGER_NOEXCEPT
{
	clear();

	char result[PATH_MAX];
	readlink("/proc/self/exe", result, PATH_MAX);
	SetFromNative(result);
	return kErrorNone;
}
#endif
