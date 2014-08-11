/***************************************

	Windows application manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#include "brwindowsapp.h"
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

#if !defined(WIN32_LEAN_AND_MEAN) && !defined(DOXYGEN)
#define WIN32_LEAN_AND_MEAN
#endif

#if !defined(_WIN32_WINNT) && !defined(DOXYGEN)
#define _WIN32_WINNT 0x0600
#endif

#include <windows.h>
#include <WindowsX.h>
#include <shellapi.h>
#include <stdlib.h>

// Needed for code that manually grabs the parm list
#if !defined(DOXYGEN)
extern "C" char **__argv;
#endif

#if !defined(GET_SC_WPARAM) && !defined(DOXYGEN)
#define GET_SC_WPARAM(wParam) ((int)wParam & 0xFFF0)
#endif

#if !defined(PBT_APMQUERYSUSPEND) && !defined(DOXYGEN)
#define PBT_APMQUERYSUSPEND 0x0000
#endif

#if !defined(PBT_APMRESUMESUSPEND) && !defined(DOXYGEN)
#define PBT_APMRESUMESUSPEND 0x0007
#endif

#if !defined(WM_MOUSEHWHEEL) && !defined(DOXYGEN)
#define WM_MOUSEHWHEEL 0x020E
#endif

//
// Name of the game class
//

static const wchar_t g_GameClass[] = L"BurgerGameClass";

//
// Code used from time to time to track window events
//

#if defined(_DEBUG)
#define CASE(x) case x: pMessage = #x; break

static void OutputWindowsMessage(Word uMessage,WordPtr wParam,WordPtr lParam)
{
	if (!(Burger::Globals::GetTraceFlag()&Burger::Globals::TRACE_MESSAGES)) {
		return;
	}
	const char *pMessage;
	char Message[32]; 
	static Word ID=0;

	switch (uMessage) {
#if defined(WM_DWMNCRENDERINGCHANGED)
	CASE(WM_DWMNCRENDERINGCHANGED);
#endif
#if defined(WM_DWMCOLORIZATIONCOLORCHANGED)
	CASE(WM_DWMCOLORIZATIONCOLORCHANGED);
#endif
#if defined(WM_DWMWINDOWMAXIMIZEDCHANGE)
	CASE(WM_DWMWINDOWMAXIMIZEDCHANGE);
#endif
	CASE(WM_DEVMODECHANGE);
	CASE(WM_ACTIVATEAPP);
	CASE(WM_FONTCHANGE);
	CASE(WM_TIMECHANGE);
	CASE(WM_MENUSELECT);
	CASE(WM_MENUCHAR);
	CASE(WM_ENTERIDLE);
	CASE(WM_CANCELMODE);
	CASE(WM_NEXTMENU);
	CASE(WM_SIZING);
	CASE(WM_CAPTURECHANGED);
	CASE(WM_MOVING);
	CASE(WM_SETCURSOR);
	CASE(WM_MOUSEACTIVATE);
	CASE(WM_CHILDACTIVATE);
	CASE(WM_QUEUESYNC);
	CASE(WM_GETMINMAXINFO);
	CASE(WM_NCCREATE);
	CASE(WM_NCDESTROY);
	CASE(WM_NCCALCSIZE);
	CASE(WM_PARENTNOTIFY);
	CASE(WM_ENTERMENULOOP);
	CASE(WM_EXITMENULOOP);
	CASE(WM_NCHITTEST);
	CASE(WM_NCPAINT);
	CASE(WM_NCACTIVATE);
	CASE(WM_GETDLGCODE);
	CASE(WM_SYNCPAINT);
	CASE(WM_MOUSEMOVE);
	CASE(WM_LBUTTONDOWN);
	CASE(WM_LBUTTONUP);
	CASE(WM_LBUTTONDBLCLK);
	CASE(WM_RBUTTONDOWN);
	CASE(WM_RBUTTONUP);
	CASE(WM_RBUTTONDBLCLK);
	CASE(WM_MBUTTONDOWN);
	CASE(WM_MBUTTONUP);
	CASE(WM_MBUTTONDBLCLK);
	CASE(WM_MOUSEWHEEL);
	CASE(WM_XBUTTONDOWN);
	CASE(WM_XBUTTONUP);
	CASE(WM_XBUTTONDBLCLK);
	CASE(WM_MOUSEHWHEEL);
	CASE(WM_NULL);
	CASE(WM_CREATE);
	CASE(WM_DESTROY);
	CASE(WM_MOVE);
	CASE(WM_SIZE);
	CASE(WM_ACTIVATE);
	CASE(WM_SETFOCUS);
	CASE(WM_KILLFOCUS);
	CASE(WM_ENABLE);
	CASE(WM_SETREDRAW);
	CASE(WM_SETTEXT);
	CASE(WM_GETTEXT);
	CASE(WM_GETTEXTLENGTH);
	CASE(WM_PAINT);
	CASE(WM_CLOSE);
	CASE(WM_IME_SETCONTEXT);
	CASE(WM_IME_NOTIFY);
	CASE(WM_IME_CONTROL);
	CASE(WM_IME_COMPOSITIONFULL);
	CASE(WM_IME_SELECT);
	CASE(WM_IME_CHAR);
	CASE(WM_CONTEXTMENU);
	CASE(WM_STYLECHANGING);
	CASE(WM_STYLECHANGED);
	CASE(WM_DISPLAYCHANGE);
	CASE(WM_GETICON);
	CASE(WM_SETICON);
	CASE(WM_NCMOUSEMOVE);
	CASE(WM_NCLBUTTONDOWN);
	CASE(WM_NCLBUTTONUP);
	CASE(WM_NCLBUTTONDBLCLK);
	CASE(WM_NCRBUTTONDOWN);
	CASE(WM_NCRBUTTONUP);
	CASE(WM_NCRBUTTONDBLCLK);
	CASE(WM_NCMBUTTONDOWN);
	CASE(WM_NCMBUTTONUP);
	CASE(WM_NCMBUTTONDBLCLK);
	CASE(WM_COMPACTING);
	CASE(WM_COMMNOTIFY);
	CASE(WM_WINDOWPOSCHANGING);
	CASE(WM_WINDOWPOSCHANGED);
	CASE(WM_POWER);
	CASE(WM_COPYDATA);
	CASE(WM_CANCELJOURNAL);
	CASE(WM_KEYDOWN);
	CASE(WM_KEYUP);
	CASE(WM_CHAR);
	CASE(WM_DEADCHAR);
	CASE(WM_SYSKEYDOWN);
	CASE(WM_SYSKEYUP);
	CASE(WM_SYSCHAR);
	CASE(WM_SYSDEADCHAR);
	CASE(WM_UNICHAR);
	CASE(WM_INITDIALOG);
	CASE(WM_COMMAND);
	CASE(WM_SYSCOMMAND);
	CASE(WM_TIMER);
	CASE(WM_HSCROLL);
	CASE(WM_VSCROLL);
	CASE(WM_INITMENU);
	CASE(WM_INITMENUPOPUP);
	CASE(WM_QUERYOPEN);
	CASE(WM_ERASEBKGND);
	default:
		Burger::NumberToAsciiHex(Message,static_cast<Word32>(uMessage),Burger::LEADINGZEROS|8);
		pMessage = Message;
	}
	Burger::Debug::Message("Message %08X is %s with parms %08X, %08X\n",ID,pMessage,wParam,lParam);
	++ID;
}
#else
#if !defined(DOXYGEN)
#define OutputWindowsMessage(x,y,z) (void)0
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

Burger::WindowsApp::WindowsApp(HINSTANCE__ *hInstance,const char *pGameName,MainWindowProc pCallBack,
	WordPtr uDefaultMemorySize,Word uDefaultHandleCount,WordPtr uMinReserveSize) :
	GameApp(uDefaultMemorySize,uDefaultHandleCount,uMinReserveSize),
	m_hInstance(hInstance),
	m_hWindow(NULL),
	m_ppOldArgv(NULL),
	m_pCallBack(pCallBack),
	m_bPreviousWindowXYValid(FALSE),
	m_uAtom(0)
{
	// Set the global instance
	Globals::SetInstance(hInstance);

	// Make the app handle all of its own errors
	SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX);
	
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
	LocalFree(pWideArgv);

	// Add the windows callback function
	m_RunQueue.Add(Poll,this,RunQueue::HIGHESTPRIORITY);

	// Init the global cursor
	OSCursor::Init();
	
	// Init the file system
	FileManager::Init();

	// Init the system window
	Globals::SetErrorCode(InitWindow(pGameName,pCallBack));
}

/*! ************************************

	\brief Dispose of any allocated resources

	When a windows app is shut down, dispose of 
	everything here.

***************************************/

