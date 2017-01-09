/***************************************

	Keyboard Manager

	Windows version

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brkeyboard.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)

#if !defined(DOXYGEN)

#if !defined(DIRECTINPUT_VERSION)
#define DIRECTINPUT_VERSION 0x0800
#endif

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0501		// Windows XP
#endif

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include "brgameapp.h"
#include "brglobals.h"
#include "brassert.h"
#include "brstringfunctions.h"
#include "brtick.h"
#include <dinput.h>

//
// Windows 10 broke DirectInput. Turning it off
//

//#define ENABLE_DIRECTINPUT

// Number of keyboard events to cache

#define DIRECTINPUT_KEYBOARDBUFFERSIZE 16

//
// Table, first entry is the Windows code, the second is the code for Burgerlib
//

static const Burger::Keyboard::eScanCode g_WindowsToKeyboardeScanCode[0xF0] = {
	Burger::Keyboard::SC_INVALID,	// ???
	Burger::Keyboard::SC_ESCAPE,	// DIK_ESCAPE
	Burger::Keyboard::SC_1,			// DIK_1
	Burger::Keyboard::SC_2,			// DIK_2
	Burger::Keyboard::SC_3,			// DIK_3
	Burger::Keyboard::SC_4,			// DIK_4
	Burger::Keyboard::SC_5,			// DIK_5
	Burger::Keyboard::SC_6,			// DIK_6
	Burger::Keyboard::SC_7,			// DIK_7
	Burger::Keyboard::SC_8,			// DIK_8
	Burger::Keyboard::SC_9,			// DIK_9
	Burger::Keyboard::SC_0,			// DIK_9
	Burger::Keyboard::SC_MINUS,		// DIK_MINUS
	Burger::Keyboard::SC_EQUALS,	// DIK_EQUALS
	Burger::Keyboard::SC_BACKSPACE,	// DIK_BACK
	Burger::Keyboard::SC_TAB,		// DIK_TAB
	Burger::Keyboard::SC_Q,			// DIK_Q
	Burger::Keyboard::SC_W,			// DIK_W
	Burger::Keyboard::SC_E,			// DIK_E
	Burger::Keyboard::SC_R,			// DIK_R
	Burger::Keyboard::SC_T,			// DIK_T
	Burger::Keyboard::SC_Y,			// DIK_Y
	Burger::Keyboard::SC_U,			// DIK_U
	Burger::Keyboard::SC_I,			// DIK_I
	Burger::Keyboard::SC_O,			// DIK_O
	Burger::Keyboard::SC_P,			// DIK_P
	Burger::Keyboard::SC_LEFTBRACKET,	// DIK_LBRACKET
	Burger::Keyboard::SC_RIGHTBRACKET,	// DIK_RBRACKET
	Burger::Keyboard::SC_RETURN,	// DIK_RETURN
	Burger::Keyboard::SC_LEFTCONTROL,	// DIK_LCONTROL
	Burger::Keyboard::SC_A,			// DIK_A
	Burger::Keyboard::SC_S,			// DIK_S
	Burger::Keyboard::SC_D,			// DIK_D
	Burger::Keyboard::SC_F,			// DIK_F
	Burger::Keyboard::SC_G,			// DIK_G
	Burger::Keyboard::SC_H,			// DIK_H
	Burger::Keyboard::SC_J,			// DIK_J
	Burger::Keyboard::SC_K,			// DIK_K
	Burger::Keyboard::SC_L,			// DIK_L
	Burger::Keyboard::SC_SEMICOLON,	// DIK_SEMICOLON
	Burger::Keyboard::SC_QUOTE,		// DIK_APOSTROPHE
	Burger::Keyboard::SC_GRAVE,		// DIK_GRAVE
	Burger::Keyboard::SC_LEFTSHIFT,	// DIK_LSHIFT
	Burger::Keyboard::SC_BACKSLASH,	// DIK_BACKSLASH
	Burger::Keyboard::SC_Z,			// DIK_Z
	Burger::Keyboard::SC_X,			// DIK_X
	Burger::Keyboard::SC_C,			// DIK_C
	Burger::Keyboard::SC_V,			// DIK_V
	Burger::Keyboard::SC_B,			// DIK_B
	Burger::Keyboard::SC_N,			// DIK_N
	Burger::Keyboard::SC_M,			// DIK_M
	Burger::Keyboard::SC_COMMA,		// DIK_COMMA
	Burger::Keyboard::SC_PERIOD,	// DIK_PERIOD
	Burger::Keyboard::SC_SLASH,		// DIK_SLASH
	Burger::Keyboard::SC_RIGHTSHIFT,	// DIK_RSHIFT
	Burger::Keyboard::SC_KEYPADASTERISK,	// DIK_MULTIPLY
	Burger::Keyboard::SC_LEFTALT,	// DIK_LMENU
	Burger::Keyboard::SC_SPACE,		// DIK_SPACE
	Burger::Keyboard::SC_CAPSLOCK,	// DIK_CAPITAL
	Burger::Keyboard::SC_F1,		// DIK_F1
	Burger::Keyboard::SC_F2,		// DIK_F2
	Burger::Keyboard::SC_F3,		// DIK_F3
	Burger::Keyboard::SC_F4,		// DIK_F4
	Burger::Keyboard::SC_F5,		// DIK_F5
	Burger::Keyboard::SC_F6,		// DIK_F6
	Burger::Keyboard::SC_F7,		// DIK_F7
	Burger::Keyboard::SC_F8,		// DIK_F8
	Burger::Keyboard::SC_F9,		// DIK_F9
	Burger::Keyboard::SC_F10,		// DIK_F10
	Burger::Keyboard::SC_NUMLOCK,	// DIK_NUMLOCK
	Burger::Keyboard::SC_SCROLLLOCK,	// DIK_SCROLL
	Burger::Keyboard::SC_KEYPAD7,	// DIK_NUMPAD7
	Burger::Keyboard::SC_KEYPAD8,	// DIK_NUMPAD8
	Burger::Keyboard::SC_KEYPAD9,	// DIK_NUMPAD9
	Burger::Keyboard::SC_KEYPADMINUS,		// DIK_SUBTRACT
	Burger::Keyboard::SC_KEYPAD4,	// DIK_NUMPAD4
	Burger::Keyboard::SC_KEYPAD5,	// DIK_NUMPAD5
	Burger::Keyboard::SC_KEYPAD6,	// DIK_NUMPAD6
	Burger::Keyboard::SC_KEYPADPLUS,	// DIK_ADD
	Burger::Keyboard::SC_KEYPAD1,	// DIK_NUMPAD1
	Burger::Keyboard::SC_KEYPAD2,	// DIK_NUMPAD2
	Burger::Keyboard::SC_KEYPAD3,	// DIK_NUMPAD3
	Burger::Keyboard::SC_KEYPAD0,	// DIK_NUMPAD0
	Burger::Keyboard::SC_KEYPADPERIOD,	// DIK_DECIMAL
	Burger::Keyboard::SC_INVALID,	// 0x54
	Burger::Keyboard::SC_INVALID,	// 0x55
	Burger::Keyboard::SC_OEM_102,	// DIK_OEM_102
	Burger::Keyboard::SC_F11,		// DIK_F11
	Burger::Keyboard::SC_F12,		// DIK_F12
	Burger::Keyboard::SC_INVALID,	// 0x59
	Burger::Keyboard::SC_INVALID,	// 0x5A
	Burger::Keyboard::SC_INVALID,	// 0x5B
	Burger::Keyboard::SC_INVALID,	// 0x5C
	Burger::Keyboard::SC_INVALID,	// 0x5D
	Burger::Keyboard::SC_INVALID,	// 0x5E
	Burger::Keyboard::SC_INVALID,	// 0x5F
	Burger::Keyboard::SC_INVALID,	// 0x60
	Burger::Keyboard::SC_INVALID,	// 0x61
	Burger::Keyboard::SC_INVALID,	// 0x62
	Burger::Keyboard::SC_INVALID,	// 0x63
	Burger::Keyboard::SC_F13,		// DIK_F13
	Burger::Keyboard::SC_F14,		// DIK_F14
	Burger::Keyboard::SC_F15,		// DIK_F15
	Burger::Keyboard::SC_INVALID,	// 0x67
	Burger::Keyboard::SC_INVALID,	// 0x68
	Burger::Keyboard::SC_INVALID,	// 0x69
	Burger::Keyboard::SC_INVALID,	// 0x6A
	Burger::Keyboard::SC_INVALID,	// 0x6B
	Burger::Keyboard::SC_INVALID,	// 0x6C
	Burger::Keyboard::SC_INVALID,	// 0x6D
	Burger::Keyboard::SC_INVALID,	// 0x6E
	Burger::Keyboard::SC_INVALID,	// 0x6F
	Burger::Keyboard::SC_KANA,		// DIK_KANA
	Burger::Keyboard::SC_INVALID,	// 0x71
	Burger::Keyboard::SC_INVALID,	// 0x72
	Burger::Keyboard::SC_ABNT_C1,	// DIK_ABNT_C1
	Burger::Keyboard::SC_INVALID,	// 0x74
	Burger::Keyboard::SC_INVALID,	// 0x75
	Burger::Keyboard::SC_INVALID,	// 0x76
	Burger::Keyboard::SC_INVALID,	// 0x77
	Burger::Keyboard::SC_INVALID,	// 0x78
	Burger::Keyboard::SC_CONVERT,	// DIK_CONVERT
	Burger::Keyboard::SC_INVALID,	// 0x7A
	Burger::Keyboard::SC_NOCONVERT,	// DIK_NOCONVERT
	Burger::Keyboard::SC_INVALID,	// 0x7C
	Burger::Keyboard::SC_YEN,		// DIK_YEN
	Burger::Keyboard::SC_ABNT_C2,	// DIK_ABNT_C2
	Burger::Keyboard::SC_INVALID,	// 0x7F
	Burger::Keyboard::SC_INVALID,	// 0x80
	Burger::Keyboard::SC_INVALID,	// 0x81
	Burger::Keyboard::SC_INVALID,	// 0x82
	Burger::Keyboard::SC_INVALID,	// 0x83
	Burger::Keyboard::SC_INVALID,	// 0x84
	Burger::Keyboard::SC_INVALID,	// 0x85
	Burger::Keyboard::SC_INVALID,	// 0x86
	Burger::Keyboard::SC_INVALID,	// 0x87
	Burger::Keyboard::SC_INVALID,	// 0x88
	Burger::Keyboard::SC_INVALID,	// 0x89
	Burger::Keyboard::SC_INVALID,	// 0x8A
	Burger::Keyboard::SC_INVALID,	// 0x8B
	Burger::Keyboard::SC_INVALID,	// 0x8C
	Burger::Keyboard::SC_KEYPADEQUALS,	// DIK_NUMPADEQUALS
	Burger::Keyboard::SC_INVALID,	// 0x8E
	Burger::Keyboard::SC_INVALID,	// 0x8F
	Burger::Keyboard::SC_PREVTRACK,	// DIK_PREVTRACK
	Burger::Keyboard::SC_AT,		// DIK_AT
	Burger::Keyboard::SC_COLONPC98,	// DIK_COLON
	Burger::Keyboard::SC_UNDERLINE,	// DIK_UNDERLINE
	Burger::Keyboard::SC_KANJI,		// DIK_KANJI
	Burger::Keyboard::SC_STOP,		// DIK_STOP
	Burger::Keyboard::SC_AX,		// DIK_AX
	Burger::Keyboard::SC_UNLABELED,	// DIK_UNLABELED
	Burger::Keyboard::SC_INVALID,	// 0x98
	Burger::Keyboard::SC_NEXTTRACK,	// DIK_NEXTTRACK
	Burger::Keyboard::SC_INVALID,	// 0x9A
	Burger::Keyboard::SC_INVALID,	// 0x9B
	Burger::Keyboard::SC_KEYPADENTER,	// DIK_NUMPADENTER
	Burger::Keyboard::SC_RIGHTCONTROL,	// DIK_RCONTROL
	Burger::Keyboard::SC_INVALID,	// 0x9E
	Burger::Keyboard::SC_INVALID,	// 0x9F
	Burger::Keyboard::SC_MUTE,		// DIK_MUTE
	Burger::Keyboard::SC_CALCULATOR,	// DIK_CALCULATOR
	Burger::Keyboard::SC_PLAYPAUSE,	// DIK_PLAYPAUSE
	Burger::Keyboard::SC_INVALID,	// 0xA3
	Burger::Keyboard::SC_MEDIASTOP,	// DIK_MEDIASTOP
	Burger::Keyboard::SC_INVALID,	// 0xA5
	Burger::Keyboard::SC_INVALID,	// 0xA6
	Burger::Keyboard::SC_INVALID,	// 0xA7
	Burger::Keyboard::SC_INVALID,	// 0xA8
	Burger::Keyboard::SC_INVALID,	// 0xA9
	Burger::Keyboard::SC_INVALID,	// 0xA9
	Burger::Keyboard::SC_INVALID,	// 0xAA
	Burger::Keyboard::SC_INVALID,	// 0xAC
	Burger::Keyboard::SC_INVALID,	// 0xAD
	Burger::Keyboard::SC_VOLUMEDOWN,	// DIK_VOLUMEDOWN
	Burger::Keyboard::SC_INVALID,	// 0xAF
	Burger::Keyboard::SC_VOLUMEUP,	// DIK_VOLUMEUP
	Burger::Keyboard::SC_INVALID,	// 0xB1
	Burger::Keyboard::SC_WEBHOME,	// DIK_WEBHOME
	Burger::Keyboard::SC_KEYPADCOMMA,	// DIK_NUMPADCOMMA
	Burger::Keyboard::SC_INVALID,	// 0xB4
	Burger::Keyboard::SC_KEYPADSLASH,	// DIK_DIVIDE
	Burger::Keyboard::SC_INVALID,	// 0xB6
	Burger::Keyboard::SC_SYSRQ,	// DIK_SYSRQ
	Burger::Keyboard::SC_RIGHTALT,	// DIK_RMENU
	Burger::Keyboard::SC_INVALID,	// 0xB9
	Burger::Keyboard::SC_INVALID,	// 0xBA
	Burger::Keyboard::SC_INVALID,	// 0xBB
	Burger::Keyboard::SC_INVALID,	// 0xBC
	Burger::Keyboard::SC_INVALID,	// 0xBD
	Burger::Keyboard::SC_INVALID,	// 0xBE
	Burger::Keyboard::SC_INVALID,	// 0xBF
	Burger::Keyboard::SC_INVALID,	// 0xC0
	Burger::Keyboard::SC_INVALID,	// 0xC1
	Burger::Keyboard::SC_INVALID,	// 0xC2
	Burger::Keyboard::SC_INVALID,	// 0xC3
	Burger::Keyboard::SC_INVALID,	// 0xC4
	Burger::Keyboard::SC_PAUSE,		// DIK_PAUSE
	Burger::Keyboard::SC_INVALID,	// 0xC6
	Burger::Keyboard::SC_HOME,		// DIK_HOME
	Burger::Keyboard::SC_UPARROW,	// DIK_UP
	Burger::Keyboard::SC_PAGEUP,	// DIK_PRIOR
	Burger::Keyboard::SC_INVALID,	// 0xCA
	Burger::Keyboard::SC_LEFTARROW,	// DIK_LEFT
	Burger::Keyboard::SC_INVALID,	// 0xCC
	Burger::Keyboard::SC_RIGHTARROW,	// DIK_RIGHT
	Burger::Keyboard::SC_INVALID,	// 0xCE
	Burger::Keyboard::SC_END,		// DIK_END
	Burger::Keyboard::SC_DOWNARROW,	// DIK_DOWN
	Burger::Keyboard::SC_PAGEDOWN,	// DIK_NEXT
	Burger::Keyboard::SC_INSERT,	// DIK_INSERT
	Burger::Keyboard::SC_DELETE,	// DIK_DELETE
	Burger::Keyboard::SC_INVALID,	// 0xD4
	Burger::Keyboard::SC_INVALID,	// 0xD5
	Burger::Keyboard::SC_INVALID,	// 0xD6
	Burger::Keyboard::SC_INVALID,	// 0xD7
	Burger::Keyboard::SC_INVALID,	// 0xD8
	Burger::Keyboard::SC_INVALID,	// 0xD9
	Burger::Keyboard::SC_INVALID,	// 0xDA
	Burger::Keyboard::SC_LEFTOPTION,	// DIK_LWIN
	Burger::Keyboard::SC_RIGHTOPTION,	// DIK_RWIN
	Burger::Keyboard::SC_APPS,		// DIK_APPS
	Burger::Keyboard::SC_POWER,		// DIK_POWER
	Burger::Keyboard::SC_SLEEP,		// DIK_SLEEP
	Burger::Keyboard::SC_INVALID,	// 0xE0
	Burger::Keyboard::SC_INVALID,	// 0xE1
	Burger::Keyboard::SC_INVALID,	// 0xE2
	Burger::Keyboard::SC_WAKE,		// DIK_WAKE
	Burger::Keyboard::SC_INVALID,	// 0xE4
	Burger::Keyboard::SC_WEBSEARCH,	// DIK_WEBSEARCH
	Burger::Keyboard::SC_WEBFAVORITES,	// DIK_WEBFAVORITES
	Burger::Keyboard::SC_WEBREFRESH,	// DIK_WEBREFRESH
	Burger::Keyboard::SC_WEBSTOP,	// DIK_WEBSTOP
	Burger::Keyboard::SC_WEBFORWARD,	// DIK_WEBFORWARD
	Burger::Keyboard::SC_WEBBACK,	// DIK_WEBBACK
	Burger::Keyboard::SC_MYCOMPUTER,	// DIK_MYCOMPUTER
	Burger::Keyboard::SC_MAIL,		// DIK_MAIL
	Burger::Keyboard::SC_MEDIASELECT,	// DIK_MEDIASELECT
	Burger::Keyboard::SC_INVALID,	// 0xEE
	Burger::Keyboard::SC_INVALID	// 0xEF
};

#endif

/***************************************

	\brief Private function to remove Windows Keystrokes (Windows only)
	
	This is a keyboard intercept hook to drop windows key events

	Full docs are at https://msdn.microsoft.com/en-us/library/windows/desktop/ms644985(v=vs.85).aspx

	\note This is used by the keyboard manager, it's not intended
	to be used by applications.

	\param pData Pointer to the Keyboard class instance
	\return Zero when asked to shut down

***************************************/
 
