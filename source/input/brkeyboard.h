/***************************************

    Keyboard Manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRKEYBOARD_H__
#define __BRKEYBOARD_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRBASE_H__
#include "brbase.h"
#endif

#ifndef __BRGAMEAPP_H__
#include "brgameapp.h"
#endif

#ifndef __BRCRITICALSECTION_H__
#include "brcriticalsection.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

#if defined(BURGER_MACOSX) && !defined(__BRMACOSXTYPES_H__)
#include "brmacosxtypes.h"
#endif

/* BEGIN */
namespace Burger {
class Keyboard : public Base {
    BURGER_DISABLE_COPY(Keyboard);
	BURGER_RTTI_IN_CLASS();
public:
	static const uint_t kBufferSize=128;		///< Number of keystrokes in keyboard cache

	enum {
		KEYCAPDOWN=0x01,	///< If \ref TRUE in m_KeyArray, this key is currently held down
		KEYCAPPRESSED=0x02,	///< If \ref TRUE in m_KeyArray, this key was pressed, but wasn't acknowledged by the application
		KEYCAPTOGGLE=0x4	///< If \ref TRUE in m_KeyArray, this key is toggled "on"
	};

	enum eAsciiCode {
		ASCII_BACKSPACE=0x08,	///< Backspace key
		ASCII_TAB=0x09,			///< Tab key
		ASCII_ENTER=0x0D,		///< Return/enter key
		ASCII_RETURN=0x0D,		///< Return/enter key
		ASCII_ESCAPE=0x1B,		///< Escape key
		ASCII_ESC=0x1B,			///< Escape key
		ASCII_SPACE=0x20,		///< Space bar
		ASCII_F1=0x80,			///< F1 key
		ASCII_F2=0x81,			///< F2 key
		ASCII_F3=0x82,			///< F3 key
		ASCII_F4=0x83,			///< F4 key
		ASCII_F5=0x84,			///< F5 key
		ASCII_F6=0x85,			///< F6 key
		ASCII_F7=0x86,			///< F7 key
		ASCII_F8=0x87,			///< F8 key
		ASCII_F9=0x88,			///< F9 key
		ASCII_F10=0x89,			///< F10 key
		ASCII_F11=0x8A,			///< F11 key
		ASCII_F12=0x8B,			///< F12 key
		ASCII_F13=0x8C,			///< F12 key
		ASCII_F14=0x8D,			///< F12 key
		ASCII_F15=0x8E,			///< F12 key
		ASCII_PAUSE=0x8F,		///< Pause key
		ASCII_SCROLLLOCK=0x090,	///< Scroll lock key
		ASCII_HOME=0x91,		///< Home key
		ASCII_END=0x92,			///< End key
		ASCII_LEFTARROW=0x2190,	///< Left arrow key
		ASCII_UPARROW=0x2191,	///< Up arrow key
		ASCII_RIGHTARROW=0x2192,///< Right arrow key
		ASCII_DOWNARROW=0x2193,	///< Down arrow key
		ASCII_PAGEUP=0x21DE,	///< Page up key
		ASCII_PAGEDOWN=0x21DF,	///< Page down key
		ASCII_DELETE=0x232B,	///< Delete key
		ASCII_INSERT=0x2380,	///< Insert key
		ASCII_PRINTSCREEN=0x2399 ///< Printscreen key
	};

