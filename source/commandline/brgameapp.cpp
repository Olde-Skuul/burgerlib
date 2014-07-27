/***************************************

	Game Application startup class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brgameapp.h"
#include "brdisplay.h"

/*! ************************************

	\class Burger::GameApp
	\brief Base class for video game application

	This class contains the startup code for creating an environment for
	creating all of the needed subclasses for the operation of a video game.
	It auto links to instances of the Keyboard, Joypad, Mouse, Sound, Renderer and Display classes
	so other classes only need to have a GameApp pointer to be able
	to detect their presence.

	A RunQueue is part of this class so background tasks can be executed from the main
	thread from time to time for background servicing

***************************************/

/*! ************************************

	\brief Create an instance of a GameApp

	Upon startup, a handle based memory manager is instanciated and used for
	all future memory allocations. Variables are passed for setting the 
	amount of memory the application could manage.

	This class is a base class. A system specific class will derive from this one
	which will contain system specific operating system calls and variables.

	\param uDefaultMemorySize Default memory block to manager for the handle based memory manager
	\param uDefaultHandleCount Default number of handles to create on startup.
	\param uMinReserveSize Default amount of memory to NOT allocate and leave in the hands of the operating system

***************************************/

Burger::GameApp::GameApp(WordPtr uDefaultMemorySize,Word uDefaultHandleCount,WordPtr uMinReserveSize) :
	m_pKeyboard(NULL),
	m_pMouse(NULL),
	m_pJoypad(NULL),
	m_pSound(NULL),
	m_pDisplay(NULL),
	m_pRenderer(NULL),
	m_ppArgv(NULL),
	m_MemoryManagerHandle(uDefaultMemorySize,uDefaultHandleCount,uMinReserveSize),
	m_RunQueue(),
	m_iArgc(0),
	m_bQuit(FALSE),
	m_bInBackground(FALSE),
	m_bMinimized(FALSE),
	m_bAllowWindowSwitching(TRUE)
{
}

/*! ************************************

	\brief Base destructor for a GameApp

	Releases the RunQueue and memory manager

***************************************/

Burger::GameApp::~GameApp()
{
	// Clear out the managers
	m_pKeyboard = NULL;
	m_pMouse = NULL;
	m_pJoypad = NULL;
	m_pSound = NULL;
	m_pDisplay = NULL;
	m_pRenderer = NULL;
	m_ppArgv = NULL;
	m_iArgc = 0;
}

/*! ************************************

	\fn void GameApp::Poll(void)
	\brief Perform cooperative multitasking

	Some systems only have one CPU, others require background tasks running in the
	primary thread. To handle these cases, all callbacks are logged in the GameApp
	via calls to AddRoutine() and RemoveRoutine().

	Since this background task can be called as often as possible, write any and
	all callbacks to do as little work as possible to prevent any sort of performance
	degradation.

	\sa AddRoutine() or RemoveRoutine()

***************************************/

/*! ************************************

	\fn void GameApp::AddRoutine(RunQueue::CallbackProc Proc,void *pData)
	\brief Add a RunQueue polling routine

	Given a proc pointer and a pointer to data to pass to the
	proc pointer, add this to the list of procs that are called with
	each call to Poll(). The pointer pData is not
	used by the polling manager itself. Only the polling proc uses the
	pointer for it's internal use.

	\param Proc Pointer to a function of type RunQueue::CallbackProc
	\param pData Pointer that is passed to the Proc upon calling. Otherwise, it's not used by Burger::GameApp.

	\sa Poll(), RemoveRoutine()

***************************************/

/*! ************************************

	\fn void GameApp::RemoveRoutine(RunQueue::CallbackProc Proc,void *pData)
	\brief Remove a RunQueue polling routine

	Given a proc pointer and a pointer to data to pass to the
	proc pointer, search the proc list and if a match is found, remove
	the proc from the list.

	\param Proc Pointer to a function of type RunQueue::CallbackProc
	\param pData Pointer that is passed to the Proc upon calling. Not used by the input manager.

	\sa Poll(), AddRoutine()

***************************************/

/*! ************************************

	\fn RunQueue *Burger::GameApp::GetRunQueue(void)
	\brief Get the pointer to the RunQueue

	Accessor to get the RunQueue associatied with the application.
	
	\return Pointer to the RunQueue.
	\sa Poll(), AddRoutine(), and RemoveRoutine()

***************************************/

