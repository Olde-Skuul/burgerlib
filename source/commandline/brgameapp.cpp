/***************************************

	Game Application startup class

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brgameapp.h"
#include "bratomic.h"
#include "brdisplay.h"
#include "brjoypad.h"
#include "brkeyboard.h"
#include "brmouse.h"
#include "brsound.h"
#include "brtick.h"

#if defined(BURGER_WINDOWS)
#include "brdisplaydirectx9.h"
#endif

#if !defined(DOXYGEN)

#if defined(BURGER_WATCOM)
// Disable 'sizeof' operand contains compiler generated information
#pragma disable_message(549)
#endif

BURGER_CREATE_STATICRTTI_PARENT(Burger::GameApp, Burger::Base);
#endif

/*! ************************************

	\enum Burger::eEvent
	\brief Enumeration for input events

***************************************/

/*! ************************************

	\struct Burger::EventHeader_t
	\brief Base structure for events

	All events derive from this class. Based on the \ref eEvent	enumeration
	value in \ref m_uEvent, the derived class type can be determined

	\sa KeyEvent_t, MouseButtonEvent_t, MouseMotionEvent_t, MousePositionEvent_t
		or MouseWheelEvent_t

***************************************/

/*! ************************************

	\struct Burger::KeyEvent_t

	\brief Structure holding a keyboard event

	\ref m_uEvent must be set to \ref EVENT_KEYDOWN, \ref EVENT_KEYUP or \ref
		EVENT_KEYAUTO.

	When a keyboard event is generated, an event is	filled out and stored until
	it's retrieved by the application by one of the many keyboard event
	retrieval functions. This structure contains the scan code, Unicode code,
	modifiers and time in milliseconds of when the key was pressed.

	\sa EventHeader_t or eKeyFlags

***************************************/

/*! ************************************

	\struct Burger::MouseButtonEvent_t
	\brief Structure for mouse button events

	\ref m_uEvent must be set to \ref EVENT_MOUSEDOWN or \ref EVENT_MOUSEUP.

	When posting a \ref EVENT_MOUSEDOWN or \ref EVENT_MOUSEUP event, only
	the \ref m_uButtons value needs to be set since \ref m_uX and \ref m_uY
	will be filled in by the Mouse::PostMouseEvent() function.

	When receiving this event, all entries are valid.

	\sa EventHeader_t, MouseMotionEvent_t, MousePositionEvent_t or
		MouseWheelEvent_t

***************************************/

/*! ************************************

	\struct Burger::MouseMotionEvent_t
	\brief Structure for mouse motion events

	\ref m_uEvent must be set to \ref EVENT_MOUSEMOVE.

	This structure is used for posting a mouse motion delta event.

	\sa EventHeader_t, MouseButtonEvent_t, MousePositionEvent_t or
		MouseWheelEvent_t

***************************************/

/*! ************************************

	\struct Burger::MousePositionEvent_t
	\brief Structure for mouse position events

	\ref m_uEvent must be set to \ref EVENT_MOUSEPOSITION.

	This structure is used for posting a mouse position event.
	The values will be clamped to the Mouse::SetRange() parameters.

	\sa EventHeader_t, MouseButtonEvent_t, MouseMotionEvent_t or
		MouseWheelEvent_t

***************************************/

/*! ************************************

	\struct Burger::MouseWheelEvent_t
	\brief Structure for mouse wheel events

	\ref m_uEvent must be set to \ref EVENT_MOUSEWHEEL.

	This structure is used for posting a mouse wheel event.

	\sa EventHeader_t, MouseButtonEvent_t, MouseMotionEvent_t or
		MousePositionEvent_t

***************************************/

/*! ************************************

	\struct Burger::JoypadButtonEvent_t
	\brief Structure for joypad button events

	\ref m_uEvent must be set to \ref EVENT_JOYPADDOWN or \ref EVENT_JOYPADUP.

	This structure is used for posting a joypad button event.

	\sa EventHeader_t

***************************************/