//
// By rights, these should be passed in an allocated class structure, but
// the callback doesn't have a place for a user data pointer, so
// globals are necessary for this to work
//
// Thank you, Microsoft, for ruining my elegant, non-global requiring code!
//

static Burger::Keyboard *g_pKeyboard;		// Pointer to the needed data

static LRESULT CALLBACK DisableWindowsKeysCallback(int iCode,WPARAM wParam,LPARAM lParam)
{
	// Only handle HC_ACTION events
	const Burger::Keyboard *pThis = g_pKeyboard;

	//
	// Better be valid (Likely caused by a race condition, unlikely
	// but it's better to be safe than sorry)
	//

	if (!pThis) {
		return 1;		// WTF?!?! Return as consumed, because the code is boned
	}

	// Only interested in HC_ACTION events

	if (iCode == HC_ACTION) {
		const KBDLLHOOKSTRUCT *pHookStruct = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
		
		switch (wParam) {
		case WM_KEYDOWN:
		case WM_KEYUP:
			// Only devour if the game app is in the foreground. (Be nice to the system)
			// and allow these keys if the app doesn't have focus
			if (!pThis->GetApp()->IsInBackground() && ((pHookStruct->vkCode == VK_LWIN) || (pHookStruct->vkCode == VK_RWIN))) {
				// Om nom nom the keyboard event
				return 1;
			}
			break;
		// Remaining events to handle
		//case WM_SYSKEYDOWN:
		//case WM_SYSKEYUP:
		default:
			break;
		}
	}
	
	// Pass the event on to the system function or any other hooks
	// installed by other apps/widgets
	return CallNextHookEx(pThis->GetWindowsPreviousKeyboardHook(),iCode,wParam,lParam);
}