Burger::WindowsApp::~WindowsApp()
{
	RemoveRoutine(Poll,this);
	// If there is a window, dispose of it
	if (m_hWindow) {
		Globals::SetWindow(NULL);
		CloseWindow(m_hWindow);
		m_hWindow = NULL;
	}

	// If there is a class atom, dispose of it
	if (m_uAtom) {
		UnregisterClassW(reinterpret_cast<LPCWSTR>(m_uAtom),m_hInstance);
		m_uAtom = 0;
	}
	// Release the file system
	FileManager::Shutdown();

	// Release the cursor
	OSCursor::Shutdown();

	Free(m_ppArgv);
	Globals::SetInstance(NULL);
	m_ppArgv = NULL;
	m_iArgc = 0;
	m_hInstance = NULL;
	m_pCallBack = NULL;
	if (m_ppOldArgv) {
		__argv = const_cast<char **>(m_ppOldArgv);
		m_ppOldArgv = NULL;
	}
}

/***************************************

	Internal windows dispatcher

***************************************/

static LRESULT CALLBACK InternalCallBack(HWND pWindow,UINT uMessage,WPARAM wParam,LPARAM lParam)
{
	// Firstly, get the "this" pointer
	// Note that this is not automatically set, the pointer is
	// passed in the WM_NCCREATE command and it's manually set into the 
	// WindowLongPtr index of 0.

	Burger::WindowsApp *pThis = reinterpret_cast<Burger::WindowsApp *>(GetWindowLongPtrW(pWindow,0));

	// Never initialized?
	if (!pThis) {
		// If this is a WM_NCCREATE event, then get the class instance pointer passed.
		if (uMessage==WM_NCCREATE) {
			pThis = static_cast<Burger::WindowsApp *>(reinterpret_cast<CREATESTRUCT *>(lParam)->lpCreateParams);
			SetWindowLongPtrW(pWindow,0,reinterpret_cast<LONG_PTR>(pThis));
		}
		// If I didn't get a pThis set, just call the default procedure and exit
		if (!pThis) {
			return DefWindowProcW(pWindow,uMessage,wParam,lParam);
		}
	}

	// The pThis class instance is valid!

	// If there is a user supplied callback, issue it

	Burger::WindowsApp::MainWindowProc pCallBack = pThis->GetCallBack();
	if (pCallBack) {
		WordPtr uOutput;
		// Should it shut down?
		if (pCallBack(pThis,pWindow,static_cast<Word>(uMessage),static_cast<WordPtr>(wParam),static_cast<WordPtr>(lParam),&uOutput)) {
			// Return the passed result code
			return static_cast<LRESULT>(uOutput);
		}
	}

	//
	// Handle burgerlib update events
	//

	OutputWindowsMessage(uMessage,wParam,lParam);

	switch (uMessage) {

	//
	// This function will disable the ability to resize the window
	//
	case WM_GETMINMAXINFO:
		{
			Burger::Display *pDisplay = pThis->GetDisplay();
			if (pDisplay) {
				LONG lScreenHeight = static_cast<LONG>(pDisplay->GetHeight());
				LONG lScreenWidth = static_cast<LONG>(pDisplay->GetWidth());
				// Adjust the window size to whatever the video manager says it should be
				RECT WindowSizeRect;
				WindowSizeRect.top = 0;
				WindowSizeRect.left = 0;
				WindowSizeRect.bottom = lScreenHeight;
				WindowSizeRect.right = lScreenWidth;
				AdjustWindowRectEx(&WindowSizeRect,static_cast<DWORD>(GetWindowLongW(pWindow,GWL_STYLE)),GetMenu(pWindow)!=0,static_cast<DWORD>(GetWindowLongW(pWindow,GWL_EXSTYLE)));

				// Set the minimum and maximum window sizes to the same value to perform the resize disabling
				MINMAXINFO *pMinMaxInfo = reinterpret_cast<MINMAXINFO *>(lParam);
				pMinMaxInfo->ptMaxSize.x = lScreenWidth;
				pMinMaxInfo->ptMaxSize.y = lScreenHeight;
				pMinMaxInfo->ptMaxTrackSize.x = WindowSizeRect.right-WindowSizeRect.left;
				pMinMaxInfo->ptMaxTrackSize.y = WindowSizeRect.bottom-WindowSizeRect.top;
				pMinMaxInfo->ptMinTrackSize.x = pMinMaxInfo->ptMaxTrackSize.x;
				pMinMaxInfo->ptMinTrackSize.y = pMinMaxInfo->ptMaxTrackSize.y;
				return FALSE;
			}
		}
		break;

	//
	// This handles cursor updates when the Windows cursor is
	// moving over the window when the window doesn't have focus
	//

	case WM_SETCURSOR:
		{
			// Read the current mouse position
			POINT MousePos;
			GetCursorPos(&MousePos);
			// Convert to game coordinates
			ScreenToClient(pThis->GetWindow(),&MousePos);
			// If there's a mouse device, set the position
			Burger::Mouse *pMouse = pThis->GetMouse();
			if (pMouse) {
				pMouse->SetPosition(static_cast<Word>(MousePos.x),static_cast<Word>(MousePos.y));
			}
			// Assume a hidden cursor
			Word bVisible = TRUE;
			// If the cursor in the game area, hide the cursor
			Burger::Display *pDisplay = pThis->GetDisplay();
			if (pDisplay) {
				if ((static_cast<Word>(MousePos.x)<pDisplay->GetWidth()) && (static_cast<Word>(MousePos.y)<pDisplay->GetHeight())) {
					bVisible = FALSE;
				}
			}
			// Show/hide the operating system cursor
			Burger::OSCursor::Show(bVisible);
		}
		return TRUE;

	//
	// The app is "activated"
	//
	case WM_ACTIVATEAPP:
		// If quitting, do NOT activate!
		if (pThis->GetQuitCode()) {
			return 0;
		}
		break;

	//
	// If this windows is put in the background, send a pause event
	// HIWORD(wParam) != 0 means the window is minimized
	//

	case WM_ACTIVATE:
		{
			Burger::Mouse *pMouse = pThis->GetMouse();
			Burger::Keyboard *pKeyboard = pThis->GetKeyboard();
			Burger::Joypad *pJoypad = pThis->GetJoypad();
			if ((HIWORD(wParam) == 0) && (LOWORD(wParam) != WA_INACTIVE)) {
				if (pMouse) {
					pMouse->Acquire();
				}
				if (pKeyboard) {
					pKeyboard->Acquire();
				}
				if (pJoypad) {
					pJoypad->Acquire();
				}
				pThis->SetInBackground(FALSE);
			} else {
				if (pMouse) {
					pMouse->Unacquire();
				}
				if (pKeyboard) {
					pKeyboard->Unacquire();
				}
				if (pJoypad) {
					pJoypad->Unacquire();
				}
				pThis->SetInBackground(TRUE);
			}
		}
		break;

	case WM_SIZE:
	case WM_MOVE:
		{
			RECT WindowSize;
			GetWindowRect(pWindow,&WindowSize);
			MoveWindow(pWindow,WindowSize.left,WindowSize.top,WindowSize.right - WindowSize.left,WindowSize.bottom - WindowSize.top,TRUE);

			Burger::Mouse *pMouse = pThis->GetMouse();
			if (pMouse) {
				Burger::Display *pDisplay = pThis->GetDisplay();
				if (pDisplay) {
					Word uScreenHeight = pDisplay->GetHeight();
					Word uScreenWidth = pDisplay->GetWidth();
					pMouse->SetRange(uScreenWidth,uScreenHeight);		// Reset the mouse coords for mouse handler
				}
			}
		}
		break;

	// Windows is asking for the window to be redrawn, possibly
	// from recovering from minimization?

	case WM_PAINT:
		// Any region to draw?
		if (GetUpdateRect(pWindow,NULL,FALSE)) {
			PAINTSTRUCT ps;
			// Save the old context
			HDC PaintDC = BeginPaint(pWindow,&ps);
			if (PaintDC) {
				// Get the video context
//				Burger::DisplayBase *pDisplay = pThis->GetDisplay();
//				if (pDisplay) {
//					// Force a front screen update
//					pDisplay->Update();
//				}
				EndPaint(pWindow,&ps);
			}
		}
		// Accept this event
		return 1;

	// Power functions
	case WM_POWERBROADCAST:
		switch (wParam) {
		case PBT_APMQUERYSUSPEND:
			// At this point, the app should save any data for open
			// network connections, files, etc., and prepare to go into
			// a suspended mode. The app can use the MsgProc callback
			// to handle this if desired.
			return 1;

		case PBT_APMRESUMESUSPEND:
			// At this point, the app should recover any data, network
			// connections, files, etc., and resume running from when
			// the app was suspended. The app can use the MsgProc callback
			// to handle this if desired.
			return 1;
		}
		break;

	// A menu was active and the user pressed an invalid keep. Disable the beep
	case WM_MENUCHAR:
		return MAKELRESULT(0,MNC_CLOSE);

	// Disable menu selection
	case WM_NCHITTEST:
		if (pThis->IsAppFullScreen()) {
			return HTCLIENT;
		}
		break;

	case WM_SYSCOMMAND:
		// Prevent moving/sizing in full screen mode
		switch(GET_SC_WPARAM(wParam)) {
		case SC_MOVE:
		case SC_SIZE:
		case SC_MAXIMIZE:
		case SC_KEYMENU:
			if (pThis->IsAppFullScreen()) {
				return 0;
			}
			break;
		}
		break;

	case WM_SYSKEYDOWN:
		switch (wParam) {
		case VK_RETURN:
			if ((pThis->IsWindowSwitchingAllowed()) &&
				// Is ALT also held down?
				(HIWORD(lParam) & KF_ALTDOWN)) {
				Burger::Display *pDisplay = pThis->GetDisplay();
				if (pThis) {
					pDisplay->Init(pDisplay->GetWidth(),pDisplay->GetHeight(),pDisplay->GetDepth(),pDisplay->GetFlags()^Burger::Display::FULLSCREEN);
					return 0;
				}
			}
			break;
		}
		break;
		
	case WM_DISPLAYCHANGE:
		return 0;

	// Ignore update events since redraws are happening constantly
	case WM_ERASEBKGND:
		return 1;

	case WM_DESTROY:
	case WM_QUIT:		// External quit event (System shutdown)
		pThis->SetQuitCode();
		return 0;
	default:
		break;
	}
	return DefWindowProcW(pWindow,uMessage,wParam,lParam);
}

