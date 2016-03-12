/***************************************

	Keyboard Manager

	MacOSX version

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brkeyboard.h"

#if defined(BURGER_MACOSX) || defined(DOXYGEN)

#if !defined(DOXYGEN)
#include "brglobals.h"
#include "brtick.h"
#include "brstringfunctions.h"
#import <AppKit/NSEvent.h>
#import <Carbon/Carbon.h>

#define LEFTCONTROLMASK 0x01
#define LEFTSHIFTMASK 0x02
#define RIGHTSHIFTMASK 0x04
#define LEFTALTMASK 0x08
#define RIGHTALTMASK 0x10
#define LEFTOPTIONMASK 0x20
#define RIGHTOPTIONMASK 0x40
#define RIGHTCONTROLMASK 0x2000
#define CAPSLOCKMASK 0x010000
#define FUNCTIONMASK 0x800000

//
// Table, first entry is the MacOSX code, the second is the code for Burgerlib
//

static const Burger::Keyboard::eScanCode g_MacOSXToKeyboardeScanCode[0xF0] = {
	Burger::Keyboard::SC_A,				// 0
	Burger::Keyboard::SC_S,				// 1
	Burger::Keyboard::SC_D,				// 2
	Burger::Keyboard::SC_F,				// 3
	Burger::Keyboard::SC_H,				// 4
	Burger::Keyboard::SC_G,				// 5
	Burger::Keyboard::SC_Z,				// 6
	Burger::Keyboard::SC_X,				// 7
	Burger::Keyboard::SC_C,				// 8
	Burger::Keyboard::SC_V,				// 9
	Burger::Keyboard::SC_GRAVE,			// 10
	Burger::Keyboard::SC_B,				// 11
	Burger::Keyboard::SC_Q,				// 12
	Burger::Keyboard::SC_W,				// 13
	Burger::Keyboard::SC_E,				// 14
	Burger::Keyboard::SC_R,				// 15
	Burger::Keyboard::SC_Y,				// 16
	Burger::Keyboard::SC_T,				// 17
	Burger::Keyboard::SC_1,				// 18
	Burger::Keyboard::SC_2,				// 19
	Burger::Keyboard::SC_3,				// 20
	Burger::Keyboard::SC_4,				// 21
	Burger::Keyboard::SC_6,				// 22
	Burger::Keyboard::SC_5,				// 23
	Burger::Keyboard::SC_EQUALS,		// 24
	Burger::Keyboard::SC_9,				// 25
	Burger::Keyboard::SC_7,				// 26
	Burger::Keyboard::SC_MINUS,			// 27
	Burger::Keyboard::SC_8,				// 28
	Burger::Keyboard::SC_0,				// 29
	Burger::Keyboard::SC_RIGHTBRACKET,	// 30
	Burger::Keyboard::SC_O,				// 31
	Burger::Keyboard::SC_U,				// 32
	Burger::Keyboard::SC_LEFTBRACKET,	// 33
	Burger::Keyboard::SC_I,				// 34
	Burger::Keyboard::SC_P,				// 35
	Burger::Keyboard::SC_RETURN,		// 36
	Burger::Keyboard::SC_L,				// 37
	Burger::Keyboard::SC_J,				// 38
	Burger::Keyboard::SC_QUOTE,			// 39
	Burger::Keyboard::SC_K,				// 40
	Burger::Keyboard::SC_SEMICOLON,		// 41
	Burger::Keyboard::SC_BACKSLASH,		// 42
	Burger::Keyboard::SC_COMMA,			// 43
	Burger::Keyboard::SC_SLASH,			// 44
	Burger::Keyboard::SC_N,				// 45
	Burger::Keyboard::SC_M,				// 46
	Burger::Keyboard::SC_PERIOD,		// 47
	Burger::Keyboard::SC_TAB,			// 48
	Burger::Keyboard::SC_SPACE,			// 49
	Burger::Keyboard::SC_GRAVE,			// 50
	Burger::Keyboard::SC_BACKSPACE,		// 51
	Burger::Keyboard::SC_KEYPADENTER,	// 52 (On portables)
	Burger::Keyboard::SC_ESCAPE,		// 53
	Burger::Keyboard::SC_RIGHTOPENAPPLE,	// 54
	Burger::Keyboard::SC_LEFTOPENAPPLE,	// 55
	Burger::Keyboard::SC_LEFTSHIFT,		// 56
	Burger::Keyboard::SC_CAPSLOCK,		// 57
	Burger::Keyboard::SC_LEFTOPTION,	// 58
	Burger::Keyboard::SC_LEFTCONTROL,	// 59
	Burger::Keyboard::SC_RIGHTSHIFT,	// 60
	Burger::Keyboard::SC_RIGHTOPTION,	// 61
	Burger::Keyboard::SC_RIGHTCONTROL,	// 62
	Burger::Keyboard::SC_CONVERT,		// 63 fn on portables
	Burger::Keyboard::SC_INVALID,		// 64 F17?
	Burger::Keyboard::SC_KEYPADPERIOD,	// 65
	Burger::Keyboard::SC_INVALID,		// 66
	Burger::Keyboard::SC_KEYPADASTERISK,	// 67
	Burger::Keyboard::SC_INVALID,		// 68
	Burger::Keyboard::SC_KEYPADPLUS,	// 69
	Burger::Keyboard::SC_INVALID,		// 70
	Burger::Keyboard::SC_NUMLOCK,		// 71
	Burger::Keyboard::SC_INVALID,		// 72
	Burger::Keyboard::SC_INVALID,		// 73
	Burger::Keyboard::SC_INVALID,		// 74
	Burger::Keyboard::SC_KEYPADSLASH,	// 75
	Burger::Keyboard::SC_KEYPADENTER,	// 76
	Burger::Keyboard::SC_INVALID,		// 77
	Burger::Keyboard::SC_KEYPADMINUS,	// 78
	Burger::Keyboard::SC_INVALID,		// 79 F18?
	Burger::Keyboard::SC_INVALID,		// 80 F19?
	Burger::Keyboard::SC_KEYPADEQUALS,	// 81
	Burger::Keyboard::SC_KEYPAD0,		// 82
	Burger::Keyboard::SC_KEYPAD1,		// 83
	Burger::Keyboard::SC_KEYPAD2,		// 84
	Burger::Keyboard::SC_KEYPAD3,		// 85
	Burger::Keyboard::SC_KEYPAD4,		// 86
	Burger::Keyboard::SC_KEYPAD5,		// 87
	Burger::Keyboard::SC_KEYPAD6,		// 88
	Burger::Keyboard::SC_KEYPAD7,		// 89
	Burger::Keyboard::SC_INVALID,		// 90
	Burger::Keyboard::SC_KEYPAD8,		// 91
	Burger::Keyboard::SC_KEYPAD9,		// 92
	Burger::Keyboard::SC_YEN,			// 93
	Burger::Keyboard::SC_INVALID,		// 94
	Burger::Keyboard::SC_INVALID,		// 95
	Burger::Keyboard::SC_F5,			// 96
	Burger::Keyboard::SC_F6,			// 97
	Burger::Keyboard::SC_F7,			// 98
	Burger::Keyboard::SC_F3,			// 99
	Burger::Keyboard::SC_F8,			// 100
	Burger::Keyboard::SC_F9,			// 101
	Burger::Keyboard::SC_KANA,			// 102
	Burger::Keyboard::SC_F11,			// 103
	Burger::Keyboard::SC_INVALID,		// 104
	Burger::Keyboard::SC_PRINTSCREEN,	// 105
	Burger::Keyboard::SC_INVALID,		// 106
	Burger::Keyboard::SC_F14,			// 107
	Burger::Keyboard::SC_INVALID,		// 108
	Burger::Keyboard::SC_F10,			// 109
	Burger::Keyboard::SC_RIGHTOPENAPPLE,	// 110
	Burger::Keyboard::SC_F12,			// 111
	Burger::Keyboard::SC_INVALID,		// 112
	Burger::Keyboard::SC_PAUSE,			// 113
	Burger::Keyboard::SC_INSERT,		// 114
	Burger::Keyboard::SC_HOME,			// 115
	Burger::Keyboard::SC_PAGEUP,		// 116
	Burger::Keyboard::SC_DELETE,		// 117
	Burger::Keyboard::SC_F4,			// 118
	Burger::Keyboard::SC_END,			// 119
	Burger::Keyboard::SC_F2,			// 120
	Burger::Keyboard::SC_PAGEDOWN,		// 121
	Burger::Keyboard::SC_F1,			// 122
	Burger::Keyboard::SC_LEFTARROW,		// 123
	Burger::Keyboard::SC_RIGHTARROW,	// 124
	Burger::Keyboard::SC_DOWNARROW,		// 125
	Burger::Keyboard::SC_UPARROW,		// 126
	Burger::Keyboard::SC_POWER			// 127
};

#endif

/***************************************

	Initialize DirectInput for an event driven keyboard

***************************************/

