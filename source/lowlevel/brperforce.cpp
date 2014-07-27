/***************************************

	Class to add perforce integration to Windows tools

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brperforce.h"

/*! ************************************

	\class Burger::Perforce
	\brief Handle a connection to a perforce server

	This class encapsulates the connecting and invoking of perforce
	within an application. It simplifies the opening, editing and
	checking in files for development tools or runtime data
	management across a development team.

	This class is not intended for shipping code. It's inclusion
	in a shipping game application is discouraged.

	\note This class is only available on Windows targets.
	On all other targets, it will perform no action and return an error code.

***************************************/

/*! ************************************

	\fn Burger::Perforce::Perforce()
	\brief Initialize the class

	Initialize the class variables, but it won't actually
	connect to Perforce. Connecting occurs when Perforce::Init() is called.

	\sa Perforce::Init() and Perforce::Shutdown()

***************************************/

/*! ************************************

	\fn Burger::Perforce::~Perforce()
	\brief Release the perforce tool and clean up

	\sa Perforce::Init() and Perforce::Shutdown()

***************************************/

/*! ************************************

	\fn int Burger::Perforce::Init(void)
	\brief Open a connection to perforce

	Load the Perforce DLL and initialize it. This call is required before
	calling Perforce::Edit() or Perforce::RevertIfUnchanged()

	\return Zero if no error, non-zero if an perforce error had occurred
	\sa Perforce::Shutdown()
	
***************************************/

/*! ************************************

	\fn int Burger::Perforce::Shutdown(void)
	\brief Shut down any pending commands from Perforce and release
	all resources
	\return Zero if no error, non-zero if an perforce error had occurred
	\sa Perforce::Init()

***************************************/


/*! ************************************

	\fn int Burger::Perforce::Edit(const char *pFilename)
	\brief Issue an "edit" command to open a file
	\param pFilename Pointer to a Burgerlib format filename
	\return Zero if no error, non-zero if an perforce error had occurred
	\sa Perforce::RevertIfUnchanged()

***************************************/

/*! ************************************

	\fn int Burger::Perforce::RevertIfUnchanged(const char *pFilename)
	\brief Issue an "revert" command to revert a file if it hasn't changed

	Given a Burgerlib pathname, perform a "p4 -a revert" command on it to
	revert the file if it has not changed

	\param pFilename Pointer to a Burgerlib format filename
	\return Zero if no error, non-zero if an perforce error had occurred
	\sa Perforce::Edit()

***************************************/

