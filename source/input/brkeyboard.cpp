/***************************************

    Keyboard Manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brkeyboard.h"
#include "brmemoryfunctions.h"
#include "brtick.h"
#include "brutf32.h"

#if defined(BURGER_WINDOWS) && !defined(DOXYGEN)

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

//#define ENABLE_DIRECTINPUT

#include <Windows.h>
#endif

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::Keyboard,Burger::Base);
#endif

/*! ************************************

	\class Burger::Keyboard

	\brief Keyboard input manager.

	This class manages keyboard input down to the individual
	key cap. It's main use is to use the keyboard as a gaming
	input device. Emphasis is on speed, low latency and 
	accuracy.

	When this class is instantiated, a keyboard monitor is activated
	which will check for all key strokes. There are a maximum of 256 keyboard
	scan codes that can be monitored. Each byte will contain the current status of
	the key by scan code. The lowest bit (mask with 0x01) will determine the
	key down status at the current moment in time. Bit #1 (mask with 0x02) is set
	when the key is pressed but never cleared unless you call a function
	that clears the flag or Keyboard::Flush() with flushes all events.

	This array is checked by several routines that use the keyboard as a game
	input device.

	The array is volatile since some keyboard monitors run as a separate thread.

	\sa Burger::Joypad and Burger::Mouse

***************************************/

/*! ************************************

	\enum Burger::Keyboard::eAsciiCode

	\brief Unicode translations for several common keys.

	When these keys are pressed, they return these
	specific Unicode compatible "ASCII" codes.
	
***************************************/

/*! ************************************

	\enum Burger::Keyboard::eScanCode

	\brief Unified scan codes

	All platforms will have their keyboard scan codes mapped
	to these enumerations. It allows an application to save
	a keyboard mapping file on one platform and read it 
	in on another and have the keys map properly.
	
***************************************/

/*! ************************************

	\enum Burger::Keyboard::eKeyFlags

	\brief Keycode modifier flags

	When a keyboard event is generated, a set of flags
	can be attached to denote any type of modification that
	was applied to the keystroke at the moment of press.

	\sa KeyEvent_t
	
***************************************/


/***************************************

	Array to convert scan codes to human readable text
	First entry is a pointer to the text, the second is the
	associated scan code. There can be multiple
	entries from conversion to a scan code, however
	only the first instance that matches a scan code
	will be use for scan code to ASCII conversion.
	
***************************************/

#if !defined(DOXYGEN)

struct ScanEntry_t {
	const char *m_pName;	///< Pointer to key name
	Burger::Keyboard::eScanCode m_uScanCode;		///< Actual scan code
};

static const ScanEntry_t g_ScanCodeNames[] = {
	{"Escape",Burger::Keyboard::SC_ESCAPE},
	{"Esc",Burger::Keyboard::SC_ESCAPE},
	{"0",Burger::Keyboard::SC_0},
	{"1",Burger::Keyboard::SC_1},
	{"2",Burger::Keyboard::SC_2},
	{"3",Burger::Keyboard::SC_3},
	{"4",Burger::Keyboard::SC_4},
	{"5",Burger::Keyboard::SC_5},
	{"6",Burger::Keyboard::SC_6},
	{"7",Burger::Keyboard::SC_7},
	{"8",Burger::Keyboard::SC_8},
	{"9",Burger::Keyboard::SC_9},
	{"A",Burger::Keyboard::SC_A},
	{"B",Burger::Keyboard::SC_B},
	{"C",Burger::Keyboard::SC_C},
	{"D",Burger::Keyboard::SC_D},
	{"E",Burger::Keyboard::SC_E},
	{"F",Burger::Keyboard::SC_F},
	{"G",Burger::Keyboard::SC_G},
	{"H",Burger::Keyboard::SC_H},
	{"I",Burger::Keyboard::SC_I},
	{"J",Burger::Keyboard::SC_J},
	{"K",Burger::Keyboard::SC_K},
	{"L",Burger::Keyboard::SC_L},
	{"M",Burger::Keyboard::SC_M},
	{"N",Burger::Keyboard::SC_N},
	{"O",Burger::Keyboard::SC_O},
	{"P",Burger::Keyboard::SC_P},
	{"Q",Burger::Keyboard::SC_Q},
	{"R",Burger::Keyboard::SC_R},
	{"S",Burger::Keyboard::SC_S},
	{"T",Burger::Keyboard::SC_T},
	{"U",Burger::Keyboard::SC_U},
	{"V",Burger::Keyboard::SC_V},
	{"W",Burger::Keyboard::SC_W},
	{"X",Burger::Keyboard::SC_X},
	{"Y",Burger::Keyboard::SC_Y},
	{"Z",Burger::Keyboard::SC_Z},
	{"F1",Burger::Keyboard::SC_F1},
	{"F2",Burger::Keyboard::SC_F2},
	{"F3",Burger::Keyboard::SC_F3},
	{"F4",Burger::Keyboard::SC_F4},
	{"F5",Burger::Keyboard::SC_F5},
	{"F6",Burger::Keyboard::SC_F6},
	{"F7",Burger::Keyboard::SC_F7},
	{"F8",Burger::Keyboard::SC_F8},
	{"F9",Burger::Keyboard::SC_F9},
	{"F10",Burger::Keyboard::SC_F10},
	{"F11",Burger::Keyboard::SC_F11},
	{"F12",Burger::Keyboard::SC_F12},
	{"-",Burger::Keyboard::SC_MINUS},
	{"=",Burger::Keyboard::SC_PLUS},
	{"BakSpc",Burger::Keyboard::SC_BACKSPACE},
	{"Tab",Burger::Keyboard::SC_TAB},
	{"[",Burger::Keyboard::SC_LEFTBRACE},
	{"]",Burger::Keyboard::SC_RIGHTBRACE},
	{";",Burger::Keyboard::SC_SEMICOLON},
	{"'",Burger::Keyboard::SC_QUOTE},
	{"`",Burger::Keyboard::SC_TILDE},
	{"\\",Burger::Keyboard::SC_BACKSLASH},
	{",",Burger::Keyboard::SC_COMMA},
	{".",Burger::Keyboard::SC_PERIOD},
	{"/",Burger::Keyboard::SC_SLASH},
	{"Enter",Burger::Keyboard::SC_RETURN},
	{"LCtrl",Burger::Keyboard::SC_LEFTCONTROL},
	{"RCtrl",Burger::Keyboard::SC_RIGHTCONTROL},
	{"LShift",Burger::Keyboard::SC_LEFTSHIFT},
	{"RShift",Burger::Keyboard::SC_RIGHTSHIFT},
	{"LAlt",Burger::Keyboard::SC_LEFTALT},
	{"RAlt",Burger::Keyboard::SC_RIGHTALT},
	{"Space",Burger::Keyboard::SC_SPACE},
	{"CapsLock",Burger::Keyboard::SC_CAPSLOCK},
	{"CapLck",Burger::Keyboard::SC_CAPSLOCK},
	{"NumLock",Burger::Keyboard::SC_NUMLOCK},
	{"NumLck",Burger::Keyboard::SC_NUMLOCK},
	{"ScrLck",Burger::Keyboard::SC_SCROLLLOCK},
	{"Pause",Burger::Keyboard::SC_PAUSE},
	{"Up",Burger::Keyboard::SC_UPARROW},
	{"Down",Burger::Keyboard::SC_DOWNARROW},
	{"Left",Burger::Keyboard::SC_LEFTARROW},
	{"Right",Burger::Keyboard::SC_RIGHTARROW},
	{"Insert",Burger::Keyboard::SC_INSERT},
	{"Delete",Burger::Keyboard::SC_DELETE},
	{"Home",Burger::Keyboard::SC_HOME},
	{"End",Burger::Keyboard::SC_END},
	{"PgUp",Burger::Keyboard::SC_PAGEUP},
	{"PgDn",Burger::Keyboard::SC_PAGEDOWN},
	{"PrtScn",Burger::Keyboard::SC_PRINTSCREEN},
	{"KPad0",Burger::Keyboard::SC_KEYPAD0},
	{"KPad1",Burger::Keyboard::SC_KEYPAD1},
	{"KPad2",Burger::Keyboard::SC_KEYPAD2},
	{"KPad3",Burger::Keyboard::SC_KEYPAD3},
	{"KPad4",Burger::Keyboard::SC_KEYPAD4},
	{"KPad5",Burger::Keyboard::SC_KEYPAD5},
	{"KPad6",Burger::Keyboard::SC_KEYPAD6},
	{"KPad7",Burger::Keyboard::SC_KEYPAD7},
	{"KPad8",Burger::Keyboard::SC_KEYPAD8},
	{"KPad9",Burger::Keyboard::SC_KEYPAD9},
	{"KPad*",Burger::Keyboard::SC_KEYPADASTERISK},
	{"KPad-",Burger::Keyboard::SC_KEYPADMINUS},
	{"KPad+",Burger::Keyboard::SC_KEYPADPLUS},
	{"KPad.",Burger::Keyboard::SC_KEYPADPERIOD},
	{"KPad/",Burger::Keyboard::SC_KEYPADSLASH},
	{"KPadEnter",Burger::Keyboard::SC_KEYPADENTER},
	{"KPdEnt",Burger::Keyboard::SC_KEYPADENTER},
	{"KPadEnt",Burger::Keyboard::SC_KEYPADENTER}
};

