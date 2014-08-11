/***************************************

	MacOSX application manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brmacosxapp.h"
#if defined(BURGER_MACOSX) || defined(DOXYGEN)
#include "broscursor.h"
#include "brfilemanager.h"
#include "brfilename.h"
#include "brstringfunctions.h"

/*! ************************************

	\class Burger::MacOSXApp
	\brief Base class for MacOSX game applications.

	This class contains everything needed to create a
	cross-platform environment for writing console based
	applications. Functions are included to allow for
	the application to gain <i>drag and drop</i> functionality
	on platforms that support it.

***************************************/

/*! ************************************

	\brief Base constructor.

	Start up a MacOSX app

***************************************/

Burger::MacOSXApp::MacOSXApp(const char *pGameName,WordPtr uDefaultMemorySize,Word uDefaultHandleCount,WordPtr uMinReserveSize) :
	GameApp(uDefaultMemorySize,uDefaultHandleCount,uMinReserveSize)
{

	// Init the global cursor
	OSCursor::Init();
	
	// Init the file system
	FileManager::Init();
	
	// Since MacOSX Apps can be standalone filenames
	// or they can be in a ".app" bundle, let's figure it out
	
	// If it's not a bundle, directory 9 is fine.
	
	// First test if in a bundle. Is the code in a folder called "MacOS"?
	
	Filename Name;
	FileManager::GetPrefix(&Name,9);
	const char *pFileName = Name.GetPtr();
	WordPtr uLength = StringLength(pFileName);
	if ((uLength>7) && !StringCompare((pFileName+uLength)-7,":MacOS:")) {
		
		// Secondary test. Does the PkgInfo file exist?
		if (FileManager::DoesFileExist("9:..:PkgInfo")) {
			
			// The application is in a bundle
			// If there is a folder called "Resources", assume that's where the data resides,
			// otherwise, pop three folders to get to the sam folder as the app and
			// assume the data is next to the application
			
			if (FileManager::DoesFileExist("9:..:Resources")) {
				FileManager::SetPrefix(9,"9:..:Resources");
			} else {
				// Pop prefixes so the data is external
				FileManager::PopPrefix(9);
				FileManager::PopPrefix(9);
				FileManager::PopPrefix(9);
			}
		}
	}
}

/*! ************************************

	\brief Dispose of any allocated resources

	When a OSX app is shut down, dispose of 
	everything here.

***************************************/

Burger::MacOSXApp::~MacOSXApp()
{
	// Release the file system
	FileManager::Shutdown();

	// Release the cursor
	OSCursor::Shutdown();
}


#endif