	enum eScanCode {
		SC_INVALID,			///< Zero means no key
		SC_A,				///< A Key
		SC_B,				///< B Key
		SC_C,				///< C Key
		SC_D,				///< D Key
		SC_E,				///< E Key
		SC_F,				///< F Key
		SC_G,				///< G Key
		SC_H,				///< H Key
		SC_I,				///< I Key
		SC_J,				///< J Key
		SC_K,				///< K Key
		SC_L,				///< L Key
		SC_M,				///< M Key
		SC_N,				///< N Key
		SC_O,				///< O Key
		SC_P,				///< P Key
		SC_Q,				///< Q Key
		SC_R,				///< R Key
		SC_S,				///< S Key
		SC_T,				///< T Key
		SC_U,				///< U Key
		SC_V,				///< V Key
		SC_W,				///< W Key
		SC_X,				///< X Key
		SC_Y,				///< Y Key
		SC_Z,				///< Z Key
		SC_FUSS,			///< German ss
		SC_0,				///< )/0 Key
		SC_1,				///< !/1 Key
		SC_2,				///< @/2 Key
		SC_3,				///< #/3 Key
		SC_4,				///< $/4 Key
		SC_5,				///< %/5 Key
		SC_6,				///< ^/6 Key
		SC_7,				///< &/7 Key
		SC_8,				///< */8 Key
		SC_9,				///< (/9 Key
		SC_TILDE,			///< ~/` Key
		SC_UNDERSCORE,		///< _/- Key
		SC_MINUS=SC_UNDERSCORE,	///< _/- Key
		SC_PLUS,			///< +/= Key
		SC_EQUALS=SC_PLUS,	///< +/= Key
		SC_BACKSPACE,		///< Backspace key
		SC_RIGHTBRACE,		///< {/[ key
		SC_RIGHTBRACKET=SC_RIGHTBRACE,	///< {/[ key
		SC_LEFTBRACE,		///< }/] key
		SC_LEFTBRACKET=SC_LEFTBRACE,	///< }/] key
		SC_RETURN,			///< Return key
		SC_QUOTE,			///< "/' key
		SC_GRAVE=SC_QUOTE,	///< "/' key
		SC_COLON,			///< :/; key
		SC_SEMICOLON=SC_COLON,	///< :/; key
		SC_VERTBAR,			///< |/\ key
		SC_BACKSLASH=SC_VERTBAR,	///< |/\ key
		SC_COMMA,			///< </, key
		SC_PERIOD,			///< >/. key
		SC_SLASH,			///< ?// key
		SC_TAB,				///< Tab key
		SC_SPACE,			///< Spacebar
		SC_POWERBOOKENTER,	///< Mac powerbook enter key
		SC_ESCAPE,			///< Escape key
		SC_LEFTCONTROL,		///< Left control key
		SC_RIGHTCONTROL,	///< Right control key
		SC_LEFTOPTION,		///< Left option key
		SC_RIGHTOPTION,		///< Right option key
		SC_LEFTALT,			///< Left Alt/Apple key
		SC_LEFTOPENAPPLE=SC_LEFTALT,	///< Left Alt/Apple key
		SC_RIGHTALT,		///< Right Alt/Apple key
		SC_RIGHTOPENAPPLE=SC_RIGHTALT,	///< Right Alt/Apple key
		SC_LEFTSHIFT,		///< Left shift key
		SC_RIGHTSHIFT,		///< Right shift key
		SC_LEFTMENU,		///< Left menu key
		SC_RIGHTMENU,		///< Right menu key
		SC_CAPSLOCK,		///< Caps lock key
		SC_KEYPADPERIOD,	///< Keypad period
		SC_KEYPADCOMMA,		///< Keypad comma (NEC PC98)
		SC_KEYPADASTERISK,	///< Keypad asterisk
		SC_KEYPADPLUS,		///< Keypad +
		SC_NUMLOCK,			///< Numlock
		SC_KEYPADSLASH,		///< Keypad divide
		SC_KEYPADENTER,		///< Keypad enter
		SC_KEYPADMINUS,		///< Keypad -
		SC_KEYPADEQUALS,	///< Keypad =
		SC_KEYPAD0,			///< Keypad 0
		SC_KEYPAD1,			///< Keypad 1
		SC_KEYPAD2,			///< Keypad 2
		SC_KEYPAD3,			///< Keypad 3
		SC_KEYPAD4,			///< Keypad 4
		SC_KEYPAD5,			///< Keypad 5
		SC_KEYPAD6,			///< Keypad 6
		SC_KEYPAD7,			///< Keypad 7
		SC_KEYPAD8,			///< Keypad 8
		SC_KEYPAD9,			///< Keypad 9
		SC_PRINTSCREEN,		///< Print screen
		SC_SYSRQ=SC_PRINTSCREEN,	///< System	Request
		SC_SCROLLLOCK,		///< Scroll lock
		SC_PAUSE,			///< Pause key
		SC_INSERT,			///< Insert key
		SC_DELETE,			///< Delete key
		SC_HOME,			///< Home key
		SC_END,				///< End key
		SC_PAGEUP,			///< Pageup key
		SC_PAGEDOWN,		///< Pagedown key		
		SC_LEFTARROW,		///< Left arrow key
		SC_RIGHTARROW,		///< Right arrow key
		SC_DOWNARROW,		///< Down arrow key
		SC_UPARROW,			///< Up arrow key
		SC_F1,				///< Function key 1
		SC_F2,				///< Function key 2
		SC_F3,				///< Function key 3
		SC_F4,				///< Function key 4
		SC_F5,				///< Function key 5
		SC_F6,				///< Function key 6
		SC_F7,				///< Function key 7
		SC_F8,				///< Function key 8
		SC_F9,				///< Function key 9
		SC_F10,				///< Function key 10
		SC_F11,				///< Function key 11
		SC_F12,				///< Function key 12
		SC_F13,				///< Function key 13
		SC_F14,				///< Function key 14
		SC_F15,				///< Function key 15
		SC_OEM_102,			///< <> or \| on RT 102-key keyboard (Non-U.S.)
		SC_KANA,			///< (Japanese keyboard)
		SC_ABNT_C1,			///< /? on Brazilian keyboard
		SC_CONVERT,			///< (Japanese keyboard)
		SC_NOCONVERT,		///< (Japanese keyboard)
		SC_YEN,				///< (Japanese keyboard)
		SC_ABNT_C2,			///< Numpad . on Brazilian keyboard
		SC_PREVTRACK,		///< Previous Track (CIRCUMFLEX on Japanese keyboard)
		SC_AT,				///< (NEC PC98)
		SC_COLONPC98,		///< (NEC PC98)
		SC_UNDERLINE,		///< (NEC PC98)
		SC_KANJI,			///< (Japanese keyboard)
		SC_STOP,			///< (NEC PC98)
		SC_AX,				///< (Japan AX)
		SC_UNLABELED,		///< (J3100)
		SC_NEXTTRACK,		///< Next Track
		SC_MUTE,			///< Mute
		SC_CALCULATOR,		///< Calculator
		SC_PLAYPAUSE,		///< Play / Pause
		SC_MEDIASTOP,		///< Media Stop
		SC_VOLUMEDOWN,		///< Volume -
		SC_VOLUMEUP,		///< Volume +
		SC_WEBHOME,			///< Web home
		SC_APPS,			///< AppMenu key
		SC_POWER,			///< System Power
		SC_SLEEP,			///< System Sleep
		SC_WAKE,			///< System Wake
		SC_WEBSEARCH,		///< Web Search
		SC_WEBFAVORITES,	///< Web Favorites
		SC_WEBREFRESH,		///< Web Refresh
		SC_WEBSTOP,			///< Web Stop
		SC_WEBFORWARD,		///< Web Forward
		SC_WEBBACK,			///< Web Back
		SC_MYCOMPUTER,		///< My Computer
		SC_MAIL,			///< Mail
		SC_MEDIASELECT,		///< Media Select
		SC_EXTRA,			///< Extra key codes
		SC_MAXENTRY=255		///< Highest scan code valid value
	};