Burger::Keyboard::Keyboard(GameApp *pAppInstance) :
	m_pAppInstance(pAppInstance),
	m_uArrayStart(0),
	m_uArrayEnd(0),
	m_uInitialDelay(250),
	m_uRepeatDelay(33)
{
	pAppInstance->SetKeyboard(this);
	// Clear my variables
	MemoryClear(const_cast<Word8 *>(m_KeyArray),sizeof(m_KeyArray));
}

/***************************************

	Release DirectInput for an event driven keyboard

***************************************/

Burger::Keyboard::~Keyboard()
{
	m_pAppInstance->SetKeyboard(NULL);
}

/***************************************

	\brief Peek at the next keyboard press event

	See if a key is pending from the keyboard, if
	so, return the event without removing it from
	the queue.

	\sa Keyboard::Get()

***************************************/

Word BURGER_API Burger::Keyboard::PeekKeyEvent(KeyEvent_t *pEvent)
{
	m_KeyboardLock.Lock();
	Word uIndex = m_uArrayStart;		/* Get the starting index */
	Word uResult = 0;
	if (uIndex!=m_uArrayEnd) {	/* Anything in the buffer? */
		pEvent[0] = m_KeyEvents[uIndex];
		uResult = 1;
	}
	// No event pending
	m_KeyboardLock.Unlock();
	return uResult;
}


