/***************************************

	Keyboard Manager

	Windows version

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brkeyboard.h"

#if defined(BURGER_WINDOWS)
#define DIRECTINPUT_VERSION 0x0800
#include "brwindowsapp.h"
#include "brglobals.h"
#include "brstringfunctions.h"
#include <dinput.h>

// Number of keyboard events to cache

#define DIRECTINPUT_KEYBOARDBUFFERSIZE 20

// Private scan code to Burgerlib scan code table
struct ScanCodeTranslation_t {
	Word8 m_uWindowsCode;		// Windows scan code
	Word8 m_uScanCode;			// Burgerlib scan code
	Word8 m_uAsciiCode;			// Ascii code
	Word8 m_uShiftCode;			// Shifted ascii code
	Word8 m_uControlCode;		// Control ascii code
};

// Table, first entry is the Windows code, the second is the code for Burgerlib

static const ScanCodeTranslation_t TranslationTable[] = {
	{DIK_A,Burger::Keyboard::SC_A,'a','A',0x01},
	{DIK_B,Burger::Keyboard::SC_B,'b','B',0x02},
	{DIK_C,Burger::Keyboard::SC_C,'c','C',0x03},
	{DIK_D,Burger::Keyboard::SC_D,'d','D',0x04},
	{DIK_E,Burger::Keyboard::SC_E,'e','E',0x05},
	{DIK_F,Burger::Keyboard::SC_F,'f','F',0x06},
	{DIK_G,Burger::Keyboard::SC_G,'g','G',0x07},
	{DIK_H,Burger::Keyboard::SC_H,'h','H',0x08},
	{DIK_I,Burger::Keyboard::SC_I,'i','I',0x09},
	{DIK_J,Burger::Keyboard::SC_J,'j','J',0x0A},
	{DIK_K,Burger::Keyboard::SC_K,'k','K',0x0B},
	{DIK_L,Burger::Keyboard::SC_L,'l','L',0x0C},
	{DIK_M,Burger::Keyboard::SC_M,'m','M',0x0D},
	{DIK_N,Burger::Keyboard::SC_N,'n','N',0x0E},
	{DIK_O,Burger::Keyboard::SC_O,'o','O',0x0F},
	{DIK_P,Burger::Keyboard::SC_P,'p','P',0x10},
	{DIK_Q,Burger::Keyboard::SC_Q,'q','Q',0x11},
	{DIK_R,Burger::Keyboard::SC_R,'r','R',0x12},
	{DIK_T,Burger::Keyboard::SC_T,'s','S',0x13},
	{DIK_S,Burger::Keyboard::SC_S,'t','T',0x14},
	{DIK_U,Burger::Keyboard::SC_U,'u','U',0x15},
	{DIK_V,Burger::Keyboard::SC_V,'v','V',0x16},
	{DIK_W,Burger::Keyboard::SC_W,'w','W',0x17},
	{DIK_X,Burger::Keyboard::SC_X,'x','X',0x18},
	{DIK_Y,Burger::Keyboard::SC_Y,'y','Y',0x19},
	{DIK_Z,Burger::Keyboard::SC_Z,'z','Z',0x1A},

	{DIK_SPACE,Burger::Keyboard::SC_SPACE,Burger::Keyboard::ASCII_SPACE},
	{DIK_ESCAPE,Burger::Keyboard::SC_ESCAPE,Burger::Keyboard::ASCII_ESCAPE},
	{DIK_RETURN,Burger::Keyboard::SC_RETURN,Burger::Keyboard::ASCII_RETURN},
	{DIK_TAB,Burger::Keyboard::SC_TAB,Burger::Keyboard::ASCII_TAB},
	{DIK_UP,Burger::Keyboard::SC_UPARROW,Burger::Keyboard::ASCII_UPARROW},
	{DIK_DOWN,Burger::Keyboard::SC_DOWNARROW,Burger::Keyboard::ASCII_DOWNARROW},
	{DIK_LEFT,Burger::Keyboard::SC_LEFTARROW,Burger::Keyboard::ASCII_LEFTARROW},
	{DIK_RIGHT,Burger::Keyboard::SC_RIGHTARROW,Burger::Keyboard::ASCII_RIGHTARROW},

	{DIK_0,Burger::Keyboard::SC_0,'0',')'},
	{DIK_1,Burger::Keyboard::SC_1,'1','!'},
	{DIK_2,Burger::Keyboard::SC_2,'2','@'},
	{DIK_3,Burger::Keyboard::SC_3,'3','#'},
	{DIK_4,Burger::Keyboard::SC_4,'4','$'},
	{DIK_5,Burger::Keyboard::SC_5,'5','%'},
	{DIK_6,Burger::Keyboard::SC_6,'6','^'},
	{DIK_7,Burger::Keyboard::SC_7,'7','&'},
	{DIK_8,Burger::Keyboard::SC_8,'8','*'},
	{DIK_9,Burger::Keyboard::SC_9,'9','('},

	{DIK_HOME,Burger::Keyboard::SC_HOME,Burger::Keyboard::ASCII_HOME},
	{DIK_END,Burger::Keyboard::SC_END,Burger::Keyboard::ASCII_END},
	{DIK_PRIOR,Burger::Keyboard::SC_PAGEUP,Burger::Keyboard::ASCII_PAGEUP},
	{DIK_NEXT,Burger::Keyboard::SC_PAGEDOWN,Burger::Keyboard::ASCII_PAGEDOWN},
	{DIK_INSERT,Burger::Keyboard::SC_INSERT,Burger::Keyboard::ASCII_INSERT},
	{DIK_DELETE,Burger::Keyboard::SC_DELETE,Burger::Keyboard::ASCII_DELETE},
	{DIK_SYSRQ,Burger::Keyboard::SC_SYSRQ,Burger::Keyboard::ASCII_PRINTSCREEN},
	{DIK_PAUSE,Burger::Keyboard::SC_PAUSE,Burger::Keyboard::ASCII_PAUSE},
	{DIK_SCROLL,Burger::Keyboard::SC_SCROLLLOCK,Burger::Keyboard::ASCII_SCROLLLOCK},

	{DIK_GRAVE,Burger::Keyboard::SC_GRAVE,'`','~'},
	{DIK_MINUS,Burger::Keyboard::SC_MINUS,'-','_'},
	{DIK_EQUALS,Burger::Keyboard::SC_EQUALS,'=','+'},
	{DIK_BACK,Burger::Keyboard::SC_BACKSPACE,Burger::Keyboard::ASCII_BACKSPACE},
	{DIK_LBRACKET,Burger::Keyboard::SC_LEFTBRACKET,'[','{'},
	{DIK_RBRACKET,Burger::Keyboard::SC_RIGHTBRACKET,']','}'},
	{DIK_BACKSLASH,Burger::Keyboard::SC_BACKSLASH,'\\','|'},
	{DIK_SEMICOLON ,Burger::Keyboard::SC_SEMICOLON,';',':'},
	{DIK_APOSTROPHE,Burger::Keyboard::SC_QUOTE,'\'','"'},
	{DIK_COMMA,Burger::Keyboard::SC_COMMA,',','<'},
	{DIK_PERIOD,Burger::Keyboard::SC_PERIOD,'.','>'},
	{DIK_SLASH,Burger::Keyboard::SC_SLASH,'/','?'},

	{DIK_LCONTROL,Burger::Keyboard::SC_LEFTCONTROL},
	{DIK_RCONTROL,Burger::Keyboard::SC_RIGHTCONTROL},
	{DIK_LSHIFT,Burger::Keyboard::SC_LEFTSHIFT},
	{DIK_RSHIFT,Burger::Keyboard::SC_RIGHTSHIFT},
	{DIK_LWIN,Burger::Keyboard::SC_LEFTOPTION},
	{DIK_RWIN,Burger::Keyboard::SC_RIGHTOPTION},
	{DIK_LMENU,Burger::Keyboard::SC_LEFTMENU},
	{DIK_RMENU,Burger::Keyboard::SC_RIGHTMENU},
	{DIK_CAPITAL,Burger::Keyboard::SC_CAPSLOCK},

	{DIK_F1,Burger::Keyboard::SC_F1,Burger::Keyboard::ASCII_F1},
	{DIK_F2,Burger::Keyboard::SC_F2,Burger::Keyboard::ASCII_F2},
	{DIK_F3,Burger::Keyboard::SC_F3,Burger::Keyboard::ASCII_F3},
	{DIK_F4,Burger::Keyboard::SC_F4,Burger::Keyboard::ASCII_F4},
	{DIK_F5,Burger::Keyboard::SC_F5,Burger::Keyboard::ASCII_F5},
	{DIK_F6,Burger::Keyboard::SC_F6,Burger::Keyboard::ASCII_F6},
	{DIK_F7,Burger::Keyboard::SC_F7,Burger::Keyboard::ASCII_F7},
	{DIK_F8,Burger::Keyboard::SC_F8,Burger::Keyboard::ASCII_F8},
	{DIK_F9,Burger::Keyboard::SC_F9,Burger::Keyboard::ASCII_F9},
	{DIK_F10,Burger::Keyboard::SC_F10,Burger::Keyboard::ASCII_F10},
	{DIK_F11,Burger::Keyboard::SC_F11,Burger::Keyboard::ASCII_F11},
	{DIK_F12,Burger::Keyboard::SC_F12,Burger::Keyboard::ASCII_F12},
	{DIK_F13,Burger::Keyboard::SC_F13,Burger::Keyboard::ASCII_F13},
	{DIK_F14,Burger::Keyboard::SC_F14,Burger::Keyboard::ASCII_F14},
	{DIK_F15,Burger::Keyboard::SC_F15,Burger::Keyboard::ASCII_F15},

	{DIK_NUMLOCK,Burger::Keyboard::SC_NUMLOCK},
	{DIK_NUMPAD0,Burger::Keyboard::SC_KEYPAD0,Burger::Keyboard::ASCII_INSERT,'0'},
	{DIK_NUMPAD1,Burger::Keyboard::SC_KEYPAD1,Burger::Keyboard::ASCII_END,'1'},
	{DIK_NUMPAD2,Burger::Keyboard::SC_KEYPAD2,Burger::Keyboard::ASCII_DOWNARROW,'2'},
	{DIK_NUMPAD3,Burger::Keyboard::SC_KEYPAD3,Burger::Keyboard::ASCII_PAGEDOWN,'3'},
	{DIK_NUMPAD4,Burger::Keyboard::SC_KEYPAD4,Burger::Keyboard::ASCII_LEFTARROW,'4'},
	{DIK_NUMPAD5,Burger::Keyboard::SC_KEYPAD5,'5','5'},
	{DIK_NUMPAD6,Burger::Keyboard::SC_KEYPAD6,Burger::Keyboard::ASCII_RIGHTARROW,'6'},
	{DIK_NUMPAD7,Burger::Keyboard::SC_KEYPAD7,Burger::Keyboard::ASCII_HOME,'7'},
	{DIK_NUMPAD8,Burger::Keyboard::SC_KEYPAD8,Burger::Keyboard::ASCII_UPARROW,'8'},
	{DIK_NUMPAD9,Burger::Keyboard::SC_KEYPAD9,Burger::Keyboard::ASCII_PAGEUP,'9'},
	{DIK_SUBTRACT,Burger::Keyboard::SC_KEYPADMINUS,'-'},
	{DIK_ADD,Burger::Keyboard::SC_KEYPADPLUS,'+'},
	{DIK_DECIMAL,Burger::Keyboard::SC_KEYPADPERIOD,'.'},
	{DIK_MULTIPLY,Burger::Keyboard::SC_KEYPADASTERISK,'*'},
	{DIK_NUMPADEQUALS,Burger::Keyboard::SC_KEYPADEQUALS,'='},
	{DIK_NUMPADENTER,Burger::Keyboard::SC_KEYPADENTER,Burger::Keyboard::ASCII_ENTER},
	{DIK_NUMPADCOMMA,Burger::Keyboard::SC_KEYPADCOMMA,','},
	{DIK_DIVIDE,Burger::Keyboard::SC_KEYPADSLASH,'/'},

	{DIK_OEM_102,Burger::Keyboard::SC_OEM_102},
	{DIK_KANA,Burger::Keyboard::SC_KANA},
	{DIK_ABNT_C1,Burger::Keyboard::SC_ABNT_C1},
	{DIK_CONVERT,Burger::Keyboard::SC_CONVERT},
	{DIK_NOCONVERT,Burger::Keyboard::SC_NOCONVERT},
	{DIK_YEN,Burger::Keyboard::SC_YEN},
	{DIK_ABNT_C2,Burger::Keyboard::SC_ABNT_C2},
	{DIK_PREVTRACK,Burger::Keyboard::SC_PREVTRACK},
	{DIK_AT,Burger::Keyboard::SC_AT},
	{DIK_COLON,Burger::Keyboard::SC_COLONPC98},
	{DIK_UNDERLINE,Burger::Keyboard::SC_UNDERLINE},
	{DIK_KANJI,Burger::Keyboard::SC_KANJI},
	{DIK_STOP,Burger::Keyboard::SC_STOP},
	{DIK_AX,Burger::Keyboard::SC_AX},
	{DIK_UNLABELED,Burger::Keyboard::SC_UNLABELED},
	{DIK_NEXTTRACK,Burger::Keyboard::SC_NEXTTRACK},
	{DIK_MUTE,Burger::Keyboard::SC_MUTE},
	{DIK_CALCULATOR,Burger::Keyboard::SC_CALCULATOR},
	{DIK_PLAYPAUSE,Burger::Keyboard::SC_PLAYPAUSE},
	{DIK_MEDIASTOP,Burger::Keyboard::SC_MEDIASTOP},
	{DIK_VOLUMEDOWN,Burger::Keyboard::SC_VOLUMEDOWN},
	{DIK_VOLUMEUP,Burger::Keyboard::SC_VOLUMEUP},
	{DIK_WEBHOME,Burger::Keyboard::SC_WEBHOME},
	{DIK_APPS,Burger::Keyboard::SC_APPS},
	{DIK_POWER,Burger::Keyboard::SC_POWER},
	{DIK_SLEEP,Burger::Keyboard::SC_SLEEP},
	{DIK_WAKE,Burger::Keyboard::SC_WAKE},
	{DIK_WEBSEARCH,Burger::Keyboard::SC_WEBSEARCH},
	{DIK_WEBFAVORITES,Burger::Keyboard::SC_WEBFAVORITES},
	{DIK_WEBREFRESH,Burger::Keyboard::SC_WEBREFRESH},
	{DIK_WEBSTOP,Burger::Keyboard::SC_WEBSTOP},
	{DIK_WEBFORWARD,Burger::Keyboard::SC_WEBFORWARD},
	{DIK_WEBBACK,Burger::Keyboard::SC_WEBBACK},
	{DIK_MYCOMPUTER,Burger::Keyboard::SC_MYCOMPUTER},
	{DIK_MAIL,Burger::Keyboard::SC_MAIL},
	{DIK_MEDIASELECT,Burger::Keyboard::SC_MEDIASELECT}
};

/***************************************

	Convert a scan code to an ascii code.

***************************************/