//
// This array converts scan codes to Unicode
// It's in the same order as the scan code eScanCode
//

struct ScanCodeToAscii_t {
	uint32_t m_uAsciiCode;		// Ascii code
	uint32_t m_uShiftCode;		// Shifted ascii code
	uint32_t m_uControlCode;		// Control ascii code
};

static const ScanCodeToAscii_t g_ScanCodeTranslation[Burger::Keyboard::SC_EXTRA+1] = {
	{0,0,0x00},		// SC_INVALID
	{'a','A',0x01},	// SC_A
	{'b','B',0x02},	// SC_B
	{'c','C',0x03},	// SC_C
	{'d','D',0x04},	// SC_D
	{'e','E',0x05},	// SC_E
	{'f','F',0x06},	// SC_F
	{'g','G',0x07},	// SC_G
	{'h','H',0x08},	// SC_H
	{'i','I',0x09},	// SC_I
	{'j','J',0x0A},	// SC_J
	{'k','K',0x0B},	// SC_K
	{'l','L',0x0C},	// SC_L
	{'m','M',0x0D},	// SC_M
	{'n','N',0x0E},	// SC_N
	{'o','O',0x0F},	// SC_O
	{'p','P',0x10},	// SC_P
	{'q','Q',0x11},	// SC_Q
	{'r','R',0x12},	// SC_R
	{'s','S',0x13},	// SC_S
	{'t','T',0x14},	// SC_T
	{'u','U',0x15},	// SC_U
	{'v','V',0x16},	// SC_V
	{'w','W',0x17},	// SC_W
	{'x','X',0x18},	// SC_X
	{'y','Y',0x19},	// SC_Y
	{'z','Z',0x1A},	// SC_Z
	{0x00DF,0x1E9E,0x00},	// SC_FUSS,
	{'0',')',0x00},		// SC_0
	{'1','!',0x00},		// SC_1
	{'2','@',0x00},		// SC_2
	{'3','#',0x00},		// SC_3
	{'4','$',0x00},		// SC_4
	{'5','%',0x00},		// SC_5
	{'6','^',0x00},		// SC_6
	{'7','&',0x00},		// SC_7
	{'8','*',0x00},		// SC_8
	{'9','(',0x00},		// SC_9
	{'`','~',0x00},		// SC_TILDE
	{'-','_',0x00},		// SC_UNDERSCORE
	{'=','+',0x00},		// SC_PLUS
	{Burger::Keyboard::ASCII_BACKSPACE,Burger::Keyboard::ASCII_BACKSPACE,0x00},	// SC_BACKSPACE
	{'[','{',0x00},		// SC_RIGHTBRACE
	{']','}',0x00},		// SC_LEFTBRACE
	{Burger::Keyboard::ASCII_RETURN,Burger::Keyboard::ASCII_RETURN,0x00},	// SC_RETURN
	{'\'','"',0x00},	// SC_QUOTE
	{';',':',0x00},		// SC_COLON
	{'\\','|',0x00},	// SC_VERTBAR
	{',','<',0x00},		// SC_COMMA
	{'.','>',0x00},		// SC_PERIOD
	{'/','?',0x00},		// SC_SLASH
	{Burger::Keyboard::ASCII_TAB,Burger::Keyboard::ASCII_TAB,0x00},	// SC_TAB
	{Burger::Keyboard::ASCII_SPACE,Burger::Keyboard::ASCII_SPACE,0x00},	// SC_SPACE
	{0x0000,0x0000,0x00},	// SC_POWERBOOKENTER
	{Burger::Keyboard::ASCII_ESCAPE,Burger::Keyboard::ASCII_ESCAPE,0x00},	// SC_ESCAPE
	{0x0000,0x0000,0x00},	// SC_LEFTCONTROL
	{0x0000,0x0000,0x00},	// SC_RIGHTCONTROL
	{0x0000,0x0000,0x00},	// SC_LEFTOPTION
	{0x0000,0x0000,0x00},	// SC_RIGHTOPTION
	{0x0000,0x0000,0x00},	// SC_LEFTALT
	{0x0000,0x0000,0x00},	// SC_RIGHTALT
	{0x0000,0x0000,0x00},	// SC_LEFTSHIFT
	{0x0000,0x0000,0x00},	// SC_RIGHTSHIFT
	{0x0000,0x0000,0x00},	// SC_LEFTMENU
	{0x0000,0x0000,0x00},	// SC_RIGHTMENU
	{0x0000,0x0000,0x00},	// SC_CAPSLOCK
	{'.','.',0x00},			// SC_KEYPADPERIOD
	{',',',',0x00},			// SC_KEYPADCOMMA
	{'*','*',0x00},			// SC_KEYPADASTERISK
	{'+','+',0x00},			// SC_KEYPADPLUS,
	{0x0000,0x0000,0x00},	// SC_NUMLOCK
	{'/','/',0x00},			// SC_KEYPADSLASH
	{Burger::Keyboard::ASCII_ENTER,Burger::Keyboard::ASCII_ENTER,0x00},	// SC_KEYPADENTER
	{'-','-',0x00},			// SC_KEYPADMINUS
	{'=','=',0x00},			// SC_KEYPADEQUALS
	{Burger::Keyboard::ASCII_INSERT,'0',0x00},		// SC_KEYPAD0
	{Burger::Keyboard::ASCII_END,'1',0x00},			// SC_KEYPAD1
	{Burger::Keyboard::ASCII_DOWNARROW,'2',0x00},	// SC_KEYPAD2
	{Burger::Keyboard::ASCII_PAGEDOWN,'3',0x00},	// SC_KEYPAD3
	{Burger::Keyboard::ASCII_LEFTARROW,'4',0x00},	// SC_KEYPAD4
	{'5','5',0x00},									// SC_KEYPAD5
	{Burger::Keyboard::ASCII_RIGHTARROW,'6',0x00},	// SC_KEYPAD6
	{Burger::Keyboard::ASCII_HOME,'7',0x00},		// SC_KEYPAD7
	{Burger::Keyboard::ASCII_UPARROW,'8',0x00},		// SC_KEYPAD8
	{Burger::Keyboard::ASCII_PAGEUP,'9',0x00},		// SC_KEYPAD9
	{Burger::Keyboard::ASCII_PRINTSCREEN,Burger::Keyboard::ASCII_PRINTSCREEN,0x00},	// SC_PRINTSCREEN
	{Burger::Keyboard::ASCII_SCROLLLOCK,Burger::Keyboard::ASCII_SCROLLLOCK,0x00},	// SC_SCROLLLOCK
	{Burger::Keyboard::ASCII_PAUSE,Burger::Keyboard::ASCII_PAUSE,0x00},				// SC_PAUSE
	{Burger::Keyboard::ASCII_INSERT,Burger::Keyboard::ASCII_INSERT,0x00},			// SC_INSERT
	{Burger::Keyboard::ASCII_DELETE,Burger::Keyboard::ASCII_DELETE,0x00},			// SC_DELETE
	{Burger::Keyboard::ASCII_HOME,Burger::Keyboard::ASCII_HOME,0x00},				// SC_HOME
	{Burger::Keyboard::ASCII_END,Burger::Keyboard::ASCII_END,0x00},					// SC_END
	{Burger::Keyboard::ASCII_PAGEUP,Burger::Keyboard::ASCII_PAGEUP,0x00},			// SC_PAGEUP
	{Burger::Keyboard::ASCII_PAGEDOWN,Burger::Keyboard::ASCII_PAGEDOWN,0x00},		// SC_PAGEDOWN		
	{Burger::Keyboard::ASCII_LEFTARROW,Burger::Keyboard::ASCII_LEFTARROW,0x00},		// SC_LEFTARROW
	{Burger::Keyboard::ASCII_RIGHTARROW,Burger::Keyboard::ASCII_RIGHTARROW,0x00},	// SC_RIGHTARROW
	{Burger::Keyboard::ASCII_DOWNARROW,Burger::Keyboard::ASCII_DOWNARROW,0x00},		// SC_DOWNARROW
	{Burger::Keyboard::ASCII_UPARROW,Burger::Keyboard::ASCII_UPARROW,0x00},			// SC_UPARROW
	{Burger::Keyboard::ASCII_F1,Burger::Keyboard::ASCII_F1,0x00},	// SC_F1
	{Burger::Keyboard::ASCII_F2,Burger::Keyboard::ASCII_F2,0x00},	// SC_F2
	{Burger::Keyboard::ASCII_F3,Burger::Keyboard::ASCII_F3,0x00},	// SC_F3
	{Burger::Keyboard::ASCII_F4,Burger::Keyboard::ASCII_F4,0x00},	// SC_F4
	{Burger::Keyboard::ASCII_F5,Burger::Keyboard::ASCII_F5,0x00},	// SC_F5
	{Burger::Keyboard::ASCII_F6,Burger::Keyboard::ASCII_F6,0x00},	// SC_F6
	{Burger::Keyboard::ASCII_F7,Burger::Keyboard::ASCII_F7,0x00},	// SC_F7
	{Burger::Keyboard::ASCII_F8,Burger::Keyboard::ASCII_F8,0x00},	// SC_F8
	{Burger::Keyboard::ASCII_F9,Burger::Keyboard::ASCII_F9,0x00},	// SC_F9
	{Burger::Keyboard::ASCII_F10,Burger::Keyboard::ASCII_F10,0x00},	// SC_F10
	{Burger::Keyboard::ASCII_F11,Burger::Keyboard::ASCII_F11,0x00},	// SC_F11
	{Burger::Keyboard::ASCII_F12,Burger::Keyboard::ASCII_F12,0x00},	// SC_F12
	{Burger::Keyboard::ASCII_F13,Burger::Keyboard::ASCII_F13,0x00},	// SC_F13
	{Burger::Keyboard::ASCII_F14,Burger::Keyboard::ASCII_F14,0x00},	// SC_F14
	{Burger::Keyboard::ASCII_F15,Burger::Keyboard::ASCII_F15,0x00},	// SC_F15
	{0x0000,0x0000,0x00},	// SC_OEM_102
	{0x0000,0x0000,0x00},	// SC_KANA
	{0x0000,0x0000,0x00},	// SC_ABNT_C1
	{0x0000,0x0000,0x00},	// SC_CONVERT
	{0x0000,0x0000,0x00},	// SC_NOCONVERT
	{0x0000,0x0000,0x00},	// SC_YEN
	{0x0000,0x0000,0x00},	// SC_ABNT_C2
	{0x0000,0x0000,0x00},	// SC_PREVTRACK
	{0x0000,0x0000,0x00},	// SC_AT
	{0x0000,0x0000,0x00},	// SC_COLONPC98
	{0x0000,0x0000,0x00},	// SC_UNDERLINE
	{0x0000,0x0000,0x00},	// SC_KANJI
	{0x0000,0x0000,0x00},	// SC_STOP
	{0x0000,0x0000,0x00},	// SC_AX
	{0x0000,0x0000,0x00},	// SC_UNLABELED
	{0x0000,0x0000,0x00},	// SC_NEXTTRACK
	{0x0000,0x0000,0x00},	// SC_MUTE
	{0x0000,0x0000,0x00},	// SC_CALCULATOR
	{0x0000,0x0000,0x00},	// SC_PLAYPAUSE
	{0x0000,0x0000,0x00},	// SC_MEDIASTOP
	{0x0000,0x0000,0x00},	// SC_VOLUMEDOWN
	{0x0000,0x0000,0x00},	// SC_VOLUMEUP
	{0x0000,0x0000,0x00},	// SC_WEBHOME
	{0x0000,0x0000,0x00},	// SC_APPS
	{0x0000,0x0000,0x00},	// SC_POWER
	{0x0000,0x0000,0x00},	// SC_SLEEP
	{0x0000,0x0000,0x00},	// SC_WAKE
	{0x0000,0x0000,0x00},	// SC_WEBSEARCH
	{0x0000,0x0000,0x00},	// SC_WEBFAVORITES
	{0x0000,0x0000,0x00},	// SC_WEBREFRESH
	{0x0000,0x0000,0x00},	// SC_WEBSTOP
	{0x0000,0x0000,0x00},	// SC_WEBFORWARD
	{0x0000,0x0000,0x00},	// SC_WEBBACK
	{0x0000,0x0000,0x00},	// SC_MYCOMPUTER
	{0x0000,0x0000,0x00},	// SC_MAIL
	{0x0000,0x0000,0x00},	// SC_MEDIASELECT
	{0x0000,0x0000,0x00}	// SC_EXTRA
};

