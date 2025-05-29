/***************************************

	Windows application manager

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brgameapp.h"
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brdebug.h"
#include "brdisplay.h"
#include "brfilemanager.h"
#include "brglobals.h"
#include "brjoypad.h"
#include "brkeyboard.h"
#include "brmouse.h"
#include "broscursor.h"
#include "brstring16.h"
#include "brutf8.h"
#include "win_globals.h"
#include "win_winutils.h"
#include "brtick.h"

#if !defined(DOXYGEN)

#if !defined(_CRT_DECLARE_GLOBAL_VARIABLES_DIRECTLY)
// Needed for Visual Studio 2015 or higher
#define _CRT_DECLARE_GLOBAL_VARIABLES_DIRECTLY
#endif
#endif

#include "win_windows.h"

#include <CommCtrl.h>
#include <ObjBase.h>
#include <WindowsX.h>
#include <mmsystem.h>
#include <shellapi.h>
#include <stdlib.h>

#if !defined(DOXYGEN)
// Needed for code that manually grabs the parm list

extern "C" char** __argv;

#if defined(BURGER_WATCOM)
// Needed for InitCommonControls()
#pragma library("Comctl32.lib")
// Needed for timeBeginPeriod
#pragma library("Winmm.lib")
#else
#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "Winmm.lib")
#endif

#endif

/*! ************************************

	\class Burger::WindowsApp
	\brief Base class for window game applications.

	This class contains everything needed to create a cross-platform environment
	for writing console based applications. Functions are included to allow for
	the application to gain _drag and drop_ functionality on platforms that
	support it.

***************************************/

/*! ************************************

	\brief Base constructor.

	When a Windows app is spawned using WinMain(), it is given the application
	instance. This parm are cached inside of this base class so the member
	functions can handle automatic parameter passing and decoding. The return
	code is initialized to zero.

***************************************/

Burger::GameApp::GameApp(uintptr_t uDefaultMemorySize,
	uint_t uDefaultHandleCount, uintptr_t uMinReserveSize) BURGER_NOEXCEPT
	: m_MemoryManagerHandle(
		  uDefaultMemorySize, uDefaultHandleCount, uMinReserveSize),
	  //	m_hInstance(nullptr),		// Will initialize below
	  m_hWindow(nullptr),
	  m_hMenu(nullptr),
	  m_pDefaultCursor(nullptr),
	  m_ppOldArgv(nullptr),
	  m_pCallBack(nullptr),
	  m_uErrorMode(0),
	  m_bCoCreateInstanceInit(FALSE),
	  m_bInSizeMove(FALSE)
{
	m_WindowRect.Clear();

	// Set the global process instance
	HINSTANCE hInstance = GetModuleHandleW(nullptr);
	m_hInstance = hInstance;
	Win32::set_instance(hInstance);

	// Increase the speed of the timer from 15.6 ticks per second to
	// 1000 ticks per second so sleep_ms() actually has millisecond
	// accuracy
	timeBeginPeriod(1);

	// Ensure that threading is serialized since it's assumed this
	// is a GUI based application

	// Also disable OLE 1.0, since any code from XP on shouldn't use that

	if (CoInitializeEx(
			nullptr, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE) == S_OK) {
		m_bCoCreateInstanceInit = TRUE;
	}

	// Make the app handle all of its own errors
	m_uErrorMode =
		SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);

	// Not always needed, but lets the app create GDI dialogs under Windows XP
	// More recent operating systems, this function does nothing
	InitCommonControls();

	// Set up the shared values
	Tick::init();
	InitDefaults();

	// In order to support unicode command lines under windows,
	// the command line needs to be re-processed by calling the
	// shellapi and manually extracting the commands and
	// convert them to UTF-8

	// Get the UTF-16 command line string from windows
	LPWSTR* pWideArgv = CommandLineToArgvW(GetCommandLineW(), &m_iArgc);
	// Get the parameter count
	int iArgc = m_iArgc;
	// Do I bother?
	if (iArgc > 0) {
		// Allocate a buffer to contain the string pointers and all of
		// the strings. This allows the pointers to be
		// manipulated within the list without
		// the strings being leaked on shutdown
		// since when the pointer array is released, it
		// also releases the strings as well

		// Determine how much space the strings will take as
		// UTF8
		uint16_t** ppWork = reinterpret_cast<uint16_t**>(pWideArgv);
		uintptr_t uDataSize = 0;
		do {
			uDataSize += UTF8::GetUTF16Size(ppWork[0]) + 1;
			++ppWork;
		} while (--iArgc);

		// Allocate the buffer for a pointer array and the strings
		iArgc = m_iArgc;
		const char** ppArgv = static_cast<const char**>(
			allocate_memory((sizeof(char*) * iArgc) + uDataSize));

		// Store the pointer for later use and disposal
		m_ppArgv = ppArgv;
		// Get the pointer beyond the pointer array
		char* pDest = reinterpret_cast<char*>(ppArgv) + (sizeof(char*) * iArgc);
		ppWork = reinterpret_cast<uint16_t**>(pWideArgv);

		// This "hack" is to allow any other code that manually grabs the
		// parameter list to inherit the UTF8 support
		m_ppOldArgv = const_cast<const char**>(__argv);
		//__argv = const_cast<char**>(ppArgv);

		// Convert all the strings
		do {
			// Store the pointer in the array
			ppArgv[0] = pDest;
			++ppArgv;
			// Convert a string
			uintptr_t uLength =
				UTF8::from_UTF16(pDest, uDataSize, ppWork[0]) + 1;
			// Adjust the pointers
			uDataSize -= uLength;
			pDest += uLength;
			++ppWork;
		} while (--iArgc);
	}
	// Release the data Windows gave me for the parsed parameters
	// See docs for CommandLineToArgvW()
	LocalFree(pWideArgv);

	// Add the windows callback function
	// and set it to be the first entry to be called
	m_RunQueue.Add(Poll, NULL, this, RunQueue::PRIORITY_FIRST);

	// Init the global cursor
	OSCursor::Init();

	// Init the file system
	FileManager::initialize();
}