/*! ************************************

	\fn MemoryManagerGlobalHandle * Burger::GameApp::GetMemoryManager(void)
	\brief Get the pointer to the MemoryManagerGlobalHandle

	Accessor to get the MemoryManagerGlobalHandle associatied with the application.
	
	\return Pointer to the MemoryManagerGlobalHandle.

***************************************/

/*! ************************************

	\fn void Burger::GameApp::SetArgv(const char **ppArgv)
	\brief Set the current argv parameter.

	Override the argv input value passed to main().
	
	\param ppArgv \ref NULL or pointer to an array of char pointers of command line parameters.
	\sa SetArgc(int)

***************************************/

/*! ************************************

	\fn const char ** Burger::GameApp::GetArgv(void) const
	\brief Get the current argv parameter.

	Accessor to get the current argv input value passed
	to main().
	
	\return \ref NULL or pointer to an array of char pointers of command line parameters.
	\sa GetArgc() const

***************************************/

/*! ************************************

	\fn void Burger::GameApp::SetArgc(int iArgc)
	\brief Set the current argc parameter.

	Override the argc input value passed to main().
	
	\param iArgc New number of parameters present (Can be zero)
	\sa SetArgv(const char **)

***************************************/

/*! ************************************

	\fn int Burger::GameApp::GetArgc(void) const
	\brief Get the current argc parameter.

	Accessor to get the current argc input value passed
	to main().
	
	\return Integer with the number of valid argv commands. Can be zero.
	\sa GetArgv() const

***************************************/


/*! ************************************

	\fn void Burger::GameApp::SetQuitCode(void)
	\brief Alert the application to shut down

	Set a flag so that at the end of the update/draw loop, the application can
	exit cleanly. 

	\sa GetQuitCode() and ClearQuitCode()

***************************************/

/*! ************************************

	\fn void Burger::GameApp::ClearQuitCode(void)
	\brief Cancel an application shutdown

	If an application decides to cancel a shutdown (User chooses to
	return to the application instead of quitting), call this
	function to clear the flag. The application will return to executing the main loop.

	\sa GetQuitCode() and SetQuitCode()

***************************************/

/*! ************************************

	\fn Word Burger::GameApp::GetQuitCode(void) const
	\brief Get the current return code.

	Accessor to get the current return code that main()
	would give back to the operating system if the application
	exited immediately.
	
	\return Integer with main() return code.

***************************************/

/*! ************************************

	\fn void Burger::GameApp::SetInBackground(Word bInBackground)
	\brief Set the state if app should go into pause mode

	On many platforms, apps can be put in the background either
	through OS or user events from an external source. This flag
	is set and cleared by Burgerlib when the app is in the background
	or foreground.

	If the game is in progress and this flag is set, force the game to
	pause and wait for direct user input before resuming.

	\param bInBackground \ref TRUE if the application is in the background

***************************************/

/*! ************************************

	\fn Word Burger::GameApp::IsInBackground(void) const
	\brief Detect if the app is should go into pause mode

	On many platforms, apps can be put in the background either
	through OS or user events from an external source. This flag
	is set and cleared by Burgerlib when the app is in the background
	or foreground.

	If the game is in progress and this flag is set, force the game to
	pause and wait for direct user input before resuming.

	\return \ref TRUE if the application is in the background

***************************************/

/*! ************************************

	\fn void Burger::GameApp::SetKeyboard(Keyboard *pKeyboard)
	\brief Set the pointer to the current Keyboard class instance

	Sets the pointer to the active Keyboard instance

	\param pKeyboard Pointer to an active Keyboard instance or \ref NULL to disable the connection
	\sa GetKeyboard()

***************************************/

/*! ************************************

	\fn Keyboard *Burger::GameApp::GetKeyboard(void) const
	\brief Get the current Keyboard class instance

	\return A pointer to the active Keyboard class or \ref NULL if no class is active
	\sa SetKeyboard()

***************************************/

/*! ************************************

	\fn void Burger::GameApp::SetMouse(Mouse *pMouse)
	\brief Set the pointer to the current Mouse class instance

	Sets the pointer to the active Mouse instance

	\param pMouse Pointer to an active Mouse instance or \ref NULL to disable the connection
	\sa GetMouse()

***************************************/

/*! ************************************

	\fn Mouse *Burger::GameApp::GetMouse(void) const
	\brief Get the current Mouse class instance

	\return A pointer to the active Mouse class or \ref NULL if no class is active
	\sa SetMouse()

***************************************/

