/***************************************

	Global variable manager

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brglobals.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brdisplay.h"
#include "brfilemanager.h"
#include "brgameapp.h"
#include "brkeyboard.h"
#include "brmemoryfunctions.h"
#include "brmouse.h"
#include "brnumberto.h"
#include "brstring.h"
#include "brstring16.h"
#include "brwindowstypes.h"
#include "win_ddraw.h"
#include "win_globals.h"
#include "win_kernel32.h"
#include "win_launcher.h"
#include "win_platformshims.h"
#include "win_registry.h"
#include "win_user32.h"
#include "win_version.h"
#include "win_winutils.h"
#include "win_xinput.h"

#if !defined(DOXYGEN)

//
// Handle some annoying defines that some windows SDKs may or may not
// have
//

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

// Windows 8 API level
#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0602
#endif

#if !defined(DIRECTINPUT_VERSION)
#define DIRECTINPUT_VERSION 0x800
#endif

#if !defined(DIRECT3D_VERSION)
#define DIRECT3D_VERSION 0x900
#endif

#if !defined(DIRECTDRAW_VERSION)
#define DIRECTDRAW_VERSION 0x700
#endif

#if !defined(BUILD_WINDOWS)
#define BUILD_WINDOWS
#endif

#include <Windows.h>

#include <MMSystem.h>

#include <SetupAPI.h>
#include <WindowsX.h>
#include <Xinput.h>
#include <d3d9.h>
#include <d3dcommon.h>
#include <ddraw.h>
#include <dinput.h>
#include <dsound.h>
#include <io.h>
#include <shellapi.h>
#include <shlobj.h>
#include <winnt.h>

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

#if !defined(WM_MOUSEHWHEEL)
#define WM_MOUSEHWHEEL 0x020E
#endif

#if !defined(WM_NCMOUSELEAVE)
#define WM_NCMOUSELEAVE 0x02A2
#endif

#if !defined(INVALID_ATOM)
#define INVALID_ATOM ((ATOM)0)
#endif

#if !defined(_WIN32_WINNT_VISTA) && !defined(_WIN32_WINNT_LONGHORN)
#define TokenElevation static_cast<TOKEN_INFORMATION_CLASS>(20)
struct _TOKEN_ELEVATION {
	DWORD TokenIsElevated;
};
#endif

//
// Globals
//

ATOM Burger::Globals::g_uAtom =
	INVALID_ATOM; ///< Atom assigned to my class (Windows only)
uint32_t Burger::Globals::g_uQuickTimeVersion;
uint8_t Burger::Globals::g_bQuickTimeVersionValid;

#endif // Allow doxygen

/***************************************

	Internal windows dispatcher

***************************************/

#if !defined(DOXYGEN)