/*! ************************************

	\brief Dispose of any allocated resources

	When a windows app is shut down, dispose of everything here.

***************************************/

Burger::GameApp::~GameApp()
{
	m_pCallBack = NULL;
	RemoveRoutine(Poll, this);

	// Release the file system
	FileManager::shut_down();

	// Release the cursor
	OSCursor::Shutdown();

	// Release the command line
	free_memory(m_ppArgv);
	m_ppArgv = nullptr;

	// Restore the previous command line so crt0.s can clean it up
	if (m_ppOldArgv) {
		__argv = const_cast<char**>(m_ppOldArgv);
		m_ppOldArgv = nullptr;
	}

	// Clear out the default variables
	// It also unlinks the Display class
	ShutdownDefaults();
	Tick::shutdown();

	// If there is a window, dispose of it
	if (m_hWindow) {
		Win32::set_window(nullptr);
		CloseWindow(m_hWindow);
		m_hWindow = nullptr;
	}

	// Restore the system error mode
	SetErrorMode(m_uErrorMode);

	// Restore COM to previous state
	if (m_bCoCreateInstanceInit) {
		CoUninitialize();
		m_bCoCreateInstanceInit = FALSE;
	}

	// Release the Windows high speed timer
	timeEndPeriod(1);

	// The instance is not tracked anymore
	m_hInstance = nullptr;
	Win32::set_instance(nullptr);
}

/*! ************************************

	\fn HINSTANCE Burger::GameApp::GetInstance(void) const
	\brief Get the HINSTANCE of the running application

	When started, an HINSTANCE is assigned to an application and
	this value is used for some Windows specific function.

	\windowsonly
	\return The application's HINSTANCE

***************************************/

/*! ************************************

	\fn HWND Burger::GameApp::GetWindow(void) const
	\brief Get the HWND of the running application

	When started, a window is created and attached to an application and this is
	the reference to the global window instance. It is used extensively by the
	rendering sub-systems.

	\windowsonly

	\return The application's window reference
	\sa Display

***************************************/

/*! ************************************

	\fn HMENU Burger::GameApp::GetMenu(void) const
	\brief Get the HMENU of the running application

	Return the HMENU that's attached to the window

	\windowsonly

	\return The application's HMENU
	\sa Display or SetMenu(HMENU__ *)

***************************************/

/*! ************************************

	\fn void Burger::GameApp::SetMenu(HMENU__ *hMenu)
	\brief Set the HMENU of the running application

	\windowsonly

	\param hMenu The application's new HMENU
	\sa Display or GetMenu(void) const

***************************************/

/*! ************************************

	\fn Burger::GameApp::MainWindowProc Burger::GameApp::GetCallBack(void) const
	\brief Get the pointer to the window callback

	The main application window has a callback function to handle most functions
	automatically. A user supplied callback is used or augment or override
	default behavior. This function returns the pointer to the user supplied
	callback function.

	\windowsonly
	\return The user supplied callback function pointer

***************************************/

/*! ************************************

	\fn uint_t Burger::GameApp::GetInSizeMove(void) const
	\brief Return \ref TRUE if the window is resizing

	\windowsonly
	\return \ref TRUE if in resizing, \ref FALSE if not.
	\sa SetInSizeMove(uint_t)

***************************************/

