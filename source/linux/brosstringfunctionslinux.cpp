/***************************************

    Operating system string functions
    Linux version

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brosstringfunctions.h"

#if defined(BURGER_LINUX) && !defined(DOXYGEN)
#include "brstring.h"
#include "brstring16.h"

#include <limits.h>
#include <pwd.h>
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
    eError uResult = kErrorGeneric;
    // Get the user information
    struct passwd* pPasswd = getpwuid(getuid());
    if (pPasswd) {
        // Get the user folder name
        uResult = pOutput->Set(pPasswd->pw_name);
    }
    if (uResult) {
        pOutput->Set("User");
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

    eError uResult = kErrorGeneric;
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
            uLength = StringLength(pPasswd->pw_gecos);
        }
        // Only use it if there is a string
        if (uLength) {
            uResult = pOutput->Set(pPasswd->pw_gecos, uLength);
        }
    }
    if (uResult) {
        // Get the user folder name
        uResult = pOutput->Set(pPasswd->pw_name);
        if (uResult) {
            pOutput->Set("User");
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

Burger::eError BURGER_API Burger::GetMachineName(
    String* pOutput) BURGER_NOEXCEPT
{
    // Assume error
    eError uResult = kErrorGeneric;

    char TempBuffer[HOST_NAME_MAX];
    if (!gethostname(TempBuffer, sizeof(TempBuffer))) {
        uResult = pOutput->Set(TempBuffer);
    }
    // If that didn't work, punt.
    if (uResult) {
        pOutput->Set("Computer");
    }

    return uResult;
}

#endif
