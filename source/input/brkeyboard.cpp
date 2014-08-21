/***************************************

	Keyboard Manager
	
	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brkeyboard.h"
#include "brstringfunctions.h"

#if defined(BURGER_WINDOWS)
#include <Windows.h>
#endif

/*! ************************************

	\class Burger::Keyboard

	\brief Keyboard input manager.

	This class manages keyboard input down to the individual
	keycap. It's main use is to use the keyboard as a gaming
	input device. Emphasis is on speed, low latency and 
	accuracy.

	When Init() is called, a keyboard monitor is activated
	which will check for all key strokes. There are a maximum of 256 keyboard
	scan codes that can be monitored. Each byte will contain the current status of
	the key by scan code. The lowest bit (mask with 0x01) will determine the
	keydown status at the current moment in time. Bit #1 (mask with 0x02) is set
	when the key is pressed but never cleared unless you call a function
	that clears the flag or Keyboard::Flush() with flushes all events.

	This array is checked by several routines that use the keyboard as a game
	input device.

	The array is volatile since some keyboard monitors run as a separate thread.

	\sa Burger::Joypad and Burger::Mouse

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
	Word m_uScanCode;			///< Actual scan code
};
#endif

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

/*! ************************************

	\brief Default constructor

	Installs the heartbeak task to periodacally
	refresh the m_KeyArray and generate the lists of
	upcoming key events

	\sa Init()

***************************************/

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_XBOX360)) || defined(DOXYGEN)
Burger::Keyboard::Keyboard(GameApp *pAppInstance) :
	m_uArrayStart(0),
	m_uArrayEnd(0),
	m_pAppInstance(pAppInstance)
{
	pAppInstance->SetKeyboard(this);
	MemoryClear(const_cast<Word8 *>(m_KeyArray),sizeof(m_KeyArray));
}

/*! ************************************

	\brief Default destructor

	Shuts down the heartbeat thread and releases 
	all allocated system resources.

***************************************/

Burger::Keyboard::~Keyboard()
{
}

/*! ************************************

	\brief Wait for a keyboard event.

	Wait for a keyboard event, if the event
	is an extended key then translate to
	Burgerlib extended ASCII

***************************************/

Word Burger::Keyboard::GetKeyEvent(void)
{
	m_pAppInstance->Poll();
	// Return nothing
	return 0;
}

/*! ************************************

	\brief Peek at the next keyboard press event

	See if a key is pending from the keyboard, if
	so, return the event without removing it from
	the queue.

	\sa Keyboard::Get()

***************************************/

Word Burger::Keyboard::PeekKeyEvent(void)
{
	return 0;		/* No event */
}

#endif



/*! ************************************

	\brief Clear the key event in the specific scan code.
	If the code is invalid, do nothing

***************************************/

void Burger::Keyboard::ClearKey(Word uScanCode)
{
	if (uScanCode<=SC_MAXENTRY) {
		// Remove the keyboard pressed event
		m_KeyArray[uScanCode] &= (~KEYCAPPRESSED);		
	}
}

/*! ************************************

	\brief Check if a key is pressed at this very moment
	Return a -1 if not found or the SC_ scancode if so

***************************************/

#if defined(BURGER_BIGENDIAN)
#define MASK1 KEYCAPDOWN<<24
#define MASK2 KEYCAPDOWN<<16
#define MASK3 KEYCAPDOWN<<8
#else
#define MASK1 KEYCAPDOWN
#define MASK2 KEYCAPDOWN<<8
#define MASK3 KEYCAPDOWN<<16
#endif

Word Burger::Keyboard::AnyPressed(void)
{
	// Init the pointer
	volatile Word8 *pWork = m_KeyArray;
	WordPtr uCount = (SC_MAXENTRY+1)/sizeof(Word32);
	Word uResult = static_cast<Word>(-1);
	do {
		Word32 uTempVal = ((Word32 *)pWork)[0];		// I am going to perform a Word32
		// Test 4 in a row
		if (uTempVal & ((KEYCAPDOWN<<24)|(KEYCAPDOWN<<16)|(KEYCAPDOWN<<8)|(KEYCAPDOWN))) {
			// Which key was pressed?
			uResult = static_cast<Word>(pWork-m_KeyArray);
			if (!(uTempVal&MASK1)) {			// Was it the first one?
				++uResult;
				if (!(uTempVal&MASK2)) {		// Second?
					++uResult;
					if (!(uTempVal&MASK3)) {	// Third?
						++uResult;				// Must be the fourth
					}
				}
			}
			break;
		}
		pWork+=sizeof(Word32);	// test for speed
	} while (--uCount);			// Count down
	return uResult;
}

/*! ************************************

	\brief Check if the requested key is currently or previously held down.
	If so, return \ref TRUE. If the key has not been pressed or the scan
	code is invalid, return \ref FALSE. This function will not clear
	the event. Use HasBeenPressedClear() to clear the event

	\param uScanCode Valid Scan code
	\return \ref TRUE if the key was pressed, \ref FALSE if not

***************************************/

Word Burger::Keyboard::HasBeenPressed(Word uScanCode)
{
	if ((uScanCode<=SC_MAXENTRY) && (m_KeyArray[uScanCode]&KEYCAPPRESSED)) {
		return TRUE;
	}
	return FALSE;
}

