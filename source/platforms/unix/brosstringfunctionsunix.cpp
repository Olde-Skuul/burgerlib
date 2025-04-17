/***************************************

	Operating system string functions
	Unix version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brosstringfunctions.h"

#if defined(BURGER_UNIX) || defined(DOXYGEN)
#include "brmemoryfunctions.h"
#include "brstring.h"
#include "brstring16.h"

#include <limits.h>
#include <pwd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

/***************************************

	\brief Retrieves the login name of the user associated with the current
		thread.

	On systems that use user logins, return the login name of the account
	associated with the current thread. If the platform doesn't support multiple
	user accounts, it will return "User" and the error code \ref
	kErrorNotSupportedOnThisPlatform.

	\param pOutput Pointer to a \ref String to receive the name in UTF-8
		encoding
	 \return Zero on no error, or non zero on failure.

	 \note On platforms where networking or user level access isn't available,
		it will return \ref kErrorNotSupportedOnThisPlatform as an error code.

	\sa GetUserRealName(String *) or GetMachineName(String *)

***************************************/

Burger::eError BURGER_API Burger::GetUserLoginName(
	String* pOutput) BURGER_NOEXCEPT
{
	eError uResult = kErrorItemNotFound;
	// Get the user information
	struct passwd* pPasswd = getpwuid(getuid());
	if (pPasswd) {
		// Set the string
		uResult = pOutput->assign(pPasswd->pw_name);
	}
	if (uResult) {
		// The name wasn't present, use the default
		pOutput->assign("User");
	}
	return uResult;
}

/***************************************

	\brief Get the real name of the current user.

	When someone has logged onto a computer, that person can associate a real
	name to the login user account. This routine will retrieve real name of the
	user. If for some reason a user name can't be found or the operating system
	doesn't support user logins, the name "User" will be returned.

	\param pOutput Pointer to a \ref String to receive the real name in UTF-8
		encoding
	\return Zero on no error, or non zero on failure.

	 \note On platforms where networking or user level access isn't available,
		it will always return \ref kErrorNotSupportedOnThisPlatform as an error
		code.

	\sa GetUserLoginName(String *) or GetMachineName(String *)

***************************************/

Burger::eError BURGER_API Burger::GetUserRealName(
	String* pOutput) BURGER_NOEXCEPT
{

	eError uResult = kErrorItemNotFound;
	// Get the user information
	struct passwd* pPasswd = getpwuid(getuid());

	// Get the comment which would have the name
	// Test if value or garbage
	if (pPasswd->pw_gecos) {

		// Only use the first part of a comma delimited string.
		const char* pEnd = StringCharacter(pPasswd->pw_gecos, ',');
		uintptr_t uLength;
		if (pEnd) {
			uLength = pEnd - pPasswd->pw_gecos;
		} else {
			// Use the entire string
			uLength = string_length(pPasswd->pw_gecos);
		}
		// Only use it if there is a string
		if (uLength) {
			uResult = pOutput->assign(pPasswd->pw_gecos, uLength);
		}
	}
	if (uResult) {
		// Get the user folder name
		uResult = pOutput->assign(pPasswd->pw_name);
		if (uResult) {
			pOutput->assign("User");
		}
	}
	return uResult;
}

/***************************************

	\brief Get the name the user has called the computer.

	Some computer owners have the option to give their computer a whimsical
	name. This routine will retrieve that name. If for some reason a name can't
	be found or the operating system doesn't support naming, the name of
	"Computer" will be returned.

	\param pOutput Pointer to a \ref String to receive the name in UTF-8
		encoding
	\return Zero on no error, or non zero on failure.

	\note On platforms where networking or user level access isn't available, it
		will return \ref kErrorNotSupportedOnThisPlatform as an error code.

	\note On MacOS 9, the machine name is found in the OS string number -16413
		from the system resource file.

	\sa GetUserLoginName(String *) or NetworkManager::GetHostName()

***************************************/

// Darwin handles this differently

#if !defined(BURGER_DARWIN)
Burger::eError BURGER_API Burger::GetMachineName(
	String* pOutput) BURGER_NOEXCEPT
{
	// Assume error
	eError uResult = kErrorItemNotFound;

	char TempBuffer[HOST_NAME_MAX];
	if (!gethostname(TempBuffer, sizeof(TempBuffer))) {
		uResult = pOutput->assign(TempBuffer);
	}
	// If that didn't work, punt.
	if (uResult) {
		pOutput->assign("Computer");
	}

	return uResult;
}

#endif

/*! ************************************

	\brief Return the path of the "home" folder.

	On unix style machines, the user account has a home directory. Return that
	home directory name in native unix format.

	\param pOutput Pointer to a \ref String to receive the name in UTF-8
		encoding

	\return Zero on no error, or non zero on failure.

***************************************/

