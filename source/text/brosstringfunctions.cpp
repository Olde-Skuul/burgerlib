/***************************************

	Operating system string functions

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brosstringfunctions.h"

/*! ************************************

	\brief Get the name of the current user

	When someone has logged onto a computer, that person had to give a user name.
	This routine will retrieve that user name. If for some reason a user name 
	can't be found or the operating system doesn't support user log ons,
	the name "User" will be returned.

	\param pOutput Pointer to a \ref String to receive the name in UTF-8 encoding
	\return Zero on no error, or non zero on failure.
	\note On platforms where networking or user level access isn't available, it will
		return \ref kErrorNotSupportedOnThisPlatform as an error code.

	\a GetMachineName(String *)

***************************************/

#if !(defined(BURGER_MACOS) || defined(BURGER_WINDOWS)) || defined(DOXYGEN)
Burger::eError BURGER_API Burger::GetLoggedInUserName(String *pOutput)
{
	pOutput->Set("User");
	return kErrorNotSupportedOnThisPlatform;
}
#endif

/*! ************************************

	\brief Get the name the user has called the computer

	Some computer owners have the option to give their computer a whimsical
	name. This routine will retrieve that name. If for some reason a name can't
	be found or the operating system doesn't support naming, the name of "Computer"
	will be returned.

	\param pOutput Pointer to a \ref String to receive the name in UTF-8 encoding
	\return Zero on no error, or non zero on failure.
	\note On platforms where networking or user level access isn't available, it will
	return \ref kErrorNotSupportedOnThisPlatform as an error code.

	\note On MacOS 9, the machine name is found in the OS string
		number -16413 from the system resource file.

	\a GetLoggedInUserName(String *)

***************************************/

#if !(defined(BURGER_MACOS) || defined(BURGER_WINDOWS)) || defined(DOXYGEN)
Burger::eError BURGER_API Burger::GetMachineName(String *pOutput)
{
	pOutput->Set("Computer");
	return kErrorNotSupportedOnThisPlatform;
}
#endif
