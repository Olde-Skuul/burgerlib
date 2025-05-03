/***************************************

	Global variable manager

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brglobals.h"
#include "brassert.h"
#include "brstring.h"
#include "brstringfunctions.h"
#include "version.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#if !defined(DOXYGEN)
#if defined(BURGER_XBOXONE) || defined(BURGER_XBOX360) || \
	(defined(BURGER_WINDOWS) && !defined(BURGER_WATCOM) && \
		!defined(BURGER_METROWERKS) && \
		!(defined(_MSC_VER) && (_MSC_VER < 1400)))
#define USESECURE
#endif
#endif

/*! ************************************

	\enum Burger::eTraceFlags
	\brief Flags to enable debugging features

	Many burgerlib functions allow for logging of information at runtime, and
	the behavior is enabled by setting these flags. Normally, the default is all
	flags clear.

	\sa get_traceflags(), or set_traceflags(uint32_t)

***************************************/

#if !defined(DOXYGEN)
/** Debug information level */
static uint32_t s_uTraceFlags;
#endif

/*! ************************************

	\brief Get the current debug tracing flag.

	Several subsystems perform diagnostic logging and this
	flag enables and/or disables these features.

	\return The trace flags. Test against the \ref eTraceFlags
		enumerations.

	\sa set_traceflags(uint32_t)

***************************************/

uint32_t BURGER_API Burger::get_traceflags(void) BURGER_NOEXCEPT
{
	return s_uTraceFlags;
}

/*! ************************************

	\brief Set the current debug tracing flag.

	Several subsystems perform diagnostic logging and this
	flag enables and/or disables these features.

	\param uTraceFlags The trace flags. Generate it with \ref eTraceFlags
		enumerations.

	\sa get_traceflags()

***************************************/

void BURGER_API Burger::set_traceflags(uint32_t uTraceFlags) BURGER_NOEXCEPT
{
	s_uTraceFlags = uTraceFlags;
}

/*! ************************************

	\enum Burger::eHintFlags
	\brief Flags to enable/disable underlying code

	Some underlying code in Burgerlib may or may not cause issues with higher
	level code, such as throwing exception, debugger integration which can cause
	issues. These flags will enable or disable these underlying features.

	\sa get_hintflags(), set_hintflags(uint32_t), or
		Win32::throw_thread_naming_exception(uint32_t, const char*)

***************************************/

#if !defined(DOXYGEN)
/** eHintFlags value */
static uint32_t s_uHintFlags;
#endif

/*! ************************************

	\brief Get the current code hint flags.

	Several subsystems perform code that may or may not interfere with higher
	level code. This flag enables and/or disables these features.

	\return The hint flags. Test against the \ref eHintFlags
		enumerations.

	\sa set_hintflags(uint32_t)

***************************************/

uint32_t BURGER_API Burger::get_hintflags(void) BURGER_NOEXCEPT
{
	return s_uHintFlags;
}

/*! ************************************

	\brief Set the current debug tracing flag.

	Several subsystems perform perform code that may or may not interfere with
	higher level code. This flag enables and/or disables these features.

	\param uHintFlags The hint flags. Generate it with \ref eHintFlags
		enumerations.

	\sa get_hintflags()

***************************************/

void BURGER_API Burger::set_hintflags(uint32_t uHintFlags) BURGER_NOEXCEPT
{
	s_uHintFlags = uHintFlags;
}

/*! ************************************

	\struct Burger::Globals
	\brief Global variables shared by all functions in the application

	These globals are used for the application to manage operating
	system resources that have a global effect.

	\sa Burger::Debug or Burger::Windows

***************************************/

/***************************************

	Here are all the debug globals

***************************************/

Burger::Globals Burger::Globals::g_Globals;
Burger::eError Burger::Globals::g_iErrorCode;
char Burger::Globals::g_ErrorMsg[512];
uint_t Burger::Globals::g_bBombFlag;
uint_t Burger::Globals::g_bExitFlag;

/*! ************************************

	\fn Burger::Globals::GetErrorCode(void);
	\brief Get the current error code the application will return on exit.

	When the main() function exits, this is the error code that should be
	returned.

	\code

		int main(void)
		{
			DoStuff();
			// Somewhere, the code may have set an error code. Pass it along to
			// the operating system
			return Burger::Globals::GetErrorCode();
		}

	\endcode

	\return The current application error state

	\sa Burger::Globals::SetErrorCode(int)

***************************************/

/*! ************************************

	\fn Burger::Globals::SetErrorCode(int);
	\brief Set the current error code.

	When the application shuts down, main() will return an integer
	error code. This function will set that code.

	\param iNewError The new error code, zero means no error.

	\sa Burger::Globals::GetErrorCode()

***************************************/

