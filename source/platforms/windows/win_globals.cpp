/***************************************

	Global values for Windows

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "win_globals.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)

#if !defined(DOXYGEN)
// Current singular instance of the application
static HINSTANCE__* s_hInstance;
#endif

/*! ************************************

	\brief Get the application instance

	This function is only an accessor. It only returns what was set by the call
	to set_instance(). If set_instance() was never called, this function always
	returns \ref nullptr.


	\windowsonly

	\return Instance set by set_instance()

	\sa set_instance()

***************************************/

HINSTANCE__* BURGER_API Burger::Win32::get_instance(void) BURGER_NOEXCEPT
{
	return s_hInstance;
}

/*! ************************************

	\brief Set the application instance

	Upon application startup, an instance is assigned, use this function to
	allow Burgerlib to use this instance in other parts of the library.

	\windowsonly

	\param pInput Instance of the application

	\sa get_instance()

***************************************/

void BURGER_API Burger::Win32::set_instance(HINSTANCE__* pInput) BURGER_NOEXCEPT
{
	s_hInstance = pInput;
}

#endif