#endif

/*! ************************************

	\brief Default constructor

	Installs the heartbeat thread to periodically
	refresh the m_KeyArray and generate the lists of
	upcoming key events

	\sa ~Keyboard()

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || defined(BURGER_MACOSX)) || defined(DOXYGEN)
Burger::Keyboard::Keyboard(GameApp *pAppInstance) :
	m_pAppInstance(pAppInstance),
	m_uArrayStart(0),
	m_uArrayEnd(0),
	m_uInitialDelay(500),
	m_uRepeatDelay(33)
{
	MemoryClear(m_KeyArray,sizeof(m_KeyArray));
	MemoryClear(&m_RepeatEvent,sizeof(m_RepeatEvent));
}

/*! ************************************

	\brief Default destructor

	Shuts down the heartbeat thread and releases 
	all allocated system resources.

***************************************/

Burger::Keyboard::~Keyboard()
{
}

#endif

/*! ************************************

	\fn GameApp *Burger::Keyboard::GetApp(void) const
	\brief Return the parent application

	Get the pointer to the application that is connected
	to the Keyboard manager

	\return Pointer to the connected GameApp

***************************************/

/*! ************************************

	\brief Check if a keyboard event is pending.

	See if a key is pending from the keyboard, if
	so, return the event without removing it from
	the queue.

	\param pEvent Pointer to a KeyEvent_t structure to receive the event.
	\return Zero if no key was pending, non-zero if a keyboard event was retrieved.
	\sa GetKeyEvent(KeyEvent_t *)

***************************************/

