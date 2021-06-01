/***************************************

    Keyboard Manager

    Xbox 360 version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brkeyboard.h"

#if defined(BURGER_XBOX360)
#include "brmemoryfunctions.h"
#include "brtick.h"
#define NOD3D
#define NONET
#include <xtl.h>

// Number of keyboard events to cache

#define DIRECTINPUT_KEYBOARDBUFFERSIZE 20

// Private scan code to Burgerlib scan code table
struct ScanCodeTranslation_t {
	uint_t m_uWindowsCode;		// Windows scan code
	Burger::Keyboard::eScanCode m_uScanCode;			// Burgerlib scan code
	uint32_t m_uAsciiCode;			// Ascii code
	uint32_t m_uShiftCode;			// Shifted ascii code
	uint32_t m_uControlCode;		// Control ascii code
};

// Table, first entry is the Windows code, the second is the code for Burgerlib

static const ScanCodeTranslation_t TranslationTable[] = {
	{'A',Burger::Keyboard::SC_A,'a','A',0x01},
	{'B',Burger::Keyboard::SC_B,'b','B',0x02},
	{'C',Burger::Keyboard::SC_C,'c','C',0x03},
	{'D',Burger::Keyboard::SC_D,'d','D',0x04},
	{'E',Burger::Keyboard::SC_E,'e','E',0x05},
	{'F',Burger::Keyboard::SC_F,'f','F',0x06},
	{'G',Burger::Keyboard::SC_G,'g','G',0x07},
	{'H',Burger::Keyboard::SC_H,'h','H',0x08},
	{'I',Burger::Keyboard::SC_I,'i','I',0x09},
	{'J',Burger::Keyboard::SC_J,'j','J',0x0A},
	{'K',Burger::Keyboard::SC_K,'k','K',0x0B},
	{'L',Burger::Keyboard::SC_L,'l','L',0x0C},
	{'M',Burger::Keyboard::SC_M,'m','M',0x0D},
	{'N',Burger::Keyboard::SC_N,'n','N',0x0E},
	{'O',Burger::Keyboard::SC_O,'o','O',0x0F},
	{'P',Burger::Keyboard::SC_P,'p','P',0x10},
	{'Q',Burger::Keyboard::SC_Q,'q','Q',0x11},
	{'R',Burger::Keyboard::SC_R,'r','R',0x12},
	{'T',Burger::Keyboard::SC_T,'s','S',0x13},
	{'S',Burger::Keyboard::SC_S,'t','T',0x14},
	{'U',Burger::Keyboard::SC_U,'u','U',0x15},
	{'V',Burger::Keyboard::SC_V,'v','V',0x16},
	{'W',Burger::Keyboard::SC_W,'w','W',0x17},
	{'X',Burger::Keyboard::SC_X,'x','X',0x18},
	{'Y',Burger::Keyboard::SC_Y,'y','Y',0x19},
	{'Z',Burger::Keyboard::SC_Z,'z','Z',0x1A},

	{VK_SPACE,Burger::Keyboard::SC_SPACE,Burger::Keyboard::ASCII_SPACE},
	{VK_ESCAPE,Burger::Keyboard::SC_ESCAPE,Burger::Keyboard::ASCII_ESCAPE},
	{VK_RETURN,Burger::Keyboard::SC_RETURN,Burger::Keyboard::ASCII_RETURN},
	{VK_TAB,Burger::Keyboard::SC_TAB,Burger::Keyboard::ASCII_TAB},
	{VK_UP,Burger::Keyboard::SC_UPARROW,Burger::Keyboard::ASCII_UPARROW},
	{VK_DOWN,Burger::Keyboard::SC_DOWNARROW,Burger::Keyboard::ASCII_DOWNARROW},
	{VK_LEFT,Burger::Keyboard::SC_LEFTARROW,Burger::Keyboard::ASCII_LEFTARROW},
	{VK_RIGHT,Burger::Keyboard::SC_RIGHTARROW,Burger::Keyboard::ASCII_RIGHTARROW},

	{VK_0,Burger::Keyboard::SC_0,'0',')'},
	{VK_1,Burger::Keyboard::SC_1,'1','!'},
	{VK_2,Burger::Keyboard::SC_2,'2','@'},
	{VK_3,Burger::Keyboard::SC_3,'3','#'},
	{VK_4,Burger::Keyboard::SC_4,'4','$'},
	{VK_5,Burger::Keyboard::SC_5,'5','%'},
	{VK_6,Burger::Keyboard::SC_6,'6','^'},
	{VK_7,Burger::Keyboard::SC_7,'7','&'},
	{VK_8,Burger::Keyboard::SC_8,'8','*'},
	{VK_9,Burger::Keyboard::SC_9,'9','('},

	{VK_HOME,Burger::Keyboard::SC_HOME,Burger::Keyboard::ASCII_HOME},
	{VK_END,Burger::Keyboard::SC_END,Burger::Keyboard::ASCII_END},
	{VK_PRIOR,Burger::Keyboard::SC_PAGEUP,Burger::Keyboard::ASCII_PAGEUP},
	{VK_NEXT,Burger::Keyboard::SC_PAGEDOWN,Burger::Keyboard::ASCII_PAGEDOWN},
	{VK_INSERT,Burger::Keyboard::SC_INSERT,Burger::Keyboard::ASCII_INSERT},
	{VK_DELETE,Burger::Keyboard::SC_DELETE,Burger::Keyboard::ASCII_DELETE},
	{VK_PRINT,Burger::Keyboard::SC_SYSRQ,Burger::Keyboard::ASCII_PRINTSCREEN},
	{VK_PAUSE,Burger::Keyboard::SC_PAUSE,Burger::Keyboard::ASCII_PAUSE},
	{VK_SCROLL,Burger::Keyboard::SC_SCROLLLOCK,Burger::Keyboard::ASCII_SCROLLLOCK},

//	{VK_GRAVE,Burger::Keyboard::SC_GRAVE,'`','~'},
//	{VK_MINUS,Burger::Keyboard::SC_MINUS,'-','_'},
//	{VK_EQUALS,Burger::Keyboard::SC_EQUALS,'=','+'},
	{VK_BACK,Burger::Keyboard::SC_BACKSPACE,Burger::Keyboard::ASCII_BACKSPACE},
//	{VK_LBRACKET,Burger::Keyboard::SC_LEFTBRACKET,'[','{'},
//	{VK_RBRACKET,Burger::Keyboard::SC_RIGHTBRACKET,']','}'},
//	{VK_BACKSLASH,Burger::Keyboard::SC_BACKSLASH,'\\','|'},
//	{VK_SEMICOLON ,Burger::Keyboard::SC_SEMICOLON,';',':'},
//	{VK_APOSTROPHE,Burger::Keyboard::SC_QUOTE,'\'','"'},
	{VK_OEM_COMMA,Burger::Keyboard::SC_COMMA,',','<'},
	{VK_OEM_PERIOD,Burger::Keyboard::SC_PERIOD,'.','>'},
//	{VK_SLASH,Burger::Keyboard::SC_SLASH,'/','?'},

	{VK_LCONTROL,Burger::Keyboard::SC_LEFTCONTROL},
	{VK_RCONTROL,Burger::Keyboard::SC_RIGHTCONTROL},
	{VK_LSHIFT,Burger::Keyboard::SC_LEFTSHIFT},
	{VK_RSHIFT,Burger::Keyboard::SC_RIGHTSHIFT},
//	{VK_LWIN,Burger::Keyboard::SC_LEFTOPTION},
//	{VK_RWIN,Burger::Keyboard::SC_RIGHTOPTION},
	{VK_LMENU,Burger::Keyboard::SC_LEFTMENU},
	{VK_RMENU,Burger::Keyboard::SC_RIGHTMENU},
	{VK_CAPITAL,Burger::Keyboard::SC_CAPSLOCK},

	{VK_F1,Burger::Keyboard::SC_F1,Burger::Keyboard::ASCII_F1},
	{VK_F2,Burger::Keyboard::SC_F2,Burger::Keyboard::ASCII_F2},
	{VK_F3,Burger::Keyboard::SC_F3,Burger::Keyboard::ASCII_F3},
	{VK_F4,Burger::Keyboard::SC_F4,Burger::Keyboard::ASCII_F4},
	{VK_F5,Burger::Keyboard::SC_F5,Burger::Keyboard::ASCII_F5},
	{VK_F6,Burger::Keyboard::SC_F6,Burger::Keyboard::ASCII_F6},
	{VK_F7,Burger::Keyboard::SC_F7,Burger::Keyboard::ASCII_F7},
	{VK_F8,Burger::Keyboard::SC_F8,Burger::Keyboard::ASCII_F8},
	{VK_F9,Burger::Keyboard::SC_F9,Burger::Keyboard::ASCII_F9},
	{VK_F10,Burger::Keyboard::SC_F10,Burger::Keyboard::ASCII_F10},
	{VK_F11,Burger::Keyboard::SC_F11,Burger::Keyboard::ASCII_F11},
	{VK_F12,Burger::Keyboard::SC_F12,Burger::Keyboard::ASCII_F12},
	{VK_F13,Burger::Keyboard::SC_F13,Burger::Keyboard::ASCII_F13},
	{VK_F14,Burger::Keyboard::SC_F14,Burger::Keyboard::ASCII_F14},
	{VK_F15,Burger::Keyboard::SC_F15,Burger::Keyboard::ASCII_F15},

	{VK_NUMLOCK,Burger::Keyboard::SC_NUMLOCK},
	{VK_NUMPAD0,Burger::Keyboard::SC_KEYPAD0,Burger::Keyboard::ASCII_INSERT,'0'},
	{VK_NUMPAD1,Burger::Keyboard::SC_KEYPAD1,Burger::Keyboard::ASCII_END,'1'},
	{VK_NUMPAD2,Burger::Keyboard::SC_KEYPAD2,Burger::Keyboard::ASCII_DOWNARROW,'2'},
	{VK_NUMPAD3,Burger::Keyboard::SC_KEYPAD3,Burger::Keyboard::ASCII_PAGEDOWN,'3'},
	{VK_NUMPAD4,Burger::Keyboard::SC_KEYPAD4,Burger::Keyboard::ASCII_LEFTARROW,'4'},
	{VK_NUMPAD5,Burger::Keyboard::SC_KEYPAD5,'5','5'},
	{VK_NUMPAD6,Burger::Keyboard::SC_KEYPAD6,Burger::Keyboard::ASCII_RIGHTARROW,'6'},
	{VK_NUMPAD7,Burger::Keyboard::SC_KEYPAD7,Burger::Keyboard::ASCII_HOME,'7'},
	{VK_NUMPAD8,Burger::Keyboard::SC_KEYPAD8,Burger::Keyboard::ASCII_UPARROW,'8'},
	{VK_NUMPAD9,Burger::Keyboard::SC_KEYPAD9,Burger::Keyboard::ASCII_PAGEUP,'9'},
	{VK_SUBTRACT,Burger::Keyboard::SC_KEYPADMINUS,'-'},
	{VK_ADD,Burger::Keyboard::SC_KEYPADPLUS,'+'},
	{VK_DECIMAL,Burger::Keyboard::SC_KEYPADPERIOD,'.'},
	{VK_MULTIPLY,Burger::Keyboard::SC_KEYPADASTERISK,'*'},
//	{VK_NUMPADEQUALS,Burger::Keyboard::SC_KEYPADEQUALS,'='},
//	{VK_NUMPADENTER,Burger::Keyboard::SC_KEYPADENTER,Burger::Keyboard::ASCII_ENTER},
//	{VK_NUMPADCOMMA,Burger::Keyboard::SC_KEYPADCOMMA,','},
	{VK_DIVIDE,Burger::Keyboard::SC_KEYPADSLASH,'/'},

	{VK_OEM_102,Burger::Keyboard::SC_OEM_102},
	{VK_KANA,Burger::Keyboard::SC_KANA},
//	{VK_ABNT_C1,Burger::Keyboard::SC_ABNT_C1},
	{VK_CONVERT,Burger::Keyboard::SC_CONVERT},
//	{VK_NOCONVERT,Burger::Keyboard::SC_NOCONVERT},
//	{VK_YEN,Burger::Keyboard::SC_YEN},
//	{VK_ABNT_C2,Burger::Keyboard::SC_ABNT_C2},
	{VK_MEDIA_PREV_TRACK,Burger::Keyboard::SC_PREVTRACK},
//	{VK_AT,Burger::Keyboard::SC_AT},
//	{VK_COLON,Burger::Keyboard::SC_COLONPC98},
//	{VK_UNDERLINE,Burger::Keyboard::SC_UNDERLINE},
	{VK_KANJI,Burger::Keyboard::SC_KANJI},
	{VK_STOP,Burger::Keyboard::SC_STOP},
	{VK_OEM_AX,Burger::Keyboard::SC_AX},
//	{VK_UNLABELED,Burger::Keyboard::SC_UNLABELED},
	{VK_MEDIA_NEXT_TRACK,Burger::Keyboard::SC_NEXTTRACK},
	{VK_VOLUME_MUTE,Burger::Keyboard::SC_MUTE},
//	{VK_CALCULATOR,Burger::Keyboard::SC_CALCULATOR},
	{VK_MEDIA_PLAY_PAUSE,Burger::Keyboard::SC_PLAYPAUSE},
	{VK_MEDIA_STOP,Burger::Keyboard::SC_MEDIASTOP},
	{VK_VOLUME_DOWN,Burger::Keyboard::SC_VOLUMEDOWN},
	{VK_VOLUME_UP,Burger::Keyboard::SC_VOLUMEUP},
	{VK_BROWSER_HOME,Burger::Keyboard::SC_WEBHOME},
	{VK_APPS,Burger::Keyboard::SC_APPS},
//	{VK_POWER,Burger::Keyboard::SC_POWER},
	{VK_SLEEP,Burger::Keyboard::SC_SLEEP},
//	{VK_WAKE,Burger::Keyboard::SC_WAKE},
	{VK_BROWSER_SEARCH,Burger::Keyboard::SC_WEBSEARCH},
	{VK_BROWSER_FAVORITES,Burger::Keyboard::SC_WEBFAVORITES},
	{VK_BROWSER_REFRESH,Burger::Keyboard::SC_WEBREFRESH},
	{VK_BROWSER_STOP,Burger::Keyboard::SC_WEBSTOP},
	{VK_BROWSER_FORWARD,Burger::Keyboard::SC_WEBFORWARD},
	{VK_BROWSER_BACK,Burger::Keyboard::SC_WEBBACK},
//	{VK_MYCOMPUTER,Burger::Keyboard::SC_MYCOMPUTER},
	{VK_LAUNCH_MAIL,Burger::Keyboard::SC_MAIL},
	{VK_LAUNCH_MEDIA_SELECT,Burger::Keyboard::SC_MEDIASELECT},
	{VK_GREENMODIFIER,Burger::Keyboard::SC_EXTRA},
	{VK_ORANGEMODIFIER,static_cast<Burger::Keyboard::eScanCode>(Burger::Keyboard::SC_EXTRA+1)}
};

/***************************************

	Convert a scan code to an ascii code.

***************************************/

