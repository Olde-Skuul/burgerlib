/***************************************

	Console manager

	Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brconsolemanager.h"
#include "brfilemanager.h"
#include "brglobals.h"

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::ConsoleApp, Burger::Base);
#endif

/*! ************************************

	\class Burger::ConsoleApp
	\brief Base class for console applications.

	This class contains everything needed to create a cross-platform environment
	for writing console based applications. Functions are included to allow for
	the application to gain *drag and drop* functionality on platforms that
	support it.

***************************************/

/*! ************************************

	\brief Base constructor.

	When a console app is spawned using ANSI C++ main(), it is given two
	parameters argc and argv. These parameters are cached inside of this base
	class so the member functions can handle automatic parameter passing and
	decoding. The return code is initialized to zero.

***************************************/

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_MAC)) || defined(DOXYGEN)
Burger::ConsoleApp::ConsoleApp(int iArgc, const char** ppArgv, uint_t uFlags) BURGER_NOEXCEPT:
	m_ANSIMemoryManager(), m_bLaunchedFromDesktop(FALSE)
{
	BURGER_UNUSED(uFlags);

	// Assume the input is UTF8 for all other platforms
	m_ppArgv = ppArgv;
	m_iArgc = iArgc;

	// Init the file system
	FileManager::initialize();
}
#endif

/*! ************************************

	\brief Dispose of any allocated resources

	When a console app is shut down, dispose of everything here.

***************************************/

#if (!defined(BURGER_WINDOWS)) || defined(DOXYGEN)
Burger::ConsoleApp::~ConsoleApp()
{
	// Release the file system
	FileManager::shut_down();
}
#endif

/*! ************************************

	\fn Burger::ConsoleApp::SetArgv(const char **ppArgv)
	\brief Set the current argv parameter.

	Override the argv input value passed to main().

	\param ppArgv nullptr or pointer to an array of char pointers of command
		line parameters.

	\sa Burger::ConsoleApp::SetArgc(int)

***************************************/

/*! ************************************

	\fn Burger::ConsoleApp::GetArgv(void) const
	\brief Get the current argv parameter.

	Accessor to get the current argv input value passed to main().

	\return nullptr or pointer to an array of char pointers of command line
		parameters.
	\sa Burger::ConsoleApp::GetArgc() const

***************************************/

/*! ************************************

	\fn Burger::ConsoleApp::SetArgc(int iArgc)
	\brief Set the current argc parameter.

	Override the argc input value passed to main().

	\param iArgc New number of parameters present (Can be zero)
	\sa Burger::ConsoleApp::SetArgv(const char **)

***************************************/

/*! ************************************

	\fn Burger::ConsoleApp::GetArgc(void) const
	\brief Get the current argc parameter.

	Accessor to get the current argc input value passed to main().

	\return Integer with the number of valid argv commands. Can be zero.
	\sa Burger::ConsoleApp::GetArgv() const

***************************************/

/*! ************************************

	\brief Was the application launched from the desktop

	Returns \ref TRUE if the console application was launched by double clicking
	on the icon or \ref FALSE if the application was launched from a command
	line shell.

	\return \ref TRUE if desktop launched, \ref FALSE if from a shell.
	\sa Burger::ConsoleApp::GetArgv() const or
		Burger::ConsoleApp::GetArgc() const

***************************************/

#if !defined(BURGER_WINDOWS) || defined(DOXYGEN)
uint_t BURGER_API Burger::ConsoleApp::WasDesktopLaunched(void) BURGER_NOEXCEPT
{
	return m_bLaunchedFromDesktop;
}
#endif

/*! ************************************

	\brief Pause console output if the return code is not zero.

	If the error code is not set to zero, force the text output to remain on
	the screen if the application was launched from double clicking. If the
	application was launched from a console, this function does nothing.

	\param uError Error code to test.

***************************************/

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_MAC)) || defined(DOXYGEN)
void Burger::ConsoleApp::PauseOnError(Burger::eError uError)
{
	BURGER_UNUSED(uError);
}
#endif

/*! ************************************

	\fn Burger::ConsoleApp::ProcessFilenames(
		Burger::ConsoleApp::CallbackProc pCallback)
	\brief Handle drag and drop for console apps

	Detect if the application was launched from the Finder or from Explorer. If
	so, detect if it was because data files were "dropped" on the application
	for processing. If both cases are true, then call the user supplied function
	pointer for each file to be processed. The filenames are in Burgerlib
	format.

	\note This function will set the console return code to 1 on entry, so if
	the processing doesn't take place, it will assume an error has occurred. The
	processing procedure can set the return code to zero or any other value at
	will and that's the return code that will be retained.

	\param pCallback Function pointer to a call that accepts a Burgerlib
		filename.
	\return \ref TRUE if the function pointer was called. \ref FALSE if
		normal processing should occur.

***************************************/

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_MAC)) || defined(DOXYGEN)
uint_t BURGER_API Burger::ConsoleApp::ProcessFilenames(
	Burger::ConsoleApp::CallbackProc /* pCallback */)
{
	return FALSE; // Nothing handled.
}
#endif