/*! ************************************

	\class Burger::GameApp
	\brief Base class for video game application

	This class contains the startup code for creating an environment for
	creating all of the needed subclasses for the operation of a video game.
	It auto links to instances of the Keyboard, Joypad, Mouse, Sound, Renderer
	and Display classes so other classes only need to have a GameApp pointer to
	be able to detect their presence.

	A RunQueue is part of this class so background tasks can be executed from
	the main thread from time to time for background servicing

***************************************/

/*! ************************************

	\brief Initialize shared variables

	For variables that are present on all platforms, this function will clear
	them.

	\sa ShutdownDefaults(void)

***************************************/

void BURGER_API Burger::GameApp::InitDefaults(void) BURGER_NOEXCEPT
{
	m_pKeyboard = nullptr;
	m_pMouse = nullptr;
	m_pJoypad = nullptr;
	m_pSoundManager = nullptr;
	m_pDisplay = nullptr;
	m_ppArgv = nullptr;
	m_iArgc = 0;
	m_bQuit = FALSE;
	m_bInBackground = FALSE;
	m_bMinimized = FALSE;
	m_bAllowWindowSwitching = TRUE;
	m_bMouseOnScreen = FALSE;
	m_bWindowSwitchRequested = FALSE;
	m_bKeyboardStarted = FALSE;
	m_bMouseStarted = FALSE;
	m_bJoypadStarted = FALSE;
	m_bDisplayStarted = FALSE;
	m_bSoundManagerStarted = FALSE;
}

/*! ************************************

	\brief Release shared variables

	For variables that are present on all platforms, this function will clear
	them and release all resources under this classes' control.

	\sa InitDefaults(void)

***************************************/

void BURGER_API Burger::GameApp::ShutdownDefaults(void) BURGER_NOEXCEPT
{
	// Clear out the managers

	// Note, manually calling the base class destructor to
	// prevent the compiler from linking in these classes
	// if the application never created the instances

	SetKeyboard(nullptr);
	SetMouse(nullptr);
	SetJoypad(nullptr);
	SetSoundManager(nullptr);
	SetDisplay(nullptr);

	// Clear out the rest
	m_ppArgv = nullptr;
	m_iArgc = 0;

	// Release the RunQueue
	m_RunQueue.Clear();
}

/*! ************************************

	\brief Create an instance of a GameApp

	Upon startup, a handle based memory manager is instantiated and used for
	all future memory allocations. Variables are passed for setting the
	amount of memory the application could manage.

	This class is a base class. A system specific class will derive from this
	one which will contain system specific operating system calls and variables.

	\param uDefaultMemorySize Default memory block to manager for the handle
		based memory manager
	\param uDefaultHandleCount Default number of handles to create on startup.
	\param uMinReserveSize Default amount of memory to NOT allocate and leave in
		the hands of the operating system

***************************************/

#if !(defined(BURGER_XBOX360) || defined(BURGER_WINDOWS) || \
	defined(BURGER_DARWIN)) || \
	defined(DOXYGEN)
Burger::GameApp::GameApp(uintptr_t uDefaultMemorySize,
	uint_t uDefaultHandleCount, uintptr_t uMinReserveSize) BURGER_NOEXCEPT
	: m_MemoryManagerHandle(
		  uDefaultMemorySize, uDefaultHandleCount, uMinReserveSize)
{
	// Allow denormals on SSE registers
#if defined(BURGER_INTEL)
#if defined(BURGER_X86)
	CPUID_t ID;
	CPUID(&ID);
	if (ID.has_SSE())
#endif
		set_mxcsr_flags(0, 0x8040U);
#endif
	Tick::init();
	InitDefaults();
}
#endif

/*! ************************************

	\brief Base destructor for a GameApp

	Releases the RunQueue and memory manager

***************************************/

