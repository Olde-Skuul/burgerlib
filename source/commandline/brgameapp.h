/***************************************

	Game Application startup class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

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

/* BEGIN */
namespace Burger {
class Keyboard;
class Joypad;
class Mouse;
class Sound;
class Display;
class Renderer;
class GameApp : public Burger::Base {
	BURGER_DISABLECOPYCONSTRUCTORS(GameApp);
protected:
	Keyboard *m_pKeyboard;			///< Pointer to the active keyboard instance
	Mouse *m_pMouse;				///< Pointer to the active mouse instance
	Joypad *m_pJoypad;				///< Pointer to the active joypad instance
	Sound *m_pSound;				///< Pointer to the active sound instance
	Display *m_pDisplay;			///< Pointer to the active display instance
	Renderer *m_pRenderer;			///< Pointer to the active display renderer
	const char **m_ppArgv;			///< Arguments passed to the console
	MemoryManagerGlobalHandle m_MemoryManagerHandle;	///< Handle based memory manager for the app
	RunQueue m_RunQueue;			///< Internal run queue
	int m_iArgc;					///< Number of valid arguments
	Word m_bQuit;					///< Set to \ref TRUE when the application has to shut down
	Word m_bInBackground;			///< Set to \ref TRUE if the game should be paused due to another window being in front
	Word m_bMinimized;				///< Set to \ref TRUE if the game is minimized (PC/Mac only, always false on consoles)
	Word m_bAllowWindowSwitching;	///< Set to \ref TRUE if the game allows switching to windowed mode via Alt-Enter on desktop/laptops
	Word m_bMouseOnScreen;			///< \ref TRUE if the mouse cursor is on the screen in windows systems
public:
	GameApp(WordPtr uDefaultMemorySize=Burger::MemoryManagerHandle::DEFAULTMEMORYCHUNK,Word uDefaultHandleCount=Burger::MemoryManagerHandle::DEFAULTHANDLECOUNT,WordPtr uMinReserveSize=Burger::MemoryManagerHandle::DEFAULTMINIMUMRESERVE);
	~GameApp();
	BURGER_INLINE void Poll(void) { m_RunQueue.Call(); };
	BURGER_INLINE void AddRoutine(RunQueue::CallbackProc Proc,void *pData) { m_RunQueue.Add(Proc,pData); }
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
	BURGER_INLINE void SetSound(Sound *pSound) { m_pSound = pSound; }
	BURGER_INLINE Sound *GetSound(void) const { return m_pSound; }
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
};
}
/* END */

#endif