/*! ************************************

	\brief Function for 2 or 3 argument tools.

	If the console application was invoked with argc == 2 or argc == 3, then
	execute the callback with a Burgerlib input filename and a NULL for the
	output filename (argc==2) or a burgerlib input filename and a burgerlib
	output filename (argc==3). There is no checking performed on the filenames
	to see if they are actually pointing to valid files.

	If argc!=2 and argc!=3 then set the error code to 10 and then the pUsage
	function is called so that instructions can be displayed. If pUsage is
	nullptr, no instructions will be printed.

	\param pCallback Pointer to a function to perform the actual work.
	\param pUsage Pointer to a function to print the instructions.
	\return The current error code. Can be returned to the operating system.
	\sa int Burger::ConsoleApp::SimpleTool(
		Burger::ConsoleApp::CallbackProc,Burger::ConsoleApp::UsageProc)

***************************************/

int BURGER_API Burger::ConsoleApp::InputAndOutput(
	Burger::ConsoleApp::CallbackProc pCallback,
	Burger::ConsoleApp::UsageProc pUsage)
{
	const int iArgc = GetArgc(); // Get the number of arguments remaining
	int iResult;
	if (iArgc != 2 && iArgc != 3) { // Only 2 or 3 parameters are valid
		if (pUsage) {
			pUsage(this); // Print the docs.
		}
		iResult = 10; // Set the error code
	} else {
		Filename FirstName;
		Filename SecondName;

		// Get the parm list
		const char** ppArgv = GetArgv();
		// Convert to Burgerlib pathname
		FirstName.set_native(ppArgv[1]);
		const char* pOutputFile = NULL;
		if (iArgc == 3) {
			// Second parameter?
			SecondName.set_native(ppArgv[2]);
			pOutputFile = SecondName.c_str();
		}
		// Call the function and exit
		iResult = pCallback(this, FirstName.c_str(), pOutputFile);
	}
	Globals::SetErrorCode(static_cast<Burger::eError>(iResult));
	return iResult; // Exit
}

/*! ************************************

	\brief Handle simple console applications

	For console applications that only accept 2 or 3 parameters, namely a file
	to input data and a file to output data, this helper function performs the
	drag and drop and mundane operations for setup and tear down of the command
	line.

	All is needed are two callbacks, one for the actual console application's
	conversion function and the other for printing the instructions to the
	console.

	\param pCallback Pointer to a function to perform the actual work.
	\param pUsage Pointer to a function to print the instructions.
	\return The current error code. Can be returned to the operating system.
	\sa int Burger::ConsoleApp::InputAndOutput(
		Burger::ConsoleApp::CallbackProc pCallback,
		Burger::ConsoleApp::UsageProc pUsage)

***************************************/

int BURGER_API Burger::ConsoleApp::SimpleTool(
	Burger::ConsoleApp::CallbackProc pCallback,
	Burger::ConsoleApp::UsageProc pUsage)
{
	// Handle drag and drop
	if (!ProcessFilenames(pCallback)) {
		InputAndOutput(pCallback, pUsage); // Handle the command line
	}
	// If an error occurred, pause the display if drag and drop
	PauseOnError(Globals::GetErrorCode());
	return Globals::GetErrorCode(); // Return error code to OS
}

/*! ************************************

	\brief Set the title of the console window.

	On platforms that present console text output in a desktop window, this
	function will set the title of that window to the specified UTF-8 string.
	On platforms that don't support such a string, this function does nothing
	and returns no error.

	\param pTitle UTF-8 "C" string for new terminal title string.

	\return The current error code. Can be returned to the operating system.
	\sa WasDesktopLaunched()

***************************************/

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_LINUX) && \
	!defined(BURGER_MACOS)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::ConsoleApp::SetWindowTitle(const char* pTitle)
{
	BURGER_UNUSED(pTitle);
	return kErrorNone;
}
#endif

/*! ************************************

	\brief Set the size of the console window.

	On platforms that present console text output in a desktop window, this
	function will resize the window to the specified width and height in text
	cells. On platforms that don't text window resizing, this function does
	nothing and returns no error.

	\note The maximum supported width and height is 32767 due to limits of the
		Windows API.

	\param uWidth Number of text cells wide of the new window
	\param uHeight Number of text cells high of the new window

	\return The current error code. Can be returned to the operating system.
	\sa WasDesktopLaunched()

***************************************/

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_LINUX) && \
	!defined(BURGER_MACOSX)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::ConsoleApp::SetWindowSize(
	uint_t uWidth, uint_t uHeight)
{
	BURGER_UNUSED(uWidth);
	BURGER_UNUSED(uHeight);
	return kErrorNone;
}
#endif