#if !(defined(BURGER_XBOX360) || defined(BURGER_WINDOWS) || \
	defined(BURGER_MACOSX) || defined(BURGER_IOS)) || \
	defined(DOXYGEN)
Burger::GameApp::~GameApp()
{
	// Clear out the managers
	ShutdownDefaults();
	Tick::shutdown();
}
#endif

/*! ************************************

	\fn void Burger::GameApp::Poll(void) noexcept
	\brief Perform cooperative multitasking

	Some systems only have one CPU, others require background tasks running in
	the primary thread. To handle these cases, all callbacks are logged in the
	GameApp via calls to AddRoutine() and RemoveRoutine().

	Since this background task can be called as often as possible, write any and
	all callbacks to do as little work as possible to prevent any sort of
	performance degradation.

	\sa AddRoutine() or RemoveRoutine()

***************************************/

/*! ************************************

	\fn void GameApp::AddRoutine(RunQueue::CallbackProc \
		Proc, RunQueue::CallbackProc pShutdown, void *pData,uint_t uPriority)

	\brief Add a RunQueue polling routine

	Given a proc pointer and a pointer to data to pass to the
	proc pointer, add this to the list of procedures that are called with
	each call to Poll(). The pointer pData is not
	used by the polling manager itself. Only the polling proc uses the
	pointer for it's internal use.

	\param Proc Pointer to a function of type RunQueue::CallbackProc
	\param pShutdown Pointer to a function to call when this is deleted, can be
		\ref NULL
	\param pData Pointer that is passed to the Proc upon calling. Otherwise,
		it's not used by Burger::GameApp.
	\param uPriority Priority value to use to determine the location of this new
		entry in the linked list.

	\sa Poll(), RemoveRoutine()

***************************************/

/*! ************************************

	\fn void GameApp::RemoveRoutine(RunQueue::CallbackProc Proc,void *pData)
	\brief Remove a RunQueue polling routine

	Given a proc pointer and a pointer to data to pass to the
	proc pointer, search the proc list and if a match is found, remove
	the proc from the list.

	\param Proc Pointer to a function of type RunQueue::CallbackProc
	\param pData Pointer that is passed to the Proc upon calling. Not used by
		the input manager.

	\sa Poll(), AddRoutine()

***************************************/

/*! ************************************

	\fn RunQueue *Burger::GameApp::GetRunQueue(void)
	\brief Get the pointer to the RunQueue

	Accessor to get the RunQueue associated with the application.

	\return Pointer to the RunQueue.
	\sa Poll(), AddRoutine(), and RemoveRoutine()

***************************************/

/*! ************************************

	\brief Create an instance of the \ref Keyboard manager

	If a \ref Keyboard manager was already allocated or manually assigned,
	return the pointer to the preexisting \ref Keyboard manager. Otherwise
	allocate a new instance of a \ref Keyboard manager and use it as the
	default.

	\note If a \ref Keyboard manager was allocated by this function, it
	will automatically be released when this class shuts down.

	\return Pointer to the \ref Keyboard manager
	\sa StartupEverything(void)

***************************************/

Burger::Keyboard* BURGER_API Burger::GameApp::StartupKeyboard(
	void) BURGER_NOEXCEPT
{
	// Get the previous instance
	Keyboard* pResult = m_pKeyboard;

	// If not valid and one wasn't allocated...
	if (!pResult && !m_bKeyboardStarted) {
		// Allocate it.
		pResult = new (Alloc(sizeof(Keyboard))) Keyboard(this);
		if (pResult) {

			// Success!
			m_pKeyboard = pResult;
			m_bKeyboardStarted = TRUE;
		}
	}

	// Return the requested instance
	return pResult;
}

/*! ************************************

	\brief Create an instance of the \ref Mouse manager

	If a \ref Mouse manager was already allocated or manually assigned,
	return the pointer to the preexisting \ref Mouse manager. Otherwise
	allocate a new instance of a \ref Mouse manager and use	it as the default.

	\note If a \ref Mouse manager was allocated by this function, it
	will automatically be released when this class shuts down.

	\return Pointer to the \ref Mouse manager
	\sa StartupEverything(void)

***************************************/