static const ScanCodeTranslation_t *ScanCodeTranslate(uint_t uWindowsCode)
{
	// Get the table address
	const ScanCodeTranslation_t *pTranslationTable = TranslationTable;
	uint_t i = BURGER_ARRAYSIZE(TranslationTable);
	const ScanCodeTranslation_t *pResult = NULL;
	// Iterate over the table
	do {
		// Match?
		if (uWindowsCode==pTranslationTable->m_uWindowsCode) {
			// Encode to the current table
			pResult = pTranslationTable;
			break;
		}
		++pTranslationTable;
	} while (--i);
	return pResult;
}

/***************************************

	Initialize DirectInput for an event driven keyboard

***************************************/

Burger::Keyboard::Keyboard(GameApp *pAppInstance) :
	m_pAppInstance(pAppInstance),
	m_uArrayStart(0),
	m_uArrayEnd(0),
	m_uInitialDelay(500),
	m_uRepeatDelay(33)
{
	// Clear my variables
	MemoryClear(m_KeyArray,sizeof(m_KeyArray));
	MemoryClear(&m_RepeatEvent,sizeof(m_RepeatEvent));

	pAppInstance->AddRoutine(Poll,NULL,this,RunQueue::PRIORITY_KEYBOARD);
}

/***************************************

	Release DirectInput for an event driven keyboard

***************************************/