	enum eKeyFlags {
		FLAG_ALT=0x02,		///< Alt/Open Apple key is held down at the same time
		FLAG_CONTROL=0x04,	///< Control key is held down at the same time
		FLAG_OPTION=0x08,	///< Option key is held down at the same time
		FLAG_SHIFT=0x10,	///< Shift key is held down at the same time
		FLAG_CAPSLOCK=0x40,	///< Caps lock is active,
		FLAG_NUMLOCK=0x80	///< Num lock is active
	};

protected:
	GameApp *m_pAppInstance;			///< Application instances

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	static uintptr_t BURGER_API WindowsKeyboardThread(void *pData);
	IDirectInputDevice8W* m_pKeyboardDevice;	///< DirectInput Device reference (Windows only)
	void *m_pKeyboardEvent;						///< Event signal for DirectInput (Windows only)
	void *m_pKeyboardTimerEvent;				///< Keyboard repeat timer event (Windows only)
	HHOOK__ *m_pPreviousKeyboardHook;			///< Previous keyboard low level hook
	Thread m_KeyboardThread;					///< Asynchronous thread monitoring DirectInput (Windows only)
	uint_t m_bDirectInput8Acquired;				///< \ref TRUE if DirectInput8 is active (Windows only)
	uint_t m_bRepeatActive;						///< \ref TRUE if auto repeat time is active (Windows only)
	volatile uint32_t m_bQuit;					///< \ref TRUE when the thread is shutting down (Windows only)
    Burger_tagSTICKYKEYS m_DefaultStickyKeys;	///< Previous settings for Sticky Keys (Windows only)
    Burger_tagTOGGLEKEYS m_DefaultToggleKeys;	///< Previous settings for Toggle Keys (Windows only)
    Burger_tagFILTERKEYS m_DefaultFilterKeys;	///< Previous settings for Filter Keys (Windows only)
#endif

#if defined(BURGER_XBOX360)
	static RunQueue::eReturnCode BURGER_API Poll(void *pData);
#endif

#if (defined(BURGER_WINDOWS) || defined(BURGER_MACOSX)) || defined(DOXYGEN)
	CriticalSection m_KeyboardLock;			///< Lock for multi-threading (MacOSX and Windows only)
	BURGER_INLINE void Lock(void) { m_KeyboardLock.Lock(); }
	BURGER_INLINE void Unlock(void) { m_KeyboardLock.Unlock(); }
#else
	BURGER_INLINE void Lock(void) {}
	BURGER_INLINE void Unlock(void) {}
#endif