static LRESULT CALLBACK InternalCallBack(
	HWND pWindow, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	// Firstly, get the "this" pointer
	// Note that this is not automatically set, the pointer is
	// passed in the WM_NCCREATE command and it's manually set into the
	// WindowLongPtr index of GWLP_USERDATA.

	// Use GetWindowLongPtrW to be 64 bit clean
	Burger::GameApp* pThis = reinterpret_cast<Burger::GameApp*>(
		GetWindowLongPtrW(pWindow, GWLP_USERDATA));

	// Never initialized?
	if (!pThis) {
		// If this is a WM_NCCREATE event, then get the class instance pointer
		// passed.
		if (uMessage == WM_NCCREATE) {
			// Use SetWindowLongPtrW to be 64 bit clean
			pThis = static_cast<Burger::GameApp*>(
				reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
			SetWindowLongPtrW(pWindow, GWLP_USERDATA,
				static_cast<LONG_PTR>(reinterpret_cast<uintptr_t>(pThis)));
		}
		// If I didn't get a pThis set, just call the default procedure and exit
		if (!pThis) {
			return DefWindowProcW(pWindow, uMessage, wParam, lParam);
		}
	}

	// The pThis class instance is valid!

	// For debugging, if needed
#if defined(_DEBUG)
	if (Burger::get_traceflags() & Burger::kTraceFlagMessages) {
		Burger::Win32::output_windows_message(
			uMessage, wParam, static_cast<uintptr_t>(lParam));
	}
#endif

	// If there is a user supplied callback, issue it

	Burger::GameApp::MainWindowProc pCallBack = pThis->GetCallBack();
	if (pCallBack) {
		uintptr_t uOutput = FALSE; // Assume not handled (In case the callback
								   // doesn't set the variable)
		// If the function returns non-zero, assume it should terminate
		// immediately
		if (pCallBack(pThis, pWindow, static_cast<uint_t>(uMessage),
				static_cast<uintptr_t>(wParam), static_cast<uintptr_t>(lParam),
				&uOutput)) {
			// Return the passed result code
			return static_cast<LRESULT>(uOutput);
		}
	}

	//
	// Handle burgerlib update events
	//

	switch (uMessage) {

		//
		// This function will disable the ability to resize the window
		//

	case WM_GETMINMAXINFO: {
		// Ensure a MINIMUM size of 320x200
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.x = 320;
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.y = 200;
		// Only if a video display is present
		Burger::Display* pDisplay = pThis->GetDisplay();
		if (pDisplay) {
			if (pDisplay->HandleMinMax(
					pWindow, static_cast<uintptr_t>(lParam))) {
				return FALSE;
			}
		}
	} break;

	case WM_ENTERSIZEMOVE: {
		Burger::Display* pDisplay = pThis->GetDisplay();
		if (pDisplay) {
			// Halt frame movement while the app is sizing or moving
			pDisplay->Pause(TRUE);
			pThis->SetInSizeMove(TRUE);
		}
	} break;

	case WM_EXITSIZEMOVE: {
		Burger::Display* pDisplay = pThis->GetDisplay();
		if (pDisplay) {
			pDisplay->Pause(FALSE);
			pDisplay->CheckForWindowSizeChange();
			pDisplay->CheckForWindowChangingMonitors();
			pThis->SetInSizeMove(FALSE);
		}
	} break;

		//
		// This handles cursor updates when the Windows cursor is
		// moving over the window when the window doesn't have focus
		//

	case WM_SETCURSOR:
		if (pThis->HandleCursor(static_cast<uint_t>(lParam))) {
			return TRUE; // Handled
		}
		break;

		//
		// The app is "activated"
		//

	case WM_ACTIVATEAPP:
		// If quitting, do NOT activate!
		if (pThis->GetQuitCode()) {
			return 0; // Message is processed
		}

		// Is it active and was in the background?
		if ((wParam == TRUE) && pThis->IsInBackground()) {

			// If the joypad is present, enable XInput
			if (pThis->GetJoypad()) {
				Burger::Win32::XInputEnable(TRUE);
			}
			// Move to the foreground
			if (pThis->IsAppFullScreen()) {

				// For full screen, disable the short cut keys
				pThis->GetKeyboard()->DisableAccessibilityShortcutKeys();

				// Reacquire DirectInput
				pThis->GetInputFocus();
			} else {
				// Use the OS for input
				pThis->KillInputFocus();
			}
			pThis->SetInBackground(FALSE);

			// Is it being deactivated and was in the foreground?
		} else if ((!wParam) && !pThis->IsInBackground()) {

			// If the joypad is present, disable XInput
			if (pThis->GetJoypad()) {
				Burger::Win32::XInputEnable(FALSE);
			}

			// Ensure the OS has input
			pThis->KillInputFocus();

			// Make sure the short cut keys are available
			pThis->GetKeyboard()->RestoreAccessibilityShortcutKeys();
			pThis->SetInBackground(TRUE);
		}

		// State hasn't changed, leave as is
		return 0;

	//
	// Pass mouse motion to the mouse driver
	//

	// Mouse move events only happen when the mouse cursor is on the screen
	case WM_MOUSEMOVE:
		// If not previously tracked, ask Windows to send
		// me an event if the mouse is OFF this window
		// so the application is aware that the mouse
		// is no longer available

		if (!pThis->IsMouseOnScreen()) {
			TRACKMOUSEEVENT TrackIt;
			TrackIt.cbSize = sizeof(TrackIt);
			TrackIt.dwFlags = TME_LEAVE;
			TrackIt.hwndTrack = pWindow;
			TrackIt.dwHoverTime = 0;
			Burger::Win32::TrackMouseEvent(&TrackIt);
			pThis->SetMouseOnScreen(TRUE);
		}

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_XBUTTONDBLCLK:
	case WM_MOUSEHWHEEL: {
		// If there's a mouse device, set the position
		Burger::Mouse* pMouse = pThis->GetMouse();
		if (pMouse) {
			uint_t uMouseX;
			uint_t uMouseY;

			// Mouse wheel events give global coordinates. Go figure
			if ((uMessage == WM_MOUSEWHEEL) || (uMessage == WM_MOUSEHWHEEL)) {
				// Must use GET_X_LPARAM because the values
				// are signed shorts on multiple monitors
				POINT TempPoint;
				TempPoint.x = static_cast<LONG>(GET_X_LPARAM(lParam));
				TempPoint.y = static_cast<LONG>(GET_Y_LPARAM(lParam));
				ScreenToClient(pThis->GetWindow(), &TempPoint);
				uMouseX = static_cast<uint_t>(TempPoint.x);
				uMouseY = static_cast<uint_t>(TempPoint.y);
			} else {
				// They are unsigned values!
				uMouseX = LOWORD(lParam);
				uMouseY = HIWORD(lParam);
			}
			// Pass the value to the mouse driver
			pMouse->PostMousePosition(uMouseX, uMouseY);

			// Pass the mouse button events down
			switch (uMessage) {
			case WM_MOUSEWHEEL: {
				int iDelta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
				pMouse->PostMouseWheel(0, iDelta);
			} break;
			case WM_MOUSEHWHEEL: {
				int iDelta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
				pMouse->PostMouseWheel(iDelta, 0);
			} break;
			case WM_LBUTTONDOWN:
			case WM_LBUTTONDBLCLK:
				pMouse->PostMouseDown(Burger::Mouse::BUTTON_LEFT);
				break;
			case WM_RBUTTONDOWN:
			case WM_RBUTTONDBLCLK:
				pMouse->PostMouseDown(Burger::Mouse::BUTTON_RIGHT);
				break;
			case WM_MBUTTONDOWN:
			case WM_MBUTTONDBLCLK:
				pMouse->PostMouseDown(Burger::Mouse::BUTTON_MIDDLE);
				break;
			case WM_XBUTTONDOWN:
			case WM_XBUTTONDBLCLK: {
				// uBits is 1 or 2, convert to 0x8 or 0x10
				uint_t uBits = GET_XBUTTON_WPARAM(wParam);
				pMouse->PostMouseDown(uBits << 3U);
			}
				// XBUTTON events need to return TRUE
				// http://msdn.microsoft.com/en-us/library/windows/desktop/ms646245(v=vs.85).aspx
				return TRUE;
			case WM_LBUTTONUP:
				pMouse->PostMouseUp(Burger::Mouse::BUTTON_LEFT);
				break;
			case WM_RBUTTONUP:
				pMouse->PostMouseUp(Burger::Mouse::BUTTON_RIGHT);
				break;
			case WM_MBUTTONUP:
				pMouse->PostMouseUp(Burger::Mouse::BUTTON_MIDDLE);
				break;
			case WM_XBUTTONUP: {
				// uBits is 1 or 2, convert to 0x8 or 0x10
				uint_t uBits = GET_XBUTTON_WPARAM(wParam);
				pMouse->PostMouseUp(uBits << 3U);
			}
				// XBUTTON events need to return TRUE
				// http://msdn.microsoft.com/en-us/library/windows/desktop/ms646245(v=vs.85).aspx
				return TRUE;

			default:
				break;
			}
			return 0;
		}
	}
	// No mouse driver, pass the events down
	break;

	// Mouse is off the client area. Turn off any software cursor
	case WM_MOUSELEAVE:
	case WM_NCMOUSELEAVE:
	case WM_NCMOUSEMOVE:
		pThis->SetMouseOnScreen(FALSE);
		break;

		// An external program (The Keyboard control panel most likely)
		// changed the keyboard repeat speed.
		// Alert the keyboard manager, if one was started, about the
		// event.

	case WM_SETTINGCHANGE:
		if ((wParam == SPI_SETKEYBOARDSPEED) ||
			(wParam == SPI_SETKEYBOARDDELAY)) {
			Burger::Keyboard* pKeyboard = pThis->GetKeyboard();
			if (pKeyboard) {
				pKeyboard->ReadSystemKeyboardDelays();
			}
		} else if (wParam == SPI_SETMOUSEBUTTONSWAP) {
			Burger::Mouse* pMouse = pThis->GetMouse();
			if (pMouse) {
				pMouse->ReadSystemMouseValues();
			}
		}
		break;

	case WM_SIZE:
	case WM_MOVE: {
		Burger::Display* pDisplay = pThis->GetDisplay();
		if (pDisplay) {
			if (!(pDisplay->GetFlags() & Burger::Display::FULLSCREEN)) {
				pThis->RecordWindowLocation();
			}
			RECT TempRect;
			GetClientRect(pThis->GetWindow(), &TempRect);
			pDisplay->Resize(static_cast<uint_t>(TempRect.right),
				static_cast<uint_t>(TempRect.bottom));
			if (pDisplay->GetResizeCallback()) {
				(pDisplay->GetResizeCallback())(
					pDisplay->GetResizeCallbackData(),
					static_cast<uint_t>(TempRect.right),
					static_cast<uint_t>(TempRect.bottom));
			}
			// Alert the mouse subsystem to the new mouse bounds
			Burger::Mouse* pMouse = pThis->GetMouse();
			if (pMouse) {
				// Reset the mouse coords for mouse handler
				pMouse->SetRange(pDisplay->GetWidth(), pDisplay->GetHeight());
			}
		} else {
			pThis->RecordWindowLocation();
		}
	} break;

		// Windows is asking for the window to be redrawn, possibly
		// from recovering from minimization?

	case WM_NCPAINT:
	case WM_PAINT:
		// Any region to draw?
		if (GetUpdateRect(pWindow, NULL, FALSE)) {
			PAINTSTRUCT ps;
			// Save the old context
			HDC PaintDC = BeginPaint(pWindow, &ps);
			if (PaintDC) {
				// Get the video context
				Burger::Display* pDisplay = pThis->GetDisplay();
				if (pDisplay) {
					// Force a front screen update
					Burger::Display::RenderProc pCallback =
						pDisplay->GetRenderCallback();
					if (pCallback) {
						(pCallback)(pDisplay->GetRenderCallbackData());
					}
				}
				EndPaint(pWindow, &ps);
			}
			RECT TempRect;
			GetClientRect(pThis->GetWindow(), &TempRect);
			ValidateRect(pWindow, &TempRect);
		}
		if (uMessage == WM_PAINT) {
			return 1;
		}
		break;

	// Power functions
	case WM_POWERBROADCAST:
		switch (wParam) {
		case PBT_APMQUERYSUSPEND:
			// Do not allow the app to suspend!
			// Note: Screen savers should quit and apps should send a
			// notification!
			return BROADCAST_QUERY_DENY;

		// Resume from power saving?
		case PBT_APMRESUMESUSPEND:
			return 1;
		}
		break;

	// A menu was active and the user pressed an invalid key. Disable the beep
	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);

	// Disable menu selection by forcing high level code to think
	// everything is part of the client area.
	case WM_NCHITTEST:
		if (pThis->IsAppFullScreen()) {
			return HTCLIENT;
		}
		if (!pThis->IsResizingAllowed()) {
			// Process the test
			LRESULT lResult = DefWindowProcW(pWindow, uMessage, wParam, lParam);
			// Override the borders to the caption to change resizing
			// to window movement events
			switch (lResult) {
			case HTRIGHT:
			case HTLEFT:
			case HTTOP:
			case HTTOPLEFT:
			case HTTOPRIGHT:
			case HTBOTTOM:
			case HTBOTTOMRIGHT:
			case HTBOTTOMLEFT:
				lResult = HTCAPTION;
			default:
				break;
			}
			return lResult;
		}
		// Allow the OS handle handle the hot spots
		break;

	case WM_SYSCOMMAND:
		// Prevent moving/sizing in full screen mode
		switch (GET_SC_WPARAM(wParam)) {
		case SC_SIZE:
			if (!pThis->IsResizingAllowed()) {
				return 0; // Discard resize commands
			}
		case SC_MOVE:
		case SC_MAXIMIZE:
		case SC_KEYMENU:
			if (pThis->IsAppFullScreen()) {
				return 0;
			}
			break;
		}
		break;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP: {
		// If there's a keyboard manager, pass the keys to it.
		Burger::Keyboard* pKeyboard = pThis->GetKeyboard();
		if (pKeyboard) {
			Burger::eEvent uEvent;

			// Key up event?
			if (lParam & (KF_UP << 16)) {
				uEvent = Burger::EVENT_KEYUP;
			} else {
				// Repeated key event?
				if (lParam & (KF_REPEAT << 16)) {
					uEvent = Burger::EVENT_KEYAUTO;
				} else {
					// Normal key down event
					uEvent = Burger::EVENT_KEYDOWN;
				}
			}
			pKeyboard->PostWindowsKeyEvent(uEvent,
				((static_cast<uint32_t>(lParam) >> 16U) & 0x7FU) |
					((static_cast<uint32_t>(lParam) >> 17U) & 0x80U));
			return 0;
		}
	} break;

	case WM_KILLFOCUS:
		pThis->KillInputFocus();
		break;

	case WM_SETFOCUS:
		pThis->GetInputFocus();
		break;

	case WM_DISPLAYCHANGE:
		return 0;

	// Ignore update events since redraws are happening constantly
	case WM_ERASEBKGND:
		return 1;

	case WM_DESTROY:
	case WM_QUIT: // External quit event (System shutdown)
		pThis->SetQuitCode();
		return 0;
	default:
		break;
	}
	return DefWindowProcW(pWindow, uMessage, wParam, lParam);
}

