/***************************************

	iOS application manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "briosapp.h"
#if defined(BURGER_IOS) || defined(DOXYGEN)
#include "brfilemanager.h"
#include "brfilename.h"
#include "brstringfunctions.h"

/*! ************************************

	\class Burger::iOSApp
	\brief Base class for iOS game applications.

	This class contains everything needed to create a
	cross-platform environment for writing console based
	applications. Functions are included to allow for
	the application to gain <i>drag and drop</i> functionality
	on platforms that support it.

***************************************/

/*! ************************************

	\brief Base constructor.

	Start up a iOS app

***************************************/

Burger::iOSApp::iOSApp(const char *pGameName,WordPtr uDefaultMemorySize,Word uDefaultHandleCount,WordPtr uMinReserveSize) :
	GameApp(uDefaultMemorySize,uDefaultHandleCount,uMinReserveSize)
{	
	// Init the file system
	FileManager::Init();
}

/*! ************************************

	\brief Dispose of any allocated resources

	When a iOS app is shut down, dispose of 
	everything here.

***************************************/

Burger::iOSApp::~iOSApp()
{
	// Release the file system
	FileManager::Shutdown();
}


#endif