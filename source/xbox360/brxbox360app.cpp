/***************************************

    XBox 360 application manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brgameapp.h"
#if defined(BURGER_XBOX360) || defined(DOXYGEN)
#include "brfilemanager.h"
#define NOD3D
#define NONET
#include <xtl.h>

/***************************************

	\brief Base constructor.

	When a Windows app is spawned using WinMain(),
	it is given the application instance. This parameter
	are cached inside of this base class so the member
	functions can handle automatic parameter passing
	and decoding. The return code is initialized to zero.

***************************************/

Burger::GameApp::GameApp(WordPtr uDefaultMemorySize,Word uDefaultHandleCount,WordPtr uMinReserveSize) :
	m_MemoryManagerHandle(uDefaultMemorySize,uDefaultHandleCount,uMinReserveSize)
{
	InitDefaults();
	// Init the file system
	FileManager::Init();
}

/***************************************

	\brief Dispose of any allocated resources

	When a windows app is shut down, dispose of
	everything here.

***************************************/

Burger::GameApp::~GameApp()
{
	// Release the file system
	FileManager::Shutdown();
	ShutdownDefaults();
}


#endif