static const ScanCodeTranslation_t *ScanCodeTranslate(Word uWindowsCode)
{
	// Get the table address
	const ScanCodeTranslation_t *pTranslationTable = TranslationTable;
	Word i = BURGER_ARRAYSIZE(TranslationTable);
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
	m_uArrayStart(0),
	m_uArrayEnd(0),
	m_pAppInstance(pAppInstance),
	m_pKeyboardDevice(NULL)
{
	pAppInstance->SetKeyboard(this);
	// Clear my variables
	MemoryClear(const_cast<Word8 *>(m_KeyArray),sizeof(m_KeyArray));

	IDirectInput8W* pDirectInput8W;

	// First step, obtain DirectInput

	if (!Burger::Globals::DirectInput8Create(&pDirectInput8W)) {

		// Create a system keyboard device (Merges all keyboards)

		IDirectInputDevice8W *pKeyboardDeviceLocal = NULL;
		HRESULT hResult = pDirectInput8W->CreateDevice(GUID_SysKeyboard,&pKeyboardDeviceLocal,NULL);
		if (hResult>=0) {
			// Set the default data format of 256 "buttons"
			IDirectInputDevice8W *pKeyboardDevice = pKeyboardDeviceLocal;
			hResult = pKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
			if (hResult>=0) {

				// Play nice with the system and let others use the keyboard
				// Disable the windows key if the application is running in the foreground

				hResult = pKeyboardDevice->SetCooperativeLevel(static_cast<WindowsApp *>(pAppInstance)->GetWindow(),DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
				if (hResult>=0) {
					DIPROPDWORD KeyboardProperties;
					
					KeyboardProperties.diph.dwSize = sizeof(DIPROPDWORD);
					KeyboardProperties.diph.dwHeaderSize = sizeof(DIPROPHEADER);
					KeyboardProperties.diph.dwObj = 0;
					KeyboardProperties.diph.dwHow = DIPH_DEVICE;
					KeyboardProperties.dwData = DIRECTINPUT_KEYBOARDBUFFERSIZE;
					hResult = pKeyboardDevice->SetProperty(DIPROP_BUFFERSIZE,&KeyboardProperties.diph);
					if (hResult>=0) {
						pKeyboardDevice->Acquire();
						pAppInstance->AddRoutine(Poll,this);
					}
				}
			}
			if (hResult<0) {
				// Uh oh...
				pKeyboardDevice->Unacquire();
				pKeyboardDevice->Release();
			} else {
				m_pKeyboardDevice = pKeyboardDevice;
			}
		}
	}
}

/***************************************

	Release DirectInput for an event driven keyboard

***************************************/

Burger::Keyboard::~Keyboard()
{
	m_pAppInstance->SetKeyboard(NULL);
	m_pAppInstance->RemoveRoutine(Poll,this);
	// Was a device allocated?
	IDirectInputDevice8W *pKeyboardDevice = m_pKeyboardDevice;
	if (pKeyboardDevice) {
		// Release it!
		pKeyboardDevice->Unacquire();
		pKeyboardDevice->Release();
		m_pKeyboardDevice = NULL;
	}
}

/***************************************

	\brief Peek at the next keyboard press event

	See if a key is pending from the keyboard, if
	so, return the event without removing it from
	the queue.

	\sa Keyboard::Get()

***************************************/

Word Burger::Keyboard::PeekKeyEvent(void)
{
	Word uIndex = m_uArrayStart;		/* Get the starting index */
	if (uIndex!=m_uArrayEnd) {	/* Anything in the buffer? */
		const KeyEvent_t *pEvent = &m_KeyEvents[uIndex];
		return static_cast<Word>(pEvent->m_bAscii|(pEvent->m_bScanCode<<8U)|(pEvent->m_bKeyPressed<<16U));
	}
	// No event pending
	return 0;
}


/***************************************

	\brief Return key up and down events

	Get a key from the keyboard buffer but include key up events

***************************************/

Word Burger::Keyboard::GetKeyEvent(void)
{
	m_pAppInstance->Poll();
	Word uResult = 0;
	Word uIndex = m_uArrayStart;		/* Get the starting index */
	if (uIndex!=m_uArrayEnd) {	/* Anything in the buffer? */
		const KeyEvent_t *pEvent = &m_KeyEvents[uIndex];
		uResult = static_cast<Word>(pEvent->m_bAscii|(pEvent->m_bScanCode<<8U)|(pEvent->m_bKeyPressed<<16U));
		m_uArrayStart = (uIndex+1)&(KEYBUFFSIZE-1);	/* Next key */
	}
	// No event pending
	return uResult;
}

Burger::RunQueue::eReturnCode BURGER_API Burger::Keyboard::Poll(void *pData)
{
	Keyboard *pThis = static_cast<Keyboard *>(pData);
	// Poll the keyboard device for events
	IDirectInputDevice8W *pKeyboardDevice = pThis->m_pKeyboardDevice;
	if (pKeyboardDevice) {

		// Buffer to receive the keyboard events
		DIDEVICEOBJECTDATA KeyboardData[DIRECTINPUT_KEYBOARDBUFFERSIZE];
		DWORD uCount = DIRECTINPUT_KEYBOARDBUFFERSIZE;

		// Let's get the data from the keyboard device
		HRESULT hResult = pKeyboardDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),KeyboardData,&uCount,0);
		if (hResult<0) {
			// Try getting the keyboard again
			if ((hResult == DIERR_INPUTLOST) || (hResult == DIERR_NOTACQUIRED)) {
				hResult = pKeyboardDevice->Acquire();
				if (hResult>=0) {
					// The keyboard was reacquired, pull the data
					uCount = DIRECTINPUT_KEYBOARDBUFFERSIZE;
					hResult = pKeyboardDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),KeyboardData,&uCount,0);
				}
			}
		}

		// Was there any data read?
		if (hResult>=0) {
			Word i = uCount;
			if (i) {
				const DIDEVICEOBJECTDATA *pObject = KeyboardData;
				do {
					// Convert from Windows scan code to Burgerlib
					const ScanCodeTranslation_t *pTranslation = ScanCodeTranslate(pObject->dwOfs);
					if (pTranslation) {
						// Non-zero if it was a key down event
						Word uPressed = (pObject->dwData&0x80)>>7;
						// Direct Input keyboard scan code
						Word uScanCode = pTranslation->m_uScanCode;
						KeyEvent_t *pNewEvent = &pThis->m_KeyEvents[pThis->m_uArrayEnd];
						pNewEvent->m_bScanCode = static_cast<Word8>(uScanCode);
						pNewEvent->m_bKeyPressed = static_cast<Word8>(uPressed);
						if (uPressed) {
							// Mark as pressed
							pThis->m_KeyArray[uScanCode] |= (KEYCAPDOWN|KEYCAPPRESSED);
							Word uAscii = pTranslation->m_uAsciiCode;
							if (uAscii) {
								// Control key pressed?
								if ((pThis->m_KeyArray[SC_LEFTCONTROL]&KEYCAPDOWN) || (pThis->m_KeyArray[SC_RIGHTCONTROL]&KEYCAPDOWN)) {
									uAscii = pTranslation->m_uControlCode;
								// Shift key pressed?
								} else if ((pThis->m_KeyArray[SC_LEFTSHIFT]&KEYCAPDOWN) || (pThis->m_KeyArray[SC_RIGHTSHIFT]&KEYCAPDOWN)) {
									uAscii = pTranslation->m_uShiftCode;
								}
							}
							// Store the Ascii value
							pNewEvent->m_bAscii = static_cast<Word8>(uAscii);
						} else {
							// There is no ASCII on key up events
							pNewEvent->m_bAscii = 0;
							// Mark as released
							pThis->m_KeyArray[uScanCode] &= (~KEYCAPDOWN);
						}
						// Accept the key in the circular buffer
						Word uTemp = (pThis->m_uArrayEnd+1)&(KEYBUFFSIZE-1);
						if (uTemp!=pThis->m_uArrayStart) {
							// Didn't wrap, accept it!
							pThis->m_uArrayEnd = uTemp;
						}
					}
					++pObject;
				} while (--i);
			}
		}
	}
	return RunQueue::OKAY;
}

void Burger::Keyboard::Acquire(void)
{
	IDirectInputDevice8W *pKeyboardDevice = m_pKeyboardDevice;
	if (pKeyboardDevice) {
		pKeyboardDevice->Acquire();
	}
}

void Burger::Keyboard::Unacquire(void)
{
	IDirectInputDevice8W *pKeyboardDevice = m_pKeyboardDevice;
	if (pKeyboardDevice) {
		pKeyboardDevice->Unacquire();
	}
}


#endif