/*! ************************************

	\fn void Burger::GameApp::SetInSizeMove(uint_t bInSizeMode)
	\brief Set or clear the resizing window flag

	\windowsonly
	\param bInSizeMode \ref TRUE if in resizing, \ref FALSE if not.
	\sa GetInSizeMove(void) const

***************************************/

/*! ************************************

	\brief Create an application's default window

	Windows requires a window object to be created by the application to send
	and receive system events including sound and visual focus. Create this
	window and set internal variables so the Windows version of Burgerlib can
	function

	\param pGameName Pointer to a UTF-8 "C" string of the string to attach to
		the top of the game window
	\param pCallBack Pointer to a user supplied callback function for Windows
		(\ref NULL if no user supplied pointer is present)
	\param uIconResID Numeric ID of the Windows Icon to attach to the window
		that's in the EXE files resource data (0 to use the default application
		icon from Windows)

	\return Zero (\ref FALSE) if the window was successfully created

***************************************/

int BURGER_API Burger::GameApp::InitWindow(const char* pGameName,
	MainWindowProc pCallBack, uint_t uIconResID) BURGER_NOEXCEPT
{
	m_pCallBack = pCallBack;
	// Get the default cursor
	m_pDefaultCursor = LoadCursorW(NULL, reinterpret_cast<LPCWSTR>(IDC_ARROW));

	// Make sure the window class is defined
	const ATOM MyAtom = Globals::RegisterWindowClass(uIconResID);

	int iResult;
	// Success in creating my class?
	if (MyAtom) {

		// Convert the game name to unicode
		String16 TitleUnicode(pGameName);

		// Put the window in the center of the screen
		RECT TheRect;
		const int x = GetSystemMetrics(SM_CXSCREEN) / 2;
		const int y = GetSystemMetrics(SM_CYSCREEN) / 2;
		SetRect(&TheRect, x - 320, y - 240, x + 320, y + 240);
		AdjustWindowRect(&TheRect, WS_OVERLAPPEDWINDOW, FALSE);

		// Create the window and pass it the "this" pointer
		HWND pWindow =
			CreateWindowExW(WS_EX_OVERLAPPEDWINDOW, // Force top level to the
													// task bar when minimized
				MAKEINTRESOURCEW(MyAtom), // Pointer to registered class name
				reinterpret_cast<LPCWSTR>(
					TitleUnicode.c_str()),      // Window title string
				WS_OVERLAPPEDWINDOW,            // Make an overlapped window
				TheRect.left,                   // X coordinate
				TheRect.top,                    // Y coordinate
				(TheRect.right - TheRect.left), // Width
				(TheRect.bottom - TheRect.top), // Height
				nullptr,                        // Window parent
				nullptr,                        // Window menu
				m_hInstance,                    // Task number
				this);                          // Local parameter

		if (pWindow) {
			// Store the new window handle
			m_hWindow = pWindow;
			// Set the system global, obsolete
			Win32::set_window(pWindow);
			// Copy the bounds rect
			RecordWindowLocation();

			// Set the pointer to the "this" pointer so the window
			// function will activate
			// SetWindowLongPtrW(pWindow,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(this));

			iResult = 0; // I'm OK!
		} else {
			iResult = static_cast<int>(GetLastError());
		}
	} else {
		iResult = 10;
	}
	return iResult;
}

/*! ************************************

	\brief Change the size of the application window

	When initializing a display, the window needs to be adjusted to be able to
	accommodate the new size. This function will make the window visible and
	resize it to the requested dimensions.

	If this is the first time executing, the window will be placed in the center
	of the screen, otherwise it will be placed where at the location of the last
	time the window was opened.

	\param uWidth Width of the display rectangle in pixels
	\param uHeight Height of the display rectangle in pixels
	\return Zero if no error, non-zero windows error code

***************************************/