	uint8_t m_KeyArray[SC_MAXENTRY+1];		///< Array with the current state of the keyboard
	uint_t m_uArrayStart;						///< Read index for m_KeyEvents
	uint_t m_uArrayEnd;						///< Write index for m_KeyEvents
	uint_t m_uInitialDelay;					///< Initial delay in ms for autorepeat
	uint_t m_uRepeatDelay;					///< Delay between repeating keystrokes
	KeyEvent_t m_KeyEvents[kBufferSize];	///< Circular buffer holding keyboard events
	KeyEvent_t m_RepeatEvent;				///< Event to post on a repeat

public:
	Keyboard(GameApp *pAppInstance);
	virtual ~Keyboard();
	BURGER_INLINE GameApp *GetApp(void) const { return m_pAppInstance; }
	uint_t BURGER_API PeekKeyEvent(KeyEvent_t *pEvent);
	uint_t BURGER_API GetKeyEvent(KeyEvent_t *pEvent);
	void BURGER_API ClearKey(eScanCode uScanCode);
	eScanCode BURGER_API AnyPressed(void) const;
	uint_t BURGER_API HasBeenPressed(eScanCode uScanCode) const;
	uint_t BURGER_API HasBeenPressedClear(eScanCode uScanCode);
	uint_t BURGER_API IsPressed(eScanCode uScanCode) const;
	uint_t BURGER_API GetKey(void);
	uint_t BURGER_API GetKeyLowerCase(void);
	uint_t BURGER_API GetKeyUpperCase(void);
	void BURGER_API Flush(void);
	uint_t BURGER_API Wait(void);
	uint_t BURGER_API PostKeyDown(eScanCode uScanCode);
	uint_t BURGER_API PostKeyUp(eScanCode uScanCode);
	uint_t BURGER_API PostKey(eScanCode uScanCode);
	uint_t BURGER_API PostUnicodeDown(uint32_t uUnicode);
	uint_t BURGER_API PostUnicodeUp(uint32_t uUnicode);
	uint_t BURGER_API PostUnicode(uint32_t uUnicode);
	uint_t BURGER_API PostKeyEvent(const KeyEvent_t *pEvent);
	uint_t BURGER_API EncodeScanCode(KeyEvent_t *pEvent,eScanCode uScanCode) const;
	uint_t BURGER_API EncodeUnicode(KeyEvent_t *pEvent,uint32_t uUnicode) const;
	uint_t BURGER_API GetCurrentFlags(void) const;
	static eScanCode BURGER_API StringToScanCode(const char *pString);
	static void BURGER_API ScanCodeToString(char *pString,uintptr_t uStringSize,eScanCode uScanCode);

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	uint_t BURGER_API DisableWindowsKey(void);
	void BURGER_API EnableWindowsKey(void);
	BURGER_INLINE HHOOK__ *GetWindowsPreviousKeyboardHook(void) const { return m_pPreviousKeyboardHook; }
	BURGER_INLINE uint_t IsDirectInputActive(void) const { return m_bDirectInput8Acquired; }
	uint_t BURGER_API PostWindowsKeyEvent(eEvent uEvent,uint32_t uScanCode);
	uint_t BURGER_API EncodeWindowsScanCode(KeyEvent_t *pEvent,uint_t uWindowsCode) const;
	void BURGER_API AcquireDirectInput(void);
	void BURGER_API UnacquireDirectInput(void);
	void BURGER_API ReadSystemKeyboardDelays(void);
	void BURGER_API DisableAccessibilityShortcutKeys(void) const;
	void BURGER_API RestoreAccessibilityShortcutKeys(void);
#endif

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
	void BURGER_API ProcessEvent(NSEvent *pEvent);
#endif
};
}
/* END */
#endif
