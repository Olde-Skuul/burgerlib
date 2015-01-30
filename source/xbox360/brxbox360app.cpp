/***************************************

	XBox 360 application manager

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brxbox360app.h"
#if defined(BURGER_XBOX360) || defined(DOXYGEN)
#include "brfilemanager.h"
#define NOD3D
#define NONET
#include <xtl.h>

/*! ************************************

	\class Burger::WindowsApp
	\brief Base class for window game applications.

	This class contains everything needed to create a
	cross-platform environment for writing console based
	applications. Functions are included to allow for
	the application to gain <i>drag and drop</i> functionality
	on platforms that support it.

***************************************/

/*! ************************************

	\brief Base constructor.

	When a Windows app is spawned using WinMain(),
	it is given the application instance. This parm
	are cached inside of this base class so the member
	functions can handle automatic parameter passing
	and decoding. The return code is initialized to zero.

***************************************/

Burger::Xbox360App::Xbox360App(WordPtr uDefaultMemorySize,Word uDefaultHandleCount,WordPtr uMinReserveSize) :
	GameApp(uDefaultMemorySize,uDefaultHandleCount,uMinReserveSize)
{
	// Init the file system
	FileManager::Init();
}

/*! ************************************

	\brief Dispose of any allocated resources

	When a windows app is shut down, dispose of 
	everything here.

***************************************/

Burger::Xbox360App::~Xbox360App()
{
	// Release the file system
	FileManager::Shutdown();
}


#endif