Burger::Mouse* BURGER_API Burger::GameApp::StartupMouse(void) BURGER_NOEXCEPT
{
	// Get the previous instance
	Mouse* pResult = m_pMouse;

	// If not valid and one wasn't allocated...
	if (!pResult && !m_bMouseStarted) {
		// Allocate it.
		pResult = new (Alloc(sizeof(Mouse))) Mouse(this);
		if (pResult) {

			// Success!
			m_pMouse = pResult;
			m_bMouseStarted = TRUE;
		}
	}

	// Return the requested instance
	return pResult;
}

/*! ************************************

	\brief Create an instance of the \ref Joypad manager

	If a \ref Joypad manager was already allocated or manually assigned,
	return the pointer to the preexisting \ref Joypad manager. Otherwise
	allocate a new instance of a \ref Joypad manager and use it as the default.

	\note If a \ref Joypad manager was allocated by this function, it
	will automatically be released when this class shuts down.

	\return Pointer to the \ref Joypad manager
	\sa StartupEverything(void)

***************************************/

Burger::Joypad* BURGER_API Burger::GameApp::StartupJoypad(void) BURGER_NOEXCEPT
{
	// Get the previous instance
	Joypad* pResult = m_pJoypad;

	// If not valid and one wasn't allocated...
	if (!pResult && !m_bJoypadStarted) {
		// Allocate it.
		pResult = new (Alloc(sizeof(Joypad))) Joypad(this);
		if (pResult) {

			// Success!
			m_pJoypad = pResult;
			m_bJoypadStarted = TRUE;
		}
	}

	// Return the requested instance
	return pResult;
}

/*! ************************************

	\brief Create an instance of the \ref Display manager

	If a \ref Display manager was already allocated or manually assigned,
	return the pointer to the preexisting \ref Display manager. Otherwise
	allocate a new instance of a \ref Display manager and use it as the default.

	\note If a \ref Display manager was allocated by this function, it
	will automatically be released when this class shuts down.

	\note On Windows, the default display is OpenGL. If a different
	renderer is desired like DirectX 9 or DirectX 11, create and
	attach the custom display instance before calling this function
	or \ref StartupEverything(void)

	\return Pointer to the \ref Display manager
	\sa StartupEverything(void)

***************************************/

Burger::Display* BURGER_API Burger::GameApp::StartupDisplay(
	void) BURGER_NOEXCEPT
{
	// Get the previous instance
	Display* pResult = m_pDisplay;

	// If not valid and one wasn't allocated...
	if (!pResult && !m_bDisplayStarted) {
		// Allocate it.
#if defined(BURGER_WINDOWS)
		pResult = new (Alloc(sizeof(DisplayDirectX9))) DisplayDirectX9(this);
#else
		pResult = new (Alloc(sizeof(Display))) Display(this);
#endif
		if (pResult) {

			// Success!
			m_pDisplay = pResult;
			m_bDisplayStarted = TRUE;
		}
	}

	// Return the requested instance
	return pResult;
}

/*! ************************************

	\brief Create an instance of the \ref SoundManager

	If a \ref SoundManager was already allocated or manually assigned,
	return the pointer to the preexisting \ref SoundManager . Otherwise
	allocate a new instance of a \ref SoundManager and use it as the default.

	\note If a \ref SoundManager was allocated by this function, it
	will automatically be released when this class shuts down.

	\return Pointer to the \ref SoundManager
	\sa StartupEverything(void)

***************************************/