uint_t BURGER_API Burger::Keyboard::PeekKeyEvent(KeyEvent_t *pEvent)
{
	Lock();
	uint_t uIndex = m_uArrayStart;	// Get the starting index
	uint_t bResult = FALSE;			// No event pending
	if (uIndex!=m_uArrayEnd) {		// Anything in the buffer?
		pEvent[0] = m_KeyEvents[uIndex];	// Copy the data
		bResult = TRUE;
	}
	Unlock();

	// No event pending
	return bResult;
}


/*! ************************************

	\brief Retrieve a keyboard event.

	If a keyboard event is in the queue, return
	the event and remove it from the list.

	\param pEvent Pointer to a KeyEvent_t structure to receive the event.
	\return Zero if no key was pending, non-zero if a keyboard event was retrieved.
	\sa PeekKeyEvent(KeyEvent_t *)

***************************************/

uint_t BURGER_API Burger::Keyboard::GetKeyEvent(KeyEvent_t *pEvent)
{
	Lock();
	uint_t uIndex = m_uArrayStart;		// Get the starting index
	uint_t bResult = FALSE;
	if (uIndex!=m_uArrayEnd) {			// Anything in the buffer?
		pEvent[0] = m_KeyEvents[uIndex];
		m_uArrayStart = (uIndex+1)&(kBufferSize-1);		// Next key
		bResult = TRUE;
	}
	Unlock();

#if defined(BURGER_WINDOWS)
	if (bResult && m_pAppInstance->IsWindowSwitchingAllowed()) {
		if ((pEvent->m_uAscii==ASCII_RETURN) &&
			(pEvent->m_uEvent==EVENT_KEYDOWN) &&
			((pEvent->m_uFlags&FLAG_ALT)!=0)) {
			m_pAppInstance->SetWindowSwitchRequested(TRUE);
		}
	}
#endif

#if defined(BURGER_MACOSX)
	if (bResult && m_pAppInstance->IsWindowSwitchingAllowed()) {
		if ((pEvent->m_uAscii==ASCII_RETURN) &&
			(pEvent->m_uEvent==EVENT_KEYDOWN) &&
			((pEvent->m_uFlags&FLAG_OPTION)!=0)) {
			m_pAppInstance->SetWindowSwitchRequested(TRUE);
		}
	}
#endif
	// Return TRUE if an event was found
	return bResult;
}