#endif

/*! ************************************

	\brief Create the global Burgerlib Window Class

	Windows requires a WNDCLASS definition declared to allow the creation of
	application specific windows. Burgerlib uses it's own custom callback
	function and it's declared after this function is called which calls
	RegisterClassExW() in windows that creates an ATOM of the type
	BurgerGameClass.

	The string can be obtained with a call to get_window_class_name() if it's
	desired for an application to create a Burgerlib window without Burgerlib
	doing it for you.

	The ATOM is released on application shutdown, or manually with a call to
	UnregisterWindowClass()

	If this function had previously created the game class, it will return the
	ATOM that was created before and exit immediately.

	\note This function requires that set_instance() has been called. Burgerlib
	does this automatically when creating a GameApp class instance, however if
	no GameApp was created, the INSTANCE must be passed manually for this
	function to work properly

	\windowsonly

	\param uIconResID Numeric ID of the Windows Icon to attach to the window
	that's in the EXE files resource data (0 to use the default application icon
	from Windows)

	\sa UnregisterWindowClass(void)

***************************************/

uint16_t BURGER_API Burger::Globals::RegisterWindowClass(uint_t uIconResID)
{
	ATOM uAtom = g_uAtom;
	if (uAtom == INVALID_ATOM) {

		// Is the an app instance?
		if (!Win32::get_instance()) {
			// Obtain it
			Win32::set_instance(GetModuleHandleW(NULL));
		}

		// Try to load the icon for the app
		HICON hIcon = NULL;
		if (uIconResID) {
			hIcon =
				LoadIcon(Win32::get_instance(), MAKEINTRESOURCE(uIconResID));
		}
		// No icon?
		if (hIcon) {
			// Try pulling the icon from the app itself by getting the first
			// icon found
			WCHAR TheExePath[1024];
			GetModuleFileNameW(NULL, TheExePath, BURGER_ARRAYSIZE(TheExePath));
			hIcon = ExtractIconW(Win32::get_instance(), TheExePath, 0);
		}

		WNDCLASSEXW WindowClass;
		MemoryClear(&WindowClass, sizeof(WindowClass));
		WindowClass.cbSize = sizeof(WindowClass);
		WindowClass.style = CS_DBLCLKS;             // Accept double clicks
		WindowClass.lpfnWndProc = InternalCallBack; // My window callback
		//	WindowClass.cbClsExtra = 0;			// No extra class bytes
		//	WindowClass.cbWndExtra = 0;			// No extra space was needed
		WindowClass.hInstance = Win32::get_instance();
		WindowClass.hIcon = hIcon;
		// Keep the cursor NULL to allow updating of the cursor by the app
		//	WindowClass.hCursor = NULL;
		////LoadCursorW(NULL,MAKEINTRESOURCEW(IDC_ARROW));
		WindowClass.hbrBackground =
			reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		//	WindowClass.lpszMenuName = NULL;
		WindowClass.lpszClassName = (LPCWSTR)Win32::get_window_class_name();
		//	WindowClass.hIconSm = NULL;		// Assume hIcon has the small icon
		// in the set

		// Register my window's class
		uAtom = RegisterClassExW(&WindowClass);

		// If this function fails, it will be INVALID_ATOM
		g_uAtom = uAtom;
	}
	return uAtom;
}