/*! ************************************

	\fn void Burger::GameApp::SetJoypad(Joypad *pJoypad)
	\brief Set the pointer to the current Joypad class instance

	Sets the pointer to the active Joypad instance

	\param pJoypad Pointer to an active Joypad instance or \ref NULL to disable the connection
	\sa GetJoypad()

***************************************/

/*! ************************************

	\fn Joypad *Burger::GameApp::GetJoypad(void) const
	\brief Get the current Joypad class instance

	\return A pointer to the active Joypad class or \ref NULL if no class is active
	\sa SetJoypad()

***************************************/

/*! ************************************

	\fn void Burger::GameApp::SetSound(Sound *pSound)
	\brief Set the pointer to the current Sound class instance

	Sets the pointer to the active Sound instance

	\param pSound Pointer to an active Sound instance or \ref NULL to disable the connection
	\sa GetSound()

***************************************/

/*! ************************************

	\fn Sound *Burger::GameApp::GetSound(void) const
	\brief Get the current Sound class instance

	\return A pointer to the active Sound class or \ref NULL if no class is active
	\sa SetSound()

***************************************/

/*! ************************************

	\fn void Burger::GameApp::SetDisplay(Display *pDisplay)
	\brief Set the pointer to the current Display class instance

	Sets the pointer to the active Display instance

	\param pDisplay Pointer to an active Display instance or \ref NULL to disable the connection
	\sa GetDisplay()

***************************************/

/*! ************************************

	\fn Display *Burger::GameApp::GetDisplay(void) const
	\brief Get the current Display class instance

	\return A pointer to the active Display class or \ref NULL if no class is active
	\sa SetDisplay()

***************************************/

/*! ************************************

	\fn void Burger::GameApp::SetRenderer(Renderer *pRenderer)
	\brief Set the pointer to the current Renderer class instance

	Sets the pointer to the active Renderer instance

	\param pRenderer Pointer to an active Renderer instance or \ref NULL to disable the connection
	\sa GetRenderer()

***************************************/

/*! ************************************

	\fn Display *Burger::GameApp::GetRenderer(void) const
	\brief Get the current Renderer class instance

	\return A pointer to the active Renderer class or \ref NULL if no class is active
	\sa SetRenderer()

***************************************/

/*! ************************************

	\fn void Burger::GameApp::SetWindowSwitching(Word)
	\brief Enable the Alt-Enter key

	Enable the desktop application to switch from full screen to window mode
	by pressing Alt-Enter or equivalent for the native operating system.

	\param bAllowWindowSwitching \ref TRUE to enable switching, \ref FALSE to disable
	\sa IsWindowSwitchingAllowed()

***************************************/

/*! ************************************

	\fn Word Burger::GameApp::IsWindowSwitchingAllowed(void) const
	\brief Get the flag for video mode switching

	\return \ref TRUE if switching video modes is allowed, \ref FALSE if not
	\sa SetWindowSwitching()

***************************************/

/*! ************************************

	\fn Word Burger::GameApp::IsMinimized(void) const
	\brief Return \ref TRUE if the application is in the background or minimized

	Determine if the state of the application is for it to be minimized
	and not drawing anything.

	\return \ref TRUE if the app is minimized, \ref FALSE if not
	\sa SetMinimized()

***************************************/

/*! ************************************

	\fn void Burger::GameApp::SetMinimized(Word bMinimized)
	\brief Sets the application flag for being in the background or minimized

	\param bMinimized \ref TRUE if the app is minimized, \ref FALSE if not
	\sa SetMinimized()

***************************************/

/*! ************************************

	\brief Return \ref TRUE if the application is in full screen mode

	\note \ref TRUE does not mean that the screen is currently enabled, it
	only means the application expects to be full screen when in the foreground.
	If the app is in the background, it is minimized and therefore drawing
	is not necessary.

	\return \ref TRUE if the screen mode is full screen. \ref FALSE if not
	\sa IsMinimized()

***************************************/

Word Burger::GameApp::IsAppFullScreen(void) const
{
	Word uResult = FALSE;
	const Display *pDisplay = m_pDisplay;
	if (pDisplay) {
		if (pDisplay->GetFlags() & Display::FULLSCREEN) {
			uResult = TRUE;
		}
	}
	return uResult;
}