/*! ************************************

	\brief Check if the requested key is currently or previously held down.
	If so, return \ref TRUE
	If the key has not been pressed or the scan code is invalid, return \ref FALSE

***************************************/

Word Burger::Keyboard::HasBeenPressedClear(Word uScanCode)
{
	if (uScanCode<=SC_MAXENTRY) {
		Word uTemp = m_KeyArray[uScanCode];
		if (uTemp&KEYCAPPRESSED) {
			m_KeyArray[uScanCode] = static_cast<Word8>(uTemp&(~KEYCAPPRESSED));
			return TRUE;
		}
	}
	return FALSE;
}

/*! ************************************

	\brief Check if the requested key is currently held down.
	If so, return \ref TRUE
	If the key is not pressed or the scan code is invalid, return \ref FALSE

***************************************/

Word Burger::Keyboard::IsPressed(Word uScanCode)
{
	if (uScanCode<=SC_MAXENTRY) {
		return static_cast<Word>(m_KeyArray[uScanCode]&KEYCAPDOWN);
	}
	return FALSE;
}

/*! ************************************

	Check if a key is pending and return zero if not, else the
	ASCII code
	
	\return Zero if no key is pending, ASCII key if there was a pending key

***************************************/

Word Burger::Keyboard::GetKey(void)
{
	Word uResult = GetKeyEvent();
	if (uResult) {
		do {
			// Key down?
			if (uResult&0x10000) {
				// Isolate the ASCII
				uResult &= 0xFF;
				break;
			}
			// Get the next key
			uResult = GetKeyEvent();
		} while (uResult);
	}
	return uResult;
}

/*! ************************************

	\brief Return a key in lower case

	Check if a key is pending and return zero if not, else the
	ASCII code in lower case

	\return Zero if no key is pending, lower case key if there was a pending key

***************************************/

Word Burger::Keyboard::GetKeyLowerCase(void)
{
	Word uResult = GetKey();	/* Key pending? */
	if (uResult>='A' && uResult<('Z'+1)) {		/* Convert to lower case */
		uResult += 32;
	}
	return uResult;
}

/*! ************************************

	\brief Return a key in upper case

	Check if a key is pending and return zero if not, else the
	ASCII code in upper case

	\return Zero if no key is pending, upper case key if there was a pending key

***************************************/

Word Burger::Keyboard::GetKeyUpperCase(void)
{
	Word uResult = GetKey();
	if (uResult>='a' && uResult<('z'+1)) {		/* Convert to upper case */
		uResult = uResult-32;
	}
	return uResult;
}

/*! ************************************

	\brief Flush the keyboard buffer

***************************************/

void Burger::Keyboard::Flush(void)
{
	while (GetKeyEvent()) {
	}
	MemoryClear(const_cast<Word8 *>(m_KeyArray),sizeof(m_KeyArray));
}

/*! ************************************

	\brief Wait for a keypress

	Wait for a keypress, but call Kbhit() to allow
	screen savers to kick in. 

	\return ASCII code of pressed key

***************************************/

Word Burger::Keyboard::Wait(void)
{
	while (!PeekKeyEvent()) {
#if defined(BURGER_WINDOWS)		/* Sleep the application until a key is pressed */
//		Word uOld = TickWakeUpFlag;
//		TickWakeUpFlag = TRUE;	/* Get wake up events */
		WaitMessage();		/* Any messages arrived? */
//		TickWakeUpFlag = uOld;
#endif
	}	/* Key pending? */
	return GetKeyEvent();	/* Return the key */
}

/*! ************************************

	\brief Convert a string to a scan code
	
	Given a pointer to a string describing a keyboard scan code. Look up
	the scan code and return the value. I return a -1 if there is no
	match.

	\param pString Pointer to a "C" string
	\return	Burgerlib scancode or a -1 if no match is found.	
	\sa ScanCodeToString()

***************************************/

Word Burger::Keyboard::StringToScanCode(const char *pString)
{
	const ScanEntry_t *pEntry = g_ScanCodeNames;		/* Pointer to the array */
	WordPtr uCount = sizeof(g_ScanCodeNames)/sizeof(g_ScanCodeNames[0]);
	do {
		if (!StringCaseCompare(pString,pEntry->m_pName)) {	/* Match? */
			return pEntry->m_uScanCode;		/* Return the match */
		}
		++pEntry;			/* Next entry */
	} while (--uCount);		/* All done? */
	return static_cast<Word>(-1);			/* Return the bogus entry code */
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

void Burger::Keyboard::ScanCodeToString(char *pString,WordPtr uStringSize,Word uScanCode)
{
	if (uStringSize) {
		const ScanEntry_t *pEntry = g_ScanCodeNames;
		WordPtr uCount = sizeof(g_ScanCodeNames)/sizeof(g_ScanCodeNames[0]);
		do {
			if (pEntry->m_uScanCode==uScanCode) {
				StringCopy(pString,uStringSize,pEntry->m_pName);	/* Length of the string */
				return;
			}
			++pEntry;
		} while (--uCount);
		pString[0] = 0;		/* Don't return the string */
	}
}