Burger::SoundManager* BURGER_API Burger::GameApp::StartupSoundManager(
	void) BURGER_NOEXCEPT
{
	// Get the previous instance
	SoundManager* pResult = m_pSoundManager;

	// If not valid and one wasn't allocated...
	if (!pResult && !m_bSoundManagerStarted) {
		// Allocate it.
		pResult = new (Alloc(sizeof(SoundManager))) SoundManager(this);
		if (pResult) {

			// Success!
			m_pSoundManager = pResult;
			m_bSoundManagerStarted = TRUE;
		}
	}

	// Return the requested instance
	return pResult;
}

/*! ************************************

	\brief Create instances of all default classes for a game

	Game applications usually need Mouse/Keyboard/Joypad input,	video display
	and audio managers initialized. This function will initialize all five
	subsystems and return an error code if it failed.

	All managers this function creates will be disposed of when	this class shuts
	down

	\return Zero on success, error code if not.
	\sa StartupKeyboard(void), StartupMouse(void), StartupJoypad(void),
		StartupDisplay(void) or StartupSoundManager(void)S

***************************************/

uint_t BURGER_API Burger::GameApp::StartupEverything(void) BURGER_NOEXCEPT
{
	// Assume error
	uint_t uResult = 10;
	if (StartupDisplay() && StartupKeyboard() && StartupMouse() &&
		StartupJoypad() && StartupSoundManager()) {
		uResult = 0;
	}
	return uResult;
}

/*! ************************************

	\fn MemoryManagerGlobalHandle * Burger::GameApp::GetMemoryManager(void)
	\brief Get the pointer to the MemoryManagerGlobalHandle

	Accessor to get the MemoryManagerGlobalHandle associated with the
	application.

	\return Pointer to the MemoryManagerGlobalHandle.

***************************************/

/*! ************************************

	\fn void Burger::GameApp::SetArgv(const char **ppArgv)
	\brief Set the current argv parameter.

	Override the argv input value passed to main().

	\param ppArgv \ref NULL or pointer to an array of char pointers of command
		line parameters.

	\sa SetArgc(int)

***************************************/

/*! ************************************

	\fn const char ** Burger::GameApp::GetArgv(void) const
	\brief Get the current argv parameter.

	Accessor to get the current argv input value passed	to main().

	\return \ref NULL or pointer to an array of char pointers of command line
		parameters.

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

	Accessor to get the current argc input value passed to main().

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

	If an application decides to cancel a shutdown (User chooses to	return to
	the application instead of quitting), call this function to clear the flag.
	The application will return to executing the main loop.

	\sa GetQuitCode() and SetQuitCode()

***************************************/

/*! ************************************

	\fn uint_t Burger::GameApp::GetQuitCode(void) const
	\brief Get the current return code.

	Accessor to get the current return code that main() would give back to the
	operating system if the application exited immediately.

	\return Integer with main() return code.

***************************************/

/*! ************************************

	\fn void Burger::GameApp::SetInBackground(uint_t bInBackground)
	\brief Set the state if app should go into pause mode

	On many platforms, applications can be put in the background either through
	OS or user events from an external source. This flag is set and cleared by
	Burgerlib when the app is in the background or foreground.

	If the game is in progress and this flag is set, force the game to pause and
	wait for direct user input before resuming.

	\param bInBackground \ref TRUE if the application is in the background

***************************************/

/*! ************************************

	\fn uint_t Burger::GameApp::IsInBackground(void) const
	\brief Detect if the app is should go into pause mode

	On many platforms, applications can be put in the background either through
	OS or user events from an external source. This flag is set and cleared by
	Burgerlib when the app is in the background or foreground.

	If the game is in progress and this flag is set, force the game to pause and
	wait for direct user input before resuming.

	\return \ref TRUE if the application is in the background

***************************************/

/*! ************************************

	\brief Set the pointer to the current Keyboard class instance

	Sets the pointer to the active Keyboard instance

	\param pKeyboard Pointer to an active Keyboard instance or \ref NULL to
		disable the connection or delete the instance allocated by
		StartupKeyboard(void)

	\sa GetKeyboard(void) const or StartupKeyboard(void)

***************************************/