Burger::eError BURGER_API Burger::get_home_directory(
	String* pOutput) BURGER_NOEXCEPT
{
	// First, try getting the environment variable HOME
	const char* pHomeEnv = getenv("HOME");
	if (!pHomeEnv) {

		// Try getting the path from the user ID
		struct passwd* pPasswd = getpwuid(getuid());
		pHomeEnv = pPasswd->pw_dir;

		// Failure?
		if (!pHomeEnv) {
			pOutput->clear();
			return kErrorItemNotFound;
		}
	}

	// Copy the found string
	eError uResult = pOutput->assign(pHomeEnv);
	if (!uResult) {

		// Make sure it doesn't end with a slash.
		if ((pOutput->length() >= 2U) && pOutput->ends_with('/')) {
			pOutput->pop_back();
		}
	}
	return uResult;
}

/*! ************************************

	\brief Return the absolute path of a pathname.

	On unix style machines, paths can start with ~ or ., whereas a prefix string
	needs to be inserted to get a full pathname. If there are /./ or /../
	entries, they will be resolved.

	\note Unlike realpath(), this function does not look for the existence of
		the file, all parsing is done internally so files that haven't been
		created yet will still parse correctly.

	\param pOutput Pointer to a \ref String to receive the name in UTF-8
		encoding
	\param pInput Pointer to a "C" string of the native unix pathname.

	\return Zero on no error, or non zero on failure.

***************************************/

Burger::eError BURGER_API Burger::get_abspath(
	String* pOutput, const char* pInput) BURGER_NOEXCEPT
{
	// Start with the pathname, as is.
	eError uResult = pOutput->assign(pInput);
	if (!uResult) {
		uintptr_t uInputLength = pOutput->length();
		pInput = pOutput->c_str();

		// First, check if it's relative to the home directory.
		if (pInput[0] == '~') {
			if ((uInputLength == 1U) || (pInput[1] == '/')) {

				// Remove the ~
				pOutput->Remove(0, 1);

				// Insert the home directory
				String HomeDir;
				get_home_directory(&HomeDir);
				pOutput->insert(0, HomeDir.c_str(), HomeDir.length());
			}
		}

		// Next, check if it's relative to the current working directory
		if (pInput[0] != '/') {
			char* pTemp = getcwd(nullptr, 0);
			if (pTemp) {
				// Insert the directory
				pOutput->insert(0, "/", 1U);
				pOutput->insert(0, pTemp, string_length(pTemp));
				free(pTemp);
			}
		}

		// Make sure it ends with a slash for the code below
		if (!pOutput->ends_with('/')) {
			pOutput->push_back('/');
		}

		// Finally, remove all . and .. entries
		// Get rid of all of the /./ entries
		for (;;) {
			const char* pSinglePeriod = StringString(pOutput->c_str(), "/./");
			if (!pSinglePeriod) {
				break;
			}
			pOutput->Remove(
				static_cast<uintptr_t>(pSinglePeriod - pOutput->c_str()), 2U);
		}

		// Now deal with the /../ directory entries by turning them into /./
		// entries and removing the previous directory.

		// Make sure the volume name is never discarded.

		for (;;) {

			// Look for "/..", if not found, abort
			const char* pDoublePeriod = StringString(pOutput->c_str(), "/..");
			if (!pDoublePeriod) {
				break;
			}

			// Locate the start of the previous directory
			const char* pDirName = MemoryCharacterReverse(pOutput->c_str(),
				static_cast<uintptr_t>(pDoublePeriod - pOutput->c_str()), '/');

			uintptr_t uRemoveStart;
			uintptr_t uRemoveLength;

			// Is the previous directory a label? Or nonexistent?
			if (!pDirName || (pDirName == pOutput->c_str())) {

				// Only remove the period in the center of "/.."
				uRemoveStart =
					static_cast<uintptr_t>(pDoublePeriod - pOutput->c_str()) +
					1;
				uRemoveLength = 1;
				// Special case, if it turns a "/../" into a "/./", then get rid
				// of it.
				if (!MemoryCompare(pDoublePeriod, "/../", 4)) {
					uRemoveLength = 3;
				}

			} else {
				// Remove the previous directory, starting after the colon, and
				// ending with the "/." at the end.
				uRemoveStart =
					static_cast<uintptr_t>(pDirName - pOutput->c_str()) + 1;
				uRemoveLength =
					static_cast<uintptr_t>(pDoublePeriod - pDirName) + 1;

				// Special case. If the entry is "/../", get rid of it too.
				if (pDoublePeriod[3] == '/') {
					uRemoveLength += 2;
				}
			}

			// Remove either just a dot from after the colon in "/.." or remove
			// the previous directory and a dot
			pOutput->Remove(uRemoveStart, uRemoveLength);
		}

		// Linux prefers native pathnames to not end with a slash.
		if ((pOutput->length() >= 2U) && pOutput->ends_with('/')) {
			pOutput->pop_back();
		}
	}
	return uResult;
}

#endif