/*! ************************************

	\fn Burger::Globals::GetErrorMsg(void);
	\brief Get the pointer to the global error message buffer.

	Calls to many functions that fail will update this buffer to
	return verbose output.

	\note This buffer is modified by functions passing warnings
		and errors. Copy the string immediately and use the copy.

	\note Do not attempt to modify the buffer the pointer points
		to.

	\return A pointer to a global static buffer.

	\sa Burger::Globals::SetErrorMsg(const char *,...)

***************************************/

/*! ************************************

	\brief Update the current error message.

	If the input message pointer is not \ref NULL, then store
	the message string into the global error string buffer
	using printf() rules. If it's \ref NULL, set the string to "".

	\param pMessage Pointer to a string suitable for printf or \ref NULL.

	\sa Burger::Globals::GetErrorMsg(), Burger::Debug::Fatal or
		Burger::Debug::Warning

***************************************/

void BURGER_ANSIAPI Burger::Globals::SetErrorMsg(const char* pMessage, ...)
{
	va_list Args;
	if (pMessage) {               // No message, no error!
		va_start(Args, pMessage); // Start parm passing
#if defined(USESECURE)
		vsprintf_s(g_ErrorMsg, sizeof(g_ErrorMsg), pMessage,
			Args); // Create the message
#else
		vsprintf(g_ErrorMsg, pMessage, Args); // Create the message
		BURGER_ASSERT(string_length(g_ErrorMsg) < sizeof(g_ErrorMsg));
#endif
		va_end(Args); // End parm passing
	} else {
		g_ErrorMsg[0] = 0; // Kill the string
	}
}

/*! ************************************

	\fn Burger::Globals::AreWarningsEnabled(void);
	\brief Return non-zero if warning logging is enabled

	For debugging, if this flag returns a non-zero value,
	log messages if a non-fatal error condition occurs.

	\return \ref FALSE if logging is not requests, non-zero to enable logging.

	\sa Burger::get_traceflags()

***************************************/

/*! ************************************

	\fn Burger::Globals::GetErrorBombFlag(void);
	\brief Get the current flag to treat warnings as fatal errors.

	When Burger::Debug::Warning is called, it will test the state of this
	boolean flag and if \ref TRUE, it will abort code execution and
	exit immediately.

	\return \ref TRUE if warnings are treated as errors, \ref FALSE if not.
		(\ref FALSE is default).

	\sa Burger::Globals::SetErrorBombFlag(uint_t)

***************************************/

/*! ************************************

	\fn Burger::Globals::SetErrorBombFlag(uint_t);
	\brief Set the current flag to treat warnings as fatal errors.

	When Burger::Debug::Warning is called, it will test the state of this
	boolean flag and if \ref TRUE, it will abort code execution and
	exit immediately.

	\param uNewFlag If non-zero, it will set the flag, zero will clear it.
	\return The previous Error Bomb Flag
	\sa Burger::Globals::GetErrorBombFlag()

***************************************/

/*! ************************************

	\fn Burger::Globals::GetExitFlag(void);
	\brief Get the "shutdown in progress" flag.

	If the application has called Globals::Shutdown(), this flag
	is set to \ref TRUE to prevent recursion if Globals::Shutdown() is
	called by cleanup code.

	\return \ref TRUE if shut down is in progress, \ref FALSE if not.

	\sa Burger::Globals::SetExitFlag(uint_t)

***************************************/

/*! ************************************

	\fn Burger::Globals::SetExitFlag(uint_t);
	\brief Set the current flag if a shut down is in progress.

	If the application has called Globals::Shutdown(), this flag
	is set to \ref TRUE to prevent recursion if Globals::Shutdown() is
	called by cleanup code.

	This function should set this flag if it's implementing its own
	form of Globals::Shutdown() or is manually calling exit()

	\param uNewFlag If non-zero, it will set the flag, zero will clear it.
	\sa Burger::Globals::GetExitFlag()

***************************************/

/*! ************************************

	\brief Immediately shut down the application.

	Calls exit() with the currently logged error code (Default is zero)
	The exit flag is set to prevent recursion.

	\note This will kill the application. All global destructors are called
		to clean up all sub-systems.

	\sa Burger::Globals::Shutdown(int)

***************************************/

void BURGER_API Burger::Globals::Shutdown(void)
{
	Shutdown(g_iErrorCode);
}

/*! ************************************

	\brief Immediately shut down the application with an error code

	Calls exit() with the error code (Default is zero)

	\note This will kill the application unless it's already in shutdown.
		All global destructors are called to clean up all sub-systems.
		If this function returns, immediately exit from the function since the
		application is already in shut down mode.

	\param iError Error code to return when main() is returned from.
	\sa Burger::Globals::Shutdown() and Globals::SetExitFlag()

***************************************/