void BURGER_API Burger::GameApp::SetKeyboard(
	Keyboard* pKeyboard) BURGER_NOEXCEPT
{
	// Was the keyboard locally started?
	if (m_bKeyboardStarted) {

		// Is this pointer the same as before?
		if (pKeyboard != m_pKeyboard) {
			// It's not, so dispose of the allocated one
			Delete(static_cast<Base*>(m_pKeyboard));
			m_bKeyboardStarted = FALSE;
		}
	}
	// Set the new pointer
	m_pKeyboard = pKeyboard;
}

/*! ************************************

	\fn Keyboard *Burger::GameApp::GetKeyboard(void) const
	\brief Get the current Keyboard class instance

	\return A pointer to the active Keyboard class or \ref NULL if no class is
		active

	\sa SetKeyboard()

***************************************/

/*! ************************************

	\brief Set the pointer to the current Mouse class instance

	Sets the pointer to the active Mouse instance

	\param pMouse Pointer to an active Mouse instance or \ref NULL to disable
		the connection or delete the instance allocated by StartupMouse(void)

	\sa GetMouse(void) const or StartupMouse(void)

***************************************/

void BURGER_API Burger::GameApp::SetMouse(Mouse* pMouse) BURGER_NOEXCEPT
{
	// Was the keyboard locally started?
	if (m_bMouseStarted) {

		// Is this pointer the same as before?
		if (pMouse != m_pMouse) {
			// It's not, so dispose of the allocated one
			Delete(static_cast<Base*>(m_pMouse));
			m_bMouseStarted = FALSE;
		}
	}
	// Set the new pointer
	m_pMouse = pMouse;
}

/*! ************************************

	\fn Mouse *Burger::GameApp::GetMouse(void) const
	\brief Get the current Mouse class instance

	\return A pointer to the active Mouse class or \ref NULL if no class is
		active

	\sa SetMouse()

***************************************/

/*! ************************************

	\brief Set the pointer to the current Joypad class instance

	Sets the pointer to the active Joypad instance

	\param pJoypad Pointer to an active Joypad instance or \ref NULL to disable
		the connection or delete the instance allocated by StartupJoypad(void)

	\sa GetJoypad(void) const or StartupJoypad(void)

***************************************/

void BURGER_API Burger::GameApp::SetJoypad(Joypad* pJoypad) BURGER_NOEXCEPT
{
	// Was the keyboard locally started?
	if (m_bJoypadStarted) {

		// Is this pointer the same as before?
		if (pJoypad != m_pJoypad) {
			// It's not, so dispose of the allocated one
			Delete(static_cast<Base*>(m_pJoypad));
			m_bJoypadStarted = FALSE;
		}
	}
	// Set the new pointer
	m_pJoypad = pJoypad;
}

/*! ************************************

	\fn Joypad *Burger::GameApp::GetJoypad(void) const
	\brief Get the current Joypad class instance

	\return A pointer to the active Joypad class or \ref NULL if no class is
		active

	\sa SetJoypad()

***************************************/

/*! ************************************

	\brief Set the pointer to the current SoundManager class instance

	Sets the pointer to the active SoundManager instance

	\param pSetSoundManager Pointer to an active SoundManager instance or \ref
		NULL to disable the connection or delete the instance allocated by
		StartupSoundManager(void)

	\sa GetSoundManager(void) const or StartupSoundManager(void)

***************************************/

void BURGER_API Burger::GameApp::SetSoundManager(
	SoundManager* pSetSoundManager) BURGER_NOEXCEPT
{
	// Was the keyboard locally started?
	if (m_bSoundManagerStarted) {

		// Is this pointer the same as before?
		if (pSetSoundManager != m_pSoundManager) {
			// It's not, so dispose of the allocated one
			Delete(static_cast<Base*>(m_pSoundManager));
			m_bSoundManagerStarted = FALSE;
		}
	}
	// Set the new pointer
	m_pSoundManager = pSetSoundManager;
}

