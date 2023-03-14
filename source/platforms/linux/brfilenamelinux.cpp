/***************************************

	Linux version

	Copyright (c) 2021-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

Burger::eError BURGER_API Burger::Filename::set_application_directory(
	void) BURGER_NOEXCEPT
{
	char result[PATH_MAX];
	ssize_t iBytes = readlink("/proc/self/exe", result, PATH_MAX);
	if (iBytes == -1) {
		clear();
		return kErrorItemNotFound;
	}
	eError uResult = set_native(result);
	if (!uResult) {
		// Pop off the application name
		dirname();
	}
	return uResult;
}

/***************************************

	\brief Set the filename to the boot volume directory

	Determine the directory of the drive volume that the operating system was
	loaded from. The path is converted into UTF8 character encoding and stored
	in Burgerlib filename format.

	On platforms where a current working directory doesn't make sense, like a
	ROM based system, the filename is cleared out.

***************************************/

Burger::eError BURGER_API Burger::Filename::set_boot_volume(
	void) BURGER_NOEXCEPT
{
	return assign(":boot_volume:");
}

/***************************************

	\brief Set the filename to the local machine preferences directory

	Determine the directory where the user's preferences that are local to the
	machine is located. The path is converted into UTF8 character encoding and
	stored in Burgerlib filename format.

	On platforms where a current working directory doesn't make sense, like a
	ROM based system, the filename is cleared out.

***************************************/

Burger::eError BURGER_API Burger::Filename::set_system_prefs_directory(
	void) BURGER_NOEXCEPT
{
	return set_native("~/");
}

/***************************************

	\brief Set the filename to the user's preferences directory

	Determine the directory where the user's preferences that could be shared
	among all machines the user has an account with is located. The path is
	converted into UTF8 character encoding and stored in Burgerlib filename
	format.

	On platforms where a current working directory doesn't make sense, like a
	ROM based system, the filename is cleared out.

***************************************/

Burger::eError BURGER_API Burger::Filename::set_user_prefs_directory(
	void) BURGER_NOEXCEPT
{
	return set_native("~/");
}

#endif
