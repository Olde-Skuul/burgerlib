/***************************************

	Game Application startup class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRGAMEAPP_H__
#define __BRGAMEAPP_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRBASE_H__
#include "brbase.h"
#endif

#ifndef __BRMEMORYHANDLE_H__
#include "brmemoryhandle.h"
#endif

#ifndef __BRRUNQUEUE_H__
#include "brrunqueue.h"
#endif

#ifndef __BRRECT_H__
#include "brrect.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

#if defined(BURGER_IOS) && !defined(__BRIOSTYPES_H__)
#include "briostypes.h"
#endif

#if defined(BURGER_MACOSX) && !defined(__BRMACOSXTYPES_H__)
#include "brmacosxtypes.h"
#endif

#if defined(BURGER_XBOX360) && !defined(__BRXBOX360TYPES_H__)
#include "brxbox360types.h"
#endif

/* BEGIN */
namespace Burger {
class Keyboard;
class Joypad;
class Mouse;
class SoundManager;
class Display;
class Renderer;

enum eEvent {
	EVENT_NULL,				///< Keyboard null event
	EVENT_TIMER,			///< Keyboard timer event
	EVENT_KEYDOWN,			///< Keyboard keypress event
	EVENT_KEYUP,			///< Keyboard key up event
	EVENT_KEYAUTO,			///< Keyboard repeated key event
	EVENT_MOUSEDOWN,		///< Mouse button down event
	EVENT_MOUSEUP,			///< Mouse button up event
	EVENT_MOUSEMOVE,		///< Mouse motion event
	EVENT_MOUSEPOSITION,	///< Mouse position event
	EVENT_MOUSEWHEEL,		///< Mouse wheel event
	EVENT_JOYPADDOWN,		///< Joypad button press
	EVENT_JOYPADUP,			///< Joypad button release
	EVENT_JOYPADAXIS,		///< Joypad axis motion
	EVENT_COUNT				///< Maximum event code
};

struct EventHeader_t {
	eEvent m_uEvent;		///< Event type
	Word m_uWhich;			///< Which device generated the event
	Word32 m_uMSTimeStamp;	///< Time when the event occurred
};

struct KeyEvent_t : public EventHeader_t {
	Word32 m_uAscii;		///< Unicode ASCII value
	Word32 m_uFlags;		///< Flags for key modifiers
	Word32 m_uScanCode;		///< Scan code of the key
};

struct MouseButtonEvent_t : public EventHeader_t {
	Word32 m_uX;			///< Absolute X position on the screen
	Word32 m_uY;			///< Absolute Y position on the screen
	Word32 m_uButtons;		///< Mouse buttons
};

struct MouseWheelEvent_t : public EventHeader_t {
	Int32 m_iMouseWheelX;	///< Horizontal mouse wheel motion (Apple ball mice support this)
	Int32 m_iMouseWheelY;	///< Vertical mouse wheel motion (Most mice support this)
};

struct MouseMotionEvent_t : public EventHeader_t {
	Int32 m_iDeltaX;		///< Relative X motion
	Int32 m_iDeltaY;		///< Relative Y motion
};

struct MousePositionEvent_t : public EventHeader_t {
	Word32 m_uX;			///< Absolute X position on the screen
	Word32 m_uY;			///< Absolute Y position on the screen
};

struct JoypadButtonEvent_t : public EventHeader_t {
	Word m_uButtons;		///< Pressed or released joypad buttons
};

class GameApp : public Base {
	BURGER_DISABLECOPYCONSTRUCTORS(GameApp);
	BURGER_RTTI_IN_CLASS();
protected:
	Keyboard *m_pKeyboard;			///< Pointer to the active keyboard instance
	Mouse *m_pMouse;				///< Pointer to the active mouse instance
	Joypad *m_pJoypad;				///< Pointer to the active joypad instance
	SoundManager *m_pSoundManager;	///< Pointer to the active sound manager instance
	Display *m_pDisplay;			///< Pointer to the active display instance
	const char **m_ppArgv;			///< Arguments passed to the console
	MemoryManagerGlobalHandle m_MemoryManagerHandle;	///< Handle based memory manager for the app
	RunQueue m_RunQueue;			///< Internal run queue

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
public:
	typedef Word(BURGER_API *MainWindowProc)(GameApp *pThis,HWND__ *hWnd,Word uMessage,WordPtr wParam,WordPtr lParam,WordPtr *pOutput);	///< Callback custom window message handler
protected:
	HINSTANCE__ *m_hInstance;		///< (Windows only) Current instance of the application
	HWND__ *m_hWindow;				///< (Windows only) Main window for the application
	HMENU__ *m_hMenu;				///< (Windows only) Menu bar for the application
	HICON__ *m_pDefaultCursor;		///< (Windows only) Default cursor when not in the game area
	const char **m_ppOldArgv;		///< (Windows only) Saved pointer to the global __argv for restoring on exit
	MainWindowProc m_pCallBack;		///< (Windows only) Custom application window proc
	Rect_t m_WindowRect;			///< (Windows only) Bounds rect of the window
	Word m_uErrorMode;				///< (Windows only) Initial GetErrorMode() value
	Word8 m_bCoCreateInstanceInit;	///< (Windows only) \ref TRUE if CoInitializeEx() was initialized
	Word8 m_bInSizeMove;			///< (Windows only) \ref TRUE if the window is being resized
#endif

#if defined(BURGER_IOS) || defined(DOXYGEN)
	UIWindow *m_pWindow;					///< (iOS only) Pointer to the UIWindow for the iOS application
	UIViewController *m_pViewController;	///< (iOS only) Pointer to the UIViewController to the window
	UIView *m_pView;						///< (iOS only) Pointer to the UIView being used
	static GameApp *g_piOSApp;				///< (iOS only) Global singular application instance
#endif

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
	NSApplication *m_pApplication;			///< Pointer to the current application context (Copy of NSApp)
	NSApplicationDelegate *m_pApplicationDelegate;	///< Pointer to the current application delegate
	NSWindow *m_pWindow;					///< Game window
	NSResponder *m_pListener;               ///< Main listener for window events
	Word m_bCenterWindow;					///< \ref TRUE if the window needs to be centered
	static RunQueue::eReturnCode BURGER_API EventPoll(void *pData);
#endif