int BURGER_API Burger::GameApp::SetWindowSize(
	uint_t uWidth, uint_t uHeight) BURGER_NOEXCEPT
{
	// Get the application's window
	HWND pWindow = m_hWindow;

	// Get the style of the window
	LONG_PTR dwStyle = GetWindowLongPtrW(pWindow, GWL_STYLE);
	// Can't be a pop-up window
	dwStyle &= ~(WS_POPUP);
	// Enable all the bells and whistles!
	dwStyle |= WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX |
		WS_SYSMENU;
	// Set the style (May not be visible if this is the first setting)
	// Note: Will issue WM_STYLECHANGING, WM_STYLECHANGED and WM_GETICON
	// messages to the window proc
	SetWindowLongPtrW(pWindow, GWL_STYLE, dwStyle);

	// Init the rect of the window's display area
	RECT NewWindowRect;
	NewWindowRect.left = 0;
	NewWindowRect.top = 0;
	NewWindowRect.right = static_cast<LONG>(uWidth);
	NewWindowRect.bottom = static_cast<LONG>(uHeight);

	// Calculate the rect of the window after the borders are added...

	AdjustWindowRectEx(&NewWindowRect, static_cast<DWORD>(dwStyle),
		m_hMenu != 0,
		static_cast<DWORD>(GetWindowLongPtrW(pWindow, GWL_EXSTYLE)));

	// Get the rect of the main screen (Note, removes the
	// windows task bar if one is present)
	RECT TempRect; // Temp rect
	SystemParametersInfoW(SPI_GETWORKAREA, 0, &TempRect, 0);

	// Resize the window to the new rect

	const uint_t uAdjustedWidth =
		static_cast<uint_t>(NewWindowRect.right - NewWindowRect.left);
	const uint_t uAdjustedHeight =
		static_cast<uint_t>(NewWindowRect.bottom - NewWindowRect.top);

	// Get the center x,y position of the window

	if (!m_WindowRect.GetLeft()) {
		//		ResetWindowLocation();		// Find the center of the main
		// monitor
	}

	// Get the screen size and center it there
	NewWindowRect.left =
		static_cast<LONG>(m_WindowRect.GetLeft() - (uAdjustedWidth / 2));
	NewWindowRect.top =
		static_cast<LONG>(m_WindowRect.GetTop() - (uAdjustedHeight / 2));

	// Make sure the window is on screen

	if (NewWindowRect.left < TempRect.left) {
		NewWindowRect.left =
			TempRect.left; // Anchor the window to the screen bar
	}
	if (NewWindowRect.top < TempRect.top) {
		NewWindowRect.top = TempRect.top;
	}
	// Set the style (Makes it visible)
	ShowWindow(pWindow, SW_SHOWNORMAL);

	// Position the window on the screen with the new size
	SetWindowPos(pWindow, HWND_NOTOPMOST, NewWindowRect.left, NewWindowRect.top,
		static_cast<int>(uAdjustedWidth), static_cast<int>(uAdjustedHeight),
		SWP_NOACTIVATE);

	return 0;
}

/*! ************************************

	\brief Prepare an application window for full screen mode

	When initializing a display, the window needs to be adjusted to be size of
	the display and all borders removed. This function will make the window
	visible and adjust if for full screen drawing.

	\param uWidth Width of the display rectangle in pixels
	\param uHeight Height of the display rectangle in pixels
	\return Zero if no error, non-zero windows error code

***************************************/

int BURGER_API Burger::GameApp::SetWindowFullScreen(
	uint_t uWidth, uint_t uHeight) BURGER_NOEXCEPT
{
	// Get the application's window
	HWND pWindow = m_hWindow;

	// Get the style of the window
	LONG_PTR dwStyle = GetWindowLongPtrW(pWindow, GWL_STYLE);
	dwStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX);
	dwStyle |=
		WS_POPUP | WS_OVERLAPPED | WS_EX_TOPMOST;   // Can't be a pop-up window
	SetWindowLongPtrW(pWindow, GWL_STYLE, dwStyle); // Set the style

	// Set the style (Makes it visible)
	ShowWindow(pWindow, SW_SHOWNORMAL);

	// Position the window on the screen in the center or where it was located
	// last time
	SetWindowPos(pWindow, HWND_TOP, 0, 0, static_cast<int>(uWidth),
		static_cast<int>(uHeight), SWP_NOZORDER | SWP_NOACTIVATE);

	return 0;
}

/*! ************************************

	\brief Purge the cached window location

	When calling SetWindowSize(), it will center the window in the middle of the
	screen on the first call and then use the last known origin location on all
	subsequent calls so if the user moves the window, the location of the window
	won't be lost on toggling from full screen to window mode. If the window
	needs to be re-centered, call this function to purge the cache.

	\windowsonly

***************************************/

void BURGER_API Burger::GameApp::ResetWindowLocation(void) BURGER_NOEXCEPT
{
	// Record the center of the main monitor as the center point of the window
	m_WindowRect.SetLeft(GetSystemMetrics(SM_CXSCREEN) / 2);
	m_WindowRect.SetTop(GetSystemMetrics(SM_CYSCREEN) / 2);
}