/*! ************************************

	\brief Clear the key event for the specific scan code.

	If the code is invalid, do nothing. Otherwise, clear
	the keyboard flags in the m_KeyArray

	\param uScanCode Valid keyboard scan code to check
	\sa AnyPressed() or IsPressed(eScanCode)

***************************************/

void BURGER_API Burger::Keyboard::ClearKey(eScanCode uScanCode)
{
	if ((uScanCode!=SC_INVALID) && 
		(static_cast<uint_t>(uScanCode)<static_cast<uint_t>(SC_MAXENTRY+1))) {
		// Remove the keyboard pressed event
		m_KeyArray[uScanCode] &= (~KEYCAPPRESSED);
	}
}

/*! ************************************

	\brief Check if a key is pressed at this very moment

	Scan the keyboard status array and return the scan code of
	the first key found that's held down.

	\return \ref SC_INVALID if no key was pressed or the scan code of the first key found

***************************************/

#if !defined(DOXYGEN)

#if defined(BURGER_BIGENDIAN)
#define MASK1 (KEYCAPDOWN<<24U)
#define MASK2 (KEYCAPDOWN<<16U)
#define MASK3 (KEYCAPDOWN<<8U)
#else
#define MASK1 KEYCAPDOWN
#define MASK2 (KEYCAPDOWN<<8U)
#define MASK3 (KEYCAPDOWN<<16U)
#endif

#endif

Burger::Keyboard::eScanCode BURGER_API Burger::Keyboard::AnyPressed(void) const
{
    // Init the pointer
    const uint8_t *pWork = m_KeyArray;
	uintptr_t uCount = sizeof(m_KeyArray)/sizeof(uint32_t);
	eScanCode uResult = SC_INVALID;
	do {
		// I am going to perform a uint32_t test
		uint32_t uTempVal = reinterpret_cast<const uint32_t *>(pWork)[0];
		// Test 4 in a row
		if (uTempVal & (KEYCAPDOWN*0x01010101U)) {
			// Which key was pressed?
			uintptr_t uPossible = static_cast<uintptr_t>(pWork-m_KeyArray);
			if (!(uTempVal&MASK1)) {			// Was it the first one?
				++uPossible;
				if (!(uTempVal&MASK2)) {		// Second?
					++uPossible;
					if (!(uTempVal&MASK3)) {	// Third?
						++uPossible;				// Must be the fourth
					}
				}
			}
			uResult = static_cast<eScanCode>(uPossible);
			break;
		}
		pWork+=sizeof(uint32_t);	// test for speed
	} while (--uCount);			// Count down
	return uResult;
}

/*! ************************************

	\brief Check if the requested key is currently or previously held down.

	If a key has been pressed, return \ref TRUE. If the key has not been pressed or the scan
	code is invalid, return \ref FALSE. This function will not clear
	the event. Use HasBeenPressedClear() to clear the event

	\param uScanCode Valid Scan code
	\return \ref TRUE if the key was pressed, \ref FALSE if not
	\sa HasBeenPressedClear(eScanCode)

***************************************/

uint_t BURGER_API Burger::Keyboard::HasBeenPressed(eScanCode uScanCode) const
{
	uint_t bResult = FALSE;
	if ((uScanCode!=SC_INVALID) && 
		(static_cast<uint_t>(uScanCode)<static_cast<uint_t>(SC_MAXENTRY+1)) && 
		(m_KeyArray[uScanCode]&KEYCAPPRESSED)) {
		bResult = TRUE;
	}
	return bResult;
}

/*! ************************************

	\brief Check if the requested key is currently or previously held down and clear the event.

	If a key is pressed, clear the event and return \ref TRUE,
	otherwise or if the scan code is invalid, return \ref FALSE
	\param uScanCode Valid Scan code
	\return \ref TRUE if the key was pressed, \ref FALSE if not
	
	\sa HasBeenPressed(eScanCode)

***************************************/

uint_t BURGER_API Burger::Keyboard::HasBeenPressedClear(eScanCode uScanCode)
{
	uint_t bResult = FALSE;
	if ((uScanCode!=SC_INVALID) && 
		(static_cast<uint_t>(uScanCode)<static_cast<uint_t>(SC_MAXENTRY+1))) {
		uint_t uTemp = m_KeyArray[uScanCode];
		if (uTemp&KEYCAPPRESSED) {
			// Xor will clear the flag
			m_KeyArray[uScanCode] = static_cast<uint8_t>(uTemp^KEYCAPPRESSED);
			bResult = TRUE;
		}
	}
	return bResult;
}

/*! ************************************

	\brief Check if the requested key is currently held down.

	If the key is currently held down, return \ref TRUE,
	otherwise or if the scan code is invalid, return \ref FALSE
	\param uScanCode Valid Scan code
	\return \ref TRUE if the key was pressed, \ref FALSE if not

***************************************/

uint_t BURGER_API Burger::Keyboard::IsPressed(eScanCode uScanCode) const
{
	uint_t bResult = FALSE;
	if ((uScanCode!=SC_INVALID) && 
		(static_cast<uint_t>(uScanCode)<static_cast<uint_t>(SC_MAXENTRY+1))) {
		bResult = static_cast<uint_t>(m_KeyArray[uScanCode]&KEYCAPDOWN);
	}
	return bResult;
}

/*! ************************************

	\brief Return the Unicode value of a pending key press.

	Check if a key is pending and return zero if not, else return
	the Unicode value. All key up events up until the key down
	are consumed.
	
	\return Zero if no key is pending, Unicode key if there was a pending key

***************************************/