/*! ************************************

	\brief Windows thread for monitoring keyboard events (Windows only)

	Burgerlib reads keyboard events using DirectInput and
	asynchronous thread events.

	This function runs as a separate thread and processes
	all DirectInput events.

	\note This is used by the keyboard manager, it's not intended
	to be used by applications.

	\param pData Pointer to the Keyboard class instance
	\return Zero when asked to shut down

***************************************/

#if defined(ENABLE_DIRECTINPUT) || defined(DOXYGEN)
WordPtr BURGER_API Burger::Keyboard::WindowsKeyboardThread(void *pData)
{
	// Get the pointer to the class instance
	Keyboard *pThis = static_cast<Keyboard *>(pData);
	DWORD uEventCode;
	for (;;) {
		// Sleep until an event occurred from DirectInput (Or shutdown)
		uEventCode = WaitForMultipleObjects(2,&pThis->m_pKeyboardEvent,FALSE,INFINITE);
		// Was the quit flag set for shutdown?
		if (pThis->m_bQuit) {
			break;
		}
		if (uEventCode==WAIT_OBJECT_0) {
			// Buffer to receive the keyboard events
			DIDEVICEOBJECTDATA KeyboardData[DIRECTINPUT_KEYBOARDBUFFERSIZE];
			// Maximum number of entries to read!
			DWORD uCount = DIRECTINPUT_KEYBOARDBUFFERSIZE;
			IDirectInputDevice8W *pKeyboardDevice = pThis->m_pKeyboardDevice;

			// Let's get the data from the keyboard device
			HRESULT hResult = pKeyboardDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),KeyboardData,&uCount,0);
			if (hResult<0) {
				// Try getting the keyboard again
				if (hResult == DIERR_INPUTLOST) {
					hResult = pKeyboardDevice->Acquire();
					if (hResult>=0) {
						// The keyboard was reacquired, pull the data
						uCount = DIRECTINPUT_KEYBOARDBUFFERSIZE;
						hResult = pKeyboardDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),KeyboardData,&uCount,0);
					} else {
						pThis->m_bDirectInput8Acquired = FALSE;
					}
				} else {
					pThis->m_bDirectInput8Acquired = FALSE;
				}
			}

			// Was there any data read?
			if (hResult>=0) {
				Word i = uCount;
				if (i) {
					if (pThis->m_bRepeatActive) {
						CancelWaitableTimer(pThis->m_pKeyboardTimerEvent);
						pThis->m_bRepeatActive = FALSE;
					}
					Word bEnableTimer = FALSE;
					const DIDEVICEOBJECTDATA *pObject = KeyboardData;
					KeyEvent_t NewEvent;
					do {
						if (!pThis->EncodeWindowsScanCode(&NewEvent,pObject->dwOfs)) {
							// Overwrite the time stamp
							NewEvent.m_uMSTimeStamp = Tick::ReadMilliseconds();
							bEnableTimer = (pObject->dwData&0x80)>>7;
							if (bEnableTimer) {
								NewEvent.m_uEvent = EVENT_KEYDOWN;
							} else {
								NewEvent.m_uEvent = EVENT_KEYUP;
							}
							NewEvent.m_uWhich = 0;
							pThis->PostKeyEvent(&NewEvent);
						}
						++pObject;
					} while (--i);
					// Last key was a keypress of an ascii code
					if (bEnableTimer && NewEvent.m_uAscii) {
						// Start the auto-repeat timer
						LARGE_INTEGER Time;
						Time.QuadPart = static_cast<Int64>(pThis->m_uInitialDelay)*-10000LL;
						SetWaitableTimer(pThis->m_pKeyboardTimerEvent,&Time,0,NULL,NULL,0);
						pThis->m_bRepeatActive = TRUE;
					}
				}
			}

		// Keyboard repeat event
		} else if (uEventCode == (WAIT_OBJECT_0+1)) {
			// Lock the data and update it
			KeyEvent_t NewEvent;
			NewEvent.m_uEvent = EVENT_KEYAUTO;
			NewEvent.m_uWhich = pThis->m_RepeatEvent.m_uWhich;
			NewEvent.m_uAscii = pThis->m_RepeatEvent.m_uAscii;
			NewEvent.m_uFlags = pThis->m_RepeatEvent.m_uFlags;
			NewEvent.m_uScanCode = pThis->m_RepeatEvent.m_uScanCode;
			NewEvent.m_uMSTimeStamp = Tick::ReadMilliseconds();
			pThis->PostKeyEvent(&NewEvent);

			// Set the delay to the next key press
			LARGE_INTEGER Time;
			Time.QuadPart = static_cast<Int64>(pThis->m_uRepeatDelay)*-10000LL;
			SetWaitableTimer(pThis->m_pKeyboardTimerEvent,&Time,0,NULL,NULL,0);
			pThis->m_bRepeatActive = TRUE;
		}
	}
	// Exit normally
	return 0;
}
#endif