	int m_iArgc;					///< Number of valid arguments
	Word8 m_bQuit;					///< Set to \ref TRUE when the application has to shut down
	Word8 m_bInBackground;			///< Set to \ref TRUE if the game should be paused due to another window being in front
	Word8 m_bMinimized;				///< Set to \ref TRUE if the game is minimized (PC/Mac only, always false on consoles)
	Word8 m_bAllowWindowSwitching;	///< Set to \ref TRUE if the game allows switching to windowed mode via Alt-Enter on desktop/laptops
	Word8 m_bMouseOnScreen;			///< \ref TRUE if the mouse cursor is on the screen in windows systems
	Word8 m_bWindowSwitchRequested;	///< \ref TRUE if the window resolution switch key was pressed
	Word8 m_bKeyboardStarted;		///< \ref TRUE if a \ref Keyboard class was started with a call to StartupKeyboard(void)
	Word8 m_bMouseStarted;			///< \ref TRUE if a \ref Mouse class was started with a call to StartupMouse(void)
	Word8 m_bJoypadStarted;			///< \ref TRUE if a \ref Joypad class was started with a call to StartupJoypad(void)
	Word8 m_bDisplayStarted;		///< \ref TRUE if a \ref Display class was started with a call to StartupDisplay(void)
	Word8 m_bSoundManagerStarted;	///< \ref TRUE if a \ref SoundManager class was started with a call to StartupSoundManager(void)

	void BURGER_API InitDefaults(void);
	void BURGER_API ShutdownDefaults(void);

public:
	GameApp(WordPtr uDefaultMemorySize = Burger::MemoryManagerHandle::DEFAULTMEMORYCHUNK,Word uDefaultHandleCount = Burger::MemoryManagerHandle::DEFAULTHANDLECOUNT,WordPtr uMinReserveSize = Burger::MemoryManagerHandle::DEFAULTMINIMUMRESERVE);
	virtual ~GameApp();

	BURGER_INLINE void Poll(void) { m_RunQueue.Call(); };
	BURGER_INLINE void AddRoutine(RunQueue::CallbackProc Proc,RunQueue::CallbackProc pShutdown,void *pData,Word uPriority = RunQueue::PRIORITY_MEDIUM) { m_RunQueue.Add(Proc,pShutdown,pData,uPriority); }
	BURGER_INLINE void RemoveRoutine(RunQueue::CallbackProc Proc,void *pData) { m_RunQueue.Remove(Proc,pData); }
	BURGER_INLINE RunQueue *GetRunQueue(void) { return &m_RunQueue; }

	Keyboard * BURGER_API StartupKeyboard(void);
	Mouse * BURGER_API StartupMouse(void);
	Joypad * BURGER_API StartupJoypad(void);
	Display * BURGER_API StartupDisplay(void);
	SoundManager * BURGER_API StartupSoundManager(void);
	Word BURGER_API StartupEverything(void);

	BURGER_INLINE MemoryManagerGlobalHandle * GetMemoryManager(void) { return &m_MemoryManagerHandle; }

	BURGER_INLINE void SetArgv(const char **ppArgv) { m_ppArgv = ppArgv; }
	BURGER_INLINE const char ** GetArgv(void) const { return m_ppArgv; }
	BURGER_INLINE void SetArgc(int iArgc) { m_iArgc = iArgc; }
	BURGER_INLINE int GetArgc(void) const { return m_iArgc; }

	BURGER_INLINE void SetQuitCode(void) { m_bQuit = TRUE; }
	BURGER_INLINE void ClearQuitCode(void) { m_bQuit = FALSE; }
	BURGER_INLINE Word GetQuitCode(void) const { return m_bQuit; }