uint_t BURGER_API Burger::Keyboard::GetKey(void)
{
	KeyEvent_t NewEvent;
	// Get the event
	uint_t uResult = GetKeyEvent(&NewEvent);
	if (uResult) {
		// Was is a key down?
		do {
			// Key down?
			if ((NewEvent.m_uEvent==EVENT_KEYDOWN) || 
				(NewEvent.m_uEvent==EVENT_KEYAUTO)) {
				// Isolate the ASCII
				uResult = NewEvent.m_uAscii;
				break;
			}
			// Get the next key
			uResult = GetKeyEvent(&NewEvent);
		} while (uResult);
	}
	return uResult;
}

/*! ************************************

	\brief Return a key in lower case

	Check if a key is pending and return zero if not, else the
	Unicode code in lower case

	\return Zero if no key is pending, lower case key if there was a pending key

***************************************/

uint_t BURGER_API Burger::Keyboard::GetKeyLowerCase(void)
{
	return ToLower(GetKey());	// Key pending?
}

/*! ************************************

	\brief Return a key in upper case

	Check if a key is pending and return zero if not, else the
	Unicode code in upper case

	\return Zero if no key is pending, upper case key if there was a pending key

***************************************/

uint_t BURGER_API Burger::Keyboard::GetKeyUpperCase(void)
{
	return ToUpper(GetKey());
}

/*! ************************************

	\brief Flush the keyboard buffer

	Release all pending keyboard events and erase the
	keyboard array.

***************************************/

void BURGER_API Burger::Keyboard::Flush(void)
{
	KeyEvent_t NewEvent;

	//
	// This is done as a loop instead of just killing the array
	// to allow "special" keys to be processed or other
	// platform specific code
	//

	while (GetKeyEvent(&NewEvent)) {
	}
	uint32_t *pWork = static_cast<uint32_t *>(static_cast<void*>(m_KeyArray));

	// Clear all key presses
	uintptr_t uCount = sizeof(m_KeyArray)/sizeof(uint32_t);
	do {
		pWork[0] = pWork[0]&(~(KEYCAPPRESSED*0x01010101U));
		++pWork;
	} while (--uCount);
	MemoryClear(&m_RepeatEvent,sizeof(m_RepeatEvent));
}

/*! ************************************

	\brief Wait for a key press

	Wait for a keypress, but call GetKey() to allow
	screen savers to kick in. 

	\return Unicode code of pressed key

***************************************/

uint_t BURGER_API Burger::Keyboard::Wait(void)
{
	KeyEvent_t NewEvent;
	// Key pending?
	while (!GetKeyEvent(&NewEvent)) {

		// Sleep the application until a key is pressed
#if defined(BURGER_WINDOWS)	
		::WaitMessage();			
#endif
		// Handle operating system events
		m_pAppInstance->Poll();
	}
	return NewEvent.m_uAscii;
}

/*! ************************************

	\brief Post a scan code key down

	Given a keyboard scan code, post a
	key down event

	\param uScanCode Keyboard scan code
	\return Zero if posted successfully, non-zero if not.
	\sa PostKeyUp(eScanCode)

***************************************/

uint_t BURGER_API Burger::Keyboard::PostKeyDown(eScanCode uScanCode)
{
	KeyEvent_t NewEvent;
	uint_t bResult = EncodeScanCode(&NewEvent,uScanCode);
	if (!bResult) {
		NewEvent.m_uEvent = EVENT_KEYDOWN;
		NewEvent.m_uWhich = 0;
		bResult = PostKeyEvent(&NewEvent);
	}
	return bResult;
}

/*! ************************************

	\brief Post a scan code key up

	Given a keyboard scan code, post a
	key up event

	\param uScanCode Keyboard scan code
	\return Zero if posted successfully, non-zero if not.
	\sa PostKeyDown(eScanCode)

***************************************/

uint_t BURGER_API Burger::Keyboard::PostKeyUp(eScanCode uScanCode)
{
	KeyEvent_t NewEvent;
	uint_t bResult = EncodeScanCode(&NewEvent,uScanCode);
	if (!bResult) {
		NewEvent.m_uEvent = EVENT_KEYUP;
		NewEvent.m_uWhich = 0;
		bResult = PostKeyEvent(&NewEvent);
	}
	return bResult;
}

/*! ************************************

	\brief Post a scan code key down and a key up

	Given a keyboard scan code, post a
	key down event followed by a key up event

	\param uScanCode Keyboard scan code
	\return Zero if posted successfully, non-zero if not.
	\sa PostKeyUp(eScanCode) or PostKeyDown(eScanCode)

***************************************/

uint_t BURGER_API Burger::Keyboard::PostKey(eScanCode uScanCode)
{
	KeyEvent_t NewEvent;
	uint_t bResult = EncodeScanCode(&NewEvent,uScanCode);
	if (!bResult) {
		NewEvent.m_uEvent = EVENT_KEYDOWN;
		NewEvent.m_uWhich = 0;
		bResult = PostKeyEvent(&NewEvent);
		if (!bResult) {
			NewEvent.m_uEvent = EVENT_KEYUP;
			bResult = PostKeyEvent(&NewEvent);
		}
	}
	return bResult;
}



/*! ************************************

	\brief Post an ASCII/Unicode key down

	Given an ASCII/Unicode character, post a
	key down event

	\param uUnicode Unicode32 character
	\return Zero if posted successfully, non-zero if not.
	\sa PostUnicodeUp(uint32_t)

***************************************/

uint_t BURGER_API Burger::Keyboard::PostUnicodeDown(uint32_t uUnicode)
{
	KeyEvent_t NewEvent;
	uint_t bResult = EncodeUnicode(&NewEvent,uUnicode);
	if (!bResult) {
		NewEvent.m_uEvent = EVENT_KEYDOWN;
		NewEvent.m_uWhich = 0;
		bResult = PostKeyEvent(&NewEvent);
	}
	return bResult;
}

/*! ************************************

	\brief Post an ASCII/Unicode key up

	Given an ASCII/Unicode character, post a
	key up event

	\param uUnicode Unicode32 character
	\return Zero if posted successfully, non-zero if not.
	\sa PostUnicodeDown(uint32_t)

***************************************/