/***************************************

	Initialize DirectInput for an event driven keyboard

***************************************/

Burger::Keyboard::Keyboard(GameApp *pAppInstance) :
	m_pAppInstance(pAppInstance),
	m_pKeyboardDevice(NULL),
	m_pKeyboardEvent(NULL),
	m_pKeyboardTimerEvent(NULL),
	m_pPreviousKeyboardHook(NULL),
	m_KeyboardThread(),
	m_KeyboardLock(),
	m_bDirectInput8Acquired(FALSE),
	m_bRepeatActive(FALSE),
	m_bQuit(FALSE),
	m_uArrayStart(0),
	m_uArrayEnd(0),
	m_uInitialDelay(250),
	m_uRepeatDelay(33)
{
	// Clear my variables
	MemoryClear(m_KeyArray,sizeof(m_KeyArray));
	MemoryClear(&m_RepeatEvent,sizeof(m_RepeatEvent));

	// Safety switch to verify the declaration in brwindowstypes.h matches the real thing
	BURGER_COMPILE_TIME_ASSERT(sizeof(::tagSTICKYKEYS)==sizeof(Burger::tagSTICKYKEYS));
	BURGER_COMPILE_TIME_ASSERT(sizeof(::tagTOGGLEKEYS)==sizeof(Burger::tagTOGGLEKEYS));
	BURGER_COMPILE_TIME_ASSERT(sizeof(::tagFILTERKEYS)==sizeof(Burger::tagFILTERKEYS));

	// Save the current sticky/toggle/filter key settings so they can be restored them later
	m_DefaultStickyKeys.cbSize = sizeof(m_DefaultStickyKeys);
	m_DefaultToggleKeys.cbSize = sizeof(m_DefaultToggleKeys);
	m_DefaultFilterKeys.cbSize = sizeof(m_DefaultFilterKeys);
	SystemParametersInfoW(SPI_GETSTICKYKEYS,sizeof(m_DefaultStickyKeys),&m_DefaultStickyKeys,0);
	SystemParametersInfoW(SPI_GETTOGGLEKEYS,sizeof(m_DefaultToggleKeys),&m_DefaultToggleKeys,0);
	SystemParametersInfoW(SPI_GETFILTERKEYS,sizeof(m_DefaultFilterKeys),&m_DefaultFilterKeys,0);

	// Read from windows the current keyboard delays (And capture CapsLock and NumLock)
	ReadSystemKeyboardDelays();

	// Disable the windows key while the game is running
	DisableWindowsKey();

	// Disable the windows shortcut keys
	DisableAccessibilityShortcutKeys();

	// Next step, obtain DirectInput

#if defined(ENABLE_DIRECTINPUT)
	IDirectInput8W* pDirectInput8W = Globals::GetDirectInput8Singleton();
	if (pDirectInput8W) {

		// Create a system keyboard device (Merges all keyboards)

		IDirectInputDevice8W *pKeyboardDeviceLocal = NULL;
		HRESULT hResult = pDirectInput8W->CreateDevice(GUID_SysKeyboard,&pKeyboardDeviceLocal,NULL);
		if (hResult>=0) {
			// Set the default data format of 256 "buttons"
			IDirectInputDevice8W *pKeyboardDevice = pKeyboardDeviceLocal;
			m_pKeyboardDevice = pKeyboardDevice;
			hResult = pKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
			if (hResult>=0) {

				// Play nice with the system and let others use the keyboard
				// Disable the windows key if the application is running in the foreground

				hResult = pKeyboardDevice->SetCooperativeLevel(pAppInstance->GetWindow(),DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
				if (hResult>=0) {
					DIPROPDWORD KeyboardProperties;
					
					KeyboardProperties.diph.dwSize = sizeof(DIPROPDWORD);
					KeyboardProperties.diph.dwHeaderSize = sizeof(DIPROPHEADER);
					KeyboardProperties.diph.dwObj = 0;
					KeyboardProperties.diph.dwHow = DIPH_DEVICE;
					KeyboardProperties.dwData = DIRECTINPUT_KEYBOARDBUFFERSIZE;
					hResult = pKeyboardDevice->SetProperty(DIPROP_BUFFERSIZE,&KeyboardProperties.diph);
					if (hResult>=0) {
						// Create an event for thread callbacks
						m_pKeyboardEvent = CreateEventW(NULL,FALSE,FALSE,NULL);
						m_pKeyboardTimerEvent = CreateWaitableTimerW(NULL,TRUE,NULL);
						hResult = pKeyboardDevice->SetEventNotification(m_pKeyboardEvent);
						m_KeyboardThread.Start(WindowsKeyboardThread,this);

						// Acquire Direct Input only if the application is full screen
						if (pAppInstance->IsAppFullScreen()) {
							AcquireDirectInput();
						}
					}
				}
			}
			if (hResult<0) {
				// Uh oh...
				pKeyboardDevice->Unacquire();
				m_bDirectInput8Acquired = FALSE;
				pKeyboardDevice->SetEventNotification(NULL);
				if (m_pKeyboardEvent) {
					// Send the quit event
					m_bQuit = TRUE;
					SetEvent(m_pKeyboardEvent);
					// Wait for the thread to shut down
					m_KeyboardThread.Wait();
					CloseHandle(m_pKeyboardEvent);
					m_pKeyboardEvent = NULL;
				}
				if (m_pKeyboardTimerEvent) {
					CloseHandle(m_pKeyboardTimerEvent);
					m_pKeyboardTimerEvent = NULL;
				}
				// Release Direct Input
				pKeyboardDevice->Release();
				m_pKeyboardDevice = NULL;
			}
		}
	}
#endif

}

/***************************************

	Release DirectInput for an event driven keyboard

***************************************/

Burger::Keyboard::~Keyboard()
{

#if defined(ENABLE_DIRECTINPUT)
	// Was a device allocated?
	IDirectInputDevice8W *pKeyboardDevice = m_pKeyboardDevice;
	if (pKeyboardDevice) {
		// Release it!
		pKeyboardDevice->Unacquire();
		m_bDirectInput8Acquired = FALSE;
		// Turn off asynchronous events
		pKeyboardDevice->SetEventNotification(NULL);
		m_bQuit = TRUE;
		// Send the quit event
		SetEvent(m_pKeyboardEvent);
		// Wait for the thread to shut down
		m_KeyboardThread.Wait();
		// Release the events
		CloseHandle(m_pKeyboardEvent);
		m_pKeyboardEvent = NULL;
		CloseHandle(m_pKeyboardTimerEvent);
		m_pKeyboardTimerEvent = NULL;
		m_bRepeatActive = FALSE;
		pKeyboardDevice->Release();
		m_pKeyboardDevice = NULL;
	}
#endif

	// Remove the keyboard intercept hook
	EnableWindowsKey();

	// Restore remote accessibility
	RestoreAccessibilityShortcutKeys();
}


/*! ************************************

	\brief Install keyboard intercept function disabling Windows Key

	Installs a keyboard hook that disables the windows key.

	\note If \ref Globals::TRACE_ACTIVEDEBUGGING is set, this feature is
	disabled since it causes Visual Studio 2010's debugger to wait 5
	seconds per keystroke

	\windowsonly
	\return Zero if no error, Windows error code on failure
	\sa EnableWindowsKey(void)

***************************************/

Word BURGER_API Burger::Keyboard::DisableWindowsKey(void)
{
	// Assume success
	Word uResult = 0;
	// Do nothing if already installed
	if (!m_pPreviousKeyboardHook) {
		if (!(Globals::GetTraceFlag()&Globals::TRACE_ACTIVEDEBUGGING)) {
			// Install the function
			HHOOK pResult = SetWindowsHookExW(WH_KEYBOARD_LL,DisableWindowsKeysCallback,Globals::GetInstance(),0);
			if (!pResult) {
				// Uh, oh. Get the error code
				uResult = GetLastError();
			} else {
				// Store the previous hook
				m_pPreviousKeyboardHook = pResult;

				// Set a global to the this pointer because
				// there is no mechanism to pass a local pointer
				// to the WindowsHook chain.

				// Curse you, Microsoft.

				g_pKeyboard = this;
			}
		}
	}
	return uResult;
}

/*! ************************************

	\brief Remove keyboard intercept function disabling Windows Key

	If DisableWindowsKey(void) was called, it installed a keyboard hook
	that disabled the windows key. This function removes that
	intercept.

	\windowsonly
	\sa DisableWindowsKey(void)

***************************************/

void BURGER_API Burger::Keyboard::EnableWindowsKey(void)
{
	if (m_pPreviousKeyboardHook) {
		UnhookWindowsHookEx(m_pPreviousKeyboardHook);
		m_pPreviousKeyboardHook = NULL;
	}
}

/*! ************************************

	\fn HHOOK__ *Burger::Keyboard::GetWindowsPreviousKeyboardHook(void) const
	\brief Return the Windows default Keyboard Hook function

	If a keyboard hook was installed via a call to DisableWindowsKey(),
	this pointer is valid and points to the function in Windows to
	be passed to the function CallNextHookEx()

	\windowsonly
	\return Pointer to the connected previous Windows Keyboard Hook

***************************************/

/*! ************************************

	\fn Word Burger::Keyboard::IsDirectInputActive(void) const
	\brief Is DirectInput active?

	Returns \ref TRUE is keyboard input is managed by DirectInput. This
	mode is only active if the game has focus and is full screen.

	\windowsonly
	\return \ref TRUE if DirectInput is active, \ref FALSE if not

***************************************/

 
/*! ************************************

	\brief Post a windows scan code key event

	Given a Windows keyboard scan code, convert it to a
	Burgerlib key press and post the event

	\windowsonly
	\param uEvent Type of key event to post
	\param uScanCode Windows keyboard scan code
	\return Zero if posted successfully, non-zero if not.
	\sa PostKeyEvent(const KeyEvent_t *)

***************************************/

Word BURGER_API Burger::Keyboard::PostWindowsKeyEvent(eEvent uEvent,Word32 uScanCode)
{
	Word bResult = FALSE;
	if (!m_bDirectInput8Acquired) {
		KeyEvent_t NewEvent;
		bResult = EncodeWindowsScanCode(&NewEvent,uScanCode);
		if (!bResult) {
			NewEvent.m_uEvent = uEvent;
			NewEvent.m_uWhich = 0;
			bResult = PostKeyEvent(&NewEvent);
		}
	}
	return bResult;
}

/*! ************************************

	\brief Convert a windows a scan code to a KeyEvent_t (Windows only)

	Convert a windows scan code to a Keyboard event

	\windowsonly
	\return Zero if successful, non-zero if a scan code is unknown.

***************************************/

Word BURGER_API Burger::Keyboard::EncodeWindowsScanCode(KeyEvent_t *pEvent,Word uWindowsCode) const
{
	// Look up the scan code
	Word bResult = 1;		// Assume error
	if (uWindowsCode<BURGER_ARRAYSIZE(g_WindowsToKeyboardeScanCode)) {
		bResult = EncodeScanCode(pEvent,g_WindowsToKeyboardeScanCode[uWindowsCode]);
	}
	return bResult;
}

/*! ************************************

	\brief Acquire DirectInput (Windows only)

	Call Acquire() on the DirectInput mouse device.

	\windowsonly
	\sa UnacquireDirectInput(void)

***************************************/

void BURGER_API Burger::Keyboard::AcquireDirectInput(void)
{
#if defined(ENABLE_DIRECTINPUT)
	IDirectInputDevice8W *pKeyboardDevice = m_pKeyboardDevice;
	if (pKeyboardDevice) {
		HRESULT hResult = pKeyboardDevice->Acquire();
		if (hResult>=0) {
			m_bDirectInput8Acquired = TRUE;
		}
	}
#endif
}

/*! ************************************

	\brief Release DirectInput (Windows only)

	Call Unacquire() on the DirectInput mouse device.

	\windowsonly
	\sa AcquireDirectInput(void)

***************************************/

void BURGER_API Burger::Keyboard::UnacquireDirectInput(void)
{
	// Release DirectInput
#if defined(ENABLE_DIRECTINPUT)
	IDirectInputDevice8W *pKeyboardDevice = m_pKeyboardDevice;
	if (pKeyboardDevice) {

		// If there is a keyboard held event in progress, cancel it.
		CancelWaitableTimer(m_pKeyboardTimerEvent);
		m_bRepeatActive = FALSE;

		m_bDirectInput8Acquired = FALSE;
		pKeyboardDevice->Unacquire();
	}
#endif
}

/*! ************************************

	\brief Read System keyboard delay constants (Windows only)

	On startup and when a WM_SETTINGCHANGE event is triggered,
	read the settings for the keyboard auto-repeat from
	Windows and record it so auto-repeat is as the user requested it.

	\windowsonly

***************************************/

void BURGER_API Burger::Keyboard::ReadSystemKeyboardDelays(void)
{
	// Get the initial keyboard delay to mimic the user's desired response
	// This is set in the Keyboard windows control panel

	DWORD uValue;
	if (SystemParametersInfo(SPI_GETKEYBOARDDELAY,0,&uValue,0)) {
		// Save to register
		Word uTemp = uValue;
		if (uTemp>=3) {		// Valid values are 0-3
			uTemp = 3;		// Max at 3
		}
		m_uInitialDelay = (uTemp*250)+250;	// 0 = 250ms, 3 = 1000ms
	}

	// Get the repeat speed to mimic the user's desired speed
	// This is set in the Keyboard windows control panel
	if (SystemParametersInfo(SPI_GETKEYBOARDSPEED,0,&uValue,0)) {
		// Save to register
		Word uTemp2 = uValue;
		if (uTemp2>=31) {		// Valid values are 0-31
			uTemp2 = 31;		// Max at 31
		}
		m_uRepeatDelay = ((31-uTemp2)*12)+33;	// 31 = 33ms, 0 = 405ms
	}

	//
	// Get the state of the caps lock key
	//

	// Mask with 0x01 for the toggle state
	if (GetKeyState(VK_CAPITAL)&0x1) {
		m_KeyArray[SC_CAPSLOCK] |= static_cast<Word8>(KEYCAPTOGGLE);
	} else {
		m_KeyArray[SC_CAPSLOCK] &= static_cast<Word8>(~KEYCAPTOGGLE);
	}

	//
	// Get the state of the num lock key
	//

	if (GetKeyState(VK_NUMLOCK)&0x1) {
		m_KeyArray[SC_NUMLOCK] |= static_cast<Word8>(KEYCAPTOGGLE);
	} else {
		m_KeyArray[SC_NUMLOCK] &= static_cast<Word8>(~KEYCAPTOGGLE);
	}
}

/*! ************************************

	\brief Disable the accessibility shortcut keys (Windows only)

	There are some keyboard modes that are intended for people with
	typing problems. If any of these features were not enabled
	when the application started, disable the ability to turn them
	on by using keyboard shortcuts. It helps prevent a player using
	the keyboard from accidentally enabling this feature while mashing
	buttons like crazy.

	\windowsonly
	\sa RestoreAccessibilityShortcutKeys(void)

***************************************/

void BURGER_API Burger::Keyboard::DisableAccessibilityShortcutKeys(void) const
{
	// Disable StickyKeys/etc shortcuts but if the accessibility feature is on, 
	// then leave the settings alone as its probably being usefully used

	if (!(m_DefaultStickyKeys.dwFlags & SKF_STICKYKEYSON)) {
		// Disable the hotkey and the confirmation
		STICKYKEYS TempSticky;
		TempSticky.cbSize = sizeof(TempSticky);
		TempSticky.dwFlags = m_DefaultStickyKeys.dwFlags & (~(SKF_HOTKEYACTIVE|SKF_CONFIRMHOTKEY));
		SystemParametersInfoW(SPI_SETSTICKYKEYS,sizeof(TempSticky),&TempSticky,0);
	}

	if (!(m_DefaultToggleKeys.dwFlags & TKF_TOGGLEKEYSON)) {
		// Disable the hotkey and the confirmation
		TOGGLEKEYS TempToggle;
		TempToggle.cbSize = sizeof(TempToggle);
		TempToggle.dwFlags = m_DefaultToggleKeys.dwFlags & (~(TKF_HOTKEYACTIVE|TKF_CONFIRMHOTKEY));
		SystemParametersInfoW(SPI_SETTOGGLEKEYS,sizeof(TempToggle),&TempToggle,0);
	}

	if (!(m_DefaultFilterKeys.dwFlags & FKF_FILTERKEYSON)) {
		// Disable the hotkey and the confirmation
		FILTERKEYS TempFilter;
		TempFilter.cbSize = sizeof(TempFilter);
		TempFilter.dwFlags = m_DefaultFilterKeys.dwFlags & (~(FKF_HOTKEYACTIVE|FKF_CONFIRMHOTKEY));
		TempFilter.iWaitMSec = m_DefaultFilterKeys.iWaitMSec;
		TempFilter.iDelayMSec = m_DefaultFilterKeys.iDelayMSec;
		TempFilter.iRepeatMSec = m_DefaultFilterKeys.iRepeatMSec;
		TempFilter.iBounceMSec = m_DefaultFilterKeys.iBounceMSec;
		SystemParametersInfoW(SPI_SETFILTERKEYS,sizeof(TempFilter),&TempFilter,0);
	}
}

/*! ************************************

	\brief Restore the accessibility shortcut keys (Windows only)

	Restore the accessibility shortcut keys to the settings
	that were captured upon instantiation of the Keyboard class

	\windowsonly
	\sa DisableAccessibilityShortcutKeys(void)

***************************************/

void BURGER_API Burger::Keyboard::RestoreAccessibilityShortcutKeys(void)
{
	SystemParametersInfoW(SPI_SETSTICKYKEYS,sizeof(m_DefaultStickyKeys),&m_DefaultStickyKeys,0);
	SystemParametersInfoW(SPI_SETTOGGLEKEYS,sizeof(m_DefaultToggleKeys),&m_DefaultToggleKeys,0);
	SystemParametersInfoW(SPI_SETFILTERKEYS,sizeof(m_DefaultFilterKeys),&m_DefaultFilterKeys,0);
}

#endif