void BURGER_API Burger::Globals::Shutdown(int iError)
{
	if (!g_bExitFlag) {
		// Ensure all future calls to "Warnings" don't accidentally call Fatal()
		g_bExitFlag = TRUE;
		g_bBombFlag = FALSE;
		exit(iError);
	}
}

/*! ************************************

	\brief Return the version of Burgerlib

	Returns the version of Burgerlib in a single number.

	Currently, Burgerlib is 5.0.3

	\code
	uint32_t uVersion = Globals::Version();
	uint32_t uMajor = (uVersion>>24);
	uint32_t uMinor = (uVersion>>16)&0xFFU;
	uint32_t uPatch = (uVersion&0xFFFFU);
	\endcode

	\return Version in a single 32 bit value
	\sa VersionBuild(void)

***************************************/

uint32_t BURGER_API Burger::Globals::Version(void)
{
	return 0x05000003;
}

/*! ************************************

	\brief Return the build number of Burgerlib

	Returns the change list number that generated this version of Burgerlib. Use
	this value to determine if a specific version is needed for compatibility

	\return Build number in a single 32 bit value
	\sa Version(void)

***************************************/

uint32_t BURGER_API Burger::Globals::VersionBuild(void)
{
	return P4_CHANGELIST;
}

/*! ************************************

	\brief Load and launch a web page from an address string

	For Windows and MacOS platforms, this will open a web browser to a specific
	URL. It will use the default browser selected by the user from the operating
	system settings

	\param pURL Pointer to a UTF8 string with the URL to a web page to open
	\return Zero for no error, non-zero for error

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MACOS) || \
	defined(BURGER_IOS)) || \
	defined(DOXYGEN)
uint_t BURGER_API Burger::Globals::LaunchURL(const char* /* pURL */)
{
	return TRUE; // Unimplemented
}
#endif

/*! ************************************

	\brief Execute a tool and capture the text output

	If another program needs to be executed, call it with a command line
	and capture the text output into a stream if desired

	\param pFilename Pointer to a "C" string of the executable in Burgerlib
		format
	\param pParameters Pointer to a "C" string of the command line parameters
		(No translation is performed)
	\param pOutput Pointer to a OutputMemoryStream to store the captured output.
		\ref NULL is acceptable if output capturing is not desired

	\return Error code on failure, zero if the program was executed and it
		returned zero on exit

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MACOSX)) || defined(DOXYGEN)
int BURGER_API Burger::Globals::ExecuteTool(const char* /* pFilename */,
	const char* /* pParameters */, OutputMemoryStream* /* pOutput */)
{
	return 10;
}
#endif

/*! ************************************

	\brief Retrieve an environment string

	On systems that support it, query the system for an environment variable and
	return a COPY of the string. Once the string is no longer needed, call
	Burger::free_memory(const void *) on it to release it.

	Once obtained, the string will not change due to it being a copy of the one
	that exists in the operating system.

	The string is encoded to UTF8 on all systems

	\param pKey Pointer to a "C" string of the environment variable name

	\return nullptr on failure or if the string didn't exist. A valid "C"
		string pointer otherwise. Release with a call to
		Burger::free_memory(const void *)

	\sa SetEnvironmentString(const char *,const char *)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_MACOSX) || defined(BURGER_LINUX)) || \
	defined(DOXYGEN)
const char* BURGER_API Burger::GetEnvironmentString(
	const char* /* pKey */) BURGER_NOEXCEPT
{
	return nullptr;
}
#endif

/*! ************************************

	\brief Set an environment string

	On systems that support it, set or create an environment variable.

	The string will be properly converted from UTF8 into the operating system's
	native character encoding.

	\param pKey Pointer to a "C" string of the environment variable name
	\param pInput Pointer to a "C" string of the environment variable value or
		nullptr to remove the variable
	\return Zero on success, non-zero on error

	\sa GetEnvironmentString(const char *)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_MACOSX) || defined(BURGER_LINUX)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::SetEnvironmentString(
	const char* /* pKey */, const char* /* pInput */) BURGER_NOEXCEPT
{
	return kErrorNotSupportedOnThisPlatform;
}
#endif

/*! ************************************

	\fn Burger::IsElevated(void)
	\brief Test if the application has elevated privileges

	On systems that support it, check if the user has elevated privileges.

	Systems such as MSDOS and game consoles allow full access to all system
	resources, but on modern desktops, only Administrators or root accounts can
	modify system folders.

	\note Windows XP tests for access restriction, where all other systems test
		for access token elevation.

	\return Zero if user access is not granted, non-zero if elevated access was
		granted.

***************************************/