uint_t BURGER_API Burger::Keyboard::PostUnicodeUp(uint32_t uUnicode)
{
	KeyEvent_t NewEvent;
	uint_t bResult = EncodeUnicode(&NewEvent,uUnicode);
	if (!bResult) {
		NewEvent.m_uEvent = EVENT_KEYUP;
		NewEvent.m_uWhich = 0;
		bResult = PostKeyEvent(&NewEvent);
	}
	return bResult;
}

/*! ************************************

	\brief Post an ASCII/Unicode key down

	Given an ASCII/Unicode character, post a
	key down event

	\param uUnicode Unicode32 character
	\return Zero if posted successfully, non-zero if not.
	\sa PostUnicodeUp(uint32_t) or PostUnicodeDown(uint32_t)

***************************************/

uint_t BURGER_API Burger::Keyboard::PostUnicode(uint32_t uUnicode)
{
	KeyEvent_t NewEvent;
	uint_t bResult = EncodeUnicode(&NewEvent,uUnicode);
	if (!bResult) {
		NewEvent.m_uEvent = EVENT_KEYDOWN;
		NewEvent.m_uWhich = 0;
		bResult = PostKeyEvent(&NewEvent);
		if (!bResult) {
			NewEvent.m_uEvent = EVENT_KEYUP;
			bResult = PostKeyEvent(&NewEvent);
		}
	}
	return bResult;
}

/*! ************************************

	\brief Post the keyboard event
	
	Given a valid keyboard event structure,
	post the event

	\param pEvent Pointer to a valid keyboard event
	\return Zero if successful, error code if not

***************************************/

uint_t BURGER_API Burger::Keyboard::PostKeyEvent(const KeyEvent_t *pEvent)
{
	Lock();

	uint_t bResult = 10;
	uint_t uEnd = m_uArrayEnd;
	// See if there's room in the buffer
	uint_t uTemp = (uEnd+1)&(kBufferSize-1);
	if (uTemp!=m_uArrayStart) {

		// Didn't wrap, accept it!
		m_uArrayEnd = uTemp;
	
		// Insert the new event
		KeyEvent_t *pNewEvent = &m_KeyEvents[uEnd];
		pNewEvent->m_uEvent = pEvent->m_uEvent;
		pNewEvent->m_uWhich = pEvent->m_uWhich;
		pNewEvent->m_uAscii = pEvent->m_uAscii;
		eScanCode uScanCode = static_cast<eScanCode>(pEvent->m_uScanCode);
		pNewEvent->m_uScanCode = static_cast<uint32_t>(uScanCode);
		uint32_t uFlags = pEvent->m_uFlags;
		pNewEvent->m_uFlags = uFlags;

		// Add the proper time stamp
		uint32_t uTime = pEvent->m_uMSTimeStamp;
		if (!uTime) {
			uTime = Tick::ReadMilliseconds();
		}
		pNewEvent->m_uMSTimeStamp = uTime;

		if ((uScanCode!=SC_INVALID) &&
			(static_cast<uint_t>(uScanCode)<static_cast<uint_t>(SC_MAXENTRY+1))) {

			// Update the running state
			if ((pEvent->m_uEvent==EVENT_KEYDOWN) || (pEvent->m_uEvent==EVENT_KEYAUTO)) {

				uint8_t uKey = m_KeyArray[uScanCode];
				uKey = static_cast<uint8_t>((uKey|(KEYCAPDOWN|KEYCAPPRESSED))^KEYCAPTOGGLE);
				m_KeyArray[uScanCode] = uKey;

				if (pEvent->m_uEvent!=EVENT_KEYAUTO) {
					m_RepeatEvent.m_uEvent = EVENT_KEYAUTO;
					m_RepeatEvent.m_uWhich = pNewEvent->m_uWhich;
					m_RepeatEvent.m_uMSTimeStamp = 0;
					m_RepeatEvent.m_uAscii = pNewEvent->m_uAscii;
					m_RepeatEvent.m_uFlags = pNewEvent->m_uFlags;
					m_RepeatEvent.m_uScanCode = pNewEvent->m_uScanCode;
				}
			} else {
				// Mark as pressed
				m_KeyArray[uScanCode] &= (~KEYCAPDOWN);

				// DirectInput for Windows has a timer to simulate autorepeat
#if defined(ENABLE_DIRECTINPUT)
				if (m_bRepeatActive) {
					CancelWaitableTimer(m_pKeyboardTimerEvent);
					m_bRepeatActive = FALSE;
				}
#endif
			}
		}
		// Key was accepted
		bResult = 0;
	}
	// Unlock the buffer
	Unlock();
	// Return the result
	return bResult;
}

/*! ************************************

	\brief Convert a scan code to a KeyEvent_t

	Given a scan code, check the state of the keyboard
	and determine the Unicode/ASCII code to be generated for
	this keypress.

	\return Zero if successful, non-zero if a scan code is unknown.

***************************************/

uint_t BURGER_API Burger::Keyboard::EncodeScanCode(KeyEvent_t *pEvent,eScanCode uScanCode) const
{
	// Look up the scan code
	uint_t uResult = 10;
    if ((uScanCode != SC_INVALID) &&
        (static_cast<uintptr_t>(uScanCode) < BURGER_ARRAYSIZE(g_ScanCodeTranslation))) {
		// Initialize the time stamp
		pEvent->m_uMSTimeStamp = 0;
		pEvent->m_uScanCode = static_cast<uint32_t>(uScanCode);

		// Determine the flags for the key
		uint_t uFlags = GetCurrentFlags();
		pEvent->m_uFlags = static_cast<uint32_t>(uFlags);

		// Which translation entry to use?
		const ScanCodeToAscii_t *pTranslation = &g_ScanCodeTranslation[uScanCode];
		uint_t uAscii = pTranslation->m_uAsciiCode;
		if (uAscii) {

			// Affected by NumLock?
			if ((uScanCode>=SC_KEYPAD0) &&
				(uScanCode<=SC_KEYPAD9)) {
				if (uFlags&FLAG_NUMLOCK) {
					uAscii = pTranslation->m_uShiftCode;
				}
			} else {
				// Control key pressed? (If allowed)
				if ((uFlags&FLAG_CONTROL) && 
					pTranslation->m_uControlCode) {
					uAscii = pTranslation->m_uControlCode;
				} else {
					if ((uFlags&FLAG_SHIFT) &&
						pTranslation->m_uShiftCode) {
						uAscii = pTranslation->m_uShiftCode;
					}
				}
			}
		}
		// Store the Ascii value
		pEvent->m_uAscii = uAscii;
		uResult = 0;
	}
	return uResult;
}