/*! ************************************

	\fn Sound *Burger::GameApp::GetSoundManager(void) const
	\brief Get the current SoundManager class instance

	\return A pointer to the active SoundManager class or \ref NULL if no class
		is active

	\sa SetSoundManager()

***************************************/

/*! ************************************

	\brief Set the pointer to the current Display class instance

	Sets the pointer to the active Display instance

	\param pDisplay Pointer to an active Display instance or \ref NULL to
		disable the connection or delete the instance allocated by
		StartupDisplay(void)

	\sa GetDisplay(void) const or StartupDisplay(void)

***************************************/

void BURGER_API Burger::GameApp::SetDisplay(Display* pDisplay) BURGER_NOEXCEPT
{
	// Was the keyboard locally started?
	if (m_bDisplayStarted) {

		// Is this pointer the same as before?
		if (pDisplay != m_pDisplay) {
			// It's not, so dispose of the allocated one
			Delete(static_cast<Base*>(m_pDisplay));
			m_bDisplayStarted = FALSE;
		}
	}
	// Set the new pointer
	m_pDisplay = pDisplay;
}

/*! ************************************

	\fn Display *Burger::GameApp::GetDisplay(void) const
	\brief Get the current Display class instance

	\return A pointer to the active Display class or \ref NULL if no class is
		active

	\sa SetDisplay()

***************************************/

/*! ************************************

	\fn void Burger::GameApp::SetWindowSwitching(uint_t)
	\brief Enable the Alt-Enter key

	Enable the desktop application to switch from full screen to window mode
	by pressing Alt-Enter or equivalent for the native operating system.

	\param bAllowWindowSwitching \ref TRUE to enable switching, \ref FALSE to
		disable

	\sa IsWindowSwitchingAllowed()

***************************************/

/*! ************************************

	\fn uint_t Burger::GameApp::IsWindowSwitchingAllowed(void) const
	\brief Get the flag for video mode switching

	\return \ref TRUE if switching video modes is allowed, \ref FALSE if not
	\sa SetWindowSwitching()

***************************************/

/*! ************************************

	\brief Switch from full screen to windowed mode and back again

	If supported, both by platform and by having the m_bAllowWindowSwitching
	flag being non-zero, this function will switch to full screen if the
	game is in a window and vice versa.

	\return Zero if no error occurred and the mode was switched, or non-zero
		if not supported or there was an error in the mode switch.

	\sa SetWindowSwitching(uint_t) or IsWindowSwitchingAllowed(void) const

***************************************/

uint_t BURGER_API Burger::GameApp::SwitchVideo(void) BURGER_NOEXCEPT
{
	uint_t uResult = 10;
	// Is switching allowed?
	if (m_bAllowWindowSwitching) {
		// Is there a display class?
		Display* pDisplay = GetDisplay();
		if (pDisplay) {
#if defined(BURGER_WINDOWS)
			KillInputFocus();
#endif
			uResult = pDisplay->Init(
				0, 0, 0, pDisplay->GetFlags() ^ Display::FULLSCREEN);
#if defined(BURGER_WINDOWS)
			GetInputFocus();
#endif
		}
	}
	m_bWindowSwitchRequested = FALSE;
	return uResult;
}

/*! ************************************

	\fn uint_t Burger::GameApp::IsMinimized(void) const
	\brief Return \ref TRUE if the application is in the background or minimized

	Determine if the state of the application is for it to be minimized
	and not drawing anything.

	\return \ref TRUE if the app is minimized, \ref FALSE if not
	\sa SetMinimized()

***************************************/

