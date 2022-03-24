/***************************************

	Typedefs specific to Unix/Posix

	Copyright (c) 2021-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brunixtypes.h"

#if defined(BURGER_UNIX) || defined(DOXYGEN)
#include "brmemoryfunctions.h"

#include <errno.h>
#include <sys/utsname.h>

#if !defined(DOXYGEN)
static const char g_VolumeLinux[] = "/mnt";
static const char g_VolumeDarwin[] = "/Volumes";
static const char g_VolumeMSYS2[] = "/";
static const char g_VolumeCygwin[] = "/cygdrive";
#endif

/*! ************************************

	\enum Burger::eUnixFlavor
	\brief Enum describing the actual platform Unix is running on.

***************************************/

/*! ************************************

	\brief Folder for mounted volumes

	Every flavor of Unix uses a different folder for where all the mounted
	volumes are located. This array contains all the base folders for every
	flavor of unix supported by Burgerlib.

	\unixonly

	\sa \ref eUnixFlavor or UnixGetFlavor()

***************************************/

const char* Burger::g_VolumePrefixes[kUnixStadia + 1] = {g_VolumeLinux,
	g_VolumeLinux, g_VolumeMSYS2, g_VolumeLinux, g_VolumeCygwin, g_VolumeDarwin,
	g_VolumeDarwin, g_VolumeLinux, g_VolumeLinux};

/*! ************************************

	\brief Return Unix uname data

	Calls Unix uname() and captures the result.

	\unixonly

	\return kErrorNone if no error, kErrorReadFailure on read failure.

***************************************/

Burger::eError Burger::Uname::Init(void) BURGER_NOEXCEPT
{
	eError uResult = kErrorReadFailure;
	// Load the data from uname
	struct utsname TempBuffer;
	errno = 0;
	if (uname(&TempBuffer) >= 0) {
		// Copy the data into the cache
		m_sysnam = TempBuffer.sysname;
		m_nodename = TempBuffer.nodename;
		m_release = TempBuffer.release;
		m_version = TempBuffer.version;
		m_machine = TempBuffer.machine;

		// Only set if this distro supports networking
#if _UTSNAME_DOMAIN_LENGTH - 0
// Because, reasons...
#ifdef __USE_GNU
		m_domainname = TempBuffer.domainname;
#else
		m_domainname = TempBuffer.__domainname;
#endif
#endif
		uResult = kErrorNone;
	}
	return uResult;
}

/*! ************************************

	Return Linux flavor.

	Do some runtime checks to determine the distribution type of Linux. It will
	check for Cygwin, MSYS, Windows Subsystem for Linux and default to native
	Linux.

	\unixonly

	\return Type of Linux runtime found.

	\sa \ref eUnixFlavor

***************************************/

Burger::eUnixFlavor Burger::UnixGetFlavor(void) BURGER_NOEXCEPT
{
	// Apple platforms are hard coded
#if defined(BURGER_IOS)
	return kUnixiOS;
#elif defined(BURGER_MACOSX)
	return kUnixMacOSX;
#elif defined(BURGER_ANDROID)
	return kUnixAndroid;
#elif defined(BURGER_STADIA)
	return kUnixStadia;
#else
	Uname TempUName;
	eUnixFlavor uFlavor;

	// Query Uname to determine what machine this is running on
	if (TempUName.Init()) {
		uFlavor = kUnixUnknown;
	} else {

		// Check the usual suspects, Cygwin and MSYS2?
		if (!MemoryCaseCompare(TempUName.m_sysnam.c_str(), "CYGWIN", 6)) {
			uFlavor = kUnixCygwin;

		} else if (!MemoryCaseCompare(TempUName.m_sysnam.c_str(), "MSYS", 4)) {
			uFlavor = kUnixMSYS2;

			// Microsoft inserts their name in the string for WSL
		} else if (StringString(TempUName.m_release.c_str(), "Microsoft")) {
			uFlavor = kUnixWSL;

		} else {
			// It's a stock Linux distro
			uFlavor = kUnixNative;
		}
	}
	return uFlavor;
#endif
}

#endif