Burger::Keyboard::~Keyboard()
{
	m_pAppInstance->RemoveRoutine(Poll,this);
}

Burger::RunQueue::eReturnCode BURGER_API Burger::Keyboard::Poll(void *pData)
{
	Keyboard *pThis = static_cast<Keyboard *>(pData);
	// Poll the keyboard device for events

	XINPUT_KEYSTROKE KeyStroke;

	// Note: Calling this function without the 2nd parameter set to XINPUT_FLAG_KEYBOARD is a TCR #43
	// violation.

	while (XInputGetKeystroke(XUSER_INDEX_ANY,XINPUT_FLAG_KEYBOARD,&KeyStroke) == ERROR_SUCCESS) {
		// Convert from Windows scan code to Burgerlib
		const ScanCodeTranslation_t *pTranslation = ScanCodeTranslate(KeyStroke.VirtualKey);
		if (pTranslation) {
			// Non-zero if it was a key down event
			uint_t uPressed = (KeyStroke.Flags&(XINPUT_KEYSTROKE_KEYDOWN|XINPUT_KEYSTROKE_REPEAT))!=0;
			// Direct Input keyboard scan code
			uint_t uScanCode = pTranslation->m_uScanCode;
			KeyEvent_t *pNewEvent = &pThis->m_KeyEvents[pThis->m_uArrayEnd];
			if (!uPressed) {
				pNewEvent->m_uEvent = EVENT_KEYUP;
			} else if (KeyStroke.Flags&XINPUT_KEYSTROKE_REPEAT) {
				pNewEvent->m_uEvent = EVENT_KEYAUTO;
			} else {
				pNewEvent->m_uEvent = EVENT_KEYDOWN;
			}
			// Which player pressed the key?
			pNewEvent->m_uWhich = KeyStroke.UserIndex;
			pNewEvent->m_uScanCode = static_cast<uint32_t>(uScanCode);
			pNewEvent->m_uFlags = 0;
			pNewEvent->m_uMSTimeStamp = Tick::ReadMilliseconds();

			if (uPressed) {
				// Mark as pressed
				pThis->m_KeyArray[uScanCode] |= (KEYCAPDOWN|KEYCAPPRESSED);
				uint_t uAscii = KeyStroke.Unicode;
				if (!uAscii) {
					uAscii = pTranslation->m_uAsciiCode;
					if (uAscii) {
						// Control key pressed?
						if ((pThis->m_KeyArray[SC_LEFTCONTROL]&KEYCAPDOWN) || (pThis->m_KeyArray[SC_RIGHTCONTROL]&KEYCAPDOWN)) {
							uAscii = pTranslation->m_uControlCode;
						// Shift key pressed?
						} else if ((pThis->m_KeyArray[SC_LEFTSHIFT]&KEYCAPDOWN) || (pThis->m_KeyArray[SC_RIGHTSHIFT]&KEYCAPDOWN)) {
							uAscii = pTranslation->m_uShiftCode;
						}
					}
				}
				// Store the Ascii value
				pNewEvent->m_uAscii = static_cast<uint32_t>(uAscii);
			} else {
				// There is no ASCII on key up events
				pNewEvent->m_uAscii = 0;
				// Mark as released
				pThis->m_KeyArray[uScanCode] &= (~KEYCAPDOWN);
			}
			// Accept the key in the circular buffer
			uint_t uTemp = (pThis->m_uArrayEnd+1)&(kBufferSize-1);
			if (uTemp!=pThis->m_uArrayStart) {
				// Didn't wrap, accept it!
				pThis->m_uArrayEnd = uTemp;
			}
		}
	}
	return RunQueue::OKAY;
}



#endif