/*! ************************************

	\brief Create an application's default window

	Windows requires a window object to be created by the
	application to send and receive system events including
	sound and visual focus. Create this window and set internal
	variables so the Windows version of Burgerlib can function
		
	\return Zero (\ref FALSE) if the window was successfully created

***************************************/

int Burger::WindowsApp::InitWindow(const char *pGameName,MainWindowProc pCallBack)
{
	m_pCallBack = pCallBack;
	WNDCLASSEXW WindowClass;
	MemoryClear(&WindowClass,sizeof(WindowClass));
	WindowClass.cbSize = sizeof(WindowClass);
	WindowClass.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;		// Accept double clicks, redraw on resize
	WindowClass.lpfnWndProc = InternalCallBack;	// My window callback
//	WindowClass.cbClsExtra = 0;
	WindowClass.cbWndExtra = sizeof(void *);	// Space for the "this" pointer
	WindowClass.hInstance = m_hInstance;
//	WindowClass.hIcon = NULL;
//	WindowClass.hCursor = NULL;
	WindowClass.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
//	WindowClass.lpszMenuName = NULL;
	WindowClass.lpszClassName = reinterpret_cast<LPCWSTR>(g_GameClass);
//	WindowClass.hIconSm = NULL;

	// Register my window's class
	ATOM MyAtom = RegisterClassExW(&WindowClass);

	int iResult;
	// Success in creating my class?
	if (MyAtom) {
		m_uAtom = MyAtom;
		// Convert the game name to unicode
		String16 TitleUnicode(pGameName);

		// Create the window and pass it the "this" pointer
		HWND pWindow = CreateWindowExW(
			WS_EX_APPWINDOW,	// Force top level to the task bar when minimized
			reinterpret_cast<LPCWSTR>(g_GameClass),				// Pointer to registered class name
			reinterpret_cast<LPCWSTR>(TitleUnicode.c_str()),	// Window title string
			WS_MINIMIZE,	// Make it an INVISIBLE window
			0,				// X coord
			0,				// Y coord
			320,			// Width
			200,			// Height
			NULL,			// Window parent
			NULL,			// Window menu
			m_hInstance,	// Task number
			this);			// Local parameter
		if (pWindow) {
			// Store the new window handle
			m_hWindow = pWindow;
			// Set the system global, obsolete
			Globals::SetWindow(pWindow);
			UpdateWindow(pWindow);		// Blank out the window to black
			SetFocus(pWindow);			// All input is directed to me
			iResult = 0;				// I'm OK!
		} else {
			iResult = static_cast<int>(GetLastError());
		} 
	} else {
		iResult = static_cast<int>(GetLastError());
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
	otherwise it will be place in at the last
	recorded location.

	\param uWidth Width of the display rectangle in pixels
	\param uHeight Height of the display rectangle in pixels
	\return Zero if no error, non-zero windows error code

***************************************/

int Burger::WindowsApp::SetWindowSize(Word uWidth,Word uHeight)
{
	// Get the application's window

	HWND pWindow = m_hWindow;
	// Get the style of the window
	LONG dwStyle = GetWindowLongW(pWindow,GWL_STYLE);
	// Can't be a pop-up window
	dwStyle &= ~(WS_POPUP);
	// Enable all the bells and whistles!
	dwStyle |= WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_SYSMENU;
	// Set the style (May not be visible if this is the first setting)
	SetWindowLongW(pWindow,GWL_STYLE,dwStyle);

	// Init the rect of the window's display area
	RECT NewWindowRect;
	NewWindowRect.left = 0;
	NewWindowRect.top = 0;
	NewWindowRect.right = static_cast<LONG>(uWidth);
	NewWindowRect.bottom = static_cast<LONG>(uHeight);

	// Calculate the rect of the window after the borders are added...

	AdjustWindowRectEx(&NewWindowRect,static_cast<DWORD>(dwStyle),static_cast<BOOL>(GetMenu(pWindow)!= 0),static_cast<DWORD>(GetWindowLongW(pWindow,GWL_EXSTYLE)));

	// Resize the window to the new rect

	Word uAdjustedWidth = static_cast<Word>(NewWindowRect.right-NewWindowRect.left);
	Word uAdjustedHeight = static_cast<Word>(NewWindowRect.bottom-NewWindowRect.top);
	SetWindowPos(pWindow,HWND_NOTOPMOST,0,0,static_cast<int>(uAdjustedWidth),static_cast<int>(uAdjustedHeight),SWP_NOMOVE | /* SWP_NOZORDER | */ SWP_NOACTIVATE);
	// Get the rect of the information bar

	RECT TempRect;		// Temp rect
	SystemParametersInfoW(SPI_GETWORKAREA,0,&TempRect,0);

	// Get the x,y position of the window

	if (m_bPreviousWindowXYValid) {
		NewWindowRect.left = m_iPreviousWindowX;		// Restore the window position
		NewWindowRect.top = m_iPreviousWindowY;
	} else {
		// Get the screen size and center it there
		NewWindowRect.left = static_cast<LONG>(GetSystemMetrics(SM_CXSCREEN)-uAdjustedWidth)/2;
		NewWindowRect.top = static_cast<LONG>(GetSystemMetrics(SM_CYSCREEN)-uAdjustedHeight)/2;
	}

	// Make sure the window is on screen

	if (NewWindowRect.left < TempRect.left) {
		NewWindowRect.left = TempRect.left;		// Anchor the window to the screen bar
	}
	if (NewWindowRect.top < TempRect.top) {
		NewWindowRect.top = TempRect.top;
	}
	// Set the style (Makes it visible)

	ShowWindow(pWindow,SW_SHOWNORMAL);

	// Position the window on the screen in the center or where it was located last time
	SetWindowPos(pWindow,HWND_TOP,NewWindowRect.left,NewWindowRect.top,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	return 0;
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

void Burger::WindowsApp::RecordWindowLocation(void)
{
	// Save off the window position only if a context was already successfully created
	RECT TempRect;
	GetWindowRect(m_hWindow,&TempRect);
	m_iPreviousWindowX = TempRect.left;
	m_iPreviousWindowY = TempRect.top;
	m_bPreviousWindowXYValid = TRUE;
}

/*! ************************************

	\brief Process Windows events

	Internal function that will call PeekMessage(), TranslateMessage() and DispatchMessage()
	to handle messages sent to the app
		
	\param pSelf "this" pointer to the WindowsApp class
	\return RunQueue::OKAY since this function never shuts down on its own

***************************************/

Burger::RunQueue::eReturnCode BURGER_API Burger::WindowsApp::Poll(void * /* pSelf */)
{
	MSG msg;
	while (PeekMessageW(&msg,NULL,0,0,PM_REMOVE)) {
		// Translate the keyboard (Localize)
		TranslateMessage(&msg);
		// Pass to the window event proc
		DispatchMessageW(&msg);
	}
	return RunQueue::OKAY;
}

#endif