/***************************************

	Code library (DLL) manager

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brcodelibrary.h"

/*! ************************************

	\class Burger::CodeLibrary
	\brief Manage code library files.

	Loading a DLL, .so or any other type of shared library is managed with this
	generic class. Initialize it with a call to init() and then call
	get_function() to extract each and every pointer to the code or data
	contained within.

	If this class is shut down by the shutdown(void) call, all of the pointers
	will become invalid and should never be used again.

***************************************/

/*! ************************************

	\fn Burger::CodeLibrary::CodeLibrary()
	\brief Initialize variables

	On creation, variables are initialized but no attempt is made to load in a
	shared library. The shared library is loaded only with a call to
	init()

	\sa init(const char *) and shutdown(void)

***************************************/

/*! ************************************

	\fn Burger::CodeLibrary::~CodeLibrary()
	\brief Release variables

	On disposal, if a share library was loaded, it will be released and all
	function pointers will be invalid.

	\sa shutdown(void)

***************************************/

Burger::CodeLibrary::~CodeLibrary() BURGER_NOEXCEPT
{
	shutdown();
}

#if !( \
	defined(BURGER_WINDOWS) || defined(BURGER_MAC) || defined(BURGER_UNIX)) || \
	defined(DOXYGEN)

/*! ************************************

	\brief Load in a shared library

	If the filename has any colons in it, it's assumed to be a full pathname and
	the library is directly loaded. If the library name has no colons, it's
	assumed to be a system code library and the system path(s) are searched
	until it is found.

	\param pFilename Pointer to a UTF-8 encoded pathname to the library

	\return \ref eError error code

	\sa get_function()

***************************************/

Burger::eError Burger::CodeLibrary::init(
	const char* /* pFilename */) BURGER_NOEXCEPT
{
	return kErrorNotSupportedOnThisPlatform;
}

/*! ************************************

	\brief Release a shared library

	If a library was loaded, release it back to the operating system.

	\note All function pointers returned by calls to CodeLibrary::get_function()
		will be immediately invalid after this call is complete.

***************************************/

void Burger::CodeLibrary::shutdown(void) BURGER_NOEXCEPT {}

/*! ************************************

	\brief Grab a function pointer from a shared library

	Search the loaded library for a specific function and if found, return the
	pointer to use to call it.

	\param pFunctionName Pointer to a "C" string of the function's name.
	\return \ref nullptr on failure or a valid pointer to the function in the
l		ibrary.

***************************************/

void* Burger::CodeLibrary::get_function(
	const char* /* pFunctionName */) BURGER_NOEXCEPT
{
	return nullptr;
}

/*! ************************************

	\fn Burger::CodeLibrary::is_initialized(void) const
	\brief Has a library been loaded?

	Returns \ref TRUE if a library was successfully loaded and ready for
	querying for function pointers.

	\return \ref TRUE if a library is loaded, \ref FALSE if not.

***************************************/

#endif