	BURGER_INLINE void SetInBackground(Word bInBackground) { m_bInBackground = (bInBackground != 0); }
	BURGER_INLINE Word IsInBackground(void) const { return m_bInBackground; }

	void BURGER_API SetKeyboard(Keyboard *pKeyboard);
	BURGER_INLINE Keyboard *GetKeyboard(void) const { return m_pKeyboard; }

	void BURGER_API SetMouse(Mouse *pMouse);
	BURGER_INLINE Mouse *GetMouse(void) const { return m_pMouse; }

	void BURGER_API SetJoypad(Joypad *pJoypad);
	BURGER_INLINE Joypad *GetJoypad(void) const { return m_pJoypad; }

	void BURGER_API SetSoundManager(SoundManager *pSoundManager);
	BURGER_INLINE SoundManager *GetSoundManager(void) const { return m_pSoundManager; }

	void BURGER_API SetDisplay(Display *pDisplay);
	BURGER_INLINE Display *GetDisplay(void) const { return m_pDisplay; }

	BURGER_INLINE void SetWindowSwitching(Word bAllowWindowSwitching) { m_bAllowWindowSwitching = (bAllowWindowSwitching != 0); }
	BURGER_INLINE Word IsWindowSwitchingAllowed(void) const { return m_bAllowWindowSwitching; }
	Word BURGER_API SwitchVideo(void);

	BURGER_INLINE Word IsMinimized(void) const { return m_bMinimized; }
	BURGER_INLINE void SetMinimized(Word bMinimized) { m_bMinimized = (bMinimized != 0); }

	Word BURGER_API IsAppFullScreen(void) const;
	Word BURGER_API IsResizingAllowed(void) const;

	BURGER_INLINE void SetMouseOnScreen(Word bMouseOnScreen) { m_bMouseOnScreen = (bMouseOnScreen != 0); }
	BURGER_INLINE Word IsMouseOnScreen(void) const { return m_bMouseOnScreen; }

	BURGER_INLINE void SetWindowSwitchRequested(Word bWindowSwitchRequested) { m_bWindowSwitchRequested = (bWindowSwitchRequested != 0); }
	BURGER_INLINE Word IsWindowSwitchRequested(void) const { return m_bWindowSwitchRequested; }

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	BURGER_INLINE HINSTANCE__ *GetInstance(void) const { return m_hInstance; }
	BURGER_INLINE HWND__ *GetWindow(void) const { return m_hWindow; }
	BURGER_INLINE HMENU__ *GetMenu(void) const { return m_hMenu; }
	BURGER_INLINE void SetMenu(HMENU__ *hMenu) { m_hMenu = hMenu; }
	BURGER_INLINE MainWindowProc GetCallBack(void) const { return m_pCallBack; }
	BURGER_INLINE Word GetInSizeMove(void) const { return m_bInSizeMove; }
	BURGER_INLINE void SetInSizeMove(Word bInSizeMode) { m_bInSizeMove = (bInSizeMode != 0); }
	int BURGER_API InitWindow(const char *pGameName,MainWindowProc pCallBack,Word uIconResID=0);
	int BURGER_API SetWindowSize(Word uWidth,Word uHeight);
	int BURGER_API SetWindowFullScreen(Word uWidth,Word uHeight);
	void BURGER_API ResetWindowLocation(void);
	void BURGER_API RecordWindowLocation(void);
	Word BURGER_API HandleCursor(Word uParam);
	void BURGER_API GetInputFocus(void);
	void BURGER_API KillInputFocus(void);
	static RunQueue::eReturnCode BURGER_API Poll(void *pSelf);
#endif

#if defined(BURGER_IOS) || defined(DOXYGEN)
	int BURGER_API Run(void);
	UIWindow *BURGER_API CreateWindow(void);
	void BURGER_API ReleaseWindow(void);
	void BURGER_API SetViewController(UIViewController *pViewController,UIView *pView);
	BURGER_INLINE UIWindow *GetWindow(void) const { return m_pWindow; }
	BURGER_INLINE UIViewController *GetViewController(void) const { return m_pViewController; }
	BURGER_INLINE UIView *GetView(void) const { return m_pView; }
	BURGER_INLINE static GameApp *GetApp(void) { return g_piOSApp; }
	static RunQueue::eReturnCode BURGER_API Poll(void *pSelf);
#endif

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
	void BURGER_API FocusWindow(void);
	Word BURGER_API SetWindowSize(Word uWidth,Word uHeight);
	BURGER_INLINE NSApplication *GetApplication(void) const { return m_pApplication; }
	BURGER_INLINE NSApplicationDelegate *GetDelegate(void) const { return m_pApplicationDelegate; }
	BURGER_INLINE NSWindow *GetWindow(void) const { return m_pWindow; }
#endif
};
}

extern int BURGER_API CodeEntry(Burger::GameApp *pGameApp);

/* END */

#endif