/*! ************************************

	\brief Release the global Burgerlib Window Class

	When RegisterWindowClass(uint_t) is called, it will create a global ATOM of
	the window class. This function will unregister the class. This function is
	called on shutdown automatically

	\windowsonly

	\sa RegisterWindowClass(uint_t)

***************************************/

void BURGER_API Burger::Globals::UnregisterWindowClass(void)
{
	const ATOM uAtom = g_uAtom;
	if (uAtom != INVALID_ATOM) {
		// Release the class
		UnregisterClassW(MAKEINTRESOURCEW(uAtom), Win32::get_instance());
		g_uAtom = INVALID_ATOM;
	}
}

/***************************************

	Load and launch a web page from an address string

***************************************/

uint_t BURGER_API Burger::Globals::LaunchURL(const char* pURL)
{
	// I launched if greater than 32
	const uint_t uResult = (Win32::ShellExecuteOpen(pURL) > 32);
	return uResult;
}

/***************************************

	Execute a tool and capture the text output

***************************************/

int BURGER_API Burger::Globals::ExecuteTool(
	const char* pFilename, const char* pParameters, OutputMemoryStream* pOutput)
{
	// Get the parameter list
	Filename AppName(pFilename);
	String Full("\"", AppName.get_native(), "\" ", pParameters);
	// Create the full Unicode command string
	String16 Unicode(Full);

	// Prepare the process information
	PROCESS_INFORMATION ProcessInfo;
	MemoryClear(&ProcessInfo, sizeof(ProcessInfo));
	STARTUPINFOW StartupInfo;
	MemoryClear(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(StartupInfo);

	// Assume no text capturing

	HANDLE hCaptureIn = nullptr;
	HANDLE hCaptureOut = nullptr;

	// Is capturing requested?
	if (pOutput) {
		// Create a pipe for STDOUT
		SECURITY_ATTRIBUTES SecurityAttributes;
		MemoryClear(&SecurityAttributes, sizeof(SecurityAttributes));
		SecurityAttributes.nLength = sizeof(SecurityAttributes);
		SecurityAttributes.bInheritHandle = TRUE;
		SecurityAttributes.lpSecurityDescriptor = nullptr;
		// Create them and allow the capture pipe to inherit permissions
		if (CreatePipe(&hCaptureIn, &hCaptureOut, &SecurityAttributes, 0)) {
			if (SetHandleInformation(hCaptureIn, HANDLE_FLAG_INHERIT, 0)) {
				// It's good, capture the output
				StartupInfo.hStdError = hCaptureOut;
				StartupInfo.hStdOutput = hCaptureOut;
				StartupInfo.dwFlags = STARTF_USESTDHANDLES;
			}
		}
	}

	// Assume failure
	DWORD nExitCode = 10;

	// Call the tool
	BOOL bResult = CreateProcessW(nullptr,
		reinterpret_cast<LPWSTR>(Unicode.GetPtr()), nullptr, nullptr,
		pOutput != nullptr, 0, nullptr, nullptr, &StartupInfo, &ProcessInfo);

	// Did it even launch?
	if (bResult) {
		// Wait for the tool to finish executing
		if (WaitForSingleObject(ProcessInfo.hProcess, INFINITE) ==
			WAIT_OBJECT_0) {
			// Get the exit code from the tool
			if (!GetExitCodeProcess(ProcessInfo.hProcess, &nExitCode)) {
				// Failure! Assume an error code of 10
				nExitCode = 10;
			}
		}
		// Release the handles (Since launch was successful)
		CloseHandle(ProcessInfo.hProcess);
		CloseHandle(ProcessInfo.hThread);
	}

	// Release the capture pipe (So the input pipe is finite)
	if (hCaptureOut) {
		CloseHandle(hCaptureOut);
	}

	// Only capture if needed
	if (bResult && pOutput) {
		DWORD uBytesRead;
		uint8_t Buffer[1024];
		for (;;) {
			// Read from the finite pipe
			const BOOL bSuccess = ReadFile(
				hCaptureIn, Buffer, sizeof(Buffer), &uBytesRead, nullptr);
			// Error or all done?
			if ((!bSuccess) || (!uBytesRead)) {
				break;
			}
			pOutput->Append(Buffer, uBytesRead);
		}
	}
	// Clean up the last handle
	if (hCaptureIn) {
		CloseHandle(hCaptureIn);
	}
	// Exit with the tool's error code
	return static_cast<int>(nExitCode);
}

/***************************************

	Read an environment variable as UTF8

***************************************/

const char* BURGER_API Burger::GetEnvironmentString(
	const char* pKey) BURGER_NOEXCEPT
{
	String16 Key16;

	// Convert the key to UTF16
	char* pValue = nullptr;
	if (!Key16.Set(pKey)) {
		// How long is the key?
		const DWORD uLength = GetEnvironmentVariableW(
			reinterpret_cast<LPCWSTR>(Key16.c_str()), nullptr, 0);

		// Any key?
		if (uLength) {
			// Set the buffer to accept the value
			String16 Output;
			if (!Output.SetBufferSize(uLength)) {
				// Read in the environment variable as UTF16
				GetEnvironmentVariableW(
					reinterpret_cast<LPCWSTR>(Key16.c_str()),
					reinterpret_cast<LPWSTR>(Output.c_str()), uLength + 1);
				// Convert to UTF8
				String Final;
				if (!Final.assign(Output.c_str())) {

					// Return a copy
					pValue = StringDuplicate(Final.c_str());
				}
			}
		}
	}
	return pValue;
}

/***************************************

	Set an environment variable with a UTF8 string

***************************************/

Burger::eError BURGER_API Burger::SetEnvironmentString(
	const char* pKey, const char* pInput) BURGER_NOEXCEPT
{
	// Convert the key to UTF16
	String16 Key16;
	// Convert the input to UTF16
	String16 Input16;

	eError uResult = Key16.Set(pKey);
	if (!uResult) {
		uResult = Input16.Set(pInput);
		if (!uResult) {
			// If the input is an empty string or null, get rid of the string
			LPWSTR pInput16 = nullptr;
			if (pInput && pInput[0]) {
				// Set to the new value
				pInput16 = reinterpret_cast<LPWSTR>(Input16.GetPtr());
			}
			// Set or delete the variable!
			if (!SetEnvironmentVariableW(
					reinterpret_cast<LPCWSTR>(Key16.GetPtr()), pInput16)) {
				uResult = static_cast<eError>(GetLastError());
			}
		}
	}
	return uResult;
}

/***************************************

	Test if the application has elevated privileges

***************************************/

uint_t BURGER_API Burger::IsElevated(void) BURGER_NOEXCEPT
{
	// Assume not elevated
	uint_t bResult = FALSE;

	// Get the security token
	HANDLE hToken = nullptr;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		DWORD dwSize = 0;
		// Vista or higher has true "root" access, test for it.
		if (Win32::is_vista_or_higher()) {

			// Test for elevation
			_TOKEN_ELEVATION elevation;
			if (GetTokenInformation(hToken, TokenElevation, &elevation,
					sizeof(elevation), &dwSize)) {
				// Return the result
				bResult = elevation.TokenIsElevated;
			}
		} else {
			// Windows XP doesn't have this concept. Instead, test if the token
			// has restrictions, which is the equivalent of running as a user

			if (!IsTokenRestricted(hToken)) {
				bResult = TRUE;
			}
		}
		CloseHandle(hToken);
	}
	return bResult;
}

#endif
