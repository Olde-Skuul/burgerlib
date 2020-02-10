/***************************************

    Windows application manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brgameapp.h"
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brdebug.h"
#include "brmouse.h"
#include "brkeyboard.h"
#include "brjoypad.h"
#include "brdisplay.h"
#include "brglobals.h"
#include "brstring16.h"
#include "broscursor.h"
#include "brutf8.h"
#include "brfilemanager.h"

#if !defined(DOXYGEN)
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0501			// Windows XP
#endif

#if !defined(_CRT_DECLARE_GLOBAL_VARIABLES_DIRECTLY)
#define _CRT_DECLARE_GLOBAL_VARIABLES_DIRECTLY		// Needed for Visual Studio 2015 or higher
#endif
#endif

#include <windows.h>
#include <WindowsX.h>
#include <shellapi.h>
#include <stdlib.h>
#include <ObjBase.h>
#include <CommCtrl.h>

#if !defined(DOXYGEN)
// Needed for code that manually grabs the parm list

extern "C" char ** __argv;

//
// These defines are missing from some versions of windows.h
// especially when building against older versions of the
// windows SDK (Necessary, since some obscure compilers
// don't ship with up to date headers)
//

#if !defined(GET_SC_WPARAM)
#define GET_SC_WPARAM(wParam) ((int)wParam & 0xFFF0)
#endif

#if !defined(PBT_APMQUERYSUSPEND)
#define PBT_APMQUERYSUSPEND 0x0000
#endif

#if !defined(PBT_APMRESUMESUSPEND)
#define PBT_APMRESUMESUSPEND 0x0007
#endif

#if !defined(WM_KEYF1)
#define WM_KEYF1 0x004D
#endif

#if !defined(WM_UAHDESTROYWINDOW)
#define WM_UAHDESTROYWINDOW 0x0090
#endif

#if !defined(WM_UAHDRAWMENU)
#define WM_UAHDRAWMENU 0x0091
#endif

#if !defined(WM_UAHDRAWMENUITEM)
#define WM_UAHDRAWMENUITEM 0x0092
#endif

#if !defined(WM_UAHINITMENU)
#define WM_UAHINITMENU 0x0093
#endif

#if !defined(WM_UAHMEASUREMENUITEM)
#define WM_UAHMEASUREMENUITEM 0x0094
#endif

#if !defined(WM_UAHNCPAINTMENUPOPUP)
#define WM_UAHNCPAINTMENUPOPUP 0x0095
#endif

#if !defined(WM_NCUAHDRAWCAPTION)
#define WM_NCUAHDRAWCAPTION 0x00AE
#endif

#if !defined(WM_NCUAHDRAWFRAME)
#define WM_NCUAHDRAWFRAME 0x00AF
#endif

#if !defined(WM_INPUT_DEVICE_CHANGE)
#define WM_INPUT_DEVICE_CHANGE 0x00FE
#endif

#if !defined(WM_INPUT)
#define WM_INPUT 0x00FF
#endif

#if !defined(WM_GESTURE)
#define WM_GESTURE 0x0119
#endif

#if !defined(WM_GESTURENOTIFY)
#define WM_GESTURENOTIFY 0x011A
#endif

#if !defined(WM_MOUSEHWHEEL)
#define WM_MOUSEHWHEEL 0x020E
#endif

#if !defined(WM_TOUCH)
#define WM_TOUCH 0x0240
#endif

#if !defined(WM_NCMOUSEHOVER)
#define WM_NCMOUSEHOVER 0x02A0
#endif

#if !defined(WM_NCMOUSELEAVE)
#define WM_NCMOUSELEAVE 0x02A2
#endif

#if !defined(WM_CLIPBOARDUPDATE)
#define WM_CLIPBOARDUPDATE 0x031D
#endif

#if !defined(WM_DWMCOMPOSITIONCHANGED)
#define WM_DWMCOMPOSITIONCHANGED 0x031E
#endif

#if !defined(WM_DWMNCRENDERINGCHANGED)
#define WM_DWMNCRENDERINGCHANGED 0x031F
#endif

#if !defined(WM_DWMCOLORIZATIONCOLORCHANGED)
#define WM_DWMCOLORIZATIONCOLORCHANGED 0x0320
#endif

#if !defined(WM_DWMWINDOWMAXIMIZEDCHANGE)
#define WM_DWMWINDOWMAXIMIZEDCHANGE 0x0321
#endif

// Needed for InitCommonControls()
#if defined(BURGER_MSVC)
#pragma comment(lib, "Comctl32.lib")
#endif

#endif

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

Burger::GameApp::GameApp(WordPtr uDefaultMemorySize,Word uDefaultHandleCount,WordPtr uMinReserveSize) :
	m_MemoryManagerHandle(uDefaultMemorySize,uDefaultHandleCount,uMinReserveSize),
//	m_hInstance(NULL),		// Will initialize below
	m_hWindow(NULL),
	m_hMenu(NULL),
	m_pDefaultCursor(NULL),
	m_ppOldArgv(NULL),
	m_pCallBack(NULL),
	m_uErrorMode(0),
	m_bCoCreateInstanceInit(FALSE),
	m_bInSizeMove(FALSE)
{
	m_WindowRect.Clear();
	HINSTANCE hInstance = GetModuleHandleW(NULL);
	m_hInstance = hInstance;
	// Set the global instance
	Windows::SetInstance(hInstance);

	// Ensure that threading is serialized since it's assumed this
	// is a GUI based application
	
	// Also disable OLE 1.0, since any code from XP on shouldn't use that
	
	if (CoInitializeEx(NULL,COINIT_APARTMENTTHREADED|COINIT_DISABLE_OLE1DDE) == S_OK) {
		m_bCoCreateInstanceInit = TRUE;
	}

	// Make the app handle all of its own errors
	m_uErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX);

	// Not always needed, but lets the app create GDI dialogs under Windows XP
	// More recent operating systems, this function does nothing
	InitCommonControls();
	
	// Set up the shared values
	InitDefaults();

	// In order to support unicode command lines under windows,
	// the command line needs to be re-processed by calling the
	// shellapi and manually extracting the commands and 
	// convert them to UTF-8

	// Get the UTF-16 command line string from windows
	LPWSTR *pWideArgv = CommandLineToArgvW(GetCommandLineW(),&m_iArgc);
	// Get the parameter count
	int iArgc = m_iArgc;
	// Do I bother?
	if (iArgc>0) {
		// Allocate a buffer to contain the string pointers and all of
		// the strings. This allows the pointers to be
		// manipulated within the list without
		// the strings being leaked on shutdown
		// since when the pointer array is released, it
		// also releases the strings as well

		// Determine how much space the strings will take as
		// UTF8
		Word16 **ppWork = reinterpret_cast<Word16 **>(pWideArgv);
		WordPtr uDataSize = 0;
		do {
			uDataSize += UTF8::FromUTF16(NULL,0,ppWork[0])+1;
			++ppWork;
		} while (--iArgc);

		// Allocate the buffer for a pointer array and the strings
		iArgc = m_iArgc;
		const char **ppArgv = static_cast<const char **>(Alloc((sizeof(char *)*iArgc)+uDataSize));
		
		// Store the pointer for later use and disposal
		m_ppArgv = ppArgv;
		// Get the pointer beyond the pointer array
		char *pDest = reinterpret_cast<char *>(ppArgv)+(sizeof(char *)*iArgc);
		ppWork = reinterpret_cast<Word16 **>(pWideArgv);

		// This "hack" is to allow any other code that manually grabs the 
		// parameter list to inherit the UTF8 support
		m_ppOldArgv = const_cast<const char **>(__argv);
		__argv = const_cast<char **>(ppArgv);

		// Convert all the strings
		do {
			// Store the pointer in the array
			ppArgv[0] = pDest;
			++ppArgv;
			// Convert a string
			WordPtr uLength = UTF8::FromUTF16(pDest,uDataSize,ppWork[0])+1;
			// Adjust the pointers
			uDataSize -= uLength;
			pDest+=uLength;
			++ppWork;
		} while (--iArgc);
	}
	// Release the data Windows gave me for the parsed parameters
	// See docs for CommandLineToArgvW()
	LocalFree(pWideArgv);

	// Add the windows callback function
	// and set it to be the first entry to be called
	m_RunQueue.Add(Poll,NULL,this,RunQueue::PRIORITY_FIRST);

	// Init the global cursor
	OSCursor::Init();
	
	// Init the file system
	FileManager::Init();
}

/*! ************************************

	\brief Dispose of any allocated resources

	When a windows app is shut down, dispose of 
	everything here.

***************************************/

