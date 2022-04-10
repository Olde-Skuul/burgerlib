/***************************************

	Operating system string functions

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brosstringfunctions.h"

/*! ************************************

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

#if !( \
	defined(BURGER_MAC) || defined(BURGER_WINDOWS) || defined(BURGER_UNIX)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::GetUserLoginName(
	String* pOutput) BURGER_NOEXCEPT
{
	pOutput->assign("User");
	return kErrorNotSupportedOnThisPlatform;
}
#endif

/*! ************************************

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

#if !( \
	defined(BURGER_MAC) || defined(BURGER_WINDOWS) || defined(BURGER_UNIX)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::GetUserRealName(
	String* pOutput) BURGER_NOEXCEPT
{
	pOutput->assign("User");
	return kErrorNotSupportedOnThisPlatform;
}
#endif

/*! ************************************

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

	\note On Android, for security reasons, it may return ``localhost``

	\sa GetUserLoginName(String *) or NetworkManager::GetHostName()

***************************************/

#if !( \
	defined(BURGER_MAC) || defined(BURGER_WINDOWS) || defined(BURGER_UNIX)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::GetMachineName(
	String* pOutput) BURGER_NOEXCEPT
{
	pOutput->assign("Computer");
	return kErrorNotSupportedOnThisPlatform;
}

#endif