/***************************************

	\brief Return key up and down events

	Get a key from the keyboard buffer but include key up events

***************************************/

Word BURGER_API Burger::Keyboard::GetKeyEvent(KeyEvent_t *pEvent)

{
	m_pAppInstance->Poll();
	Word uResult = 0;
	m_KeyboardLock.Lock();
	Word uIndex = m_uArrayStart;		/* Get the starting index */
	if (uIndex!=m_uArrayEnd) {	/* Anything in the buffer? */
		pEvent[0] = m_KeyEvents[uIndex];
		m_uArrayStart = (uIndex+1)&(cBufferSize-1);	/* Next key */
		uResult = 1;
	}
	m_KeyboardLock.Unlock();

	if (uResult && m_pAppInstance->IsWindowSwitchingAllowed()) {
		if ((pEvent->m_uAscii==ASCII_RETURN) &&
			((pEvent->m_uFlags&(FLAG_OPTION|FLAG_KEYDOWN))==(FLAG_OPTION|FLAG_KEYDOWN))) {
			m_pAppInstance->SetWindowSwitchRequested(TRUE);
		}
	}
	// No event pending
	return uResult;
}

/***************************************

	Post the keyboard event

***************************************/

Word BURGER_API Burger::Keyboard::PostKeyEvent(const KeyEvent_t *pEvent)
{
	m_KeyboardLock.Lock();
	Word uResult = 10;
	Word uEnd = m_uArrayEnd;
	// See if there's room in the buffer
	Word uTemp = (uEnd+1)&(cBufferSize-1);
	if (uTemp!=m_uArrayStart) {
		// Didn't wrap, accept it!
		m_uArrayEnd = uTemp;
	
		// Insert the new event
		KeyEvent_t *pNewEvent = &m_KeyEvents[uEnd];
		pNewEvent->m_uAscii = pEvent->m_uAscii;
		eScanCode uScanCode = static_cast<eScanCode>(pEvent->m_uScanCode);
		pNewEvent->m_uScanCode = static_cast<Word16>(uScanCode);
		Word uFlags = pEvent->m_uFlags;
		pNewEvent->m_uFlags = static_cast<Word16>(uFlags);

		// Add the proper time stamp
		Word32 uTime = pEvent->m_uMSTimeStamp;
		if (!uTime) {
			uTime = Tick::ReadMilliseconds();
		}
		pNewEvent->m_uMSTimeStamp = uTime;

		// Update the running state
		if (uFlags&FLAG_KEYDOWN) {

			Word8 uKey = m_KeyArray[uScanCode];
			uKey = static_cast<Word8>((uKey|(KEYCAPDOWN|KEYCAPPRESSED))^KEYCAPTOGGLE);
			m_KeyArray[uScanCode] = uKey;

			if (!(pNewEvent->m_uFlags&FLAG_REPEAT)) {
				m_RepeatEvent.m_uAscii = pNewEvent->m_uAscii;
				m_RepeatEvent.m_uFlags = pNewEvent->m_uFlags;
				m_RepeatEvent.m_uScanCode = pNewEvent->m_uScanCode;
				m_RepeatEvent.m_uMSTimeStamp = 0;
			}
		} else {
			// Mark as pressed
			m_KeyArray[uScanCode] &= (~KEYCAPDOWN);
		}
		uResult = 0;
	}
	m_KeyboardLock.Unlock();
	return uResult;
}