Burger::GameApp::~GameApp()
{
	m_pCallBack = NULL;
	RemoveRoutine(Poll,this);

	// Release the file system
	FileManager::Shutdown();

	// Release the cursor
	OSCursor::Shutdown();

	// Release the command line
	Free(m_ppArgv);
	m_ppArgv = NULL;

	// Restore the previous command line so crt0.s can clean it up
	if (m_ppOldArgv) {
		__argv = const_cast<char **>(m_ppOldArgv);
		m_ppOldArgv = NULL;
	}

	// Clear out the default variables
	// It also unlinks the Display class
	ShutdownDefaults();

	// If there is a window, dispose of it
	if (m_hWindow) {
		Globals::SetWindow(NULL);
		CloseWindow(m_hWindow);
		m_hWindow = NULL;
	}

	// Restore the system error mode
	SetErrorMode(m_uErrorMode);

	// Restore COM to previous state
	if (m_bCoCreateInstanceInit) {
		CoUninitialize();
		m_bCoCreateInstanceInit = FALSE;
	}

	// The instance is not tracked anymore
	m_hInstance = NULL;
	Windows::SetInstance(nullptr);
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

	When started, a window is created and attached
	to an application and this is the reference to
	the global window instance. It is used 
	extensively by the rendering sub-systems.

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

	The main application window has a callback function to handle
	most functions automatically. A user supplied callback
	is used or augment or override default behavior. This
	function returns the pointer to the user supplied
	callback function.

	\windowsonly
	\return The user supplied callback function pointer

***************************************/

/*! ************************************

	\fn Word Burger::GameApp::GetInSizeMove(void) const
	\brief Return \ref TRUE if the window is resizing

	\windowsonly
	\return \ref TRUE if in resizing, \ref FALSE if not.
	\sa SetInSizeMove(Word)

***************************************/

/*! ************************************

	\fn void Burger::GameApp::SetInSizeMove(Word bInSizeMode)
	\brief Set or clear the resizing window flag

	\windowsonly
	\param bInSizeMode \ref TRUE if in resizing, \ref FALSE if not.
	\sa GetInSizeMove(void) const

***************************************/



/*! ************************************

	\brief Create an application's default window

	Windows requires a window object to be created by the
	application to send and receive system events including
	sound and visual focus. Create this window and set internal
	variables so the Windows version of Burgerlib can function
	
	\param pGameName Pointer to a UTF-8 "C" string of the string to attach to the top of the game window
	\param pCallBack Pointer to a user supplied callback function for Windows (\ref NULL if no user supplied pointer is present)
	\param uIconResID Numeric ID of the Windows Icon to attach to the window that's in the EXE files resource data (0 to use the default application icon from Windows)

	\return Zero (\ref FALSE) if the window was successfully created

***************************************/

int BURGER_API Burger::GameApp::InitWindow(const char *pGameName,MainWindowProc pCallBack,Word uIconResID)
{
	m_pCallBack = pCallBack;
	// Get the default cursor
	m_pDefaultCursor = LoadCursorW(NULL,reinterpret_cast<LPCWSTR>(IDC_ARROW));

	// Make sure the window class is defined
	ATOM MyAtom = Globals::RegisterWindowClass(uIconResID);
	
	int iResult;
	// Success in creating my class?
	if (MyAtom) {

		// Convert the game name to unicode
		String16 TitleUnicode(pGameName);

		// Put the window in the center of the screen
		RECT TheRect;
		int x = GetSystemMetrics(SM_CXSCREEN)/2;
		int y = GetSystemMetrics(SM_CYSCREEN)/2;
		SetRect(&TheRect,x-320,y-240,x+320,y+240);
		AdjustWindowRect(&TheRect,WS_OVERLAPPEDWINDOW,FALSE);

		// Create the window and pass it the "this" pointer
		HWND pWindow = CreateWindowExW(
			WS_EX_OVERLAPPEDWINDOW,				// Force top level to the task bar when minimized
			MAKEINTRESOURCEW(MyAtom),			// Pointer to registered class name
			reinterpret_cast<LPCWSTR>(TitleUnicode.c_str()),	// Window title string
			WS_OVERLAPPEDWINDOW,	// Make an overlapped window
			TheRect.left,			// X coordinate
			TheRect.top,			// Y coordinate
			(TheRect.right-TheRect.left),	// Width
			(TheRect.bottom-TheRect.top),	// Height
			NULL,			// Window parent
			NULL,			// Window menu
			m_hInstance,	// Task number
			this);			// Local parameter

		if (pWindow) {
			// Store the new window handle
			m_hWindow = pWindow;
			// Set the system global, obsolete
			Globals::SetWindow(pWindow);
			// Copy the bounds rect
			RecordWindowLocation();

			// Set the pointer to the "this" pointer so the window
			// function will activate
			//SetWindowLongPtrW(pWindow,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(this));

			iResult = 0;				// I'm OK!
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

	When initializing a display, the window needs to
	be adjusted to be able to accommodate the new size.
	This function will make the window visible and
	resize it to the requested dimensions.

	If this is the first time executing, the window
	will be placed in the center of the screen,
	otherwise it will be placed where at the location
	of the last time the window was opened.

	\param uWidth Width of the display rectangle in pixels
	\param uHeight Height of the display rectangle in pixels
	\return Zero if no error, non-zero windows error code

***************************************/

int BURGER_API Burger::GameApp::SetWindowSize(Word uWidth,Word uHeight)
{
	// Get the application's window
	HWND pWindow = m_hWindow;

	// Get the style of the window
	LONG_PTR dwStyle = GetWindowLongPtrW(pWindow,GWL_STYLE);
	// Can't be a pop-up window
	dwStyle &= ~(WS_POPUP);
	// Enable all the bells and whistles!
	dwStyle |= WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_SYSMENU;
	// Set the style (May not be visible if this is the first setting)
	// Note: Will issue WM_STYLECHANGING, WM_STYLECHANGED and WM_GETICON messages to the window proc
	SetWindowLongPtrW(pWindow,GWL_STYLE,dwStyle);

	// Init the rect of the window's display area
	RECT NewWindowRect;
	NewWindowRect.left = 0;
	NewWindowRect.top = 0;
	NewWindowRect.right = static_cast<LONG>(uWidth);
	NewWindowRect.bottom = static_cast<LONG>(uHeight);

	// Calculate the rect of the window after the borders are added...

	AdjustWindowRectEx(&NewWindowRect,static_cast<DWORD>(dwStyle),m_hMenu!= 0,static_cast<DWORD>(GetWindowLongPtrW(pWindow,GWL_EXSTYLE)));

	// Get the rect of the main screen (Note, removes the
	// windows task bar if one is present)
	RECT TempRect;		// Temp rect
	SystemParametersInfoW(SPI_GETWORKAREA,0,&TempRect,0);

	// Resize the window to the new rect

	Word uAdjustedWidth = static_cast<Word>(NewWindowRect.right-NewWindowRect.left);
	Word uAdjustedHeight = static_cast<Word>(NewWindowRect.bottom-NewWindowRect.top);

	// Get the center x,y position of the window

	if (!m_WindowRect.GetLeft()) {
//		ResetWindowLocation();		// Find the center of the main monitor
	}

	// Get the screen size and center it there
	NewWindowRect.left = static_cast<LONG>(m_WindowRect.GetLeft()-(uAdjustedWidth/2));
	NewWindowRect.top = static_cast<LONG>(m_WindowRect.GetTop()-(uAdjustedHeight/2));

	// Make sure the window is on screen

	if (NewWindowRect.left < TempRect.left) {
		NewWindowRect.left = TempRect.left;		// Anchor the window to the screen bar
	}
	if (NewWindowRect.top < TempRect.top) {
		NewWindowRect.top = TempRect.top;
	}
	// Set the style (Makes it visible)
	ShowWindow(pWindow,SW_SHOWNORMAL);

	// Position the window on the screen with the new size
	SetWindowPos(pWindow,HWND_NOTOPMOST,NewWindowRect.left,NewWindowRect.top,static_cast<int>(uAdjustedWidth),static_cast<int>(uAdjustedHeight),SWP_NOACTIVATE);

	return 0;
}

/*! ************************************

	\brief Prepare an application window for full screen mode

	When initializing a display, the window needs to
	be adjusted to be size of the display and all borders
	removed. This function will make the window visible and
	adjust if for full screen drawing.

	\param uWidth Width of the display rectangle in pixels
	\param uHeight Height of the display rectangle in pixels
	\return Zero if no error, non-zero windows error code

***************************************/

int BURGER_API Burger::GameApp::SetWindowFullScreen(Word uWidth,Word uHeight)
{
	// Get the application's window
	HWND pWindow = m_hWindow;

	// Get the style of the window 
	LONG_PTR dwStyle = GetWindowLongPtrW(pWindow,GWL_STYLE);
	dwStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX);
	dwStyle |= WS_POPUP | WS_OVERLAPPED | WS_EX_TOPMOST;		// Can't be a pop-up window
	SetWindowLongPtrW(pWindow,GWL_STYLE,dwStyle);				// Set the style

	// Set the style (Makes it visible)
	ShowWindow(pWindow,SW_SHOWNORMAL);

	// Position the window on the screen in the center or where it was located last time
	SetWindowPos(pWindow,HWND_TOP,0,0,static_cast<int>(uWidth),static_cast<int>(uHeight),SWP_NOZORDER | SWP_NOACTIVATE);

	return 0;
}

/*! ************************************

	\brief Purge the cached window location

	When calling SetWindowSize(), it will center the window in the
	middle of the screen on the first call and then
	use the last known origin location on all subsequent
	calls so if the user moves the window, the 
	location of the window won't be lost on
	toggling from full screen to window mode. If the
	window needs to be re-centered, call this function to
	purge the cache.

	\windowsonly

***************************************/

void BURGER_API Burger::GameApp::ResetWindowLocation(void)
{
	// Record the center of the main monitor as the center point of the window
	m_WindowRect.SetLeft(GetSystemMetrics(SM_CXSCREEN)/2);
	m_WindowRect.SetTop(GetSystemMetrics(SM_CYSCREEN)/2);
}

/*! ************************************

	\brief Save the current window location

	When changing video display modes, it's prudent to 
	save off the location of the application window so if
	the application switches to full screen mode, it can 
	later revert to a windowed mode with the window in
	its previous location. This function records the
	current location of the window for later restoration.

***************************************/

void BURGER_API Burger::GameApp::RecordWindowLocation(void)
{
	RECT TempRect;
	// Get the current size of the window
	HWND pWindow = m_hWindow;
	GetWindowRect(pWindow,&TempRect);
	m_WindowRect.Set(&TempRect);
}

/*! ************************************

	\brief Handle cursor updates

	When the mouse moves in windowed mode, a WM_CURSOR
	event is triggered an the window needs to respond
	by changing the mouse cursor if needed.

	If the game has enabled a hardware cursor, it will
	be enabled if the cursor is hovering over
	the client area, otherwise, it will be hidden or
	enabled to whatever cursor is needed for the borders
	of the window.

	The application doesn't call this function. The
	WindowsApp class will handle it in the default
	window processing function.

	\param uParam The passed "lparam" for a WM_CURSOR event.
	\return \ref TRUE if handled, \ref FALSE if not

***************************************/

Word BURGER_API Burger::GameApp::HandleCursor(Word uParam)
{
	// Only process if in the client area. Let the OS handle the cursor
	// elsewhere
	Word uResult = FALSE;
	Word uClient = LOWORD(uParam);
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

	When the window is brought forward and focus is desired,
	this function will first test if the window is part
	of a desktop or full screen. If full screen,
	it will call the attached Keyboard, Mouse and Joypad
	objects to have them Acquire focus with DirectInput.

***************************************/

void BURGER_API Burger::GameApp::GetInputFocus(void)
{
	if (IsAppFullScreen()) {
		Mouse *pMouse = GetMouse();
		if (pMouse) {
			pMouse->AcquireDirectInput();
		}
		Keyboard *pKeyboard = GetKeyboard();
		if (pKeyboard) {
			pKeyboard->AcquireDirectInput();
		}
		Joypad *pJoypad = GetJoypad();
		if (pJoypad) {
			pJoypad->AcquireDirectInput();
		}
	}
}

/*! ************************************

	\brief Release DirectInput focus (Windows only)

	When the window is moved to the background and focus
	is no longer desired it will call the attached Keyboard,
	Mouse and Joypad objects to have them Unacquire focus
	with DirectInput.

***************************************/

void BURGER_API Burger::GameApp::KillInputFocus(void)
{
	Mouse *pMouse = GetMouse();
	if (pMouse) {
		pMouse->UnacquireDirectInput();
	}
	Keyboard *pKeyboard = GetKeyboard();
	if (pKeyboard) {
		pKeyboard->UnacquireDirectInput();
	}
	Joypad *pJoypad = GetJoypad();
	if (pJoypad) {
		pJoypad->UnacquireDirectInput();
	}
}

/*! ************************************

	\brief Process Windows events

	Internal function that will call PeekMessage(), TranslateMessage() and DispatchMessage()
	to handle messages sent to the app
		
	\param pSelf "this" pointer to the WindowsApp class
	\return RunQueue::OKAY since this function never shuts down on its own

***************************************/

Burger::RunQueue::eReturnCode BURGER_API Burger::GameApp::Poll(void * /* pSelf */)
{
	MSG TempMessage;
	while (PeekMessageW(&TempMessage,nullptr,0,0,PM_REMOVE)) {
		// Translate the keyboard (Localize)
		TranslateMessage(&TempMessage);
		// Pass to the window event proc
		DispatchMessageW(&TempMessage);
	}
	return RunQueue::OKAY;
}

#endif