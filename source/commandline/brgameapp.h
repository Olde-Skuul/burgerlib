/***************************************

	Game Application startup class

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
class GameApp : public Base {
	BURGER_DISABLECOPYCONSTRUCTORS(GameApp);
	BURGER_RTTI_IN_CLASS();
protected:
	Keyboard *m_pKeyboard;			///< Pointer to the active keyboard instance
	Mouse *m_pMouse;				///< Pointer to the active mouse instance
	Joypad *m_pJoypad;				///< Pointer to the active joypad instance
	SoundManager *m_pSoundManager;	///< Pointer to the active sound manager instance
	Display *m_pDisplay;			///< Pointer to the active display instance
	Renderer *m_pRenderer;			///< Pointer to the active display renderer
	const char **m_ppArgv;			///< Arguments passed to the console
	MemoryManagerGlobalHandle m_MemoryManagerHandle;	///< Handle based memory manager for the app
	RunQueue m_RunQueue;			///< Internal run queue

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
public:
	typedef Word (BURGER_API *MainWindowProc)(GameApp *pThis,HWND__ *hWnd,Word uMessage,WordPtr wParam,WordPtr lParam,WordPtr *pOutput);	///< Callback custom window message handler
protected:
	HINSTANCE__ *m_hInstance;		///< (Windows only) Current instance of the application
	HWND__ *m_hWindow;				///< (Windows only) Main window for the application
	HICON__ *m_pDefaultCursor;		///< (Windows only) Default cursor when not in the game area
	const char **m_ppOldArgv;		///< (Windows only) Saved pointer to the global __argv for restoring on exit
	MainWindowProc m_pCallBack;		///< (Windows only) Custom application window proc
	int m_iWindowCenterX;			///< (Windows only) X coordinate of the center of the window
	int m_iWindowCenterY;			///< (Windows only) Y coordinate of the center of the window
	Word m_uErrorMode;				///< (Windows only) Initial GetErrorMode() value
	Word16 m_uAtom;					///< (Windows only) Atom assigned to my class
	Word16 m_bCoCreateInstanceInit;	///< (Windows only) \ref TRUE if CoInitializeEx() was initialized
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
	Word m_bQuit;					///< Set to \ref TRUE when the application has to shut down
	Word m_bInBackground;			///< Set to \ref TRUE if the game should be paused due to another window being in front
	Word m_bMinimized;				///< Set to \ref TRUE if the game is minimized (PC/Mac only, always false on consoles)
	Word m_bAllowWindowSwitching;	///< Set to \ref TRUE if the game allows switching to windowed mode via Alt-Enter on desktop/laptops
	Word m_bMouseOnScreen;			///< \ref TRUE if the mouse cursor is on the screen in windows systems
	Word m_bWindowSwitchRequested;	///< \ref TRUE if the window resolution switch key was pressed
	void BURGER_API InitDefaults(void);
	void BURGER_API ShutdownDefaults(void);
public:
	GameApp(WordPtr uDefaultMemorySize=Burger::MemoryManagerHandle::DEFAULTMEMORYCHUNK,Word uDefaultHandleCount=Burger::MemoryManagerHandle::DEFAULTHANDLECOUNT,WordPtr uMinReserveSize=Burger::MemoryManagerHandle::DEFAULTMINIMUMRESERVE);
	virtual ~GameApp();
	BURGER_INLINE void Poll(void) { m_RunQueue.Call(); };
	BURGER_INLINE void AddRoutine(RunQueue::CallbackProc Proc,void *pData,Word uPriority=RunQueue::PRIORITY_MEDIUM) { m_RunQueue.Add(Proc,pData,uPriority); }
	BURGER_INLINE void RemoveRoutine(RunQueue::CallbackProc Proc,void *pData) { m_RunQueue.Remove(Proc,pData); }
	BURGER_INLINE RunQueue *GetRunQueue(void) { return &m_RunQueue; }
	BURGER_INLINE MemoryManagerGlobalHandle * GetMemoryManager(void) { return &m_MemoryManagerHandle; }
	BURGER_INLINE void SetArgv(const char **ppArgv) { m_ppArgv=ppArgv; }
	BURGER_INLINE const char ** GetArgv(void) const { return m_ppArgv; }
	BURGER_INLINE void SetArgc(int iArgc) { m_iArgc=iArgc; }
	BURGER_INLINE int GetArgc(void) const { return m_iArgc; }
	BURGER_INLINE void SetQuitCode(void) { m_bQuit = TRUE; }
	BURGER_INLINE void ClearQuitCode(void) { m_bQuit = FALSE; }
	BURGER_INLINE Word GetQuitCode(void) const { return m_bQuit; }
	BURGER_INLINE void SetInBackground(Word bInBackground) { m_bInBackground = bInBackground; }
	BURGER_INLINE Word IsInBackground(void) const { return m_bInBackground; }
	BURGER_INLINE void SetKeyboard(Keyboard *pKeyboard) { m_pKeyboard = pKeyboard; }
	BURGER_INLINE Keyboard *GetKeyboard(void) const { return m_pKeyboard; }
	BURGER_INLINE void SetMouse(Mouse *pMouse) { m_pMouse = pMouse; }
	BURGER_INLINE Mouse *GetMouse(void) const { return m_pMouse; }
	BURGER_INLINE void SetJoypad(Joypad *pJoypad) { m_pJoypad = pJoypad; }
	BURGER_INLINE Joypad *GetJoypad(void) const { return m_pJoypad; }
	BURGER_INLINE void SetSoundManager(SoundManager *pSoundManager) { m_pSoundManager = pSoundManager; }
	BURGER_INLINE SoundManager *GetSoundManager(void) const { return m_pSoundManager; }
	BURGER_INLINE void SetDisplay(Display *pDisplay) { m_pDisplay = pDisplay; }
	BURGER_INLINE Display *GetDisplay(void) const { return m_pDisplay; }
	BURGER_INLINE void SetRenderer(Renderer *pRenderer) { m_pRenderer = pRenderer; }
	BURGER_INLINE Renderer *GetRenderer(void) const { return m_pRenderer; }
	BURGER_INLINE void SetWindowSwitching(Word bAllowWindowSwitching) { m_bAllowWindowSwitching = bAllowWindowSwitching; }
	BURGER_INLINE Word IsWindowSwitchingAllowed(void) const { return m_bAllowWindowSwitching; }
	Word BURGER_API SwitchVideo(void);
	BURGER_INLINE Word IsMinimized(void) const { return m_bMinimized; }
	BURGER_INLINE void SetMinimized(Word bMinimized) { m_bMinimized = bMinimized; }
	Word BURGER_API IsAppFullScreen(void) const;
	Word BURGER_API IsResizingAllowed(void) const;
	BURGER_INLINE void SetMouseOnScreen(Word bMouseOnScreen) { m_bMouseOnScreen = bMouseOnScreen; }
	BURGER_INLINE Word IsMouseOnScreen(void) const { return m_bMouseOnScreen; }
	BURGER_INLINE void SetWindowSwitchRequested(Word bWindowSwitchRequested) { m_bWindowSwitchRequested = bWindowSwitchRequested; }
	BURGER_INLINE Word IsWindowSwitchRequested(void) const { return m_bWindowSwitchRequested; }

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	static void BURGER_API OutputWindowsMessage(Word uMessage,WordPtr wParam,WordPtr lParam);
	BURGER_INLINE HINSTANCE__ *GetInstance(void) const { return m_hInstance; }
	BURGER_INLINE HWND__ *GetWindow(void) const { return m_hWindow; }
	BURGER_INLINE MainWindowProc GetCallBack(void) const { return m_pCallBack; }
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