/*! ************************************
 
	\brief Process a MacOSX keystroke
 
	When an NSEvent is created that is of a type of
	NSKeyDown, NSKeyUp or NSFlagsChanged, this
	function will process the event into a
	Burgerlib compatible keystroke

	\param pEvent Pointer to a NSEvent with a keyboard event.
 
***************************************/

void BURGER_API Burger::Keyboard::ProcessEvent(NSEvent *pEvent)
{
	Word uScanCode = [pEvent keyCode];
	// It seems that Backslash and Grave can be reversed on Japanese keyboards. Fix this
	if ((uScanCode == 10 || uScanCode == 50) && KBGetLayoutType(LMGetKbdType()) == kKeyboardISO) {
		// Swap 10 and 50
		uScanCode = uScanCode^(10^50);
	}
	if (uScanCode < BURGER_ARRAYSIZE(g_MacOSXToKeyboardeScanCode)) {
		eScanCode NewCode = g_MacOSXToKeyboardeScanCode[uScanCode];
		switch ([pEvent type]) {
		case NSKeyDown:
			PostKeyDown(NewCode);
			break;
		case NSKeyUp:
			PostKeyUp(NewCode);
			break;
		case NSFlagsChanged:
			{
				Word uFlags = static_cast<Word>([pEvent modifierFlags]);
				Word uMask;
				switch (NewCode) {
				case SC_CAPSLOCK:
					uMask = CAPSLOCKMASK;
					break;
				case SC_RIGHTCONTROL:
					uMask = RIGHTCONTROLMASK;
					break;
				case SC_LEFTCONTROL:
					uMask = LEFTCONTROLMASK;
					break;
				case SC_RIGHTALT:
					uMask = RIGHTALTMASK;
					break;
				case SC_LEFTALT:
					uMask = LEFTALTMASK;
					break;
				case SC_RIGHTSHIFT:
					uMask = RIGHTSHIFTMASK;
					break;
				case SC_LEFTSHIFT:
					uMask = LEFTSHIFTMASK;
					break;
				case SC_LEFTOPTION:
					uMask = LEFTOPTIONMASK;
					break;
				case SC_RIGHTOPTION:
					uMask = RIGHTOPTIONMASK;
					break;
				case SC_CONVERT:
					uMask = FUNCTIONMASK;
					break;
				default:
					uMask = 0;
					break;
				}
				// Was this modifier recognized?
				if (uMask) {
					// Issue the key down or up
					if (uMask&uFlags) {
						PostKeyDown(NewCode);
					} else {
						PostKeyUp(NewCode);
					}
				}
			}
			break;
		default:
			break;
		}
	}
}

#endif