/*! ************************************

	\fn void Burger::GameApp::SetMinimized(uint_t bMinimized)
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

uint_t BURGER_API Burger::GameApp::IsAppFullScreen(void) const BURGER_NOEXCEPT
{
	uint_t uResult = FALSE;
	const Display* pDisplay = m_pDisplay;
	if (pDisplay) {
		if (pDisplay->GetFlags() & Display::FULLSCREEN) {
			uResult = TRUE;
		}
	}
	return uResult;
}

/*! ************************************

	\brief Return \ref TRUE if the application can be resized at runtime

	\note \ref TRUE does not mean that the screen is currently enabled, it
	only means the application can accept desktop events that will change
	the screen size.

	\return \ref TRUE if the screen can change sizes. \ref FALSE if not
	\sa IsMinimized() or IsAppFullScreen()

***************************************/

uint_t BURGER_API Burger::GameApp::IsResizingAllowed(void) const BURGER_NOEXCEPT
{
	uint_t uResult = FALSE;
	const Display* pDisplay = m_pDisplay;
	if (pDisplay) {
		const uint_t uFlags = pDisplay->GetFlags();
		if (!(uFlags & Display::FULLSCREEN) &&
			(uFlags & Display::ALLOWRESIZING)) {
			// Not full screen and it's marked as allowed
			uResult = TRUE;
		}
	}
	return uResult;
}

/*! ************************************

	\fn void Burger::GameApp::SetMouseOnScreen(uint_t bMouseOnScreen)
	\brief Sets the application flag if a mouse cursor is on the game screen

	\param bMouseOnScreen \ref TRUE if the mouse cursor is on the game screen,
		\ref FALSE if not

	\sa IsMouseOnScreen()

***************************************/

/*! ************************************

	\fn uint_t Burger::GameApp::IsMouseOnScreen(void) const
	\brief Return \ref TRUE if the mouse cursor is on the game screen

	Return if a game drawn mouse cursor should be drawn. Do not draw
	if this flag is \ref FALSE because it means that the mouse cursor
	is somewhere else on the desktop.

	\return \ref TRUE if the mouse cursor is on the game screen, \ref FALSE if
		not

	\sa SetMouseOnScreen(uint_t)

***************************************/

/*! ************************************

	\fn void Burger::GameApp::SetWindowSwitchRequested(uint_t \
		bWindowSwitchRequested)

	\brief Sets the application flag to toggle between full screen and window
		mode.

	\param bWindowSwitchRequested \ref TRUE if a mode switch is desired, \ref
		FALSE if not

	\sa IsWindowSwitchRequested(void) const

***************************************/

/*! ************************************

	\fn uint_t Burger::GameApp::IsWindowSwitchRequested(void) const
	\brief Return \ref TRUE if there is a pending video mode switch

	The key command ALT-Enter can be issued by another thread, so to
	ensure the mode switch is done properly, this flag is set to alert
	the main thread that a full screen to window or vice versa mode
	switch is desired.

	\return \ref TRUE if a mode switch is desired, \ref FALSE if not
	\sa SetWindowSwitchRequested(uint_t)

***************************************/

/*! ************************************

	\fn int BURGER_API CodeEntry(Burger::GameApp *pGameApp);
	\brief Main entry of the application code

	Due to the mish mosh of startup code methods across platforms, Burgerlib
	uses the inclusion of the header <brstartup.h> to create the proper code to
	initialize an application and then it will call this function to start
	application execution. This is the prototype of the function that must exist
	in a Burgerlib application. This function is not supplied by Burgerlib.

	\code

	#include <burger.h>

	// Only include once in the whole project
	#include <brstartup.h>

	int BURGER_API CodeEntry(Burger::GameApp *pGameApp)
	{
		// Initialize the keyboard
		Burger::Keyboard KeyInstance(pGameApp);
		// Initialize the mouse
		Burger::Mouse MouseInstance(pGameApp);

		// Run the game
		DoAGreatGame(pGameApp);
		return 0;		// No errors
	}
	\endcode

	\return Zero if the application executed without error, non-zero if not.
	\sa Burger::GameApp, Burger::Keyboard or Burger::Mouse

***************************************/
