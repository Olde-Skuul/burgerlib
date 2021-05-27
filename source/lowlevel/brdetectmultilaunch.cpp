/***************************************

    Detect multiple launches class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brdetectmultilaunch.h"

/*! ************************************

	\class Burger::DetectMultiLaunch
	\brief Class to detect multiple launches of a desktop application
	
	For some applications, it's desired that only a single instance
	is allowed to be running systemwide. This class will create a global
	object that's visible systemwide and if this object is in
	existence when this class is queried, then it means that
	this class is running multiple times. If the function
	IsMultiLaunched() returns \ref TRUE, then immediately exit
	the application since another is running elsewhere
	in the same system

	\code 

		void main()
		{
			// This must exist during the entire run of the application
			Burger::DetectMultiLaunch OneInstanceTest;
			if (OneInstanceTest.IsMultiLaunched("MyAwesome App")) {
				exit(10);		// Already running
			}
			DoStuff();
		}
	\endcode


***************************************/

/*! ************************************

	\fn Burger::DetectMultiLaunch::DetectMultiLaunch()
	\brief Initialize the object to a power up state.
	
	Variables are initialized but no action is taken
	
***************************************/

/*! ************************************

	\fn Burger::DetectMultiLaunch::~DetectMultiLaunch()
	\brief Teardown on exit

	If IsMultiLaunched() is called, a global object exists.
	Once the class is destroyed, so is the object.

***************************************/

/*! ************************************

	\fn uint_t Burger::DetectMultiLaunch::IsMultiLaunched(const char *pSignature)
	\brief Test if this is the only instance of this class systemwide.
	
	An object is created, for Windows, it's a FileMapping
	object of 32 bytes that stays in memory. If executed again,
	the recreation of the object will return a ERROR_ALREADY_EXISTS
	error that will tell the app there is already an instance running.

	\param pSignature Pointer to a UTF8 "C" string that's unique to the application
	\return \ref TRUE if there is another instance of the application already running, \ref FALSE if this is the only instance.

***************************************/