/*! ************************************

	\brief Convert a Unicode character to a KeyEvent_t

	Given a Unicode character, look up the scan code, if available,
	and fill in a KeyEvent_t to be generated for
	this keypress.

	\return Zero if successful, non-zero if the Unicode value is unknown.

***************************************/

uint_t BURGER_API Burger::Keyboard::EncodeUnicode(KeyEvent_t *pEvent,uint32_t uUnicode) const
{
	// Make sure the code is valid
	uint_t uResult = 10;
	if (UTF32::IsValid(uUnicode)) {

		// Initialize the time stamp
		pEvent->m_uMSTimeStamp = 0;
		// Set the Unicode value as the ASCII (Full 32 bits)
		pEvent->m_uAscii = uUnicode;

		// Do a reverse lookup to find the scan code
		const ScanCodeToAscii_t *pTranslation = g_ScanCodeTranslation;

		uintptr_t uCounter = 0;
		uint_t uFlags = 0;
		eScanCode uScanCode = SC_INVALID;
		do {
			// Unmodified scan code lookup?
			if (uUnicode==pTranslation->m_uAsciiCode) {
				uScanCode = static_cast<eScanCode>(uCounter);
				break;
			}
			// Shifted?
			if (uUnicode==pTranslation->m_uShiftCode) {
				uFlags = FLAG_SHIFT;
				uScanCode = static_cast<eScanCode>(uCounter);
				break;
			}
			// Control?
			if (uUnicode==pTranslation->m_uControlCode) {
				uFlags = FLAG_CONTROL;
				uScanCode = static_cast<eScanCode>(uCounter);
				break;
			}
			++pTranslation;
		} while (++uCounter<BURGER_ARRAYSIZE(g_ScanCodeTranslation));

		// Found it?
		if (uScanCode!=SC_INVALID) {
			// Store the scan code and flags
			pEvent->m_uFlags = static_cast<uint32_t>(uFlags);
			pEvent->m_uScanCode = static_cast<uint32_t>(uScanCode);
			uResult = 0;
		}
	}
	return uResult;
}

/*! ************************************

	\brief Return the flags for a KeyEvent_t based on the currently active modifiers

	Check the state of previously pressed keys and return the 
	modifier flags for converting a scan code into an ASCII/Unicode
	value.
	
	\return FLAG_??? merged together for setting KeyEvent_t::m_uFlags

***************************************/

uint_t BURGER_API Burger::Keyboard::GetCurrentFlags(void) const
{
	uint_t uFlags = 0;
	if ((m_KeyArray[SC_LEFTALT]&KEYCAPDOWN) || (m_KeyArray[SC_RIGHTALT]&KEYCAPDOWN)) {
		uFlags |= FLAG_ALT;
	}
	if ((m_KeyArray[SC_LEFTCONTROL]&KEYCAPDOWN) || (m_KeyArray[SC_RIGHTCONTROL]&KEYCAPDOWN)) {
		uFlags |= FLAG_CONTROL;
	}
	if ((m_KeyArray[SC_LEFTSHIFT]&KEYCAPDOWN) || (m_KeyArray[SC_RIGHTSHIFT]&KEYCAPDOWN)) {
		uFlags |= FLAG_SHIFT;
	}
	if ((m_KeyArray[SC_LEFTOPTION]&KEYCAPDOWN) || (m_KeyArray[SC_RIGHTOPTION]&KEYCAPDOWN)) {
		uFlags |= FLAG_OPTION;
	}
	// Caps lock is a toggle
	if (m_KeyArray[SC_CAPSLOCK]&KEYCAPTOGGLE) {
		uFlags |= FLAG_CAPSLOCK;
		// Invert shift on Caps lock
		uFlags ^= FLAG_SHIFT;
	}
	// Num lock is a toggle
	if (m_KeyArray[SC_NUMLOCK]&KEYCAPTOGGLE) {
		uFlags |= FLAG_NUMLOCK;
	}
	return uFlags;
}

/*! ************************************

	\brief Convert a string to a scan code
	
	Given a pointer to a string describing a keyboard scan code. Look up
	the scan code and return the value. I return a -1 if there is no
	match.

	\param pString Pointer to a "C" string
	\return	Burgerlib scan code or a -1 if no match is found.	
	\sa ScanCodeToString()

***************************************/

Burger::Keyboard::eScanCode BURGER_API Burger::Keyboard::StringToScanCode(const char *pString)
{
	const ScanEntry_t *pEntry = g_ScanCodeNames;		// Pointer to the array
	uintptr_t uCount = BURGER_ARRAYSIZE(g_ScanCodeNames);
	eScanCode uResult = SC_INVALID;
	do {
		if (!StringCaseCompare(pString,pEntry->m_pName)) {	// Match?
			uResult = pEntry->m_uScanCode;					// Return the match
			break;
		}
		++pEntry;			// Next entry
	} while (--uCount);		// All done?
	return uResult;			// Return the bogus entry code
}

/*! ************************************

	\brief Convert a scan code into a string

	Given a Burgerlib keyboard scan code, return a string that best
	describes the scan code. If StringSize is 0, nothing will be done since
	the output buffer is invalid.

	\param pString Pointer to a "C" string buffer to store the result
	\param uStringSize sizeof() the buffer to prevent overruns
	\param uScanCode Valid Burgerlib scan code
	\return	StringPtr will contain the valid string. A null string will be returned
		if the scan code is not recognized.
	\sa StringToScanCode()

***************************************/

void BURGER_API Burger::Keyboard::ScanCodeToString(char *pString,uintptr_t uStringSize,eScanCode uScanCode)
{
	if (uStringSize) {
		const ScanEntry_t *pEntry = g_ScanCodeNames;
		uintptr_t uCount = BURGER_ARRAYSIZE(g_ScanCodeNames);
		do {
			if (pEntry->m_uScanCode==uScanCode) {
				StringCopy(pString,uStringSize,pEntry->m_pName);	// Length of the string
				return;
			}
			++pEntry;
		} while (--uCount);
		pString[0] = 0;		// Don't return the string
	}
}