/*! ************************************

	\brief Save the current window location

	When changing video display modes, it's prudent to save off the location of
	the application window so if the application switches to full screen mode,
	it can later revert to a windowed mode with the window in its previous
	location. This function records the current location of the window for later
	restoration.

***************************************/

void BURGER_API Burger::GameApp::RecordWindowLocation(void) BURGER_NOEXCEPT
{
	RECT TempRect;
	// Get the current size of the window
	HWND pWindow = m_hWindow;
	GetWindowRect(pWindow, &TempRect);
	m_WindowRect.Set(&TempRect);
}

/*! ************************************

	\brief Handle cursor updates

	When the mouse moves in windowed mode, a WM_CURSOR event is triggered an the
	window needs to respond by changing the mouse cursor if needed.

	If the game has enabled a hardware cursor, it will be enabled if the cursor
	is hovering over the client area, otherwise, it will be hidden or enabled to
	whatever cursor is needed for the borders of the window.

	The application doesn't call this function. The WindowsApp class will handle
	it in the default window processing function.

	\param uParam The passed "lparam" for a WM_CURSOR event.
	\return \ref TRUE if handled, \ref FALSE if not

***************************************/

uint_t BURGER_API Burger::GameApp::HandleCursor(uint_t uParam) BURGER_NOEXCEPT
{
	// Only process if in the client area. Let the OS handle the cursor
	// elsewhere
	uint_t uResult = FALSE;
	const uint_t uClient = LOWORD(uParam);
	switch (uClient) {
	// In the client area?
	case HTCLIENT:
		// Handle the hardware cursor
		if (OSCursor::IsActive()) {
			OSCursor::Refresh();
			OSCursor::Show();
		} else {
			OSCursor::Hide();
		}
		// The message was processed
		uResult = TRUE;
		break;
	case HTRIGHT:
	case HTLEFT:
	case HTTOP:
	case HTTOPLEFT:
	case HTTOPRIGHT:
	case HTBOTTOM:
	case HTBOTTOMRIGHT:
	case HTBOTTOMLEFT:
		// If full screen or not allowed, disable resizing
		if (!IsResizingAllowed()) {
			SetCursor(m_pDefaultCursor);
			uResult = TRUE;
		}
	default:
		OSCursor::Show(TRUE);
		break;
	}
	return uResult;
}

/*! ************************************

	\brief Obtain DirectInput focus (Windows only)

	When the window is brought forward and focus is desired, this function will
	first test if the window is part of a desktop or full screen. If full
	screen, it will call the attached Keyboard, Mouse and Joypad objects to have
	them Acquire focus with DirectInput.

***************************************/

void BURGER_API Burger::GameApp::GetInputFocus(void) BURGER_NOEXCEPT
{
	if (IsAppFullScreen()) {
		Mouse* pMouse = GetMouse();
		if (pMouse) {
			pMouse->AcquireDirectInput();
		}
		Keyboard* pKeyboard = GetKeyboard();
		if (pKeyboard) {
			pKeyboard->AcquireDirectInput();
		}
		Joypad* pJoypad = GetJoypad();
		if (pJoypad) {
			pJoypad->AcquireDirectInput();
		}
	}
}

/*! ************************************

	\brief Release DirectInput focus (Windows only)

	When the window is moved to the background and focus is no longer desired it
	will call the attached Keyboard, Mouse and Joypad objects to have them
	Unacquire focus with DirectInput.

***************************************/

void BURGER_API Burger::GameApp::KillInputFocus(void) BURGER_NOEXCEPT
{
	Mouse* pMouse = GetMouse();
	if (pMouse) {
		pMouse->UnacquireDirectInput();
	}
	Keyboard* pKeyboard = GetKeyboard();
	if (pKeyboard) {
		pKeyboard->UnacquireDirectInput();
	}
	Joypad* pJoypad = GetJoypad();
	if (pJoypad) {
		pJoypad->UnacquireDirectInput();
	}
}

/*! ************************************

	\brief Process Windows events

	Internal function that will call PeekMessage(), TranslateMessage() and
	DispatchMessage() to handle messages sent to the app

	\param pSelf "this" pointer to the WindowsApp class
	\return RunQueue::OKAY since this function never shuts down on its own

***************************************/

Burger::RunQueue::eReturnCode BURGER_API Burger::GameApp::Poll(
	void* /* pSelf */) BURGER_NOEXCEPT
{
	MSG TempMessage;
	while (PeekMessageW(&TempMessage, nullptr, 0, 0, PM_REMOVE)) {
		// Translate the keyboard (Localize)
		TranslateMessage(&TempMessage);
		// Pass to the window event proc
		DispatchMessageW(&TempMessage);
	}
	return RunQueue::OKAY;
}

#endif
